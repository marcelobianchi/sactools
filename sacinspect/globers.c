#include <string.h>
#include <globers.h>

glob_t *filelist(char *folder, char *fpattern) {
	glob_t *glb = NULL;
	char *pattern;
	
	pattern = malloc(sizeof(char) * (strlen(folder) + 1 + strlen(fpattern) + 1));
	sprintf(pattern, "%s/%s",folder, fpattern);

	glb = malloc(sizeof(glob_t));
	glb->gl_pathc = 0;
	glob(pattern, 0, NULL, glb);

	free(pattern);
	pattern = NULL;
	
	return glb;
}

glob_t *dirlist(char *pattern)
{
	glob_t *glb = NULL;

	glb = malloc(sizeof(glob_t));
	glb->gl_pathc = 0;
	glob(pattern, GLOB_ONLYDIR, NULL, glb);

	return glb;
}

void *killGlob(glob_t * glb)
{
	if (glb != NULL) {
		globfree(glb);
		free(glb);
	}
	return NULL;
}
