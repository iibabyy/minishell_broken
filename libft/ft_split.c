/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madamou <madamou@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 15:06:54 by marvin            #+#    #+#             */
/*   Updated: 2024/12/16 15:01:08 by madamou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int			is_in_charset(char c, char *set);

static int	ft_count_word(char const *s, char *c)
{
	int	i;
	int	cpt;

	cpt = 0;
	i = 0;
	while (s[i])
	{
		while (s[i] && is_in_charset(s[i], c))
			i++;
		while (s[i] && !is_in_charset(s[i], c))
			i++;
		cpt++;
	}
	if (i == 0)
		return (0);
	if (is_in_charset(s[i - 1], c))
		cpt--;
	return (cpt);
}

static char	*ft_norminette(char const *s, int i, int j)
{
	int		k;
	char	*str;

	k = 0;
	str = ft_malloc(sizeof(char) * (j + 1));
	if (str == NULL)
		return (NULL);
	while (k < j)
	{
		str[k] = s[i - j + k];
		k++;
	}
	str[k] = '\0';
	return (str);
}

int	ft_free_split1(char **split, int index)
{
	if (split[index] == NULL)
	{
		while (index >= 0)
			ft_free(split[index--]);
		return (0);
	}
	return (1);
}

static int	ft_split_words(char **split, char const *s, char *c)
{
	int	i;
	int	j;
	int	index;

	i = 0;
	index = 0;
	while (s[i])
	{
		while (is_in_charset(s[i], c) && s[i])
			i++;
		j = 0;
		while (!is_in_charset(s[i], c) && s[i])
		{
			i++;
			j++;
		}
		if (!is_in_charset(s[i - 1], c))
		{
			split[index] = ft_norminette(s, i, j);
			if (ft_free_split1(split, index++) == 0)
				return (0);
		}
	}
	split[index] = 0;
	return (1);
}

char	**ft_split(char *s, char *c)
{
	char	**split;

	if (!s)
	{
		split = ft_malloc(sizeof(char *));
		if (!split)
			handle_malloc_error("split");
		split[0] = NULL;
		return (split);
	}
	split = ft_malloc(sizeof(char *) * (ft_count_word(s, c) + 1));
	if (split == NULL)
		handle_malloc_error("split");
	if (ft_split_words(split, s, c) == 0)
		handle_malloc_error("split");
	return (split);
}

// int	main(void)
// {
// 	char **test;
// 	int i;

// 	i = 0;
// 	test = ft_split("KEY=VALUE", "=");
// 	if (test == NULL)
// 	{
// 		printf("tout est null\n");
// 		return (0);
// 	}
// 	char *oui = test[0];
// 	char *non = test[1];
// 	if (test[i] == NULL)
// 		printf("%u", 42);
// 	ft_free(test);
// 	printf("oui == %s\n", oui);
// 	printf("non == %s\n", non);
// 	ft_free(oui);
// 	ft_free(non);
// }
