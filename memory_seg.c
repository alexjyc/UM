/*
 * COMP40       HW6: um
 * 
 *              memory_seg.c
 *
 * Date:        04/09/2022
 * Authors:     Alex Jeon and Abraham Park
 * 
 * Purpose:     Implemntation of memory_seg moudule.
 */

#include <assert.h>
#include <mem.h>
#include "memory_seg.h"

struct Segments {
    /* _seg: A sequence of sequence(s) of uint32_t *word(s) */
    Seq_T mapped_seg;
    Seq_T unmapped_seg;
};

/*
 * Name:                initialize_segments
 * Purpose:             initialize a new segment where each sequence holds 0
 * Input:               n.a.
 * Output:              Segments
 */
Segments initialize_segments()
{
    Segments mem_segs;
    NEW(mem_segs);

    mem_segs -> mapped_seg = Seq_new(1);
    mem_segs -> unmapped_seg = Seq_new(0);

    return mem_segs;
}

/*
 * Name:                get_sequence
 * Purpose:             Depend on the mapped condition return mapped or 
 *                      unmapped segments
 * Input                Segments, bool
 * Output:              Segments
 */
Seq_T get_sequence(Segments mem_segs, bool is_mapped)
{
    if (is_mapped) {
        return mem_segs->mapped_seg;
    } else {
        return mem_segs->unmapped_seg;
    }
}

/*
 * Name:                map_segment
 * Purpose:             Create a new segment with a number of words equal to 
 *                      the previous register and return the new segment’s 
 *                      identifier value.
 * Input:               Segments, uint32_t
 * Output:              n.a
 */
uint32_t map_segment(Segments mem_segs, uint32_t seg_size)
{
    assert(mem_segs);

    uint32_t new_idx;
    uint32_t seg_len = Seq_length(mem_segs->mapped_seg);

    /* assign new_idx based on the current state of unmapped_seg sequence 
     * reuse the previously assigned seg_id as the new_idx to the new segment
     */
    if (Seq_length(mem_segs->unmapped_seg) !=  0) {
        /* deallocate all memory within this Seq(new_idx) before remhi? */
        new_idx = (uint32_t)(uintptr_t)Seq_remhi(mem_segs->unmapped_seg);
    } else {
        new_idx = seg_len;
        if (new_idx >= seg_len) {
            Seq_addhi(mem_segs->mapped_seg, Seq_new(seg_size));
        } else {
            Seq_put(mem_segs->mapped_seg, new_idx, Seq_new(seg_size));
        }
    }

    Seq_T new_seg = Seq_get(mem_segs->mapped_seg, new_idx);
    assert(new_seg);

    for (uint32_t i = 0; i < seg_size; i++) {
        uint32_t *word = malloc(sizeof(uint32_t));
        *word = 0;
        Seq_addhi(new_seg, word);
    }

    return new_idx;
}

/*
 * Name:                unmap_segment
 * Purpose:             remove the specified segment from mapped segments 
 *                      and put it into unmapped segments
 * Input:               Segments, uint32_t 
 * Output:              n.a.
 */
void unmap_segment(Segments mem_segs, uint32_t seg_id)
{
    Seq_T curr_seg = Seq_get(mem_segs -> mapped_seg, seg_id);
    if (curr_seg != NULL) {
        Seq_free(&curr_seg);
    }
    Seq_addhi(mem_segs->unmapped_seg, (void *)(uintptr_t) seg_id);
    Seq_put(mem_segs->mapped_seg, seg_id, NULL);
}

/*
 * Name:                duplicate_segment
 * Purpose:             duplicate the specified segment to another memory 
 *                      segment and put it into mapped segment.
 * Input:               Segments, uint32_t
 * Output:              n.a.
 */
void duplicate_segment(Segments mem_segs, uint32_t seg_id)
{
    Seq_T mapped_seg = mem_segs->mapped_seg;
    /* og_seg: m[r[B]] */
    Seq_T og_seg = Seq_get(mapped_seg, seg_id);
    int seg_len = Seq_length(og_seg);
    
    uint32_t new_idx = map_segment(mem_segs, seg_len);
    Seq_T new_seg = Seq_get(mapped_seg, new_idx);
    
    for (int i = 0; i < seg_len; i++) {
        uint32_t *word = Seq_get(og_seg, i);
        Seq_put(new_seg, i, word);
    }

    unmap_segment(mem_segs, 0);
    Seq_put(mapped_seg, 0, new_seg);
}

/*
 * Name:                free_segments
 * Purpose:             deallocate every segment element that is used in
 *                      mapped segments and unmapped segments, and free the
 *                      sequences.
 * Input:               Segments
 * Output:              n.a.
 */
void free_segments (Segments *mem_segs)
{
    assert(mem_segs && *mem_segs);
    int len = Seq_length((*mem_segs)->mapped_seg);

    for (int i  = 0; i < len; i++) {
        free(Seq_remhi((*mem_segs)->mapped_seg));
    }
    
    Seq_free(&((*mem_segs)->mapped_seg));
    Seq_free(&((*mem_segs)->unmapped_seg));
    FREE(*mem_segs);
}
