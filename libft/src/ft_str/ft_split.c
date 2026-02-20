/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agoldber <agoldber@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 15:29:39 by agoldber          #+#    #+#             */
/*   Updated: 2025/01/27 14:23:48 by agoldber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	count_word(char const *s, char c)
{
	int	sep;
	int	count;

	if (!s)
		return (0);
	count = 0;
	sep = 1;
	while (*s)
	{
		if (*s == c)
			sep = 1;
		else if (sep)
		{
			sep = 0;
			count++;
		}
		s++;
	}
	return (count);
}

static char	*word_split(char const *s, int *start, int *finish)
{
	int		i;
	int		j;
	int		k;
	char	*word;

	i = 0;
	j = *start;
	k = *finish;
	word = malloc((k - j + 1) * sizeof(char));
	if (!word)
	{
		free(word);
		return (NULL);
	}
	while (j < k)
		word[i++] = s[j++];
	word[i] = '\0';
	return (word);
}

static void	init_start_finish(char const *s, char c, int *start, int *finish)
{
	int	end;

	end = *finish;
	while (s[end] == c)
		end++;
	*start = end;
	while (s[end] && s[end] != c)
		end++;
	*finish = end;
}

char	**ft_split(char const *s, char c)
{
	char	**arr;
	int		start;
	int		finish;
	int		j;

	arr = malloc((count_word(s, c) + 1) * sizeof(char *));
	if (!arr)
		return (NULL);
	finish = 0;
	start = 0;
	j = 0;
	while (s[finish])
	{
		init_start_finish(s, c, &start, &finish);
		if (start < finish)
		{
			arr[j] = word_split(s, &start, &finish);
			if (!(arr[j]))
				return (free_array(arr), NULL);
			j++;
		}
	}
	arr[j] = NULL;
	return (arr);
}
