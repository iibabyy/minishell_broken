/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   before_parsing.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madamou <madamou@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/12 23:26:16 by madamou           #+#    #+#             */
/*   Updated: 2024/10/12 20:52:42 by madamou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/includes.h"
#include "parsing.h"
#include <readline/chardefs.h>
#include <unistd.h>

char	*get_open_input(char *str, char *prompt, char *flag)
{
	char	*new_line;
	t_data	*data;

	data = get_data(NULL, GET);
	new_line = readline(prompt);
	if (!new_line)
	{
		if (ft_strcmp(flag, NEWLINE1) == 0)
		{
		if (ft_strcmp(prompt, "quote> ") == 0)
			ft_putendl_fd(S_U_EOF, 2);
		else
			ft_putendl_fd(D_U_EOF, 2);
		}
		ft_fprintf(STDERR_FILENO,
			"%s: syntax error: unexpected end of file\n", data->name);
		if (ft_strcmp(flag, SEMICOLON) == 0 || ft_strcmp(flag, SPACE1) == 0)
			(ft_fprintf(2, "exit\n"), free_and_exit(data->status));
		return (data->status = 2, NULL);
	}
	str = ft_re_strjoin(str, flag);
	str = ft_re_strjoin(str, new_line);
	return (free(new_line), str);
}

char	*check_if_dquote_close(char *str, int *i)
{
	(*i)++;
	while (str[*i] && str[*i] != '"')
		(*i)++;
	if (str[*i] == '\0')
	{
		str = get_open_input(str, "dquote> ", NEWLINE1);
		*i = -1;
	}
	return (str);
}

char	*check_if_quote_close(char *str, int *i)
{
	(*i)++;
	while (str[*i] && str[*i] != '\'')
		(*i)++;
	if (str[*i] == '\0')
	{
		str = get_open_input(str, "quote> ", NEWLINE1);
		*i = -1;
	}
	return (str);
}

char	*check_last_token(char *str, int *i)
{
	char **split;
	int len;

	split = ft_split(str, " \t");
	if (split[0] == NULL)
		return (*i = 1, str);
	len  = ft_strlen_2d(split);
	if (ft_strcmp(split[len - 1], "|") == 0 && ft_strcmp(split[0], "|") != 0)
	{
		str = get_open_input(str, "pipe> ", SPACE1);
		*i = 0;
	}
	if (ft_strcmp(split[len - 1], "||") == 0 && ft_strcmp(split[0], "||") != 0)
	{
		str = get_open_input(str, "or> ", SPACE1);
		*i = 0;
	}
	if (ft_strcmp(split[len - 1], "&&") == 0 && ft_strcmp(split[0], "&&") != 0)
	{
		str = get_open_input(str, "and> ", SPACE1);
		*i = 0;
	}
	return (str);
}

char	*check_if_paranthesis_close(char *str, int i)
{
	int	open_parenthesis;
	int	close_parenthesis;

	open_parenthesis = 0;
	close_parenthesis = 0;
	while (str[++i])
	{
		if (str[i] == '"')
			continue_until_find(0, &i, str, '"');
		if (str[i] == '\'')
			continue_until_find(0, &i, str, '\'');
		if (str[i] == '(')
			open_parenthesis++;
		if (str[i] == ')')
			close_parenthesis++;
	}
	if (open_parenthesis < close_parenthesis)
		return (NULL);
	if (open_parenthesis == close_parenthesis)
		return (str);
	str = get_open_input(str, "subshell> ", SEMICOLON);
	return (check_if_paranthesis_close(str, -1));
}

char	*check_if_command_line_is_good(t_data *data, char *str)
{
	int		i;

	i = 0;
	while (i == 0)
	{
		while (g_signal == 0 && str[i])
		{
			if (g_signal == 0 && str[i] == '"')
				str = check_if_dquote_close(str, &i);
			else if (g_signal == 0 && str[i] == '\'')
				str = check_if_quote_close(str, &i);
			i++;
		}
		if (!str[i])
			str = check_last_token(str, &i);
	}
	if (g_signal != 0)
		return (set_status_if_signal(data), NULL);
	return (check_if_paranthesis_close(str, -1));
}