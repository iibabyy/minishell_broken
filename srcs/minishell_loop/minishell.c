/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madamou <madamou@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 00:18:38 by madamou           #+#    #+#             */
/*   Updated: 2024/10/10 18:11:46 by madamou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/includes.h"
#include <readline/readline.h>

char	*get_prompt(t_data *data)
{
	char	*pwd;
	char	*hostname;
	char	*prompt;

	if (data->env && ft_getenv("PWD") && ft_getenv("USER"))
	{
		pwd = ft_getenv("PWD");
		hostname = ft_getenv("USER");
		if (data->status == 0)
			prompt = ft_sprintf("%s%s:\001\033[0;32m\002%s\001\033[0m\002$ ",
					CLR, hostname, pwd);
		else
			prompt = ft_sprintf("%s%s:\001\033[0;32m\002%s\001\033%s ", CLR,
					hostname, pwd, ND);
		return (prompt);
	}
	return (ft_sprintf("minishell> "));
}

char	*ft_readline(t_data *data)
{
	char	*prompt;
	char	*command_line;

	while (1)
	{
		prompt = get_prompt(data);
		command_line = readline(prompt);
		ft_free(prompt);
		if (g_signal != 0)
		{
			set_status_if_signal(data);
			continue ;
		}
		if (!command_line)
			ft_exit(NULL);
		else if (ft_strcmp(command_line, "") == 0)
			ft_free(command_line);
		else
			break ;
	}
	prompt = ft_strdup(command_line);
	return (ft_free(command_line), prompt);
}

void	print_AST(t_token *command);
t_token	*ast_top(t_token *current);
void	print_AST_test(t_token *command);

void loop_minishell(t_data *data)
{
	char *command_line;
	t_queue queue;
	

	while (true)
	{
		// set_signal_parent();
		// clear_garbage();
		command_line = ft_readline(data);
		queue.first = lexer(data, command_line);
		if (parser(&queue) == EXIT_FAILURE)
			continue;
		// queue.first = create_ast(queue.first, -10);
		queue.first = create_ast_test(queue.first);
		ft_free(command_line);
		// print_AST(queue.first);
		print_AST_test(queue.first);
	}
}