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

int findpair(tf *files, int nfiles, SACHEAD *head) {
	int i;
	
	//	Loop thr the files and search for the matching:
	//	 * Station code
	//	 * Network code
	for(i=0; i < nfiles; i++) {
		// Station Code
		char *scode = NULL;
		hdu_getValueFromChar("KSTNM", head, NULL, NULL, &scode);
		if ((strlen(scode) != strlen(files[i].station))  || strcmp(scode, files[i].station)) {
			if (scode != NULL) free(scode);
			scode = NULL;
			continue;
		}
		
		// Network code
		char *ncode;
		hdu_getValueFromChar("KNETWK", head, NULL, NULL, &ncode);
		if ((strlen(ncode) != strlen(files[i].net))  || strcmp(ncode, files[i].net)) {
			if (scode != NULL) free(scode);
			if (ncode != NULL) free(ncode);
			scode = NULL;
			ncode = NULL;
			continue;
		}
		
		// Clean up
		if (scode != NULL) free(scode);
		if (ncode != NULL) free(ncode);
		scode = NULL;
		ncode = NULL;
		
		// return index that match
		return i;
	}
	
	return -1;
}

int io_AdjustCurrent(defs *d) {
	int i;
	
	for(i = 0; i < d->nfiles; i++) {
		if (d->zne > 0 && !d->has3)  return 1;
		
		if (d->zne == 0)
			d->files[i].current = d->files[i].z;
		
		else if (d->zne == 1 && d->has3)
			d->files[i].current = d->files[i].n;
		
		else if (d->zne == 2 && d->has3)
			d->files[i].current = d->files[i].e;
		
		else // Default is Z, for safety
			d->files[i].current = d->files[i].z;
	}
	
	return 0;
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
		
		// Allocated space for Z
		files[nfiles].z = malloc(sizeof(otf));
		if (files[nfiles].z == NULL){
			fprintf(stderr,"Failed to allocate more memory");
			continue;
		}
		
		// Filename
		files[nfiles].z->filename = NULL;
		
		// data
		files[nfiles].z->data = NULL;
		
		// filtered data
		files[nfiles].z->dataf = NULL;
		
		// header file
		files[nfiles].z->head = NULL;
		
		// Set no default data source
		files[nfiles].current = NULL;
		
		// Save a copy of the filename
		files[nfiles].z->filename = malloc(sizeof(char) * (strlen(argv[i]) + 1));
		strcpy(files[nfiles].z->filename, argv[i]);
		
		// Copy the station & network name
		hdu_getValueFromChar("KSTNM", head, NULL, NULL, &files[nfiles].station);
		hdu_getValueFromChar("KNETWK", head, NULL, NULL, &files[nfiles].net);
		
		// We mark this file as visited on load
		head->unused27 = 1;
		
		// Set not selected
		files[nfiles].selected = 0;
		
		// Set the reference to 0
		files[nfiles].z->reference = 0;
		
		// Re-adjust the pointers
		files[nfiles].z->data = data;
		files[nfiles].z->head = head;
		
		// Increament and go to next file
		nfiles++;
	}
	
	*pnfiles = nfiles;
	return files;
}

void io_loadN (glob_t *glb, tf *files, int nfiles){
	int argc = glb->gl_pathc;
	char **argv = glb->gl_pathv;
	int i;
	
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
		
		// Check that this is a N file
		if (head->cmpinc != 90.0 && head->cmpaz != 0.0) {
			fprintf(stderr,"Error loading file: %s (not N component)", argv[i]);
			continue;
		}
		
		// Search for the correct Z component file
		int j = findpair(files, nfiles, head);
		if (j == -1) {
			head = io_freeData(head);
			data = io_freeData(data);
			head = NULL;
			data = NULL; 
			fprintf(stderr,"File %s not paired.", argv[i]);
			continue;
		}
		
		// Check that we can assign a new data to component n
		if (files[j].n != NULL) {
			head = io_freeData(head);
			data = io_freeData(data);
			head = NULL;
			data = NULL; 
			fprintf(stderr,"Trace already assigned.");
			continue;
		}
		
		// Prepare space for new otf
		files[j].n = malloc(sizeof(otf));
		if (files[j].n == NULL){
			fprintf(stderr,"Failed to allocate more memory");
			continue;
		}
		
		// We mark this file as visited on load
		head->unused27 = 1;
		
		// Attach Data
		files[j].n->data = data;

		// Attach Head
		files[j].n->head = head;
		
		// Clean filer
		files[j].n->dataf = NULL;
		
		// Save a copy of the filename
		files[j].n->filename = malloc(sizeof(char) * (strlen(argv[i]) + 1));
		strcpy(files[j].n->filename, argv[i]);
	}
}

void io_loadE (glob_t *glb, tf *files, int nfiles){
	int argc = glb->gl_pathc;
	char **argv = glb->gl_pathv;
	int i;
	
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
		
		// Check that this is a N file
		if (head->cmpinc != 90.0 && head->cmpaz != 90.0) {
			fprintf(stderr,"Error loading file: %s (not N component)", argv[i]);
			continue;
		}
		
		// Search for the correct Z component file
		int j = findpair(files, nfiles, head);
		if (j == -1) {
			head = io_freeData(head);
			data = io_freeData(data);
			head = NULL;
			data = NULL; 
			fprintf(stderr,"File %s not paired.", argv[i]);
			continue;
		}
		
		// Check that we can assign a new data to component n
		if (files[j].e != NULL) {
			head = io_freeData(head);
			data = io_freeData(data);
			head = NULL;
			data = NULL; 
			fprintf(stderr,"Trace already assigned.");
			continue;
		}
		
		// Prepare space for new otf
		files[j].e = malloc(sizeof(otf));
		if (files[j].e == NULL){
			fprintf(stderr,"Failed to allocate more memory");
			continue;
		}
		
		// We mark this file as visited on load
		head->unused27 = 1;
		
		// Attach Data
		files[j].e->data = data;
		
		// Attach Head
		files[j].e->head= head;
		
		// Clear filter data space
		files[j].e->dataf = NULL;
		
		// Save a copy of the filename
		files[j].e->filename = malloc(sizeof(char) * (strlen(argv[i]) + 1));
		strcpy(files[j].e->filename, argv[i]);
	}
}

glob_t * io_loadEv(defs *d) {
	// If data is loaded free it up
	if (d->nfiles != 0) {
		tffree(d->files, d->nfiles);
		d->files = NULL;
		d->nfiles = 0;
		d->has3 = 0;
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
	killGlob(glb);

	if (getConfigAsBoolean(config, NAME_LOAD, DEFAULT_LOAD)) {
		// Load N components
		glob_t *glbn = filelist(path, getConfigAsString(config, NAME_N, DEFAULT_N));
		io_loadN(glbn, files, nfiles);
		killGlob(glbn);
		
		// Load E
		glob_t *glbe = filelist(path, getConfigAsString(config, NAME_E, DEFAULT_E));
		io_loadE(glbe, files, nfiles);
		killGlob(glbe);
		
		d->has3 = 1;
	}

	// Check the files
	if (nfiles != 0) {
		// Check that we have picks
		checkTREF(files, nfiles);
		
		// Sort the files 
		qsort(files, nfiles, sizeof(tf), sortDist);
		
		// Synch Time
		synch(files, nfiles);
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

	// Adjust the curent pointer
	io_AdjustCurrent(d);

	// Done
	return NULL;
}
