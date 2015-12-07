#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#define MAX_ARRAY_BYTE 1024*1024

void scanRandomSample(int n, char *arr, int r, char *sample, char *temp, char *output)
{
    int num;

    FILE *intermFp = fopen(temp, "wb");
    if (!intermFp) {
        printf("fail to open interim file %s\n", temp);
        return;
    }
    FILE *sampleFp = fopen(sample, "rb");
    if (!sampleFp) {
        printf("fail to open sample file %s\n", sample);
        fclose(intermFp);
        return;
    }

    bzero(arr, MAX_ARRAY_BYTE);
    while (fscanf(sampleFp, "%d\n", &num) != EOF) {
        if (num >= r*MAX_ARRAY_BYTE*8 && num < (r+1)*MAX_ARRAY_BYTE*8) {
            arr[(num - r*MAX_ARRAY_BYTE*8) >> 3] |= (1 << ((num - r*MAX_ARRAY_BYTE*8)&0x7));
        } else {
            fprintf(intermFp, "%d\n", num);
        }
    }
    fclose(sampleFp);
    fclose(intermFp);

    FILE *outputFp = fopen(output, "ab");
    if (!outputFp) {
        printf("fail to open output file %s\n", output);
        return;
    }
    for (int i=0; i<MAX_ARRAY_BYTE*8; i ++) {
        if (arr[i>>3] & (1 << (i & 0x7))) {
            fprintf(outputFp, "%d\n", i+r*MAX_ARRAY_BYTE*8);
        }
    }

    fclose(outputFp);
}

int main(int argc, char *argv[])
{
    int i, n;
    char *arr;
    char *sampleFile = argv[2];
    char *tempFile = "tmpfile";
    char *tempFile1= "tmpfile1";
    char *output   = "outfile";
    int requiredBytes;

    if (argc != 3) {
        printf("this-program number-range sample-file\n");
        return -1;
    }

    n = atoi(argv[1]);
    if (n <= 0) {
        printf("n should be positive\n");
        return -1;
    }

    arr = malloc(MAX_ARRAY_BYTE);
    if (!arr) {
        printf("fail to allocate %d bytes\n", MAX_ARRAY_BYTE); return -1;
        return -1;
    }

    // delete file
    unlink(output);

    int round = 0;
    requiredBytes = (n + 7)/ 8;
    scanRandomSample(n, arr, round, sampleFile, tempFile, output);
    round += 1;
    requiredBytes -= MAX_ARRAY_BYTE;
    sampleFile = tempFile;
    tempFile   = tempFile1;
    while (requiredBytes > 0) {
        scanRandomSample(n, arr, round, sampleFile, tempFile, output);
        round += 1;
        requiredBytes -= MAX_ARRAY_BYTE;
        // swap sampleFile and tempFile
        tempFile1 = sampleFile; 
        sampleFile = tempFile;
        tempFile  = tempFile1;
    }

    free(arr);
    return 0;
}
