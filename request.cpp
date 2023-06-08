#include "request.hpp"

Request::Request() : is_first_body(true), chunk_size(0), bytes_recv(0), is_error(0), content_length(0), is_finished(false)
{
  if (map.empty())
  {
    map["text/html"] = "html";
    map["image/png"] = "png";
    map["video/mp4"] = "mp4";
    map["audio/mpeg"] = "mp3";
    map["text/css"] = "css";
    map["text/javascript"] = "js";
    map["application/json"] = "json";
    map["application/xml"] = "xml";
    map["application/pdf"] = "pdf";
    map["application/zip"] = "zip";
    map["text/plain"] = "txt";
    map["image/gif"] = "gif";
    map["image/jpeg"] = "jpg";
    map["image/svg+xml"] = "svg";
    map["audio/wav"] = "wav";
    map["video/mpeg"] = "mpg";
    map["video/quicktime"] = "mov";
    map["video/x-msvideo"] = "avi";
    map["text/x-python-script"] = "py";
    map["text/x-php-script"] = "php";
  }
}

void Request::parse_request_line(std::string req_line)
{
  int pos = req_line.find(SPACE);
  method = req_line.substr(0, pos);

  int new_pos = req_line.find(SPACE, pos + 1);
  location = req_line.substr(pos + 1, new_pos - pos - 1);
}

void Request::parse_headers(std::string header)
{
  size_t pos = header.find(LINE_SEP) + 2;
  parse_request_line(header.substr(0, pos));

  while (pos < header.length())
  {
    size_t end_pos = header.find(LINE_SEP, pos);
    if (end_pos == std::string::npos)
      end_pos = header.length();

    size_t colon_pos = header.find(COLON, pos);
    if (colon_pos == std::string::npos)
      break;

    std::string key = header.substr(pos, colon_pos - pos);
    std::string value = header.substr(colon_pos + 2, end_pos - colon_pos - 2);

    headers[key] = value;
    pos = end_pos + 2;
  }
}

std::string Request::decode_chunked(std::string input)
{
  std::string chunk;

  if (is_first_body)
  {
    input.insert(0, "\r\n");
    is_first_body = false;
  }
  input.insert(0, temp);
  temp = "";

  while (input.length() > 0)
  {
    if (chunk_size >= input.length())
    {
      chunk.append(input);
      chunk_size -= input.length();
      break;
    }
    else
    {
      if (chunk_size > 0)
      {
        chunk.append(input.substr(0, chunk_size));
        input = input.substr(chunk_size, input.length() - chunk_size);
      }

      int start = 2;
      size_t pos = input.find("\r\n", start);
      if (pos == std::string::npos)
      {
        temp = input;
        chunk_size = 0;
        break;
      }
      std::string chunk_size_str = input.substr(start, pos - start);
      try
      {
        chunk_size = std::stoi(chunk_size_str, nullptr, 16);
      }
      catch (std::exception &err)
      {
        is_error = 400;
        break;
      }
      if (chunk_size == 0)
      {
        is_finished = true;
        break;
      }
      input = input.substr(pos + 2, input.length() - pos - 2);
    }
  }

  return chunk;
}

void Request::parse_body(std::string req_body)
{
  std::string time = std::to_string(std::time(nullptr));


  if (is_first_body)
  {
    file_name = config->servers[server_index].locations[location_index].upload_path 
          + "/" + time + "." + map[headers["Content-Type"]];
    file = new std::ofstream(file_name, std::ios_base::app);
  }

  if (content_length) {
    is_first_body = false;
    if (bytes_recv < content_length) {
      *file << req_body;
      bytes_recv += req_body.length();
    }
    if (bytes_recv == content_length) {
      is_finished = true;
      file->close();
      delete file;
    }
  }
  else {
    std::string decoded_chunk = decode_chunked(req_body); 
    *file << decoded_chunk;
    bytes_recv += decoded_chunk.length();
    if (bytes_recv > max_body_size) {
      is_error = 413;
      file->close();
      delete file;
      if (std::remove(file_name.c_str()) != 0) {
        std::cout << "Error deleting the file" << std::endl;
      } else {
        std::cout << "File deleted successfully" << std::endl;
      }
    }
    if (is_finished) {
      file->close();
      delete file;
    }
  }
}

Request::~Request()
{
}

std::string Request::get_method()
{
  return method;
}

std::string Request::get_location()
{
  return location;
}

std::map<std::string, std::string> Request::get_headers()
{
  return headers;
}