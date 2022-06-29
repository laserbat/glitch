#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "libsais/src/libsais.h"

#define INITIAL_BUFF_SIZE 512

int main(int argc, char **argv) {
    int seed = time(NULL);
    int skip = 0;
    double level = 10;

    unsigned char *in_data;
    unsigned char *compressed;
    unsigned char *result;
    int *temp;
    int size = 0;
    int allocated = 0;
    int primary_index;

    FILE *in = stdin, *out = stdout;

    int opt;
    while ((opt = getopt(argc, argv, ":i:o:s:l:k:")) != -1) {
        switch (opt) {
        case 's':
            seed = strtoul(optarg, NULL, 10);
            break;
        case 'i':
            in = fopen(optarg, "rb");
            if (in == NULL) {
                fprintf(stderr, "Cannot access specified file!\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'o':
            out = fopen(optarg, "wb");
            if (in == NULL) {
                fprintf(stderr, "Cannot access specified file!\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'l':
            level = strtoul(optarg, NULL, 10);
            if (level < 0)
                level = 0;
            if (level > 1000)
                level = 1000;
            break;
        case 'k':
            skip = strtoul(optarg, NULL, 10);
            break;
        default:
            fprintf(stderr,
                    "Usage: %s [options]\n"
                    "Options:\n"
                    " -i <in file>\n"
                    " -o <out file>\n"
                    " -s <seed>\n"
                    " -l <level of glitching 0-1000>\n"
                    " -k <number of bytes to skip>\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    level /= 1000;

    srand48(seed);

    in_data = malloc(INITIAL_BUFF_SIZE);
    if (in_data == NULL) {
        fprintf(stderr, "Error alocating memory!\n");
        exit(EXIT_FAILURE);
    }
    allocated = INITIAL_BUFF_SIZE;

    while (true) {
        int input = getc(in);
        if (input == EOF)
            break;
        in_data[size] = input;
        size += 1;

        if (size >= allocated) {
            in_data = realloc(in_data, allocated + (allocated >> 1));
            if (in_data == NULL) {
                fprintf(stderr, "Error alocating memory!\n");
                fprintf(stderr, "%d %d\n", size, allocated);
                exit(EXIT_FAILURE);
            }

            allocated = allocated + (allocated >> 1);
        }
    }

    if (skip >= size) {
        fprintf(stderr, "Skip is larger than file size!\n");
        exit(EXIT_FAILURE);
    }

    compressed = malloc(size);
    result = malloc(size);
    temp = malloc(size * sizeof(int));

    if (compressed == NULL || result == NULL) {
        fprintf(stderr, "Error alocating memory!\n");
        exit(EXIT_FAILURE);
    }

    primary_index = libsais_bwt(&in_data[skip], &compressed[skip], temp, size - skip, skip, NULL);

    primary_index = primary_index * (1 - level) + ((drand48() - 0.5) * (size - skip)) * level;

    primary_index += size - skip;
    primary_index %= size - skip;

    libsais_unbwt(&compressed[skip], &result[skip], temp, size - skip, NULL, primary_index);

    for (int i = 0; i < skip; i++) {
        result[i] = in_data[i];
    }

    fwrite(result, sizeof(char), size, out);

    free(in_data);
    free(compressed);
    free(result);

    fclose(in);
    fclose(out);

    return 0;
}
