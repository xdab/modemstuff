#ifndef _DATASTUFF_BITARRAY_H
#define _DATASTUFF_BITARRAY_H

typedef int ds_ba_bit_t;

typedef struct ds_bitarray
{
    void *supporting_array;
    int supporting_array_size;
    
    int used_bits;
    int used_full_bytes;
    int used_bits_in_last_byte;

} ds_bitarray_t;

int ds_bitarray_init(ds_bitarray_t *ba, void *supporting_array, int supporting_array_size);

int ds_bitarray_clear(ds_bitarray_t *ba);

int ds_bitarray_append(ds_bitarray_t *ba, ds_ba_bit_t bit);

#endif // _DATASTUFF_BITARRAY_H