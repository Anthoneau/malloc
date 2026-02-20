/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agoldber <agoldber@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/08 14:07:52 by agoldber          #+#    #+#             */
/*   Updated: 2024/04/23 14:33:54 by agoldber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	if (n == 0)
		return (0);
	while ((s1[i] == s2[i] && i < n - 1) && s1[i] != '\0')
		i++;
	if ((s1[i] == '\0' && s2[i] != '\0'))
		return (-1);
	else if ((s2[i] == '\0' && s1[i] != '\0'))
		return (1);
	else
		return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}
