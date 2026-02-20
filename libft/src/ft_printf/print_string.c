/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_string.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agoldber <agoldber@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 16:10:52 by agoldber          #+#    #+#             */
/*   Updated: 2024/05/17 16:05:28 by agoldber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_putchar_mod(char c)
{
	if (write(1, &c, 1) == -1)
		return (-1);
	return (1);
}

int	print_null(void)
{
	int	count;

	count = ft_putstr_mod("(null)");
	return (count);
}

int	ft_putstr_mod(char *s)
{
	int	i;

	i = 0;
	if (!s)
		return (print_null());
	while (s[i])
	{
		if (ft_putchar_mod(s[i]) == -1)
			return (-1);
		i++;
	}
	return (i);
}

int	ft_putnbr_mod(int n)
{
	long	a;
	int		count;

	a = n;
	count = 0;
	if (a < 0)
	{
		a *= -1;
		ft_putchar_mod('-');
		count++;
	}
	if (a >= 10)
	{
		count += ft_putnbr_mod(a / 10);
		a %= 10;
		ft_putchar_mod(a + '0');
	}
	else if (a <= 9)
		ft_putchar_mod((a + '0'));
	count++;
	return (count);
}

int	ft_putunbr_mod(unsigned int n)
{
	unsigned long	a;
	int				count;

	a = n;
	count = 0;
	if (a >= 10)
	{
		count += ft_putnbr_mod(a / 10);
		a %= 10;
		ft_putchar_mod((a + '0'));
	}
	else if (a <= 9)
		ft_putchar_mod((a + '0'));
	count++;
	return (count);
}
