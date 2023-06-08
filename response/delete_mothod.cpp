/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   delete_mothod.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiouane <adiouane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/25 18:32:00 by adiouane          #+#    #+#             */
/*   Updated: 2023/04/25 18:32:00 by adiouane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../response.hpp"

void Response::handle_delete(Client* c)
{
    std::string location = c->req.get_location();
    std::string response;
    int status_code = 0;


    // Remove leading slash from the location if present
    if (location[0] == '/' && location.length() > 1)
        location.erase(0, 1);

    if (location.length() == 1 && location[0] == '/')
    {
        response = "HTTP/1.1 403 Forbidden\r\nContent-Length: 0\r\n\r\n";
        status_code = 403; // Forbidden
        send(c->get_socket_fd(), response.c_str(), response.length(), 0);
        c->is_finished = true;
        return;
    }

    if (access(c->_fileName.c_str(), F_OK) == -1) // File not found
    {
        // The file does not exist
        std::cout << "File does not exist" << std::endl;
        status_code = 404; // Not Found
        response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
    }
    else
    {
        // Check write permission on the file
        if (access(c->_fileName.c_str(), W_OK) == -1)
        {
            status_code = 403; // Forbidden
            response = "HTTP/1.1 403 Forbidden\r\nContent-Length: 0\r\n\r\n";
            send(c->get_socket_fd(), response.c_str(), response.length(), 0);
            c->is_finished = true;
            return;
        }

        if (remove(c->_fileName.c_str()) != 0)
        {
            std::cout << "Error occurred while deleting the file" << std::endl;
            status_code = 500; // Internal Server Error
            response = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n";
        }
        else
        {
            status_code = 204; // No Content
            response = "HTTP/1.1 204 No Content\r\nContent-Length: 0\r\n\r\n";
        }
    }

    // Send the response
    send(c->get_socket_fd(), response.c_str(), response.length(), 0);
    c->is_finished = true;
    return;
}



/* 
    The curl command is a tool used to transfer data from or to a server, using various network protocols. Here is a breakdown of the curl command you provided:
    curl: starts the curl command-line tool.
    -v: verbose mode, which displays additional information such as headers and status codes.
    -X: specifies the HTTP method to be used for the request. In this case, it's the DELETE method.
    DELETE: the HTTP method to be used for the request.
    http://localhost:8080/root/tst: the URL of the server and the resource path to be deleted.
    Here, localhost refers to the local machine, 8080 is the port number to connect to,
    and /root/tst is the path of the resource to be deleted.
    When you run this command, curl will send an HTTP DELETE request to the server running on localhost at port 8080,
    with the resource path /root/tst. If the request is successful,
    the server will delete the resource and send an HTTP 200 OK response.
    If the request fails, the server will send an appropriate error response.
    The -v option is used to display additional information about the request and response in the terminal.

    curl -v -X DELETE http://localhost:8080/root/tst
*/