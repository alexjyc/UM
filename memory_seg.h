/*
 * COMP40       HW6: um
 * 
 *              memory_seg.h
 *
 * Date:        04/09/2022
 * Authors:     Alex Jeon and Abraham Park
 * 
 * Purpose:     Interface of memory_seg moudule.
 */

#ifndef MEMORY_SEG_H_INCLUDED
#define MEMORY_SEG_H_INCLUDED

#include <seq.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

struct Segments;
typedef struct Segments *Segments;

/*
 * Name:                initialize_segments
 * Purpose:             initialize a new segment where each sequence holds 0
 */
Segments initialize_segments();

/*
 * Name:                get_sequence
 * Purpose:             Depend on the mapped condition return mapped or 
 *                      unmapped segments
 */
Seq_T get_sequence(Segments mem_segs, bool is_mapped);

/*
 * Name:                map_segment
 * Purpose:             Create a new segment with a number of words equal to 
 *                      the previous register and return the new segment’s 
 *                      identifier value.
 */
uint32_t map_segment(Segments mem_segs, uint32_t size);

/*
 * Name:                unmap_segment
 * Purpose:             remove the specified segment from mapped segments 
 *                      and put it into unmapped segments
 */
void unmap_segment(Segments mem_segs, uint32_t seg_id);

/*
 * Name:                duplicate_segment
 * Purpose:             duplicate the specified segment to another memory 
 *                      segment and put it into mapped segment.
 */
void duplicate_segment(Segments mem_segs, uint32_t seg_id);

/*
 * Name:                free_segments
 * Purpose:             deallocate every segment element that is used in
 *                      mapped segments and unmapped segments, and free the
 *                      sequences.
 */
void free_segments (Segments *mem_segs);



#endif