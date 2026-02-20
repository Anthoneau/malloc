/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agoldber <agoldber@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 19:21:28 by agoldber          #+#    #+#             */
/*   Updated: 2024/04/23 14:33:38 by agoldber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>

size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	size_t	dlen;
	size_t	slen;
	size_t	i;
	size_t	j;

	if ((!dst || !src) && dstsize == 0)
		return (0);
	slen = ft_strlen(src);
	dlen = ft_strlen(dst);
	j = dlen;
	i = 0;
	if (dlen < dstsize && dstsize > 0)
	{
		while (src[i] && dlen + i < dstsize - 1)
		{
			dst[j] = src[i];
			j++;
			i++;
		}
		dst[j] = '\0';
	}
	if (dlen >= dstsize)
		dlen = dstsize;
	return (dlen + slen);
}
