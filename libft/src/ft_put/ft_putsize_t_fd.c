/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putsize_t_fd.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agoldber <agoldber@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 19:40:30 by agoldber          #+#    #+#             */
/*   Updated: 2026/09/17 19:42:10 by agoldber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putsize_t_fd(size_t n, int fd)
{
	if (n >= 10)
	{
		ft_putnbr_fd(n / 10, fd);
		n %= 10;
		ft_putchar_fd((n + '0'), fd);
	}
	else if (n <= 9)
		ft_putchar_fd((n + '0'), fd);
}
