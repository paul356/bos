#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void (*shift_method_type)(char *str, int shift);

void acrobat_method(char *str, int shift)
{
    int i, j;
    i = 0, j = 0;
    int len = strlen(str);
    while (i < len) {
        char *start = &str[j];
        int temp = str[j];
        int k = 1;
        while (start != &str[(shift*k+j)%len]) {
            str[(shift*k+j-shift)%len] = str[(shift*k+j)%len];
            k ++;
        }
        str[(shift*k+j-shift)%len] = temp;
        
        i += k;
        j ++;
    }
}

void sub_recursive(char *str, int left, int right, int end)
{
    while ((end - right) && (right - left)) {
        int llen = right - left;
        int rlen = end - right;
        int min  = llen < rlen ? llen : rlen;
        int max  = llen < rlen ? rlen : llen;

        for (int i = left; i < left+min; i ++) {
            int tmp = str[i];
            str[i] = str[max+i];
            str[max+i] = tmp;
        }

        if (llen > rlen) {
            //sub_recursive(str, left+min, right, end);
            left = left + min;
        } else {
            //sub_recursive(str, left, right, end-min);
            end = end - min;
        }
    }
}

void recursive_method(char *str, int shift)
{
    int len = strlen(str);
    sub_recursive(str, 0, shift, len);
}

void sub_reverse(char *str, int start, int end)
{
    int i, j;
    for (i=start,j=end; i < j; i++, j--) {
        char tmp;
        tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
    }
}

void reverse_method(char *str, int shift)
{
    int len = strlen(str);
    sub_reverse(str, 0, shift-1);
    sub_reverse(str, shift, len-1); 
    sub_reverse(str, 0, len-1);
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        printf("this-program string-file shift method\n");
        return -1;
    }

    shift_method_type arr[3] = {acrobat_method, recursive_method, reverse_method};
    char *rwStr = NULL;
    FILE* fin = fopen(argv[1], "rb");
    size_t lineCap = 0;
    int len = getline(&rwStr, &lineCap, fin);
    if (!rwStr) {
        printf("can't getline\n");
        return -1;
    }
    rwStr[len-1] = '\0';

    int shift = atoi(argv[2]); 
    int method = atoi(argv[3]);

    arr[method](rwStr, shift);
    printf("%s\n", rwStr);

    return 0;
}
