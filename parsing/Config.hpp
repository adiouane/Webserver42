/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config->hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiouane <adiouane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 01:45:31 by adiouane          #+#    #+#             */
/*   Updated: 2023/05/16 17:43:37 by adiouane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <string.h>

class Config
{
    private:
    public:
        void parseServer(std::string &line);
        void parseLocation(std::string &line);
        typedef struct
        {
            std::string root;
            std::string autoindex;
            std::string upload_path;
            std::string return_;
            std::vector<std::string> methods;
            std::vector<std::string> index;
            std::map<std::string, std::string> cgi_path;
            std::vector<std::string> location_config;
        } location;
        typedef struct
        {
            std::string host;
            std::string port;
            std::string max_body_size;
            std::map<std::string, std::string> error_page_400;
            std::map<std::string, std::string> error_page_404;
            std::vector<std::string> server_name;
            std::vector<location> locations;
        } server;

        std::vector<server> servers;
        Config();
        ~Config();
    Config(std::string filename);
};

#endif