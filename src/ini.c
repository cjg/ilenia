/* ini.c */

/* ilenia -- A package manager for CRUX
 *
 * Copyright (C) 2006 - 2007
 *     Giuseppe Coviello <cjg@cruxppc.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "list.h"
#include "memory.h"
#include "str.h"
#include "ini.h"

// TODO: split from ilenia 

ini_t *ini_new(char *filepath)
{
	ini_t *self;

	self = xmalloc(sizeof(ini_t));
	self->filepath = xstrdup(filepath);
	self->sections = dict_new();
	return self;
}

extern void ini_free(ini_t * self)
{
	unsigned i;

	assert(self);

	free(self->filepath);
	for (i = 0; i < self->sections->length; i++)
		dict_free(self->sections->elements[i]->value, free);

	dict_free(self->sections, NULL);

	free(self);
}

ini_t *ini_add(ini_t * self, char *section)
{
	assert(self && section);

	strlower(section);

	if (dict_get(self->sections, section))
		return self;

	dict_add(self->sections, section, dict_new());

	return self;
}

/*
extern ini_t *ini_remove(ini_t * self, char *section)
{
	assert(self && section);
	
	strlower(section);
	
	if (!hash_search(self->sections, section))
		return self;
	
	hash_remove(self->sections, NULL);
	hash_head(self->sections);
	
	return self;
}
*/

extern ini_t *ini_set(ini_t * self, char *section, char *var, char *value)
{
	dict_t *vars;
	char *old_value;

	assert(self && section && var && value);

	section = strlower(section);
	var = strlower(var);

	if (!(vars = dict_get(self->sections, section)))
		return self;
	if ((old_value = dict_get(vars, var)))
		free(old_value);
	dict_add(vars, var, xstrdup(value));

	return self;
}

extern ini_t *ini_set_default(ini_t * self, char *section, char *var,
			      char *value)
{
	dict_t *vars;

	assert(self && section && var && value);

	section = strlower(section);
	var = strlower(var);

	if (!(vars = dict_get(self->sections, section)))
		return self;
	if (!dict_get(vars, var))
		dict_add(vars, var, value);
	return self;
}

extern ini_t *ini_unset(ini_t * self, char *section, char *var)
{
	dict_t *vars;

	assert(self && section && var);

	strlower(section);
	strlower(var);

	if (!(vars = dict_get(self->sections, section)))
		return self;

	dict_remove(vars, var, free);

	return self;
}

extern int ini_has_section(ini_t * self, char *section)
{
	assert(self && section);

	strlower(section);

	if (dict_get(self->sections, section))
		return 1;

	return 0;
}

extern int ini_has_var(ini_t * self, char *section, char *var)
{
	dict_t *vars;

	assert(self && section && var);

	strlower(section);
	strlower(var);

	if (!(vars = dict_get(self->sections, section)))
		return 0;

	if (!dict_get(vars, var))
		return 0;

	return 1;
}

/* extern list_t *ini_get_sections(ini_t * self) */
/* { */
/* 	assert(self != NULL); */
/* 	return hash_get_keys(self->sections); */
/* } */

dict_t *ini_get_vars(ini_t * self, char *section)
{
	assert(self && section);

	return dict_get(self->sections, section);
}

extern char *ini_get(ini_t * self, char *section, char *var)
{
	dict_t *vars;

	assert(self && section && var);

	strlower(section);
	strlower(var);

	if (!(vars = dict_get(self->sections, section)))
		return NULL;

	return dict_get(vars, var);
}

extern int ini_load(ini_t * self)
{
	char *line;
	char *section;
	char *var, *value;
	int nread;
	size_t n;
	FILE *file;

	assert(self);

	file = fopen(self->filepath, "r");

	if (!file)
		return -1;

	section = NULL;

	line = NULL;

	while ((nread = getline(&line, &n, file)) >= 0) {
		*(line + strlen(line) - 1) = 0;
		strtrim(line);

		if (!strlen(line) || *line == '#') {
			continue;
		}

		if (*line == '[') {
			free(section);
			section = xmalloc(strlen(line) - 1);
			memset(section, 0, strlen(line) - 1);
			strncpy(section, line + 1, strlen(line) - 2);
			ini_add(self, strtrim(section));
			continue;
		}

		if (!section)
			continue;
		value = NULL;
		value = strchr(line, '=');

		if (!value)
			continue;

		var = xmalloc(strlen(line) - strlen(value) + 1);
		memset(var, 0, strlen(line) - strlen(value) + 1);
		strncpy(var, line, strlen(line) - strlen(value) - 1);
		++value;
		strtrim(value);
		if (*value == '"' && *(value + strlen(value) - 1) == '"') {
			*(value + strlen(value) - 1) = 0;
			value++;
		}
		ini_set(self, section, strtrim(var), strtrim(value));
		free(var);
	}
	free(line);
	free(section);
	fclose(file);

	return 0;
}

int ini_save(ini_t * self)
{
	FILE *stream;
	dict_t *vars;
	unsigned i, j;

	assert(self);

	stream = fopen(self->filepath, "w");

	if (!stream)
		return -1;

	for (i = 0; i < self->sections->length; i++) {
		fprintf(stream, "[%s]\n", self->sections->elements[i]->key);
		vars = self->sections->elements[i]->value;
		for (j = 0; j < vars->length; j++)
			fprintf(stream, "%s = %s\n",
				(char *)vars->elements[j]->key, ini_get(self,
									self->
									sections->
									elements
									[i]->
									key,
									(char *)
									vars->
									elements
									[j]->
									key));

		fprintf(stream, "\n");
	}
	fclose(stream);
	return 0;
}

void ini_dump(ini_t * self)
{
	unsigned i, j;
	dict_t *vars;

	assert(self);

	for (i = 0; i < self->sections->length; i++) {
		printf("[%s]\n", self->sections->elements[i]->key);
		vars = self->sections->elements[i]->value;
		for (j = 0; j < vars->length; j++)
			printf("%s = %s\n", (char *)vars->elements[j]->key,
			       ini_get(self, self->sections->elements[i]->key,
				       (char *)vars->elements[j]->key));

		printf("\n");
	}
}
