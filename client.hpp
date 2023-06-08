
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "main.hpp"
#include "request.hpp"
#include "response.hpp"

class Client
{
private:
  struct sockaddr_storage addr;
  socklen_t addr_len;
  int socket_fd;
  int received;
  bool is_header;
public:
  Client();
  void set_socket_fd(int fd);
  void set_address(struct sockaddr_storage &address);
  int get_socket_fd();
  void set_received(int rec);

  void parse_request(const char *req_body, size_t length);

  int check_header_errors(void);
  bool handle_post(Client *c);
  void check_errors();
  
  ~Client();

  std::string _fileName;
  std::string content;
  std::ifstream file;
  int fileSize;
  int bytes_sent;
  bool is_valid;
  bool header_sent;
  bool is_finished;
  bool is_first_body;
  int is_error;
  bool read_done;
  std::string url;
  std::string save_delete;
  std::string _cgi_path;
  Request req;
  bool is_droped;
};

#endif