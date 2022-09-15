/*
 * COMP40       HW6: um
 * 
 *              um.h
 *
 * Date:        04/09/2022
 * Authors:     Alex Jeon and Abraham Park
 * 
 * Purpose:     Interface of um moudule.
 */

#ifndef _UM_H_
#define _UM_H_

#include <stdlib.h>
#include <stdio.h>
#include "memory_seg.h"

/* SIZING */
#define UM_REGS_NUMS 8
#define UM_INST_NUMS 14

#define UM_WRD_WIDTH 32
#define UM_OPCD_SIZE 4
#define UM_REGS_SIZE 3
#define UM_INST_SIZE 8

#define A 0
#define B 1
#define C 2

#define TERMINATE 7

typedef uint32_t Um_instruction;
typedef struct Um {
    Segments        mem_segs;
    Um_instruction  registers[UM_REGS_NUMS];
    Um_instruction  p_counter;
    Um_instruction  p_size;
} Um;

/*
 * Name:                initialize_Um
 * Purpose:             initialize Um struct variables; allocate struct memory
 * Input                n.a.
 * Output:              struct *Um
 */
void initialize_Um(Um *alpha, FILE *input);

/*
 * Name:                load_words
 * Purpose:             load instructions in the input file to the 0 segment.
 * Input                Segment mem_segs (struct holding a sequence of mapped
 *                      segments), FILE *input (given .um file)
 * Output:              n.a.
 */
void load_words(Um *alpha, FILE *input);

/*
 * Name:                execute_Um
 * Purpose:             execute um instruction (words) stored in the 0 segment
 *                      loop until the program receives halt command, or it 
 *                      reads all input words
 * Input                Um alpha
 * Output:              n.a.
 */
void execute_Um(Um *alpha);

/*
 * Name:                free_Um
 * Purpose:             deallocate memory used within struct *Um
 * Input                Um *alpha
 * Output:              n.a.
 */
// void free_Um(Um *alpha);

/*
 * Name:                execute_inst
 * Purpose:             execute the current word; call different inst functions
 *                      based on the given opcode.
 * Input                Um alpha, Um_instruction (uint32_t) curr_word
 * Output:              int opc
 */
int execute_inst(Um *alpha, Um_instruction curr_word);

#endif