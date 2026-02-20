/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agoldber <agoldber@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/22 15:08:15 by agoldber          #+#    #+#             */
/*   Updated: 2024/05/17 16:05:24 by agoldber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	arg_manager(char c, va_list args)
{
	if (c == '%')
		return (ft_putchar_mod('%'));
	else if (c == 'c')
		return (ft_putchar_mod(va_arg(args, int)));
	else if (c == 's')
		return (ft_putstr_mod(va_arg(args, char *)));
	else if (c == 'd' || c == 'i')
		return (ft_putnbr_mod(va_arg(args, int)));
	else if (c == 'u')
		return (ft_putunbr_mod(va_arg(args, unsigned int)));
	else if (c == 'p')
		return (display_address(va_arg(args, unsigned long)));
	else if (c == 'x' || c == 'X')
		return (display_hexa(va_arg(args, int), c));
	return (-1);
}

int	ft_printf(const char *format, ...)
{
	int		i;
	int		count;
	int		verif;
	va_list	args;

	va_start(args, format);
	i = 0;
	count = 0;
	while (format[i])
	{
		verif = count;
		if (format[i] == '%')
			count += arg_manager(format[++i], args);
		else
			count += ft_putchar_mod(format[i]);
		if (verif > count)
			return (-1);
		i++;
	}
	va_end(args);
	return (count);
}
