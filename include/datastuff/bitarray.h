#ifndef _DATASTUFF_BITARRAY_H
#define _DATASTUFF_BITARRAY_H

typedef int ba_bit_t;

typedef struct bitarray
{
    void *supporting_array;
    int supporting_array_size;
    
    int used_bits;
    int used_full_bytes;
    int used_bits_in_last_byte;

} bitarray_t;

int bitarray_init(bitarray_t *ba, void *supporting_array, int supporting_array_size);

int bitarray_clear(bitarray_t *ba);

int bitarray_append(bitarray_t *ba, ba_bit_t bit);

#endif // _DATASTUFF_BITARRAY_H