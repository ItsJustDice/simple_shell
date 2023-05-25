#include "shell.h"

/**
 * get_history_file - gets the history file
 * @data: parameter struct
 *
 * Return: allocated string containg history file
 */

char *get_history_file(info_t *data)
{
	char *buffed, *dir;

	dir = findEnv(data, "HOME=");
	if (!dir)
		return (NULL);
	buffed = malloc(sizeof(char) * (_strlen(dir) + _strlen(HIST_FILE) + 2));
	if (!buffed)
		return (NULL);
	buffed[0] = 0;
	copyString (buffed, dir);
	_strcat(buffed, "/");
	_strcat(buffed, HIST_FILE);
	return (buffed);
}

/**
 * write_history - creates a file, or appends to an existing file
 * @data: the parameter struct
 *
 * Return: 1 on success, else -1
 */
int write_history(info_t *data)
{
	ssize_t fd;
	char *filename = get_history_file(data);
	list_t *list = NULL;

	if (!filename)
		return (-1);

	fd = open(filename, O_CREAT | O_TRUNC | O_RDWR, 0644);
	free(filename);
	if (fd == -1)
		return (-1);
	for (list = data->history; list; list = list->next)
	{
		_putsfd(list->txt, fd);
		_putfd('\n', fd);
	}
	_putfd(BUFFER_FLUSHER, fd);
	close(fd);
	return (1);
}

/**
 * read_history - reads history from file
 * @data: the parameter struct
 *
 * Return: histcount on success, 0 otherwise
 */
int read_history(info_t *data)
{
	int a, last = 0, linecount = 0;
	ssize_t fd, rdlen, fsize = 0;
	struct stat st;
	char *buffed = NULL, *filename = get_history_file(data);

	if (!filename)
		return (0);

	fd = open(filename, O_RDONLY);
	free(filename);
	if (fd == -1)
		return (0);
	if (!fstat(fd, &st))
		fsize = st.st_size;
	if (fsize < 2)
		return (0);
	buffed = malloc(sizeof(char) * (fsize + 1));
	if (!buffed)
		return (0);
	rdlen = read(fd, buffed, fsize);
	buffed[fsize] = 0;
	if (rdlen <= 0)
		return (free(buffed), 0);
	close(fd);
	for (a = 0; a < fsize; a++)
		if (buffed[a] == '\n')
		{
			buffed[a] = 0;
			build_history_list(data, buffed + last, linecount++);
			last = a + 1;
		}
	if (last != a)
		build_history_list(data, buffed + last, linecount++);
	free(buffed);
	data->histcount = linecount;
	while (data->histcount-- >= HIST_MAX)
		delete_node_at_index(&(data->history), 0);
	renumber_history(data);
	return (data->histcount);
}

/**
 * build_history_list - adds entry to a history linked list
 * @data: Structure containing potential arguments. Used to maintain
 * @buffed: fender
 * @linecount: the history linecount, histcount
 *
 * Return: Always 0
 */
int build_history_list(info_t *data, char *buffed, int linecount)
{
	list_t *list = NULL;

	if (data->history)
		list = data->history;
	add_node_end(&list, buffed, linecount);

	if (!data->history)
		data->history = list;
	return (0);
}

/**
 * renumber_history - renumbers the history linked list after changes
 * @data: Structure containing potential arguments. Used to maintain
 *
 * Return: the fresh histcount
 */
int renumber_history(info_t *data)
{
	list_t *list = data->history;
	int a = 0;

	while (list)
	{
		list->num = a++;
		list = list->next;
	}
	return (data->histcount = a);
}
