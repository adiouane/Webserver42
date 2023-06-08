/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiouane <adiouane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/11 05:40:43 by adiouane          #+#    #+#             */
/*   Updated: 2023/04/11 05:40:43 by adiouane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.hpp"

Response::Response()
{
}

void Response::handle_response(Client *c)
{
    std::string method = c->req.get_method();
    Response res;
    if (method.empty())
        return;
    if (method == "GET")
    {
        if (c->is_valid == true)
        {
            res.send_Get_response("", c);
            return;
        }
        else
        {
            res.is_valid_config(c);
            return;
        }
    }
    else if (method == "POST")
    {
        if (c->is_valid == true && c->req.is_finished == true)
        {
            std::string Response = "HTTP/1.0 200 OK\r\n\r\n";
            send(c->get_socket_fd(), Response.c_str(), Response.length(), 0);
            c->is_finished = true;
            return;
        }
    }
    else if (method == "DELETE")
    {
        if (c->is_valid == true)
        {
            res.handle_delete(c);
            return;
        } else {
            res.is_valid_config(c);
            return;
        }
    }
    else {
        sent_error(c->get_socket_fd(), 501);
        c->is_finished = true;
    }
}

Response::~Response()
{
}
