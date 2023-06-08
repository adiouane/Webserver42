/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   post_method.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiouane <adiouane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/14 13:58:28 by adiouane          #+#    #+#             */
/*   Updated: 2023/06/03 12:14:37 by adiouane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../response.hpp"


int Response::Handle_Post_Error(Client *c)
{
    Response r;
    std::string ex;

    int index = check_which_server(c);
    int l =  r.is_locations_matched(c->req.get_location(), index, c);
    if (index == -1)
        index = 0;
    c->req.server_index = index;
    c->req.location_index = l;
    ex = c->req.get_location().substr(c->req.get_location().find_last_of(".") + 1);
    if (config->servers[index].locations[l].upload_path.empty())
    {
        r.send_Get_response("404:", c);
        c->is_finished = true;
        return 0;
    }
    if (r.check_is_dir(config->servers[index].locations[l].upload_path) == 0)
    {
        r.send_Get_response("404:", c);
        c->is_finished = true;
        return 0;
    }
    if (r.is_allowed_method(c->req.get_method(), config->servers[index].locations[l].methods) == 0)
    {
        r.sent_error(c->get_socket_fd(), 405); // not allowed
        c->is_finished = true;
        return 0;
    }
    if (config->servers[index].locations[l].cgi_path[ex] != "")
    {
        c->_cgi_path = c->req.get_location();
        r.Handle_cgi_response(c, c->_cgi_path);
        c->is_finished = true;
        return (1);
    }
    c->is_valid = true;
    return 1;
}
