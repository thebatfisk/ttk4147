#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h> 

#include "array.h"

int main()
{
    // long xy_size = 1000 * 1000 * 570; // 8 GB (sizeof(long) = 8 bytes) 570
    // long x_dim = 10000;
    // long y_dim = xy_size / x_dim;

    // long **matrix = malloc(y_dim * sizeof(long *));

    // for (long y = 0; y < y_dim; y++)
    // {
    //     matrix[y] = malloc(x_dim * sizeof(long));
    //     memset(matrix[y], 0, x_dim * sizeof(long));
    // }

    // printf("Allocation complete (press any key to continue...)\n");
    // getchar();

    Array array = array_new(10);

    for (int i = 0; i < 15; i++) { // 1000 * 1000 * 300
        array_insertBack(&array, 5, 1.5);
    }

    // array_print(array);

    // free(array.data);

    return 0;
}
