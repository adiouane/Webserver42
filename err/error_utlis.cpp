/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_utlis.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiouane <adiouane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/24 01:30:41 by adiouane          #+#    #+#             */
/*   Updated: 2023/06/09 00:29:09 by adiouane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../response.hpp"

void Response::sent_error(int c_socket, int code)
{
    std::map<int, std::string> response_map;
    // there are 399 status codes in the response
    // 100-199 are informational responses
    // 200-299 are success codes
    // 300-399 are redirection codes
    // 400-500 are client error codes
    // 501-599 are server error codes
    response_map[200] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 6\r\n\r\n200 OK";
    response_map[201] = "HTTP/1.1 201 Created\r\nContent-Type: text/html\r\nContent-Length: 7\r\n\r\nCreated";
    response_map[301] = "HTTP/1.1 301 Moved Permanently\r\nContent-Type: text/html\r\nContent-Length: 21\r\n\r\n301 Moved Permanently";
    response_map[400] = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 15\r\n\r\n400 Bad Request";
    response_map[403] = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\nContent-Length: 9\r\n\r\nForbidden";
    response_map[404] = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 13\r\n\r\n404 Not Found";
    response_map[405] = "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/html\r\nContent-Length: 22\r\n\r\n405 Method Not Allowed";
    response_map[409] = "HTTP/1.1 409 Conflict\r\nContent-Type: text/html\r\nContent-Length: 12\r\n\r\n409 Conflict";
    response_map[500] = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: 25\r\n\r\n500 Internal Server Error";
    response_map[501] = "HTTP/1.1 501 Not Implemented\r\nContent-Type: text/html\r\nContent-Length: 19\r\n\r\n501 Not Implemented";
    response_map[505] = "HTTP/1.1 505 HTTP Version Not Supported\r\nContent-Type: text/html\r\nContent-Length: 26\r\n\r\nHTTP Version Not Supported";
    response_map[413] = "HTTP/1.1 413 Pyload too large\r\nContent-Type: text/html\r\nContent-Length: 20\r\n\r\n413 Pyload too large";

    std::map<int, std::string>::iterator it = response_map.find(code);
    if (it == response_map.end()) // if the code is not in the map
    {
        std::cout << "Error code not valid" << code << std::endl;
        return;
    }

    std::string response = it->second; // get the response from the map using the code as key
    send(c_socket, response.c_str(), response.length(), 0);
}

std::string Response::getContentType(std::string &filename)
{
    if (filename.empty())
        return "";
    std::map<std::string, std::string> contentTypeMap;
    contentTypeMap[".html"] = "text/html";
    contentTypeMap[".png"] = "image/png";
    contentTypeMap[".mp4"] = "video/mp4";
    contentTypeMap[".mp3"] = "audio/mpeg";
    contentTypeMap[".css"] = "text/css";
    contentTypeMap[".js"] = "text/javascript";
    contentTypeMap[".json"] = "application/json";
    contentTypeMap[".xml"] = "application/xml";
    contentTypeMap[".pdf"] = "application/pdf";
    contentTypeMap[".zip"] = "application/zip";
    contentTypeMap[".txt"] = "text/plain";
    contentTypeMap[".gif"] = "image/gif";
    contentTypeMap[".jpg"] = "image/jpeg";
    contentTypeMap[".svg"] = "image/svg+xml";
    contentTypeMap[".wav"] = "audio/wav";
    contentTypeMap[".mpg"] = "video/mpeg";
    contentTypeMap[".mov"] = "video/quicktime";
    contentTypeMap[".avi"] = "video/x-msvideo";
    contentTypeMap[".php"] = "php";
    contentTypeMap[".py"] = "python";

    std::string contentType = "application/octet-stream";                // default content type is binary
    std::string extension = filename.substr(filename.find_last_of("."));
    if (contentTypeMap.count(extension) > 0)
    {
        contentType = contentTypeMap[extension];
        return contentType;
    }
    return contentType;
}

int Response::is_autoindex_enabled(std::string autoindex)
{
    if (autoindex == "on")
        return (1);
    return (0);
}

int Response::is_allowed_method(std::string method, std::vector<std::string> &methods)
{
    if (std::find(methods.begin(), methods.end(), method) == methods.end())
        return (0);
    return (1);
}

int Response::check_which_server(Client *c)
{
    Response r;
    std::string host = c->req.get_headers()["Host"];
    if (host.empty())
    {
        r.sent_error(c->get_socket_fd(), 400);
        c->is_finished = true;
        return 0;
    }
    std::string host_c = host.substr(0, host.find(":"));
    std::string port_c = host.substr(host.find(":") + 1, host.size());
    for (size_t j = 0; j < config->servers.size(); j++)
    {
        for (size_t k = 0; k < config->servers[j].server_name.size(); k++)
        {
            if (config->servers[j].server_name[k] == host_c || config->servers[j].host == host_c)
            {
                if ((config->servers[j].port == port_c))
                    return j;
            }
        }
    }
    return -1;
}

int Response::handle_url(Client *c, std::string url)
{
    Response r;
    int j = 0;

    std::string extension = url.substr(url.find_last_of('.') + 1);
    std::string rootPath = config->servers[c->req.server_index].locations[c->req.location_index].root;
    std::string cgiPath = config->servers[c->req.server_index].locations[c->req.location_index].cgi_path[extension];

    int locationMatchIndex = r.is_locations_matched(url, c->req.server_index, c);

    if (locationMatchIndex != -1)
    {
        std::string pathLocation = rootPath + cgiPath;

        if (r.check_is_dir(pathLocation))
        {
            if (r.is_autoindex_enabled(config->servers[c->req.server_index].locations[c->req.location_index].autoindex))
            {
                std::string directory = rootPath;
                std::string indexPath = rootPath + c->save_delete;
                indexPath = config->servers[c->req.server_index].locations[c->req.location_index].location_config[j];

                if (indexPath.empty())
                {
                    r.send_Get_response("404:", c);
                    c->is_finished = true;
                    return 0;
                }

                r.send_response_autoindex(c->get_socket_fd(), indexPath, directory, c);
                send(c->get_socket_fd(), indexPath.c_str(), indexPath.length(), 0);
                c->is_finished = true;
            }
            else
            {
                r.send_Get_response("404:", c);
                c->is_finished = true;
                return 0;
            }
        }
        else if (access(pathLocation.c_str(), F_OK) != -1)
        {
            c->_fileName = pathLocation;
            return 1;
        }
    }
    else
    {
        send_Get_response("404:", c);
        c->is_finished = true;
        return 0;
    }

    return 0;
}

void Response::send_response_autoindex(int fd, std::string &url, std::string &root, Client *c)
{
    // Send the autoindex page with the clickable content of the root
    Response r;
    std::string response;
    std::string content;

    content = "<html>\n"
              "<head><title>Index of " +
              root + "</title></head>\n"
                     "<body bgcolor=\"white\">\n"
                     "<h1>Index of " +
              root + "</h1><hr><pre>\n";

    std::string dir = root;
    DIR *directory;
    struct dirent *directoryEntry;
    if ((directory = opendir(dir.c_str())) == NULL)
    {
        std::cout << "Error opening " << dir << std::endl;
        return;
    }

    while ((directoryEntry = readdir(directory)) != NULL)
    {
        if (directoryEntry->d_name[0] == '.' || directoryEntry->d_name[1] == '.')
            continue;

        content += "<a href=\"" + url + "/" + directoryEntry->d_name + "\">" + directoryEntry->d_name + "</a>\n";
    }

    closedir(directory);

    content += "</pre><hr>\n"
               "</body>\n"
               "</html>";

    response = "HTTP/1.1 200 OK\r\n"
               "Content-Type: text/html\r\n"
               "Content-Length: " +
               std::to_string(content.length()) + "\r\n\r\n" + content;

    if (send(fd, response.c_str(), response.length(), 0) < 1)
        c->is_finished = true;
    ;
    return;
}
