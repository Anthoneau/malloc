/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hexa_transfo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agoldber <agoldber@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 16:17:36 by agoldber          #+#    #+#             */
/*   Updated: 2024/05/17 16:05:26 by agoldber         ###   ########.fr       */
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

char	*hexa_transfo(unsigned long ptr_val)
{
	unsigned long	temp;
	char			*arr;
	int				i;

	i = count_hexa(ptr_val);
	arr = malloc((i + 1) * sizeof(char));
	if (!arr)
		return (NULL);
	arr[i] = '\0';
	i--;
	while (i >= 0)
	{
		temp = ptr_val % 16;
		if (temp > 9)
			temp = 'a' + (temp - 10);
		else
			temp = '0' + temp;
		arr[i] = (char)temp;
		ptr_val /= 16;
		i--;
	}
	return (arr);
}
