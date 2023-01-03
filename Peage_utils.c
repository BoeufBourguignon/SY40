//
// Created by thibaud on 27/12/22.
//
#include "Peage_utils.h"

void throw_error(char * file, int line) {
    char s[100];
    sprintf(s, "Erreur file %s ligne %d\n", file, line);
    perror(s);
    exit(-1);
}