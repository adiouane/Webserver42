/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiouane <adiouane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/22 22:39:45 by adiouane          #+#    #+#             */
/*   Updated: 2023/06/09 00:29:25 by adiouane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.hpp"
#include "server.hpp"

Config *config;

void check_extension(std::string file)
{
  std::string extension = file.substr(file.find_last_of(".") + 1);
  if (extension != "conf")
  {
    std::cout << "Error: Invalid file extension" << std::endl;
    exit(1);
  }
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    std::cout << "Usage: ./webserv <config_file>" << std::endl;
    return 1;
  }

  check_extension(argv[1]);
  config = new Config(argv[1]);

  std::vector<Server *> servers;

  for(size_t i = 0; i < config->servers.size(); i++)
  {
    Server *srv = new Server(config->servers[i].port, config->servers[i].host);
    srv->create_server();
    servers.push_back(srv);
  }
   
  Client *c = NULL;
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 300;

  while(true) {
    for(size_t i = 0; i < servers.size(); i++)
      servers[i]->start_listening(tv, c);
  }

  for(size_t i = 0; i < config->servers.size(); i++)
  {
    delete servers[i];
  }
  delete config;

  return 0;
}