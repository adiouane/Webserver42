/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response_handler.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adiouane <adiouane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/15 07:05:42 by adiouane          #+#    #+#             */
/*   Updated: 2023/04/15 07:05:42 by adiouane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../response.hpp"

bool Response::is_valid_config(Client *c)
{
    Response res;
    if (res.is_request_in_location(c) == 1)
    {
        c->is_valid = true;
        return 1;
    }
    else
    {
        send_Get_response("404:", c);
        c->is_finished = true;
        return 1;
    }
    return 0;
}
