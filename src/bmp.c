#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

int align_width(int width) {
    return ((width + 3) / 4) * 4;
}

bmp_t *load_bmp(char *file_name) {
    assert(file_name);
    if (*file_name == '\0') {
        return NULL;
    }
    FILE *input = fopen(file_name, "rb");
    if (input == NULL) {
        return NULL;
    }
    bmp_t *image = malloc(sizeof(bmp_t));
    if (image == NULL) {
        return NULL;
    }
    size_t return_value = fread(image, sizeof(int8_t), 54, input);
    if (return_value != 54) {
        free(image);
        return NULL;
    }
    image->pixels = malloc(sizeof(int8_t **) * image->bi_height);
    if (image->pixels == NULL) {
        free(image);
        return NULL;
    }
    int8_t *pixels = malloc((image->bf_size - 54) * sizeof(int8_t));
    if (pixels == NULL) {
        free(image->pixels);
        free(image);
        return NULL;
    }
    return_value = fread(pixels, sizeof(int8_t), image->bf_size - 54, input);
    if (return_value != (size_t) image->bf_size - 54) {
        del_bmp(image);
        return NULL;
    }
    fclose(input);
    int32_t line_size = (image->bf_size - 54) / image->bi_height;
    for (int32_t i = 0; i < image->bi_height; ++i) {
        image->pixels[i] = pixels + i * line_size;
    }
    return image;
}

bmp_t *crop(const bmp_t *image, int32_t x, int32_t y, int32_t w, int32_t h) {
    assert(image);
    y = image->bi_height - y - h;
    bmp_t *cropped_image = malloc(sizeof(bmp_t));
    if (cropped_image == NULL) {
        return NULL;
    }
    *cropped_image = *image;
    cropped_image->bi_size_image = align_width(w * (cropped_image->bi_bit_count / 8)) * h;
    cropped_image->bf_size = cropped_image->bi_size_image + 54;
    cropped_image->bi_width = w;
    cropped_image->bi_height = h;
    cropped_image->pixels = malloc(sizeof(int8_t **) * h);
    if (cropped_image->pixels == NULL) {
        free(cropped_image);
        return NULL;
    }
    int8_t *pixels = malloc(w * h * (cropped_image->bi_bit_count / 8) * sizeof(int8_t));
    if (pixels == NULL) {
        free(cropped_image->pixels);
        free(cropped_image);
        return NULL;
    }
    for (int32_t i = 0; i < h; ++i) {
        cropped_image->pixels[i] = pixels + i * w * (cropped_image->bi_bit_count / 8);
    }
    for (int32_t i = 0; i < h; ++i) {
        memcpy(cropped_image->pixels[i], image->pixels[y + i] + x * (cropped_image->bi_bit_count / 8),
               w * sizeof(int8_t) * (cropped_image->bi_bit_count / 8));
    }
    return cropped_image;
}

bmp_t *rotate(const bmp_t *image) {
    assert(image);
    bmp_t *rotated_image = malloc(sizeof(bmp_t));
    if (rotated_image == NULL) {
        return NULL;
    }
    *rotated_image = *image;
    rotated_image->bi_height = image->bi_width;
    rotated_image->bi_width = image->bi_height;
    rotated_image->bi_size_image =
            align_width(rotated_image->bi_width * (rotated_image->bi_bit_count / 8)) * rotated_image->bi_height;
    rotated_image->bf_size = rotated_image->bi_size_image + 54;
    rotated_image->pixels = malloc(sizeof(int8_t **) * rotated_image->bi_height);
    if (rotated_image->pixels == NULL) {
        free(rotated_image);
        return NULL;
    }
    int8_t *pixels = malloc(
            rotated_image->bi_height * rotated_image->bi_width * (rotated_image->bi_bit_count / 8) * sizeof(int8_t));
    if (pixels == NULL) {
        free(rotated_image->pixels);
        free(rotated_image);
        return NULL;
    }
    for (int32_t i = 0; i < rotated_image->bi_height; ++i) {
        rotated_image->pixels[i] = pixels + i * rotated_image->bi_width * (rotated_image->bi_bit_count / 8);
    }
    for (int32_t i = 0; i < rotated_image->bi_height; ++i) {
        for (int32_t j = 0; j < rotated_image->bi_width; ++j) {
            memcpy(rotated_image->pixels[rotated_image->bi_height - 1 - i] + j * (rotated_image->bi_bit_count / 8),
                   image->pixels[j] + i * (rotated_image->bi_bit_count / 8),
                   (image->bi_bit_count / 8) * sizeof(int8_t));
        }
    }
    return rotated_image;
}

int save_bmp(const bmp_t *image, char *file_name) {
    assert(image);
    FILE *output = fopen(file_name, "wb");
    if (output == NULL) {
        fclose(output);
        return 1;
    }
    size_t return_value = fwrite(image, sizeof(int8_t), 54, output);
    if (return_value != 54) {
        fclose(output);
        return 1;
    }
    int32_t line_end_size =
            align_width(image->bi_width * (image->bi_bit_count / 8)) - image->bi_width * (image->bi_bit_count / 8);
    char *line_end = malloc(line_end_size * sizeof(int8_t));
    if (line_end == NULL) {
        free(line_end);
        fclose(output);
        return 1;
    }
    memset(line_end, 0, line_end_size);
    for (int32_t i = 0; i < image->bi_height; ++i) {
        return_value = fwrite(image->pixels[i], sizeof(int8_t) * (image->bi_bit_count / 8), image->bi_width, output);
        if (return_value != (size_t) image->bi_width) {
            free(line_end);
            fclose(output);
            return 1;
        }
        return_value = fwrite(line_end, sizeof(int8_t), line_end_size, output);
        if (return_value != (size_t) line_end_size) {
            free(line_end);
            fclose(output);
            return 1;
        }
    }
    free(line_end);
    fclose(output);
    return 0;
}

void del_bmp(bmp_t *image) {
    assert(image);
    free(image->pixels[0]);
    free(image->pixels);
    free(image);
}
