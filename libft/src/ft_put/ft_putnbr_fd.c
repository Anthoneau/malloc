/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agoldber <agoldber@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 15:25:30 by agoldber          #+#    #+#             */
/*   Updated: 2024/04/23 14:33:11 by agoldber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putnbr_fd(int n, int fd)
{
	long	a;

	a = n;
	if (a == -2147483648)
	{
		ft_putstr_fd("-214748364", fd);
		a = 8;
	}
	if (a < 0)
	{
		a *= -1;
		ft_putchar_fd('-', fd);
	}
	if (a >= 10)
	{
		ft_putnbr_fd(a / 10, fd);
		a %= 10;
		ft_putchar_fd((a + '0'), fd);
	}
	else if (a <= 9)
		ft_putchar_fd((a + '0'), fd);
}
