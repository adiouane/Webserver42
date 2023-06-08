#ifndef SERVER_HPP
#define SERVER_HPP

#include "main.hpp"
#include "client.hpp"
#include "response.hpp"
#include "request.hpp"

class Client;
class Server
{
private:
  int sockfd;
  struct addrinfo hints;
  struct addrinfo *bind_address;
  int max_socket;
  fd_set fds;
  std::vector<Client *> clients;
  std::string port;
  std::string host;
  fd_set read_fds;
  fd_set write_fds;
public:
  Server();
  Server(std::string &port, std::string &host);

  int get_sokcet_fd();

  void get_address_info();
  void create_socket();
  void bind_socket();
  void listen_on_socket();
  void create_server();
  void add_new_client();
  Client *get_client_with_fd(int fd);
  void start_listening(struct timeval &tv, Client *c);

  // adiouane
  //  get all clients
  std::vector<Client *> get_clients();
  void handle_request(Client *c);
  void drop_client(Client *c);

  ~Server();
};

// void Server::drop_client(Client *c, int i);

#endif