/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config->cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiouane <adiouane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 02:07:00 by adiouane          #+#    #+#             */
/*   Updated: 2023/05/15 22:54:50 by adiouane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
Config::Config()
{
}

Config::~Config()
{
}
void Config::parseServer(std::string& line)
{
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end = 0;

    while (end != std::string::npos)
    {
        end = line.find(' ', start);
        std::string token = line.substr(start, end - start);
        if (!token.empty())
            tokens.push_back(token);
        start = end + 1;
    }

    if (tokens[0] == "host:")
    {
        servers.push_back(server());
        servers.back().host = tokens[1];
    }
    else if (tokens[0] == "port:")
        servers.back().port = tokens[1];
    else if (tokens[0] == "max_body_size:")
        servers.back().max_body_size = tokens[1];
    else if (tokens[0] == "server_name:")
        for (size_t i = 1; i < tokens.size(); i++)
            servers.back().server_name.push_back(tokens[i]);
    else if (tokens[0] == "error_page")
        for (size_t i = 1; i < tokens.size(); i++){
            if (tokens[i] == "400:")
                servers.back().error_page_400[tokens[i]] = tokens[i + 1];
            else if (tokens[i] == "404:")
                servers.back().error_page_404[tokens[i]] = tokens[i + 1];
        }
    else if (tokens.size() == 3 && tokens[0] == "server_name:")
        for (size_t i = 1; i < tokens.size(); i++)
            servers.back().server_name.push_back(tokens[i]);
    else
        return;
}

void Config::parseLocation(std::string& line)
{
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end = 0;

    while (end != std::string::npos)
    {
        end = line.find(' ', start);
        std::string token = line.substr(start, end - start);
        if (!token.empty())
            tokens.push_back(token);
        start = end + 1;
    }

    if (tokens.empty())
        return;

    if (tokens[0] == "}")
        return;
    else if (tokens[0] == "root:")
        servers.back().locations.back().root = tokens[1];
    else if (tokens[0] == "index:")
        for (size_t i = 1; i < tokens.size(); i++)
            servers.back().locations.back().index.push_back(tokens[i]);
    else if (tokens[0] == "autoindex:")
        servers.back().locations.back().autoindex = tokens[1];
    else if (tokens.size() == 3 && tokens[0] == "cgi_path:")
        servers.back().locations.back().cgi_path[tokens[1]] = tokens[2];
    else if (tokens[0] == "methods:")
        for (size_t i = 1; i < tokens.size(); i++)
            servers.back().locations.back().methods.push_back(tokens[i]);
    else if (tokens[0] == "return:")
        servers.back().locations.back().return_ = tokens[1];
    else if (tokens[0] == "upload_path:")
        servers.back().locations.back().upload_path = tokens[1];
    else
        return;
}


Config::Config(std::string filename)
{
    std::ifstream file(filename.c_str());
    std::string line;
    int i = 0;

    if (file.is_open())
    {
        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#')
                continue;

            if (line.find("server {") != std::string::npos)
            {
                i = 1; // if we found server then we will parse server
                continue;
            }

            // Check if the line contains the keyword "location"
            if (line.find("location") != std::string::npos)
            {
                // Create a new location and add it to the list of locations in the last server
                servers.back().locations.push_back(location());
                std::size_t start = line.find("location") + 9; // Start index of the location name
                std::size_t end = line.find("{") - 1; // End index of the location name
                std::string locationName = line.substr(start, end - start);
                servers.back().locations.back().location_config.push_back(locationName);
                // Update the counter variable
                i = 2;
                continue;
            }

            if (i == 1)
                parseServer(line);
            else if (i == 2)
                parseLocation(line);
            else
                std::cout << "Error: " << line << std::endl;
        }

        if (servers.empty())
        {
            std::cout << "No server found" << std::endl;
            exit(1);
        }

        file.close();
    }
    else
    {
        std::cout << "Unable to open file" << std::endl;
        exit(1);
    }
}
