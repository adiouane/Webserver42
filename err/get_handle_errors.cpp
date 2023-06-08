/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_utils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiouane <adiouane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/24 08:17:30 by adiouane          #+#    #+#             */
/*   Updated: 2023/04/24 08:17:30 by adiouane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../response.hpp"

void send_response_return(int fd, const std::string& return_)
{
    // Create the response headers
    std::string response = "HTTP/1.1 301 Moved Permanently\r\n";
    response += "Location: " + return_ + "\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + std::to_string(return_.length()) + "\r\n";
    response += "\r\n";
    // Send the response
    send(fd, response.c_str(), response.length(), 0);
}

void    is_config_empty(Client *c, int i, int j)
{
    Response r;
    if (config->servers[i].locations[j].root.empty())
    {
        r.sent_error(c->get_socket_fd(), 404);
        c->is_finished = true;
        return;
    }
    else if (config->servers[i].locations[j].methods.empty())
    {
        r.sent_error(c->get_socket_fd(), 404);
        c->is_finished = true;
        return;
    }
}

int if_location_has_cgi(int i, int j){
    
    if (config->servers[i].locations[j].cgi_path["py"].empty() == false ||
        config->servers[i].locations[j].cgi_path["php"].empty() == false )
        return (1);
    return (0);
}

int Response::check_is_dir(std::string location)
{
    // check if file open if yes is directory or not
    if (open(location.c_str(), O_DIRECTORY) != -1) // O_DIRECTORY is a flag for directories not being opened properly for reading and writing
        return (1);
    else
        return (0);
}


int check_is_file(std::string location)
{
   if (access(location.c_str(), F_OK) != -1)
        return (1);
    return (0);
}


int auto_index(Client *c, int index, int l)
{
    Response r;
    if (r.is_autoindex_enabled(config->servers[index].locations[l].autoindex) == 1)
    {
        // list all of content of directory and send it to client
        int j = 0;
        std::string dir = config->servers[index].locations[l].root; // root
        std::string url = config->servers[index].locations[l].root + c->save_delete;
        url = config->servers[index].locations[l].location_config[j];;
        if (url == "")
        {
            r.send_Get_response("404:", c);
            c->is_finished = true;
            return 0;
        }
        r.send_response_autoindex(c->get_socket_fd(), url, dir, c);
        c->is_finished = true;
        return 1;
    }
    return 0;
}


int is_valid_file(Client *c, int i, int j)
{
    std::string path;
    Request req;
    Response r;

   for(size_t k = 0; k < config->servers[i].locations[j].index.size(); k++)
   {
        is_config_empty(c, i, j);
        if (c->req.get_location() == "/")
        {
            path = config->servers[i].locations[j].root + config->servers[i].locations[j].index[k];
            c->_fileName = path;
            if (auto_index(c, i, j) == 1)
                return (1);
            return (1);
        }
        if (r.check_is_dir(c->url) == 1)
        {
            if (auto_index(c, i, j) == 1)
                return (1);
            for (size_t l = 0; l < config->servers[i].locations[j].index.size(); l++)
            {
                path = config->servers[i].locations[j].root + c->save_delete + "/" + config->servers[i].locations[j].index[l];
                if (check_is_file(path))
                {
                    c->_fileName = path;
                    return (1);
                }
            }
        }
        else{
            if (check_is_file(c->url) == 1) // is file
            {
                c->_fileName = c->url;
                return (1);
            }
            else
            {
                r.sent_error(c->get_socket_fd(), 404);
                c->is_finished = true;
                return (0);
            }
        }
   }
    return (0);
}

int Response::is_locations_matched(std::string request, int index, Client *c)
{
    Response r;
    for (size_t i = 0; i < config->servers[index].locations.size(); i++)
    {
        int j = 0;
        if (request  == config->servers[index].locations[i].location_config[j])
        {
            c->url = config->servers[index].locations[i].root + c->save_delete;
            return (i);
        }
        else
        {
            if (request[0] == '/' && request.find_last_of("/") != 0) 
            {
                c->save_delete = request.substr(request.find_last_of("/"), request.size());
                request.erase(request.find_last_of("/"), request.size());
            }
            if (request  == config->servers[index].locations[i].location_config[j])
            {
                c->url = config->servers[index].locations[i].root + c->save_delete;
                return (i);
            }
        }
    }
    return (-1);
}


int Response::is_request_in_location(Client *c)
{
    Response res;
    int l = -1;
    for (size_t i = 0; i < config->servers.size(); i++)
    {
        int index = check_which_server(c);
        if (index == -1)
        {
            send_Get_response("404:", c);
            c->is_finished = true;
            return 0;
        }
        for (size_t j = 0; j < config->servers[index].locations.size(); j++)
        {
            // match location
            l =  res.is_locations_matched(c->req.get_location(), index, c);
            if (l == -1)
            {
                send_Get_response("404:", c);
                c->is_finished = true;
                return 0;
            }
            if (l != -1){
                c->req.server_index = index;
                c->req.location_index = l;
                if (config->servers[index].locations[l].return_ != "")
                {
                    send_response_return(c->get_socket_fd(), config->servers[index].locations[l].return_);
                    c->is_finished = true;
                    return 1;
                }
                if (is_allowed_method(c->req.get_method(), config->servers[index].locations[l].methods) == 0)
                {
                    res.sent_error(c->get_socket_fd(), 405);
                    c->is_finished = true;
                    return 0;
                }
                if (is_valid_file(c, index, l) == 1) // if file is valid
                {
                    if (if_location_has_cgi(index, l) == 1 && check_is_file(c->_fileName) == 1)
                    {
                        c->_cgi_path = c->req.get_location();
                        res.Handle_cgi_response(c, c->_cgi_path);
    
                        return (1);
                    }
                    if (c->req.get_method() == "POST" && if_location_has_cgi(index, l) == 0)
                    {
                        sent_error(c->get_socket_fd(), 403);
                        c->is_finished = true;
                        return 0;
                    }
                }
                else{
                    if (auto_index(c, index, l) == 1)
                        return (1);
                    else{
                        send_Get_response("404:", c);
                        c->is_finished = true;
                        return 0;
                    }
                }
                return 1;
            }
        }
    }
    return 0;
}
