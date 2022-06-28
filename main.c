#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "bwts.h"

#define INITIAL_BUFF_SIZE 512

int main(int argc, char **argv) {
    int seed = time(NULL);
    int skip = 0;
    int flips = 1;

    unsigned char *in_data;
    unsigned char *compressed;
    unsigned char *result;
    int size = 0;
    int allocated = 0;

    FILE *in = stdin, *out = stdout;

    int opt;
    while ((opt = getopt(argc, argv, ":i:o:s:f:k:")) != -1) {
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
        case 'f':
            flips = strtoul(optarg, NULL, 10);
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
                    " -f <number of bytes to change>\n"
                    " -k <number of bytes to skip>\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
    }

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

    if (compressed == NULL || result == NULL) {
        fprintf(stderr, "Error alocating memory!\n");
        exit(EXIT_FAILURE);
    }

    bwts(&in_data[skip], &compressed[skip], size - skip);

    for (int i = 0; i < flips; i++) {
        int location = skip + lrand48() % (size - skip);
        compressed[location] += 1;
    }

    unbwts(&compressed[skip], &result[skip], size - skip);

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
