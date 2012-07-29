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
#include <PK.h>
#include <EX.h>

CFG *config;

int main(int argc, char **argv)
{
	glob_t *glb = NULL;
	int compare;
	char *default_path = NULL;

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
		fprintf(stderr, "\t-histogram\n");
		exit(-1);
	}

	/* Initialize the configuration infrastructure */
	config = readConfig(filename);

	/* Find the current folder pattern to use */
	default_path = getConfigAsString(config, NAME_PATTERN, DEFAULT_PATTERN);

	/* Choose module to run */
	if ((compare = strncmp(argv[1], "-pick", 5)) == 0) {
		if (argc == 3)
			glb = dirlist(argv[2]);
		else
			glb = dirlist(default_path);
		PK_process(glb);
	} else if ((compare = strncmp(argv[1], "-export", 7)) == 0) {
		char *oldlist = NULL;

		if (argc == 3)
			glb = dirlist(argv[2]);
		else
			glb = dirlist(default_path);

		if (argc == 4)
			oldlist = argv[3];

		if (glb->gl_pathc == 0) {
			fprintf(stderr, "No Directories found. Use option -pick and change the config options.\n");
		} else {
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
			glb->gl_pathv[0] = malloc(sizeof(char) * 4);
			strcpy(glb->gl_pathv[0], "./");
		}

		PK_process(glb);
	} else if ((compare = strncmp(argv[1], "-az", 10)) == 0) {
		glb = dirlist(default_path);
		EXAZ_process(glb, argv[2]);
	} else if ((compare = strncmp(argv[1], "-histogram", 10)) == 0) {
		if (argc == 3)
			glb = dirlist(argv[2]);
		else
			glb = dirlist(default_path);

		if (glb->gl_pathc == 0) {
			fprintf(stderr, "No Directories found. Use -pick -> config to change folder matching pattern.\n");
		} else {
			EXHIST_process(glb);
		}
	}

	/* Save the config */
	writeConfig(config);

	/* Clean up the config structure */
	config = killConfig(config);

	/* Clean up */
	if (glb != NULL)
		globfree(glb);

	return 0;
}
