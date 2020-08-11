#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void crop_rotate_save(char *const *argv);

int main(int argc, char **argv) {
    if (argc != 8) {
        printf("Invalid input\n");
        exit(1);
    }
    if (strcmp(argv[1], "crop-rotate") == 0) {
        crop_rotate_save(argv + 2);
    } else {
        printf("Unknown operation: %s\n", argv[1]);
        exit(1);
    }
}

void crop_rotate_save(char *const *argv) {
    bmp_t *input_img = load_bmp(argv[0]);
    if (input_img == NULL) {
        printf("File reading failed\n");
        exit(1);
    }
    int x = atoi(argv[2]);
    if (x < 0 || x >= input_img->bi_width) {
        printf("Invalid argument x: %d\n", x);
        exit(1);
    }
    int y = atoi(argv[3]);
    if (y < 0 || y >= input_img->bi_height) {
        printf("Invalid argument y: %d\n", y);
        exit(1);
    }
    int w = atoi(argv[4]);
    if (w <= 0 || x + w > input_img->bi_width) {
        printf("Invalid argument w: %d\n", w);
        exit(1);
    }
    int h = atoi(argv[5]);
    if (h <= 0 || y + h > input_img->bi_height) {
        printf("Invalid argument h: %d\n", h);
        exit(1);
    }
    bmp_t *cropped_img = crop(input_img, x, y, w, h);
    del_bmp(input_img);
    if (cropped_img == NULL) {
        printf("Crop failed\n");
        exit(1);
    }
    bmp_t *crop_rotated_img = rotate(cropped_img);
    del_bmp(cropped_img);
    if (crop_rotated_img == NULL) {
        printf("Rotate failed\n");
        exit(1);
    }
    int return_value = save_bmp(crop_rotated_img, argv[1]);
    del_bmp(crop_rotated_img);
    if (return_value != 0) {
        printf("Save failed\n");
        exit(1);
    }
}

