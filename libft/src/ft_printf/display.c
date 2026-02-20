/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agoldber <agoldber@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/27 21:29:46 by agoldber          #+#    #+#             */
/*   Updated: 2024/05/17 16:05:22 by agoldber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	*ft_toupper_arr(char *s)
{
	int	i;

	i = 0;
	while (s[i])
	{
		if (s[i] >= 'a' && s[i] <= 'f')
			s[i] -= 32;
		i++;
	}
	return (s);
}

int	display_address(unsigned long ptr_val)
{
	int		count;
	char	*hexa;

	hexa = hexa_transfo(ptr_val);
	count = ft_putstr_mod("0x");
	count += ft_putstr_mod(hexa);
	free(hexa);
	return (count);
}

int	display_hexa(int n, char c)
{
	int		count;
	char	*hexa;

	count = 0;
	hexa = hexa_transfo((unsigned int)n);
	if (c == 'x')
		count += ft_putstr_mod(hexa);
	else if (c == 'X')
		count += ft_putstr_mod(ft_toupper_arr(hexa));
	free(hexa);
	return (count);
}
