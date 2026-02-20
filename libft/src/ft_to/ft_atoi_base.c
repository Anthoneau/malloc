/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi_base.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agoldber <agoldber@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/31 13:25:25 by agoldber          #+#    #+#             */
/*   Updated: 2024/06/04 18:33:26 by agoldber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	chartoi(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	else if (c >= 'A' && c <= 'F')
		return (c - 'A' + 10);
	else if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);
	else
		return (-1);
}

int	ft_atoi_base(const char *s)
{
	long	res;
	int		val;

	val = 0;
	res = 0;
	while (*s && *s != 'x')
		s++;
	if (!s)
		return (0);
	s++;
	while (*s && ft_isalnum(*s))
	{
		val = chartoi(*s);
		if (val == -1)
			return (0);
		res = res * 16 + val;
		s++;
	}
	return (res);
}
