#ifndef BMP_BMP_H
#define BMP_BMP_H

#include <stdint.h>

#pragma pack (push, 2)
typedef struct bmp_s {
    int16_t bf_type;
    int32_t bf_size;
    int16_t bf_reserved_1;
    int16_t bf_reserved_2;
    int32_t bf_off_bits;
    int32_t bi_size;
    int32_t bi_width;
    int32_t bi_height;
    int16_t bi_planes;
    int16_t bi_bit_count;
    int32_t bi_compression;
    int32_t bi_size_image;
    int32_t bi_x_pels_per_meter;
    int32_t bi_y_pels_per_meter;
    int32_t bi_clr_used;
    int32_t bi_clr_important;
    int8_t **pixels;
} bmp_t;
#pragma pack (pop)


bmp_t *load_bmp(char *file_name);

bmp_t *crop(const bmp_t *image, int32_t x, int32_t y, int32_t w, int32_t h);

bmp_t *rotate(const bmp_t *image);

int save_bmp(const bmp_t *image, char *file_name);

void del_bmp(bmp_t *image);

#endif
