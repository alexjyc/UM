/*
 * COMP40       HW6: um
 * 
 *              um.c
 *
 * Date:        04/09/2022
 * Authors:     Alex Jeon and Abraham Park
 * 
 * Purpose:     Interface of um module. Initiates um's execution cycle and 
 *              terminates when the program receives 'halt' command or 
 *              no more input.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "um.h"

int main(int argc, char *argv[]) {
    /* Input validation */
    assert(argc == 2);
    FILE *input = fopen(argv[1], "r");
    assert(input);

    Um alpha;
    initialize_Um(&alpha, input);
    execute_Um(&alpha);
    // free_Um(&alpha);
    fclose(input);
    
    return EXIT_SUCCESS;
}