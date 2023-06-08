/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_send_response.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiouane <adiouane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/21 15:25:53 by adiouane          #+#    #+#             */
/*   Updated: 2023/06/05 19:37:06 by adiouane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../response.hpp"

void Response::send_Get_response(std::string key, Client *c)
{
    if (!c->header_sent)
    {
        if (key == "404:")
            c->file.open("root/error/404.html");
        else
            c->file.open(c->_fileName.c_str());
        if (!c->file.is_open() && !c->_fileName.empty())
        {
            std::cout << "Error opening file" << c->_fileName << std::endl;
            c->is_finished = true;
            return;
        }

        // Get file size
        c->file.seekg(0, std::ios::end);
        c->fileSize = c->file.tellg();
        c->file.seekg(0, std::ios::beg);

        // Create response header
        std::string response;
        if (key == "404:"){
            response = "HTTP/1.1 404 Not Found\r\n";
            response += "Content-Type: text/html\r\n";
            response += "Content-Length: " + std::to_string(c->fileSize) + "\r\n\r\n";
        }
        else{
            response = "HTTP/1.1 200 OK\r\n";
            response += "Content-Type: " + getContentType(c->_fileName) + "\r\n";
            response += "Content-Length: " + std::to_string(c->fileSize) + "\r\n\r\n";
        }

        if (send(c->get_socket_fd(), response.c_str(), response.size(), 0) < 1)
        {
            std::cout << "Error sending response header" << std::endl;
            c->is_finished = true;
            c->file.close();
            return;
        }

        c->header_sent = true;
    }
    else
    {
        const int buff_size = 1024;
        char buffer[buff_size];

        c->file.read(buffer, buff_size);
        int bytes_read = c->file.gcount();

        if (bytes_read == -1)
        {
            std::cout << "Error reading file" << std::endl;
            c->is_finished = true;
            c->file.close();
            return;
        }

        if (bytes_read < buff_size)
        {
            if (send(c->get_socket_fd(), buffer, bytes_read, 0) == -1)
                std::cout << "error sending response2" << std::endl;
            c->is_finished = true;
            c->file.close();
            return;
        }

        if (send(c->get_socket_fd(), buffer, bytes_read, 0) < 1)
        {
            std::cout << "Error sending response" << std::endl;
            c->file.close();
            return;
        }

        c->fileSize -= bytes_read;
    }
}
