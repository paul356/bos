#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>

int compare_func(const void *a, const void *b)
{
    return *(char *)a - *(char *)b;
}

void get_key_for_file(const char *fn, const char *out)
{
    char word[256] = {'\0'};
    char key[256]  = {'\0'};

    FILE *fp = fopen(fn, "rb");
    if (!fp) {
        printf("can't open %s\n", fn);
        return;
    }
    FILE *outFp = fopen(out, "wb");
    if (!outFp) {
        printf("can't open %s\n", out);
        fclose(fp);
        return;
    }

    while (fscanf(fp, "%s", &word[0]) != EOF) {
        int i = 0;
        while (i < sizeof(word) && word[i] != '\0') {
            key[i] = tolower(word[i]);
            i ++;
        }
        if (i == sizeof(word)) {
            key[i-1] = '\0';
        } else {
            key[i] = '\0';
        }

        qsort(&key[0], i, 1, compare_func);
        fprintf(outFp, "%s %s\n", &key[0], &word[0]);
    }

    fclose(fp);
    fclose(outFp);
}

int cmp_str(const char *a, const char *b)
{
    while (*a != '\0' && *b != '\0' && *a == *b) {
        a ++;
        b ++;
    }

    return *a - *b;
}

// Use merge and few memory to do the external sort
const char *sort_file_with_key(const char * fname)
{
#define FILE_TEMPL "temp-file"
    // Four files
    const char * fns[] = {FILE_TEMPL "0", FILE_TEMPL "1", FILE_TEMPL "2", FILE_TEMPL "3"};
    int   nwords = 0;

    FILE *pSrc[2], *pDst[2];
    char key0[256], word0[256];
    char key1[256], word1[256];
    int  alternate = 0;
    pSrc[0] = fopen(fname, "r");
    pDst[0] = fopen(fns[2], "w+");
    pDst[1] = fopen(fns[3], "w+");
    while (fscanf(pSrc[0], "%s %s\n", &key0[0], &word0[0]) != EOF) {
        nwords ++;

        int eof = fscanf(pSrc[0], "%s %s\n", &key1[0], &word1[0]);                
        if (eof == EOF) {
            fprintf(pDst[alternate], "%s %s\n", key0, word0);
            break;
        }
        nwords ++;
        
        if (cmp_str(&key0[0], &key1[0]) > 0) {
            fprintf(pDst[alternate], "%s %s\n", &key1[0], &word1[0]);
            fprintf(pDst[alternate], "%s %s\n", &key0[0], &word0[0]);
        } else {
            fprintf(pDst[alternate], "%s %s\n", &key0[0], &word0[0]);
            fprintf(pDst[alternate], "%s %s\n", &key1[0], &word1[0]);
        }

        alternate ^= 1;
    }
    rewind(pDst[0]);
    rewind(pDst[1]);
    fclose(pSrc[0]);

    int width = 2;
    int srcIdx = 2;
    pSrc[0] = pDst[0];
    pSrc[1] = pDst[1];
    pDst[0] = fopen(fns[(srcIdx^0x2)], "w+");
    pDst[1] = fopen(fns[(srcIdx^0x2)+1], "w+");
    while (width < nwords) {
        int eof0, eof1;
        int sub0, sub1;
        char read0, read1;
        alternate = 0;
        eof0 = eof1 = !EOF;
        while (eof0 != EOF || eof1 != EOF) {
            sub0 = sub1 = 0;
            read0 = read1 = 1;
            while(1) {
                if (read0 && eof0 != EOF) {
                    if (sub0 < width) {
                        eof0 = fscanf(pSrc[0], "%s %s\n", &key0[0], &word0[0]);
                    }
                    read0 = 0;
                    sub0 ++;
                }
                if (read1 && eof1 != EOF) {
                    if (sub1 < width) {
                        eof1 = fscanf(pSrc[1], "%s %s\n", &key1[0], &word1[0]);
                    }
                    read1 = 0;
                    sub1 ++;
                }

                if ((eof0 == EOF || sub0 > width) && (eof1 == EOF || sub1 > width)) {
                    break;
                }

                if (eof0 == EOF || sub0 > width) {
                    fprintf(pDst[alternate], "%s %s\n", key1, word1);
                    read1 = 1;
                } else if (eof1 == EOF || sub1 > width) {
                    fprintf(pDst[alternate], "%s %s\n", key0, word0);
                    read0 = 1;
                } else {
                    int cmp = cmp_str(key0, key1);
                    if (cmp > 0) {
                        fprintf(pDst[alternate], "%s %s\n", key1, word1);
                        read1 = 1;
                    } else {
                        fprintf(pDst[alternate], "%s %s\n", key0, word0);
                        read0 = 1;
                    }
                }

                assert(read1 ^ read0);
            } 
            alternate ^= 1;
        }
        
        rewind(pDst[0]);
        rewind(pDst[1]);
        fclose(pSrc[0]);
        fclose(pSrc[1]);
        srcIdx ^= 2;
        pSrc[0] = pDst[0];
        pSrc[1] = pDst[1];
        pDst[0] = fopen(fns[(srcIdx^0x2)], "w+");
        pDst[1] = fopen(fns[(srcIdx^0x2)+1], "w+");
        width *= 2;
    }

    fclose(pSrc[0]);
    fclose(pSrc[1]);
    fclose(pDst[0]);
    fclose(pDst[1]);

    unlink(fname);
    rename(fns[srcIdx], fname);
    for (int i = 0; i < 4; i ++) {
        unlink(fns[i]);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    
    if (argc < 2) {
        printf("this-program input-file\n");
        return -1;
    }
    
    const char *interimFile = "prefix-with-key";
    get_key_for_file(argv[1], interimFile);

    sort_file_with_key(interimFile);
    
    return 0;
}
