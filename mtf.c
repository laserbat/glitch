// Move-to-Front transform
#include <stdio.h>

void mtf(const unsigned char *in, unsigned char *out, int len) {
    unsigned char table[256];
    for (int i = 0; i < 256; i++) {
        table[i] = i;
    }

    for (int i = 0; i < len; i++) {
        for (int j = 0; j < 256; j++) {
            if (table[j] == in[i]) {
                out[i] = j;
                break;
            }
        }

        for (int j = out[i]; j > 0; j--) {
            table[j] = table[j - 1];
        }

        table[0] = in[i];
    }
}

void un_mtf(const unsigned char *in, unsigned char *out, int len) {
    unsigned char table[256];
    for (int i = 0; i < 256; i++) {
        table[i] = i;
    }

    for (int i = 0; i < len; i++) {
        out[i] = table[in[i]];
        for (int j = in[i]; j > 0; j--) {
            table[j] = table[j - 1];
        }

        table[0] = out[i];
    }
}
