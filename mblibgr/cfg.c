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

#include "cfg.h"

void _strip(char *value) {
    if ( value[strlen(value) - 1] == '\n') 
        value[strlen(value) - 1] = '\0';
    return;
}

char * _copy(char * source) {
    char * aux;
    aux = malloc(sizeof(char) * (strlen(source) + 1));
    if (aux == NULL) return NULL;
    strcpy(aux, source);
    return aux;
}

void _updateKP(KP *kp, char * value) {
    
    if (kp->value != NULL) {
        free(kp->value);
        kp->value = NULL;
    }
    
    kp->value = _copy(value);
    
    return;
}

KP * _addKP(CFG * config, char *key, char *value) {

    config->kps = realloc(config->kps, sizeof(KP) * (config->nkps + 1));
    config->kps[config->nkps].key = _copy(key);
    config->kps[config->nkps].value = _copy(value);

    config->nkps++;
    return &config->kps[config->nkps - 1];
}

void  _loadConfig(CFG *config){
    char aux[3024];
    int i;
    
    FILE *ent = fopen(config->filename,"r");
    if (ent == NULL) return;
    
    fgets(aux, 3023, ent);
    _strip(aux);
    do {
        if (strlen(aux) > 1 && aux[0] != '#') {
            for(i=0;i<strlen(aux);i++)
                if (aux[i] == '=')
                    break;
            aux[i] = '\0';
            _addKP(config, &aux[0], &aux[i+1]);
        }
        fgets(aux, 3023, ent);
        _strip(aux);
    } while (!feof(ent));
    
    fclose(ent);
    return;
}

int _key2index(CFG *config, char *key){
    int i;
    int keySize = strlen(key);
    for(i=0;i<config->nkps;i++)
        if ( (keySize == strlen(config->kps[i].key)) && (strncmp(key, config->kps[i].key, keySize) == 0))
            return i;
    return -1;
}

//// Exported methods

void * killConfig(CFG *config){
    int i;
    
    if (config != NULL) {
        // Filename
        if (config->filename != NULL)
            free(config->filename);
        config->filename = NULL;
        
        // KeyPairs items
        for(i = 0; i < config->nkps; i++) {
            if (config->kps[i].key != NULL)
                free(config->kps[i].key);
            config->kps[i].key = NULL;
            
            if (config->kps[i].value != NULL)
                free(config->kps[i].value);
            config->kps[i].value = NULL;
        }
        
        if (config->kps != NULL)
            free(config->kps);
        config->kps = NULL;
        
        free(config);
    }
    
    return NULL;
}

CFG * readConfig(char *filename) {
    CFG * config;
    
    // Alloc the config
    config = malloc(sizeof(CFG));
    if (config == NULL) return NULL;
    
    // Set the filename
    config->filename = _copy(filename);
    if (config->filename == NULL) {
        free(config);
        config = NULL;
        return NULL;
    }
    
    // Reset the other variables
    config->nkps = 0;
    config->kps = NULL;
    
    // Read the config options
    _loadConfig(config);
    
    // Return
    return config;
}

int writeConfig(CFG *config) {
    int i;
    
    FILE *ent = fopen(config->filename,"w");
    if (ent == NULL) return 1;
    
    for(i=0; i<config->nkps;i++){
        fprintf(ent, "%s=%s\n",config->kps[i].key, config->kps[i].value);
    }
    
    fclose(ent);
    return 0;
}



int getConfigValueSize(CFG *config, char *key) {
    int kn;
    
    if ((kn = _key2index(config, key)) == -1)
        return -1;
    
    return strlen(config->kps[kn].value);
}

float getConfigAsNumber(CFG *config, char *key, float defaultValue) {
    int kn = _key2index(config, key);
    
    if (kn == -1) {
        char aux[256];
        sprintf(aux,"%f",defaultValue);
        _addKP(config, key, aux);
        return defaultValue;
    }
    
    return atof(config->kps[kn].value);
}

char * getConfigAsString(CFG *config, char *key, char *defaultValue) {
    int kn = _key2index(config, key);
    
    if (kn == -1) {
        KP *kp = _addKP(config, key, defaultValue);
        return kp->value;
    }
    
    return config->kps[kn].value;
}



int setConfigString(CFG *config, char *key, char *value) {
    int kn = _key2index(config, key);
    
    if (kn == -1) {
        _addKP(config, key, value);
        return 0;
    }
    
    _updateKP(&config->kps[kn], value);
    
    return 0;
}

int setConfigNumber(CFG *config, char *key, float value) {
    char aux[256];
    int kn = _key2index(config, key);

    sprintf(aux, "%f", value);
    if (kn == -1) {
        _addKP(config, key, aux);
        return 0;
    }
    _updateKP(&config->kps[kn], aux);
    return 0;
}
