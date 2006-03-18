/***************************************************************************
 *            aggiorna.c
 *
 *  Tue Feb 15 19:02:36 2004
 *  Copyright  2004 - 2006  Coviello Giuseppe
 *  immigrant@email.it
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "manipulator.h"
#include "ilenia.h"
#include "output.h"

char *get_value(char s[], char *var)
{
	char *tmp = "";
	if (strncmp(s, var, strlen(var)) == 0) {
		struct list *splitted_s = NULL;
		splitted_s = split(s, "=");
		trim(splitted_s->data);
		if (strcmp(splitted_s->data, var) == 0) {
			trim(splitted_s->next->data);
			strcpy(splitted_s->next->data,
			       mid(splitted_s->next->data, 1, END));
			trim(splitted_s->next->data);
			if (splitted_s->next->data[0] == '\"'
			    || splitted_s->next->data[0] == '\"') {
				strcpy(splitted_s->next->data,
				       mid(splitted_s->next->data, 1,
					   strlen(splitted_s->next->data) -
					   2));
				trim(splitted_s->next->data);
			}
			tmp = strdup(splitted_s->next->data);
		}
	}
	return (tmp);
}

int parse_ileniarc()
{
	FILE *file;
	ask_for_update = 1;
	post_pkgadd = strdup("");
	not_found_policy = ASK_POLICY;
	file = fopen("/etc/ilenia.rc", "r");
	if (file == NULL) {
		warning("you don't have any ilenia.rc file.");
		return (EXIT_SUCCESS);
	}
	size_t n = 0;
	char *line = NULL;
	int nread;
	while ((nread = getline(&line, &n, file)) > 0) {
		trim(line);
		if (line[0] == '#')
			continue;
		if (strstr(line, "POST_PKGADD"))
			post_pkgadd = get_value(line, "POST_PKGADD");
		if (strstr(line, "ASK_FOR_UPDATE")) {
			if (strcasecmp
			    (get_value(line, "ASK_FOR_UPDATE"), "No") == 0)
				ask_for_update = 0;

		}
		if (strstr(line, "NOT_FOUND_POLICY")) {
			char *tmp = get_value(line, "NOT_FOUND_POLICY");
			if (strcasecmp(tmp, "ask") == 0)
				not_found_policy = ASK_POLICY;
			else if (strcasecmp(tmp, "stop") == 0)
				not_found_policy = STOP_POLICY;
			else if (strcasecmp(tmp, "nevermind") == 0)
				not_found_policy = NEVERMIND_POLICY;
		}

	}
	if (line)
		free(line);
	return EXIT_SUCCESS;
}

int ask(char *question, ...)
{
	va_list args;

	va_start(args, question);
	vprintf(question, args);
	va_end(args);

	size_t n = 0;
	char *line = NULL;

	if (getline(&line, &n, stdin)) {
		trim(line);
		if (!strcasecmp(line, "Y") || !strlen(line))
			return TRUE;
	}
	return FALSE;
}
