
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "main.hpp"

#define SEP "\r\n\r\n"
#define LINE_SEP "\r\n"
#define COLON ": "
#define SPACE ' '
#define POST "POST"
#define GET "GET"
#define DELETE "DELETE"

class Request
{
private:
  std::string method;
  std::string location;
  std::string req_file;
  std::string body;
  bool is_first_body;
  size_t chunk_size;
  std::string temp;
  std::map<std::string, std::string> map;
  std::ofstream *file;
  size_t bytes_recv;
  std::string file_name;

public:
  Request();

  void parse_headers(std::string header);
  void parse_request_line(std::string req_line);
  // void parse_body(std::string body);

  // adiouane
  std::string get_location();
  std::string get_method();
  std::string get_file_name();
  std::map<std::string, std::string> get_headers();
  std::string decode_chunked(std::string input);
  void parse_body(std::string req_body);

  ~Request();


  int is_error;
  int server_index;
  int location_index;
  size_t content_length;
  size_t max_body_size;
  bool is_finished;
  std::map<std::string, std::string> headers;
};

#endif