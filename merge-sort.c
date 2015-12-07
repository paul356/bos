#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

void sub_merge(int* arr, int left, int right, int end, int *out)
{
    int i0, i1, j;
    i0 = left, i1 = right;

    j = left;
    while (j < end) {
        if (i0 < right && (i1 >= end || arr[i0] <= arr[i1])) {
            out[j] = arr[i0];
            i0 ++;
        } else {
            out[j] = arr[i1];
            i1 ++;
        }
        j ++;
    }
}

void merge_sort(int* arr, int len)
{
    int* helpArr = malloc(sizeof(int)*len);
    int width = 1;
    int* pSrc = arr;
    int* pDst = helpArr;
    while (width < len) {
        for (int i = 0; i < len; i = i+2*width) {
            sub_merge(pSrc, i, MIN(i+width, len), MIN(i+2*width, len), pDst);
        }

        width *= 2;
        int *tmp = pSrc;
        pSrc = pDst;
        pDst = tmp;
    }

    if (pSrc != arr)
        memcpy(arr, pSrc, sizeof(int)*len);
    free(helpArr);
}


void display(int* arr, int len) {
    printf("arr = {");
    for (int i = 0; i < len; i ++) {
        printf("%d, ", arr[i]);
    }
    printf("}\n");
}

int main(int argc, char* argv[])
{
    int arr[] = {2, 1, 0, 5, 4, 3, -1, -99};
    display(arr, sizeof(arr)/sizeof(arr[0]));
    merge_sort(arr, sizeof(arr)/sizeof(arr[0]));
    display(arr, sizeof(arr)/sizeof(arr[0]));
    return 0;
}
