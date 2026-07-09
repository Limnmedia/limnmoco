// SPDX-License-Identifier: BSD-3-Clause

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "conv.hpp"

void test_convu(uint32_t);
void test_convi(int32_t);
void test_convf(float);

int main() {
    srand((uint32_t)time(nullptr));
    test_convu((uint32_t)rand());
    test_convi((int32_t)rand());
    test_convf((float)rand() / 10000.0);
    test_convf(-((float)rand() / 10000.0));
}

void test_convu(uint32_t n) {
    uint32_t limit = strlenu(n);
    char n_buffer[limit];
    memset(n_buffer, 0, limit);
    convu((uint8_t *)n_buffer, limit, n, 10);

    char n_test[limit];
    memset(n_test, 0, limit);
    snprintf(n_test, limit, "%u", n);

    int diff = strncmp(n_buffer, n_test, limit);
    if (diff == 0) {
        puts("convu passed!\n");
    } else {
        printf("convu failed!\nconvu: [%s]\nsnprintf: [%s]\n", n_buffer, n_test);
    }
}

void test_convi(int32_t n) {
    uint32_t limit = strleni(n);
    char n_buffer[limit];
    memset(n_buffer, 0, limit);
    convi((uint8_t *)n_buffer, limit, n, 10);

    char n_test[limit];
    memset(n_test, 0, limit);
    snprintf(n_test, limit, "%i", n);

    int diff = strncmp(n_buffer, n_test, limit);
    if (diff == 0) {
        puts("convi passed!\n");
    } else {
        printf("convi failed!\nconvu: [%s]\nsnprintf: [%s]\n", n_buffer, n_test);
    }
}

void test_convf(float n) {
    uint32_t limit = strlenf(n);
    char n_buffer[limit];
    memset(n_buffer, 0, limit);
    convf((uint8_t *)n_buffer, limit, n);

    char n_test[limit];
    memset(n_test, 0, limit);

    snprintf(n_test, limit, "%.6f", n);

    int diff = strncmp(n_buffer, n_test, limit);

    if (diff == 0) {
        printf("convf passed!\n");
    } else {
        printf("convf failed!\nconvf: [%s]\nsnprintf: [%s]\n", n_buffer, n_test);
    }
}

