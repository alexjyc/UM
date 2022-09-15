/*
 * COMP40       HW6: um
 * 
 *              um.c
 *
 * Date:        04/09/2022
 * Authors:     Alex Jeon and Abraham Park
 * 
 * Purpose:     Implementaion of um module.
 */

#include <math.h>
#include "assert.h"
#include "bitpack.h"
#include "mem.h"
#include "seq.h"
#include "um.h"

/* Um_opcode / Um_register DEFINITIONS */
typedef enum Um_opcode {
        CMOV = 0, SLOAD = 1, SSTORE = 2, ADD = 3, MUL = 4, DIV = 5, 
        NAND = 6, HALT = 7, ACTIVATE = 8, INACTIVATE = 9, 
        OUT = 10, IN = 11, LOADP = 12, LV = 13
} Um_opcode;
typedef enum Um_register { 
        r0 = 0, r1 = 1, r2 = 2, r3 = 3, 
        r4 = 4, r5 = 5, r6 = 6, r7 = 7
} Um_register;

/* STATIC HELPER FUNCTIONS */
static void     get_reg_idxs(Um_register *reg_idxs, Um_instruction curr_word, 
                                                    bool is_lval);
// static uint32_t get_input_size(FILE *input);

/* STATIC UM_INSTRUCTION FUNCS */
static inline void inst_cmove(Um *alpha, Um_register *reg_idxs);
static inline void inst_segload(Um *alpha, Um_register *reg_idxs);
static inline void inst_segstore(Um *alpha, Um_register *reg_idxs);
static inline void inst_add(Um *alpha, Um_register *reg_idxs);
static inline void inst_multiply(Um *alpha, Um_register *reg_idxs);
static inline void inst_divide(Um *alpha, Um_register *reg_idxs);
static inline void inst_bnand(Um *alpha, Um_register *reg_idxs);
static inline void inst_halt();
static inline void inst_activ(Um *alpha, Um_register *reg_idxs);
static inline void inst_inactiv(Um *alpha, Um_register *reg_idxs);
static inline void inst_output(Um *alpha, Um_register *reg_idxs);
static inline void inst_input(Um *alpha, Um_register *reg_idxs);
static inline void inst_loadprogram(Um *alpha, Um_register *reg_idxs);
static inline void inst_loadval(Um *alpha, Um_register *reg_idxs);

/******************************************************************************
 *
 * UM
 * 
 *****************************************************************************/
/*
 * Name:                initialize_Um
 * Purpose:             initialize Um struct variables; allocate struct memory
 * Input                n.a.
 * Output:              struct *Um
 */
void initialize_Um(Um *alpha, FILE *input)
{
    alpha->p_counter = 0;
    alpha->mem_segs  = initialize_segments();
    for (int i = 0; i < UM_REGS_NUMS; i++) {
        alpha->registers[i] = 0;
    }
    load_words(alpha, input);
}

/*
 * Name:                load_words
 * Purpose:             load instructions in the input file to the 0 segment.
 * Input                Segment mem_segs (struct holding a sequence of mapped
 *                      segments), FILE *input (given .um file)
 * Output:              n.a.
 */
void load_words(Um *alpha, FILE *input)
{
    uint32_t seg0_idx = map_segment(alpha->mem_segs, 0);
    Seq_T mapped_seg = get_sequence(alpha->mem_segs, true);
    Seq_T seg0 = Seq_get(mapped_seg, seg0_idx);

    uint32_t prog_size = ftell(input) / sizeof(uint32_t);
    rewind(input);
    
    for (unsigned i = 0; i < prog_size; i++) {
        uint32_t *word = malloc(sizeof(uint32_t));
        *word = 0;
        for (int j = 1; j <= 4; j++) {
                *word = Bitpack_newu(*word, 
                                    8, 32 - 8 * j, fgetc(input));
        }
        Seq_addhi(seg0, word);
    }



    // int w = getc(input);
    // while (w != EOF) {
    //     uint32_t *word = malloc(sizeof(uint32_t));
    //     *word = 0;
    //     for (int j = sizeof(uint32_t) - 1; j >= 0; j--) {
    //             *word = (uint32_t)Bitpack_newu((uint64_t)word, 
    //                 8, (j * 8), (uint64_t)w);
    //             w = getc(input);
    //     }

    //     for (int i = 1; i < UM_OPCD_SIZE + 1; i++) {
    //         *word = Bitpack_newu(*word, UM_INST_SIZE, 
    //                                  (UM_WRD_WIDTH - UM_INST_SIZE * i), w);
    //         w = fgetc(input);
    //     }
        
    //     Seq_addhi(seg0, word);
    // }
    
    /* get a sequence of mapped_seg (true) and add seg0 */
    Seq_addhi(mapped_seg, seg0);
}

// /*
//  * Name:                get_size
//  * Purpose:             find the size of the given file input.
//  * Input                FILE *input
//  * Output:              uint32_t size of the input.
//  */
// static uint32_t get_input_size(FILE *input)
// {
//     fseek(input, 0, SEEK_END);
//     uint32_t size = ftell(input) / sizeof(uint32_t);
//     fseek(input, 0, SEEK_SET);
//     return size;
// }


/*
 * Name:                execute_Um
 * Purpose:             execute um instruction (words) stored in the 0 segment;
 *                      loop until the program receives halt command, or it 
 *                      reads all input words
 * Input                Um *alpha
 * Output:              n.a.
 */
void execute_Um(Um *alpha)
{
    Seq_T mapped_seg = get_sequence(alpha->mem_segs, true);
    Seq_T seg0 = Seq_get(mapped_seg, 0);

    alpha->p_size = Seq_length(seg0);
    printf("p_size: %u\n", alpha->p_size);

    for (uint32_t i = 0; i < alpha->p_size; i++) {
        /*****************************************************************/
        printf("p_counter: %u\n", alpha->p_counter);
        for (unsigned j = 0; j < 8; j++) {
            printf("r[%u]: %u ", j, alpha->registers[j]);
        }
        printf("\n/*****************************************************************/\n");
        /*****************************************************************/

        uint32_t *curr_word = Seq_get(seg0, alpha->p_counter);
        alpha->p_counter++;
        int opc = execute_inst(alpha, *curr_word);

        printf("\n");
        if (opc == TERMINATE) {
            break;
        }
    }
}

/*
 * Name:                free_Um
 * Purpose:             deallocate memory used within struct *Um
 * Input                Um *alpha
 * Output:              n.a.
 */
// void free_Um(Um *alpha)
// {
//     assert(alpha && *alpha);
//     free_segments(&(*alpha)->mem_segs);
//     FREE(*alpha);
// }

/******************************************************************************
 *
 * UM_INSTRUCTIONS
 * 
 *****************************************************************************/
/*
 * Name:                execute_inst
 * Purpose:             execute the current word; call different inst functions
 *                      based on the given opcode.
 * Input                Um *alpha, Um_instruction (uint32_t) curr_word
 * Output:              int opc
 */
int execute_inst(Um *alpha, Um_instruction curr_word)
{   
    Um_opcode opc     = Bitpack_getu(curr_word, UM_OPCD_SIZE, 
                                                UM_WRD_WIDTH - UM_OPCD_SIZE);
    // assert(opc < 14);

    Um_register reg_idxs[3];
    // get_reg_idxs(reg_idxs, curr_word, false);

    /*****************************************************************/
    printf("opc: %u \n", opc);
    if (opc != 13) {
        get_reg_idxs(reg_idxs, curr_word, false);
        for (int i = 0; i < 3; i++) {
            printf("%u ", reg_idxs[i]);
        }
        printf("\n");
    } else {
        get_reg_idxs(reg_idxs, curr_word, true);
        for (int i = 0; i < 3; i++) {
            printf("%u ", reg_idxs[i]);
        }
        printf("\n");
    }
    /*****************************************************************/

    switch(opc) {
        case CMOV:
            inst_cmove(alpha, reg_idxs);
            break;
        case SLOAD:
            inst_segload(alpha, reg_idxs);
            break;
        case SSTORE:
            inst_segstore(alpha, reg_idxs);
            break;
        case ADD:
            inst_add(alpha, reg_idxs);
            break;
        case MUL:
            inst_multiply(alpha, reg_idxs);
            break;
        case DIV:
            inst_divide(alpha, reg_idxs);
            break;
        case NAND:
            inst_bnand(alpha, reg_idxs);
            break;
        case HALT:
            inst_halt();
            break;
        case ACTIVATE:
            inst_activ(alpha, reg_idxs);
            break;
        case INACTIVATE:
            inst_inactiv(alpha, reg_idxs);
            break;
        case OUT:
            inst_output(alpha, reg_idxs);
            break;
        case IN:
            inst_input(alpha, reg_idxs);
            break;
        case LOADP:
            inst_loadprogram(alpha, reg_idxs);
            break;
        case LV:
            // get_reg_idxs(reg_idxs, curr_word, true);

            // for (int i = 0; i < 3; i++) {
            //     printf("%u ", reg_idxs[i]);
            // }
            // printf("\n");

            inst_loadval(alpha, reg_idxs);
            break;
    }
    return opc;
}

/*
 * Name:                get_reg_idxs
 * Purpose:             static function to store three register values, or
 *                      (a register, val) pair when given LOADVAL command,
 *                      in the array.
 * Input                Um_register *reg_idxs: array holding the regs vals
 *                      Um_instruction curr_word: current input word
 *                      bool is_lval: LOADVAL flag
 * Output:              n.a.
 */
static inline void get_reg_idxs(Um_register *reg_idxs, Um_instruction curr_word, bool is_lval)
{
    if (is_lval) {
        reg_idxs[A] = Bitpack_getu(curr_word, UM_REGS_SIZE, 25);
        reg_idxs[B] = Bitpack_getu(curr_word, 25, 0);
        reg_idxs[C] = 0;
        assert(reg_idxs[A] < UM_REGS_NUMS);
    } else {
        reg_idxs[A] = Bitpack_getu(curr_word, UM_REGS_SIZE, 6);
        reg_idxs[B] = Bitpack_getu(curr_word, UM_REGS_SIZE, 3);
        reg_idxs[C] = Bitpack_getu(curr_word, UM_REGS_SIZE, 0);
        /* assert if the given reg_idxs[i] is not in the range */
        for (int i = 0; i < 3; i++) {
            assert(reg_idxs[i] < UM_REGS_NUMS);
        }
    }
}

/******************************************************************************
 *
 * Wrapper functions for each of the instructions
 *
 *****************************************************************************/

/* opcode[0:7] */
/*
 * Name:                0. inst_cmove
 * Purpose:             operate Conditional Move instruction of UM; 
 *                      r[A] = r[B] if r[C] != 0
 * Input                Um *alpha, Um_register *reg_idxs
 * Output:              n.a.
 */
static inline void inst_cmove(Um *alpha, Um_register *reg_idxs)
{
    Um_instruction *um_regs = alpha->registers;
    if (um_regs[reg_idxs[C]] != 0) {
        um_regs[reg_idxs[A]] = um_regs[reg_idxs[B]];
    }
}

/*
 * Name:                1. inst_segload
 * Purpose:             operate Segmented Load instruction of UM;
 *                      r[A] = m[r[B]][r[C]]
 * Input                Um *alpha, Um_register *reg_idxs
 * Output:              n.a.
 */
static inline void inst_segload(Um *alpha, Um_register *reg_idxs)
{
    Um_instruction *um_regs = alpha->registers;
    Seq_T mapped_seg = get_sequence(alpha->mem_segs, true);
    Seq_T m_rb = Seq_get(mapped_seg, um_regs[reg_idxs[B]]);
    um_regs[reg_idxs[A]] = *(uint32_t *)Seq_get(m_rb, um_regs[reg_idxs[B]]);
}

/*
 * Name:                2. inst_segstore
 * Purpose:             operate Segmented Store instruction of UM
 *                      m[r[A]][r[B]] = r[C]
 * Input                Um *alpha, Um_register *reg_idxs
 * Output:              n.a.
 */
static inline void inst_segstore(Um *alpha, Um_register *reg_idxs)
{
    Um_instruction *um_regs = alpha->registers;
    Seq_T mapped_seg = get_sequence(alpha->mem_segs, true);
    Seq_T m_ra = Seq_get(mapped_seg, um_regs[reg_idxs[A]]);
    uint32_t *m_rarb = Seq_get(m_ra, um_regs[reg_idxs[B]]);
    *m_rarb = um_regs[reg_idxs[C]];
}

/*
 * Name:                3. inst_add
 * Purpose:             operate Addition instruction of UM
 *                      r[A] = r[B] + r[C]
 * Input                Um *alpha, Um_register *reg_idxs
 * Output:              n.a.
 */
static inline void inst_add(Um *alpha, Um_register *reg_idxs) 
{
    Um_instruction *um_regs = alpha->registers;
    um_regs[reg_idxs[A]] = um_regs[reg_idxs[B]] + um_regs[reg_idxs[C]];
}

/*
 * Name:                4. inst_multiply
 * Purpose:             operate Multiplication instruction of UM
 *                      r[A] = r[B] * r[C]
 * Input                Um *alpha, Um_register *reg_idxs
 * Output:              n.a.
 */
static inline void inst_multiply(Um *alpha, Um_register *reg_idxs)
{
    Um_instruction *um_regs = alpha->registers;
    um_regs[reg_idxs[A]] = um_regs[reg_idxs[B]] * um_regs[reg_idxs[C]];
}

/*
 * Name:                5. inst_divide
 * Purpose:             operate Division instruction of UM
 *                      r[A] = r[B] / r[C]
 * Input                Um *alpha, Um_register *reg_idxs
 * Output:              n.a.
 */
static inline void inst_divide(Um *alpha, Um_register *reg_idxs)
{
    Um_instruction *um_regs = alpha->registers;
    um_regs[reg_idxs[A]] = um_regs[reg_idxs[B]] / um_regs[reg_idxs[C]];
}

/*
 * Name:                6. inst_bnand
 * Purpose:             operate Bitwise NAND instruction of UM
 *                      r[A] = ~(r[B] & r[C])
 * Input                Um *alpha, Um_register *reg_idxs
 * Output:              n.a.
 */
static inline void inst_bnand(Um *alpha, Um_register *reg_idxs)
{
    Um_instruction *um_regs = alpha->registers;
    um_regs[reg_idxs[A]] = ~(um_regs[reg_idxs[B]] & um_regs[reg_idxs[C]]);
}

/* opcode[7:13] */
/*
 * Name:                7. inst_halt
 * Purpose:             operate Halt instruction of UM
 * Input                Um *alpha, Um_register *reg_idxs
 * Output:              n.a.
 */
static inline void inst_halt() 
{
    /* no action required */
}

/*
 * Name:                8. inst_activ
 * Purpose:             operate Map Segment instruction of UM;
 *                      calls map_segment func from memory_seg.c to create a
 *                      new segment of size r[C]; map the new segment to 
 *                      m[r[B]]
 * Input                Um *alpha, Um_register *reg_idxs
 * Output:              n.a
 */
static inline void inst_activ(Um *alpha, Um_register *reg_idxs)
{
    Um_instruction *um_regs = alpha->registers;
    um_regs[reg_idxs[B]] = map_segment(alpha->mem_segs, um_regs[reg_idxs[C]]);
}

/*
 * Name:                9. inst_inactiv
 * Purpose:             operate Unmap Segment instruction of UM;
 *                      unmap the segment at m[r[C]].
 * Input                Um *alpha, Um_register *reg_idxs
 * Output:              n.a.
 */
static inline void inst_inactiv(Um *alpha, Um_register *reg_idxs)
{
    Um_instruction *um_regs = alpha->registers;
    unmap_segment(alpha->mem_segs, um_regs[reg_idxs[C]]);
}

/*
 * Name:                10. inst_output
 * Purpose:             operate Output instruction of UM;
 *                      write value in the range [0, 255] to I/O
 * Input                Um *alpha, Um_register *reg_idxs
 * Output:              n.a.
 */
static inline void inst_output(Um *alpha, Um_register *reg_idxs)
{
    Um_instruction val = alpha->registers[reg_idxs[C]];
    // assert(val <= 255);
    putchar(val);
}

/*
 * Name:                11. inst_input
 * Purpose:             operate Input instruction of UM;
 *                      receive value in the range [0, 255] and load it to r[C];
 *                      when met with EOF, r[C] = ~0
 * Input                Um *alpha, Um_register *reg_idxs
 * Output:              n.a.
 */
static inline void inst_input(Um *alpha, Um_register *reg_idxs)
{
    int val = fgetc(stdin);
    if (val == EOF) {
        alpha->registers[reg_idxs[C]] = ~0;
    } else {
        assert(val <= 255);
        alpha->registers[reg_idxs[C]] = (uint32_t) val;
    }
}

/*
 * Name:                12. inst_loadprogram
 * Purpose:             operate Load Program instruction of UM;
 *                      replace m[0] with the duplicated m[r[B]];
 *                      program counter then points to m[0][r[C]]
 * Input                Um *alpha, Um_register *reg_idxs
 * Output:              n.a.
 */
static inline void inst_loadprogram(Um *alpha, Um_register *reg_idxs)
{
    Um_instruction *um_regs = alpha->registers;
    uint32_t seg_id = um_regs[reg_idxs[B]];
    if (seg_id == 0) {
        return;
    }
    duplicate_segment(alpha->mem_segs, seg_id);
    alpha->p_counter = um_regs[reg_idxs[C]];
}

/*
 * Name:                13. inst_loadval
 * Purpose:             operate Load Value instruction of UM; 
 *                      load value in r[B] to r[A].
 * Input                Um *alpha, Um_register *reg_idxs
 * Output:              n.a
 */
static inline void inst_loadval(Um *alpha, Um_register *reg_idxs)
{
    Um_instruction *um_regs = alpha->registers;
    /* EXCEPTION: reg_idxs[B] here represents value from the last 25 bits */
    um_regs[reg_idxs[A]] = reg_idxs[B];
}