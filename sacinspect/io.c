#include <inspect.h>
#include <aux.h>

int findFilters(glob_t *glb, float *lp, float *hp) {
	int state = 0;
	
	// Set default values
	*lp = getConfigAsNumber(config, NAME_LP, DEFAULT_LP);
	*hp = getConfigAsNumber(config, NAME_HP, DEFAULT_HP);

	if (glb == NULL)
		return state;

	if (glb->gl_pathc == 0)
		return state;

	SACHEAD *head = io_readSacHead(glb->gl_pathv[0]);
	if (head == NULL)
		return state;
	
	if (head->user0 != -12345.0 && head->user1 != -12345.0 && head->user0 > head->user1) {
		*lp = head->user0;
		*hp = head->user1;
		state = (head->user0 > head->user1);
	}
	head = io_freeData(head);

	return state;
}

int findHas(glob_t *glb) {
	int has = 0;
	
	if (glb == NULL)
		return has;
	
	if (glb->gl_pathc == 0)
		return has;
	
	SACHEAD *head = io_readSacHead(glb->gl_pathv[0]);
	if (head != NULL){
		if (head->unused27 == 1) 
			has = 1;
		head = io_freeData(head);
	}
	
	return has;
}

tf * io_loadZ (glob_t *glb, int *pnfiles) {
	tf *files = NULL;
	int nfiles = 0;
	
	int i;
	int argc = glb->gl_pathc;
	char **argv = glb->gl_pathv;
	
	// Loop thru all and load
	for (i = 0; i < argc; i++) {
		SACHEAD *head = NULL;
		float * data = NULL;
		
		// Load SAC
		data = io_readSac(argv[i], &head);
		
		// Check that is loaded
		if (head == NULL || data == NULL) {
			fprintf(stderr,"Error loading file: %s", argv[i]);
			continue;
		}
		
		// Check that this is a Z file
		if (head->cmpinc != 0.0) {
			fprintf(stderr,"Error loading file: %s (not Z component)", argv[i]);
			continue;
		}
		
		// Get more memory
		files = realloc(files, sizeof(tf) * (nfiles + 1));
		if (files == NULL){
			fprintf(stderr,"Failed to allocate more memory");
			continue;
		}
		
		// Zero all
		files[nfiles].z = NULL;
		files[nfiles].n = NULL;
		files[nfiles].e = NULL;
		files[nfiles].zf = NULL;
		files[nfiles].nf = NULL;
		files[nfiles].ef = NULL;
		files[nfiles].hz = NULL;
		files[nfiles].hn = NULL;
		files[nfiles].he = NULL;
		
		// Save a copy of the filename
		files[nfiles].filename = malloc(sizeof(char) * (strlen(argv[i]) + 1));
		strcpy(files[nfiles].filename, argv[i]);
		
		// Copy the station & network name
		hdu_getValueFromChar("KSTNM", head, NULL, NULL, &files[nfiles].station);
		hdu_getValueFromChar("KNETWK", head, NULL, NULL, &files[nfiles].net);
		
		// We mark this file as visited on load
		head->unused27 = 1;
		
		// Set not selected
		files[nfiles].selected = 0;
		
		// Set the reference to 0
		files[nfiles].reference = 0;
		
		// Re-adjust the pointers
		files[nfiles].z = data;
		files[nfiles].hz = head;
		
		// Increament and go to next file
		nfiles++;
	}
	
	*pnfiles = nfiles;
	return files;
}

void io_loadN (glob_t *glb, tf *files, int *pnfiles){
	return;
}

void io_loadE (glob_t *glb, tf *files, int *pnfiles){
	return;
}

glob_t * io_loadEv(defs *d) {
	// If data is loaded free it up
	if (d->nfiles != 0) {
		tffree(d->files, d->nfiles);
		d->files = NULL;
		d->nfiles = 0;
	}

	// Prepare the pathname for the current event
	char *path = d->glb->gl_pathv[d->currentdir];
	
	// Prepare a new glob
	glob_t *glb = filelist(path, getConfigAsString(config, NAME_Z, DEFAULT_Z));
	
	// Set HAS to 0
	d->has = findHas(glb);
	
	// Find the filters in use for this event
	d->filter = findFilters(glb, &d->lp, &d->hp);
	
	// Load the Z components
	int nfiles = 0;
	tf *files = io_loadZ(glb, &nfiles);

	// Clean the glob
	killGlob(glb);

	// Check the files
	if (nfiles != 0) {
		// Check that we have picks
		checkTREF(files, nfiles);
		
		// Sort the files 
		qsort(files, nfiles, sizeof(tf), sortDist);
		
		// Synch Time
		synch(files, d);
	}

	// Set Aligmenmode
	d->alig = (d->has) ? ALIGF : ALIGA;

	// Set the filterneed if needed
	if (d->filter)
		d->needfilter = 1;

	// Set offset to 0
	d->offset = 0;

	// Prepare to return
	d->files  = files;
	d->nfiles = nfiles;

	// Done
	return NULL;
}
