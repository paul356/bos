#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swap(int *a, int *b) 
{
    int t;
    t = *a;
    *a = *b;
    *b = t;

    /*
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
    */
}

int randint(int start, int end)
{
    int tmp = rand() % (end - start);
    return tmp + start;
}

int main(int argc, char *argv[])
{
    int n, m, i;
    int *arr = NULL;
    FILE* fp;
    char fn[128];

    if (argc != 3) {
        printf("this-program n m\n");
        return -1;
    }

    n = atoi(argv[1]);
    m = atoi(argv[2]); 

    if (n <= 0 || m <= 0 || m > n) {
        printf("n, m should be positive and m <= n\n");
        return -1;
    }

    snprintf(&fn[0], sizeof(fn), "random-%d-%d", n, m);
    fp = fopen(&fn[0], "wb");
    if (!fp) {
        printf("failed to open file %s\n", &fn[0]);
        return -1;
    }

    arr = malloc(sizeof(int)*n);
    if (!arr) {
        printf("fail to allocate memory\n");
        return -1;
    }

    for (i=0; i<n; i++) {
        arr[i] = i;
    }
    srand(time(NULL));
    for (i=0; i<m; i++) {
        swap(&arr[i], &arr[randint(i, n)]);
        fprintf(fp, "%d\n", arr[i]);
    }

    free(arr);
    fclose(fp);

    return 0;
}
