

#include "minishell.h"
t_minishell	g_minishell; // Объявление глобальной переменной структуры t_minishell, которая будет использоваться для хранения состояния минишелла

static void	ft_init_minishell(char **env) // Функция инициализации минишелла, принимает переменные окружения
{
	ft_memset(&g_minishell, 0, sizeof(t_minishell)); // Обнуляем все поля структуры g_minishell
	g_minishell.environ = env; // Сохраняем переменные окружения в структуру g_minishell
	ft_init_envlst(); // Инициализируем список переменных окружения
	g_minishell.stdin = dup(0); // Дублируем дескриптор стандартного ввода
	g_minishell.stdout = dup(1); // Дублируем дескриптор стандартного вывода
	tcgetattr(STDIN_FILENO, &g_minishell.original_term); // Сохраняем текущие настройки терминала в структуру
}

static void	ft_start_execution(void) // Функция для начала выполнения команд
{
	signal(SIGQUIT, ft_sigquit_handler); // Устанавливаем обработчик сигнала SIGQUIT
	ft_init_tree(g_minishell.ast); // Инициализируем абстрактное синтаксическое дерево (AST)
	if (g_minishell.heredoc_sigint) // Если был прерван heredoc сигналом SIGINT
	{
		ft_clear_ast(&g_minishell.ast); // Очищаем AST
		g_minishell.heredoc_sigint = false; // Сбрасываем флаг
	}
	tcsetattr(STDIN_FILENO, TCSANOW, &g_minishell.original_term); // Восстанавливаем исходные настройки терминала
	g_minishell.exit_s = ft_exec_node(g_minishell.ast, false); // Выполняем корневой узел AST и сохраняем код завершения
	ft_clear_ast(&g_minishell.ast); // Очищаем AST после выполнения команды
}

int	main(int argc, char **argv, char **env) // Главная функция программы
{
	((void)argc, (void)argv); // Игнорируем аргументы командной строки (argc и argv)
	ft_init_minishell(env); // Инициализируем минишелл с переменными окружения
	while (1) // Главный цикл программы
	{
		ft_init_signals(); // Устанавливаем обработчики сигналов
		g_minishell.line = readline(PROMPT); // Считываем строку с команды пользователя с помощью readline
		if (!g_minishell.line) // Если строка пуста (например, нажата клавиша Ctrl+D)
			(ft_clean_ms(), // Очищаем минишелл
				ft_putstr_fd("exit\n", 1), exit(g_minishell.exit_s)); // Выводим сообщение "exit" и выходим с кодом завершения
		if (g_minishell.line[0]) // Если введенная строка не пуста
			add_history(g_minishell.line); // Добавляем введенную строку в историю команд
		g_minishell.tokens = ft_tokenize(); // Разбиваем строку на токены
		if (!g_minishell.tokens) // Если токенизация не удалась
			continue ; // Переходим к следующей итерации цикла
		g_minishell.ast = ft_parse(); // Парсим токены и строим абстрактное синтаксическое дерево (AST)
		if (g_minishell.parse_err.type) // Если возникла ошибка при парсинге
		{
			ft_handle_parse_err(); // Обрабатываем ошибку парсинга
			continue ; // Переходим к следующей итерации цикла
		}
		ft_start_execution(); // Начинаем выполнение команды
	}
	ft_garbage_collector(NULL, true); // Очищаем память с помощью сборщика мусора
	return (ft_clean_ms(), g_minishell.exit_s); // Очищаем минишелл и возвращаем код завершения программы
}
