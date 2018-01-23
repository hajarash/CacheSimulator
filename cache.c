/*
 * cache.c
 */
#include "cache.h"
#include <stdlib.h>
#include <string.h>

#define NUM_BYTES 4
#define NUM_BITS_IN_BYTE 8
/*
 * Initialize a new cache line with a given block size.
 */
static void cache_line_init(cache_line_t *cache_line, size_t block_size)
{
    cache_line->is_valid = 0;
    cache_line->data = (unsigned char *) malloc(block_size * sizeof(unsigned char));
}

/*
 * Initialize a new cache set with the given associativity and block size.
 */
static void cache_set_init(cache_set_t *cache_set, unsigned int associativity, size_t block_size)
{
    int i;
    cache_set->cache_lines = (cache_line_t **) malloc(associativity * sizeof (cache_line_t *));
    
    for (i = 0; i < associativity; i++) {
        cache_set->cache_lines[i] = (cache_line_t *) malloc(sizeof(cache_line_t));
        cache_line_init(cache_set->cache_lines[i], block_size);
    }
}

/*
 * Compute the shift and mask given the number of bytes/sets.
 */
static void get_shift_and_mask(int value, int *shift, int *mask, int init_shift)
{
    *mask = 0;
    *shift = init_shift;
    
    while (value > 1) {
        (*shift)++;
        value >>= 1;
        *mask = (*mask << 1) | 1;
    }
}

/*
 * Create a new cache that contains a total of num_blocks blocks, each of which is block_size
 * bytes long, with the given associativity, and the given set of cache policies for replacement
 * and write operations.
 */
cache_t *cache_new(size_t num_blocks, size_t block_size, unsigned int associativity, int policies)
{
    int i, j;
    
    /*
     * Create the cache and initialize constant fields.
     */
    cache_t *cache = (cache_t *) malloc(sizeof(cache_t));
    cache->access_count = 0;
    cache->miss_count = 0;
    
    /*
     * Initialize size fields.
     */
    cache->policies = policies;
    cache->block_size = block_size;
    cache->associativity = associativity;
    cache->num_sets = num_blocks / associativity;
    
    /*
     * Initialize shifts and masks.
     */
    get_shift_and_mask(block_size, &cache->cache_index_shift, &cache->block_offset_mask, 0);
    get_shift_and_mask(cache->num_sets, &cache->tag_shift, &cache->cache_index_mask, cache->cache_index_shift);
    
    /*
     * Initialize cache sets.
     */
    cache->sets = (cache_set_t *) malloc(cache->num_sets * sizeof (cache_set_t));
    for (i = 0; i < cache->num_sets; i++) {
        cache_set_init(&cache->sets[i], cache->associativity, cache->block_size);
    }
    
    /*
     * Done.
     */
    return cache;
}

/*
 * Determine whether or not a cache line is valid for a given tag.
 */
static int cache_line_check_validity_and_tag(cache_line_t *cache_line, intptr_t tag)
{
    /* TO BE COMPLETED BY THE STUDENT */
    if (cache_line->tag == tag) {
        if (cache_line->is_valid == 1) return 1;
    }
    return 0;
}

/*
 * Return integer data from a cache line.
 */
static int cache_line_retrieve_data(cache_line_t *cache_line, size_t offset)
{
    /* TO BE COMPLETED BY THE STUDENT */
    //return *(cache_line->data + offset);
    int i;
    int data = 0;
    for (i = 0; i < NUM_BYTES; i++) {
        data += *(cache_line->data + offset + i) << NUM_BITS_IN_BYTE * i;
    }
    return data;
}

/*
 * Move the cache lines inside a cache set so the cache line with the given index is
 * tagged as the most recently used one. The most recently used cache line will be the
 * 0'th one in the set, the second most recently used cache line will be next, etc.
 * Cache lines whose valid bit is 0 will occur after all cache lines whose valid bit
 * is 1.
 */
static cache_line_t *cache_line_make_mru(cache_set_t *cache_set, size_t line_index)
{
    int i;
    cache_line_t *line = cache_set->cache_lines[line_index];
    
    for (i = line_index - 1; i >= 0; i--)
    {
        cache_set->cache_lines[i + 1] = cache_set->cache_lines[i];
    }
    
    cache_set->cache_lines[0] = line;
    return line;
}

/*
 * Retrieve a matching cache line from a set, if one exists.
 */
static cache_line_t *cache_set_find_matching_line(cache_t *cache, cache_set_t *cache_set, intptr_t tag)
{
    /* TO BE COMPLETED BY THE STUDENT */
    
    /*
     * Don't forget to call cache_line_make_mru(cache_set, i) if you find a matching cache line.
     */
    int i, j;
    cache->access_count++;
    for (i = 0; i < cache->num_sets; i++) {
        if (&cache->sets[i] == cache_set) {
            cache_set_t *c_set = &cache->sets[i];
            for (j = 0; j < cache->associativity; j ++) {
                if (c_set->cache_lines[j]->tag == tag) {
                    return cache_line_make_mru(c_set, j);
                }
            }
        }
    }
    cache->miss_count++;
    return NULL;
}

/*
 * Function to find a cache line to use for new data.
 */
static cache_line_t *find_available_cache_line(cache_t *cache, cache_set_t *cache_set)
{
    /* TO BE COMPLETED BY THE STUDENT */
    
    /*
     * Don't forget to call cache_line_make_mru(cache_set, i) once you have selected the
     * cache line to use.
     */
    int i, j;
    for (i = 0; i < cache->num_sets; i++) {
        if (&cache->sets[i] == cache_set) {
            cache_set_t *c_set = &cache->sets[i];
            for (j = 0; j < cache->associativity; j ++) {
                if (c_set->cache_lines[j]->is_valid == 0) {
                    return cache_line_make_mru(c_set, j);
                }
            }
        }
    }
    cache_line_t *line = NULL;
    if (cache->policies == CACHE_REPLACEMENTPOLICY_LRU) {
        line = cache_set->cache_lines[cache->associativity-1];
        cache_line_make_mru(cache_set, cache->associativity-1);
    } else {
        line = cache_set->cache_lines[0];
        cache_line_make_mru(cache_set, 0);
    }
    
    return line;
}

/*
 * Add a block to a given cache set.
 */
static cache_line_t *cache_set_add(cache_t *cache, cache_set_t *cache_set, intptr_t address, intptr_t tag)
{
    /*
     * First locate the cache line to use.
     */
    cache_line_t *line = find_available_cache_line(cache, cache_set);
    
    /*
     * Now set it up.
     */
    line->tag = tag;
    line->is_valid = 1;
    memcpy(line->data,  (void *)(address & ~cache->block_offset_mask), cache->block_size);
    
    /*
     * And return it.
     */
    return line;
}

/*
 * Read a single integer from the cache.
 */
int cache_read(cache_t *cache, int* address)
{
    /* TO BE COMPLETED BY THE STUDENT */
    intptr_t tag = ((intptr_t) address) >> cache->tag_shift;
    int index = (((intptr_t) address) >> cache->cache_index_shift) & cache->cache_index_mask;
    size_t offset = ((intptr_t) address) & cache->block_offset_mask;
    cache_line_t *line = cache_set_find_matching_line(cache, &cache->sets[index], tag);
    
    if (line == NULL) {
        line = cache_set_add(cache, &cache->sets[index], (intptr_t) address, tag);
    }
    return cache_line_retrieve_data(line, offset);
}

/*
 * Return the number of cache misses since the cache was created.
 */
int cache_miss_count(cache_t *cache)
{
    return cache->miss_count;
}

/*
 * Return the number of cache accesses since the cache was created.
 */
int cache_access_count(cache_t *cache)
{
    return cache->access_count;
}
