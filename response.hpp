/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiouane <adiouane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/11 05:41:07 by adiouane          #+#    #+#             */
/*   Updated: 2023/04/11 05:41:07 by adiouane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include "server.hpp"
#include "request.hpp"
#include "parsing/Config.hpp"
#include <fcntl.h>
class Config;
class Client;

class Response
{
    private:
        /* data */
    public:
        Response();
        ~Response();
        // variables
        std::string save_delete;
        void handle_response(Client *c);
        bool is_valid_config(Client *c);
        bool Post_is_valid_config(Client *c);
        void handle_delete(Client *c);

        /*--------------------------ERRORS--------------------------*/
        int is_request_in_location(Client *c);
        void sent_error(int client_socket, int code);
        std::string getContentType(std::string &filename);
        int is_autoindex_enabled(std::string autoindex);
        int is_allowed_method(std::string method, std::vector<std::string> &methods);
        int check_which_server(Client *c);
        int is_locations_matched(std::string request, int index, Client *c);
        int handle_url(Client *c, std::string url);
        int Handle_Post_Error(Client *c);

        /*--------------------------SEND RESPONSE--------------------------*/
        void send_Get_response(std::string key, Client *c);
        void send_response_file(int fd, std::string key, Client *c);
        void send_response_autoindex(int fd, std::string &path, std::string &url, Client *c);
        int Handle_cgi_response(Client *c, std::string url);
        int check_is_dir(std::string location);
};

#endif