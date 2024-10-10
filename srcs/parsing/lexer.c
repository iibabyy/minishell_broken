/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madamou <madamou@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 00:49:14 by madamou           #+#    #+#             */
/*   Updated: 2024/10/10 17:19:30 by madamou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/includes.h"
#include "parsing.h"

int is_redirection_type(t_type type)
{
	if (type == HEREDOC || type == INFILE || type == OUT_APP
		|| type == OUT_TRUNC)
		return (1);
	return (0);
}

void set_token_type(t_token *current, t_type type)
{
	t_token *tmp;

	if (type != CMD)
	{
		current->type = type;
		return ;
	}
	if (current->prev && is_redirection_type(current->prev->type))
		current->type = FILES;
	else if (type == CMD)
	{
		tmp = current->prev;
		while (tmp && is_operator_type(tmp->type) == false)
		{
			if (tmp->type == CMD)
			{
				current->type = ARG;
				break;
			}
			current->type = ARG;
			tmp = tmp->prev;
		}
	}
	if (type == CMD && ft_strchr(current->content, '='))
		current->type = LOCAL_VAR;
}

void add_to_queue(t_queue *queue, char *content, t_type type)
{
	t_token	*token;
	t_token	*current;

	token = ft_malloc(sizeof(t_token) * 1);
	if (token == NULL){
		// handle_malloc_error("add to queue");
	}
	ft_memset(token, 0, sizeof(t_token));
	if (queue->first == NULL)
		queue->first = token;
	token->prev = queue->last;
	if (queue->last)
		queue->last->next = token;
	queue->last = token;
	token->content = content;
	set_token_type(token, type);
	current = queue->first;
	while (current->next)
		current = current->next;
	current->next = token;
	token->next = NULL;
}

void add_queue_subshell(t_queue *queue, char *str, int *i)
{
	int	j;
	int	open_parenthesis;
	int	close_parenthesis;
	char	*cmd;

	j = 0;
	open_parenthesis = 1;
	close_parenthesis = 0;
	while (str[*i + ++j])
	{
		if (str[*i + j] == '(')
			open_parenthesis++;
		if (str[*i + j] == '"')
			continue_until_find(i, &j, str, '"');
		if (str[*i + j] == '\'')
			continue_until_find(i, &j, str, '\'');
		if (str[*i + j] == ')')
			close_parenthesis++;
		if (open_parenthesis - close_parenthesis == 0)
			break ;
	}
	if (!str[*i + j] && open_parenthesis - close_parenthesis != 0)
	{
		(*i += j);
		return;
	}
	cmd = ft_substr(str, *i + 1, j - 1);
	if (!cmd)
		handle_malloc_error("parenthesis");
	add_to_queue(queue, cmd, SUBSHELL);
	*i += j + 1;
}

int add_token(t_queue *queue, char *str, int *i)
{
	int j;
	char	*cmd;

	j = 0;
	while (str[*i + j] && !is_space(str[*i + j]) && !is_a_separator(str[*i + j]))
	{
		if (str[*i + j] == '"')
			continue_until_find(i, &j, str, '"');
		else if (str[*i + j] == '\'')
			continue_until_find(i, &j, str, '\'');
		else if (str[*i + j] == '{')
			continue_until_find(i, &j, str, '}');
		j++;
	}
	cmd = ft_substr(str, *i, j);
	if (!cmd)
		handle_malloc_error("add token");
	if (!cmd[0] && !is_a_quotes(str[*i + j - 1]))
		return (*i += j, 1);
	add_to_queue(queue, cmd, CMD);
	*i += j;
	return (1);
}

int	assigne_type(char *redirection)
{
	if (!ft_strcmp(redirection, "|"))
		return (PIPE);
	if (!ft_strcmp(redirection, "||"))
		return (OR);
	if (!ft_strcmp(redirection, ">"))
		return (OUT_TRUNC);
	if (!ft_strcmp(redirection, ">>"))
		return (OUT_APP);
	if (!ft_strcmp(redirection, "<"))
		return (INFILE);
	if (!ft_strcmp(redirection, "<<"))
		return (HEREDOC);
	if (!ft_strcmp(redirection, "&&"))
		return (AND);
	if (!ft_strcmp(redirection, ";") || !ft_strcmp(redirection, "\n"))
		return (LIST);
	return (U_TOKEN);
}

void add_metachars(t_queue *queue, char *str, int *i)
{
	char	*redirection;
	char	symbol;
	int		type;
	int		j;

	symbol = str[*i];
	j = 0;
	while (str[*i + j] == symbol)
		j++;
	redirection = ft_malloc(sizeof(char) * (j + 1));
	if (!redirection)
		handle_malloc_error("redirections");
	j = -1;
	while (str[*i + ++j] == symbol)
		redirection[j] = str[*i + j];
	redirection[j] = '\0';
	type = assigne_type(redirection);
	add_to_queue(queue, redirection, type);
	*i += j;
}

void	add_elem(t_queue *queue, char *str, int *i)
{
	if (str[*i] == '(')
	{
		add_queue_subshell(queue, str, i);
	}
	else if (is_a_separator(str[*i]))
		add_metachars(queue, str, i);
	else
		add_token(queue, str, i);
}

t_token *lexer(t_data *data, char *command_line)
{
	int			i;
	char		*tmp;
	t_queue	queue;

	i = 0;
	queue.first = NULL;
	queue.last = NULL;
	tmp = ft_strdup(command_line);
	command_line = check_if_command_line_is_good(data, command_line);
	if (!command_line)
		return (add_history(tmp), NULL);
	ft_free(tmp);
	add_history(command_line);
	while (command_line[i])
	{
		skip_white_space(command_line, &i);
		if (command_line[i])
			add_elem(&queue, command_line, &i);
	}
	return (queue.first);
}