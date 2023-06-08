#include "server.hpp"

Server::Server()
{
  std::cout << "Server was Created" << std::endl;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  FD_ZERO(&fds);
}

Server::Server(std::string &port, std::string &host)
{
  std::cout << "Server was Created" << std::endl;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  this->port = port;
  this->host = host;

  FD_ZERO(&fds);
}


int Server::get_sokcet_fd()
{
  return sockfd;
}

void Server::get_address_info()
{
  int ret;

  std::cout << "Getting info address..." << std::endl;
  if ((ret = getaddrinfo(0, port.c_str(), &hints, &bind_address)))
  {
    std::cerr << gai_strerror(ret) << std::endl;
    exit(1);
  }
}

void Server::create_socket()
{
  const int enable = 1;
  std::cout << "Creating a Socket..." << std::endl;
  sockfd = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
  fcntl(sockfd, F_SETFL, O_NONBLOCK);
  max_socket = sockfd;
  if (sockfd == -1)
  {
    std::cerr << "socket: " << strerror(errno) << std::endl;
    exit(1);
  }
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
  {
    std::cerr << "setsockopt: " << strerror(errno) << std::endl;
    exit(1);
  }
}

void Server::bind_socket()
{
  std::cout << "Binding the socket with info address..." << std::endl;
  if (bind(sockfd, bind_address->ai_addr, bind_address->ai_addrlen))
  {
    std::cerr << "bind: " << strerror(errno) << std::endl;
    exit(1);
  }
  freeaddrinfo(bind_address);
}

void Server::listen_on_socket()
{
  std::cout << "listening on the socket..." << std::endl;
  if (listen(sockfd, SOCKET_LISTEN))
  {
    std::cerr << "listen: " << strerror(errno) << std::endl;
    exit(1);
  }
}

void Server::create_server()
{
  get_address_info();
  create_socket();
  bind_socket();
  listen_on_socket();
  FD_SET(sockfd, &fds);
}

void Server::add_new_client()
{
  Client *c = new Client();
  struct sockaddr_storage addr;
  socklen_t addr_len = sizeof(addr);
  const int enable = 1;

  int client_socket = accept(sockfd, (struct sockaddr *)&addr, &addr_len);
  fcntl(client_socket, F_SETFL, O_NONBLOCK);
  if (setsockopt(client_socket, SOL_SOCKET, SO_NOSIGPIPE, &enable, sizeof(enable)) < 0)
  {
    std::cerr << "setsockopt: " << strerror(errno) << std::endl;
    exit(1);
  }

  if (client_socket < 0)
  {
    std::cerr << "accept: " << strerror(errno) << std::endl;
    exit(1);
  }

  FD_SET(client_socket, &fds);
  if (client_socket > max_socket)
    max_socket = client_socket;

  c->set_socket_fd(client_socket);
  c->set_address(addr);
  clients.push_back(c);
}

Client *Server::get_client_with_fd(int fd)
{
  std::vector<Client *>::iterator start = clients.begin();

  while (start < clients.end())
  {
    if ((*start)->get_socket_fd() == fd)
      return (*start);
    start++;
  }
  return NULL;
}

void Server::handle_request(Client *c)
{
  char buff[BUFFER_SIZE];
  int bytes_received = recv(c->get_socket_fd(), buff, sizeof(buff), 0);
  if (bytes_received < 1)
    c->read_done = true;
  else
  {
    c->parse_request(buff, bytes_received);
  }
}

void Server::drop_client(Client *c)
{
  if (c && c->is_finished == true)
  {
    for (size_t i = 0; i < clients.size(); i++)
    {
      if (c->get_socket_fd() == clients[i]->get_socket_fd())
      {
        FD_CLR(c->get_socket_fd(), &fds);
        close(c->get_socket_fd());
        clients.erase(clients.begin() + i);
        delete c;
        break;
      }
    }
  }
}

void Server::start_listening(struct timeval &tv, Client *c) {
  FD_ZERO(&this->read_fds);
  FD_SET(this->sockfd, &this->read_fds);
  FD_ZERO(&this->write_fds);
  FD_SET(this->sockfd, &this->write_fds);
	for (size_t i = 0; i < this->clients.size(); i++)
	{
		FD_SET(this->clients[i]->get_socket_fd(), &this->read_fds);
    FD_SET(this->clients[i]->get_socket_fd(), &this->write_fds);
	}

  Response res;
  if (select(max_socket + 1, &read_fds, &write_fds, NULL, &tv) < 0)
  {
    std::cerr << "select: " << strerror(errno) << std::endl;
    exit(1);
  }

  for (int i = 3; i < max_socket + 1; i++)
  {
    if (i != sockfd) {
      c = get_client_with_fd(i);
      if (!c) continue;
    }
    if (FD_ISSET(i, &read_fds))
    {
      if (i == sockfd)
        add_new_client();
      else if (c and c->read_done == false)
        handle_request(c);
    }
    else if (FD_ISSET(i, &write_fds)) {
      res.handle_response(c);
    }
    drop_client(c);
  }
}