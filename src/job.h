/* job.h */

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

#ifndef _JOB_H
#define _JOB_H
#include "port.h"

typedef enum { NOT_EXECUTED_RT, FAILED_RT, SUCCESSED_RT } job_result_t;
typedef enum { UPDATE_JOB, FETCH_JOB, REMOVE_JOB } job_type_t;
typedef struct {
	port_t *port;
	job_type_t type;
	int have_readme;
	int have_preinstall;
	int have_postinstall;
	job_result_t result;
	char *post_pkgadd;
} job_t;

job_t *job_new(port_t * port, job_type_t type, char *post_pkgadd);
void job_dump(job_t * self);
int job_execute(job_t * self);

#endif
