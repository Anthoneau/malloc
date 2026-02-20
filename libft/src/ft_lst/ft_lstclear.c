/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agoldber <agoldber@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 14:54:07 by agoldber          #+#    #+#             */
/*   Updated: 2024/04/23 14:32:18 by agoldber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*actual;
	t_list	*temp;

	if (!lst || !del)
		return ;
	actual = *lst;
	while (actual)
	{
		temp = actual;
		actual = actual->next;
		ft_lstdelone(temp, del);
	}
	*lst = NULL;
}
