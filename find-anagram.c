#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/stat.h>

void find_line_start(FILE *fp, off_t* off)
{
    off_t pos = *off;
    while (pos) {
        pos --;
        fseeko(fp, pos, SEEK_SET);
        int ch = fgetc(fp);
        if (ch == '\n') {
            pos ++;
            break;
        }
    }

    *off = pos;
}

int comp_char(const void *a, const void *b)
{
    return *(char *)a - *(char *)b;
}

int cmp_str(const char *a, const char *b)
{
    while (*a != '\0' && *b != '\0' && *a == *b) {
        a ++;
        b ++;
    }

    return *a - *b;
}

void search_dict(FILE *fp, off_t size, const char *word)
{
    char search[256];
    strncpy(search, word, sizeof(search)-1);
    search[sizeof(search)-1] = '\0';
    qsort(&search[0], strlen(search), 1, comp_char);

    char key[256];
    char buf[256];
    off_t start = 0;
    off_t end   = size;
    do {
        //printf("Search %llu to %llu\n", start, end);
        off_t mid   = (start + end)/2;
        fseeko(fp, mid, SEEK_SET);
        find_line_start(fp, &mid);
        if (fscanf(fp, "%s %s\n", &key[0], &buf[0]) == EOF) {
            break;
        }
        int cmp = cmp_str(&search[0], &key[0]);
        if (cmp > 0) {
            start = mid + strlen(key) + strlen(buf) + 2;
        } else if (cmp <= 0) {
            end = mid;
        }
    } while (end > start);
    
    if (start < size) {
        fseeko(fp, start, SEEK_SET);
        while (fscanf(fp, "%s %s\n", &key[0], &buf[0]) != EOF &&
               !cmp_str(&search[0], &key[0])) {
            printf("anagram of %s :%s\n", word, buf);
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        printf("this-program input-file search-word\n");
        return -1;
    }

    FILE *fp = fopen(argv[1], "rb");
    struct stat sbuf;
    fstat(fileno(fp), &sbuf);
    off_t sz = sbuf.st_size;

    search_dict(fp, sz, argv[2]);

    return 0;
}
