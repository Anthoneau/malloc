/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putaddress_fd.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agoldber <agoldber@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/04 00:47:29 by agoldber          #+#    #+#             */
/*   Updated: 2026/04/04 00:47:29 by agoldber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	ft_putnbr_hexa_fd(unsigned long n, int fd)
{
	if (n >= 16)
	{
		ft_putnbr_hexa_fd(n / 16, fd);
		n %= 16;
		if (n > 9)
			ft_putchar_fd((n - 10) + 'a', fd);
		else
			ft_putchar_fd((n + '0'), fd);
	}
	else {
		if (n > 9)
			ft_putchar_fd((n - 10) + 'a', fd);
		else
			ft_putchar_fd((n + '0'), fd);
	}
}

void	ft_putaddress_fd(unsigned int long ptr_val)
{
	ft_putstr_fd("0x", 1);
	ft_putnbr_hexa_fd(ptr_val, 1);
}
