#include "client.hpp"

Client::Client()
{
  addr_len = sizeof(addr);
  received = 0;
  is_header = true;
  bytes_sent = 0;
  fileSize = 0;
  is_valid = false;
  header_sent = false;
  is_finished = false;
  is_droped = false;
  read_done = false;
}


void Client::set_socket_fd(int fd)
{
  socket_fd = fd;
}

int Client::get_socket_fd()
{
  return socket_fd;
}

void Client::set_received(int rec)
{
  received = rec;
}

void Client::set_address(struct sockaddr_storage &address)
{
  addr = address;
}

std::string toLowerCase(const std::string& str) {
    std::string result = str;
    for (std::string::iterator it = result.begin(); it != result.end(); ++it) {
        *it = tolower(*it);
    }
    return result;
}

void Client::check_errors() {
  std::map<std::string, std::string>::iterator it = req.headers.find("Content-Type");
  std::map<std::string, std::string>::iterator it1 = req.headers.find("Content-Length");
  std::map<std::string, std::string>::iterator it2 = req.headers.find("Transfer-Encoding");

  try {
    req.max_body_size = std::stoi(config->servers[req.server_index].max_body_size);
  } catch(...) {
    std::cerr << "Error: body size" << std::endl;
  }


  if (it == req.headers.end())
    req.is_error = 400;
  else if (it1 == req.headers.end() && it2 == req.headers.end())
    req.is_error = 400;
  else if (it1 != req.headers.end() && it2 != req.headers.end())
    req.is_error = 400;
  else if (it2 != req.headers.end() && toLowerCase(req.headers["Transfer-Encoding"]) != "chunked")
    req.is_error = 400;
  else if (it2 == req.headers.end() && it1 != req.headers.end()) {
    try {
      size_t size = std::stoi(req.headers["Content-Length"]);
      req.content_length = size;
      if (size > req.max_body_size)
        req.is_error = 413;
    } catch (...) {
      req.is_error = 413;
    }
  }
}

bool Client::handle_post(Client *c)
{
  Response res;

  if (res.Handle_Post_Error(c) == 0)
  {
    res.sent_error(c->get_socket_fd(), 404);
    c->is_finished = true;
    return 0;
  }
  check_errors();
  if (c->req.is_error)
  {
    res.sent_error(socket_fd, c->req.is_error);
    c->is_finished = true;
    return 0;
  }

  return 1;
}

void Client::parse_request(const char *req_body, size_t length)
{
  std::string extension;
  std::string str(req_body, length);
  if (is_header)
  {
    int pos = str.find(SEP);
    std::string header = str.substr(0, pos);
    std::string body = str.substr(pos + 4);

    req.parse_headers(header);
    std::string url = req.get_location();
    extension = url.substr(url.find_last_of(".") + 1);
    if (req.get_method() == "POST" && handle_post(this) && config->servers[req.server_index].locations[req.location_index].cgi_path[extension].empty())
    {
      if (!req.is_error)
        req.parse_body(body);
      is_header = false;
    }
  }
  else if (req.get_method() == "POST" && handle_post(this) && !req.is_error && config->servers[req.server_index].locations[req.location_index].cgi_path[extension].empty())
    req.parse_body(str);
};

Client::~Client()
{
}