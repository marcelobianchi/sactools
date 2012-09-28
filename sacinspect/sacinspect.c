/*
    This file is part of sactools package.
    
    sactools is a package to managing and do simple processing with SAC
    seismological files.
    Copyright (C) 2012  Marcelo B. de Bianchi

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <inspect.h>
#include <aux.h>
#include <globers.h>

#include <PK.h>
#include <EX.h>
#include <HS.h>
#include <MAP.h>

CFG *config;

int main(int argc, char **argv)
{
	glob_t *glb = NULL;
	char *pathPattern = NULL;
	int compare;

	char filename[5012];
	char *home = getenv("HOME");

	sprintf(filename, ".sacinspectrc");
	if (home != NULL)
		sprintf(filename, "%s/.sacinspectrc", home);

	if (argc == 1) {
		fprintf(stderr, "You must use one of:\n");
		fprintf(stderr, "\t-pick [Dir Pattern]\n");
		fprintf(stderr, "\t-onedir [Dir]\n");
		fprintf(stderr,
				"\t-export [Dir Pattern] [Old Station List File]\n");
		fprintf(stderr, "\t-histogram [Dir Pattern]\n");
		exit(-1);
	}

	/* Initialize the configuration infrastructure */
	config = readConfig(filename);

	/* Find the current folder pattern to use */
	pathPattern = getConfigAsString(config, NAME_PATTERN, DEFAULT_PATTERN);

	/* Choose module to run */
	if ((compare = strncmp(argv[1], "-pick", 5)) == 0) {
		glb = (argc == 3) ? dirlist(argv[2]) : dirlist(pathPattern);
		PK_process(glb);
	} else if ((compare = strncmp(argv[1], "-map", 5)) == 0) {

		if (argc == 3) {
			glb = malloc(sizeof(glob_t));
			glb->gl_pathv = malloc(sizeof(char *));
			glb->gl_pathc = 1;
			glb->gl_pathv[0] =
				malloc(sizeof(char) * (strlen(argv[2]) + 1));
			strcpy(glb->gl_pathv[0], argv[2]);
		} else {
			glb = dirlist(pathPattern);
		}
		MAP_process(glb);
		
	} else if ((compare = strncmp(argv[1], "-export", 7)) == 0) {
		char * oldlist = (argc >= 4) ? argv[3] : NULL;
		glb = (argc >= 3) ? dirlist(argv[2]) : dirlist(pathPattern);

		if (glb->gl_pathc > 0) {
			EX_process(glb, oldlist);
		}

	} else if ((compare = strncmp(argv[1], "-onedir", 7)) == 0) {
		glb = malloc(sizeof(glob_t));
		glb->gl_pathv = malloc(sizeof(char *));
		glb->gl_pathc = 1;

		if (argc == 3) {
			glb->gl_pathv[0] =
				malloc(sizeof(char) * (strlen(argv[2]) + 1));
			strcpy(glb->gl_pathv[0], argv[2]);
		} else {
			glb->gl_pathv[0] = malloc(sizeof(char) * 3);
			strcpy(glb->gl_pathv[0], "./");
		}

		PK_process(glb);
		
		free (glb->gl_pathv[0]);
		glb->gl_pathv[0] = NULL;
		
		free (glb->gl_pathv);
		glb->gl_pathv = NULL;
		
		free(glb);
		glb = NULL;
	} else if ((compare = strncmp(argv[1], "-histogram", 10)) == 0) {
		glb = (argc == 3) ? dirlist(argv[2]) : dirlist(pathPattern);
		if (glb->gl_pathc > 0)
			EXHIST_process(glb);
	}

	if (glb != NULL && glb->gl_pathc == 0) {
		fprintf(stderr, "No Directories found for pattern %s.\n",
				(argc >= 3) ? argv[2] : pathPattern);
	}
	
	/* Save the config */
	writeConfig(config);

	/* Clean up the config structure */
	config = killConfig(config);

	/* Clean up */
	glb = killGlob(glb);

	return 0;
}
