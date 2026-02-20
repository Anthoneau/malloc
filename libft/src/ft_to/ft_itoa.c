/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agoldber <agoldber@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 11:24:03 by agoldber          #+#    #+#             */
/*   Updated: 2024/04/23 14:31:58 by agoldber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	count_char(long a)
{
	long	i;
	int		count;

	i = 1;
	count = 0;
	if (a < 0)
	{
		a *= -1;
		count++;
	}
	if (a == 0)
		count++;
	while (i <= a)
	{
		i *= 10;
		count++;
	}
	return (count);
}

char	*ft_itoa(int n)
{
	char	*num;
	int		i;
	long	a;

	a = n;
	i = count_char(a);
	num = malloc((i + 1) * sizeof(char));
	if (!num)
		return (NULL);
	if (a < 0)
	{
		a *= -1;
		num[0] = '-';
	}
	num[i] = '\0';
	i--;
	while (a >= 10)
	{
		num[i] = (a % 10) + '0';
		a /= 10;
		i--;
	}
	if (a <= 9)
		num[i] = a + '0';
	return (num);
}
