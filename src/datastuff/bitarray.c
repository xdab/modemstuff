#include <datastuff/bitarray.h>

int ds_bitarray_init(ds_bitarray_t *ba, void *supporting_array, int supporting_array_size)
{
    ba->supporting_array = supporting_array;
    ba->supporting_array_size = supporting_array_size;

    ba->used_bits = 0;
    ba->used_full_bytes = 0;
    ba->used_bits_in_last_byte = 0;

    return 0;
}

int ds_bitarray_clear(ds_bitarray_t *ba)
{
    ba->used_bits = 0;
    ba->used_full_bytes = 0;
    ba->used_bits_in_last_byte = 0;

    return 0;
}

int ds_bitarray_append(ds_bitarray_t *ba, ds_ba_bit_t bit)
{
    if (ba->used_bits == ba->supporting_array_size * 8)
        return -1;

    if (ba->used_bits % 8 == 0)
    {
        ((unsigned char *)ba->supporting_array)[ba->used_full_bytes] = 0;
        ba->used_full_bytes++;
    }

    ((unsigned char *)ba->supporting_array)[ba->used_full_bytes - 1] |= bit << (7 - ba->used_bits_in_last_byte);
    ba->used_bits++;
    ba->used_bits_in_last_byte = ba->used_bits % 8;

    return 0;
}