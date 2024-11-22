/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Signal.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdufour <jdufour@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 00:45:59 by jdufour           #+#    #+#             */
/*   Updated: 2024/11/22 00:46:29 by jdufour          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNAL_HPP
# define SIGNAL_HPP

# include <csignal>
# include <iostream>

extern volatile int	g_sig;

void	handler(int signum);
void	initSignal();

#endif