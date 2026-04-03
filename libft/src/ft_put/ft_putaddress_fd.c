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

static int	count_hexa(unsigned long ptr_val)
{
	int	count;

	if (ptr_val == 0)
		return (1);
	count = 0;
	while (ptr_val > 0)
	{
		ptr_val /= 16;
		count++;
	}
	return (count);
}

void	ft_putaddress_fd(unsigned int long ptr_val)
{
	unsigned long	temp;
	int				i;
	
	i = count_hexa(ptr_val);
	i--;
	ft_putstr_fd("0x", 1);
	while (i >= 0)
	{
		temp = ptr_val % 16;
		if (temp > 9)
			ft_putchar_fd('a' + (temp - 10), 1);
		else
			ft_putchar_fd('0' + temp, 1);
		ptr_val /= 16;
		i--;
	}
}
