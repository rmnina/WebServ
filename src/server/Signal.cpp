/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Signal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 00:46:43 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/22 02:56:32 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Signal.hpp"

void handler(int signum)
{
	if (signum == SIGINT)
		g_sig = SIGINT;
}

void initSignal()
{
	signal(SIGINT, handler);
}