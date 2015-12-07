#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if (argc != 3) {
        printf("this-program n dup-num-file\n");
        return -1;
    }

    int start = 0;
    int end   = atoi(argv[1]);
    if (end <= start) {
        printf("%s is not number\n", argv[1]);
        return -1;
    }

    FILE *fin = fopen(argv[2], "rb");
    if (!fin) {
        printf("fail to open %s\n", argv[2]);
        return -1;
    }

    const char *left = "left-file";
    const char *right= "right-file";
    const char *input= "input-file";
    FILE *fleft  = fopen(left, "wb");
    FILE *fright = fopen(right, "wb");
    int nleft, nright;
    nleft = nright = 0;
    while (end > (start + 1)) {
        int mid = (start + end) / 2;
        int num;
        nleft = nright = 0;
        while (fscanf(fin, "%d\n", &num) != EOF) {
            if (num < mid) {
                fprintf(fleft, "%d\n", num);
                nleft ++;
            } else {
                fprintf(fright, "%d\n", num);
                nright ++;
            }
        }

        fclose(fin);
        fclose(fleft);
        fclose(fright);
        if (nleft > (mid - start)) {
            end = mid;
            rename(left, input);
        } else if (nright > (end - mid)) {
            start = mid;
            rename(right, input);
        } else {
            printf("can't find such dup number\n");
            return -1;
        }
        fleft = fopen(left, "wb");
        fright = fopen(right, "wb");
        fin = fopen(input, "rb");
    }

    if (end == (start + 1)) {
        printf("%d\n", start);
    }

    fclose(fin);
    fclose(fleft);
    fclose(fright);

    return 0;
}
