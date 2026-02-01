#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// #define DEBUG
#define CACHE_SIZE 64
#define CACHE_POWER 6


int n = 0;
int *A = NULL;
int *B = NULL;
int *C = NULL;
int size = 0;

int main(int argc, char* argv[]) {
    int thread_count = 8;
    if (argc >= 2) {
        thread_count = atoi(argv[1]);
    }

    // read file
    char filename[1000] = {0};
    scanf("%s", filename);

    FILE* file = fopen(filename, "r");
    fscanf(file, "%d", &n);
    
    // get size
    long file_start = ftell(file);
    fseek(file, 0, SEEK_END);
    long file_end = ftell(file);
    fseek(file, file_start, SEEK_SET);

    // buffer whole file
    int file_size = file_end - file_start;
    char *buffer = (char*)malloc(file_size + 1);
    fread(buffer, sizeof(char), file_size, file);
    buffer[file_size] = '\0';

    char *buffer_pointer = buffer;
    fclose(file);

    // init array memory
    size = n * n;
    A = malloc(size * sizeof(int));
    B = malloc(size * sizeof(int));
    C = calloc(size, sizeof(int));

    // read array A
    for (int y = 0; y < size; y += n) {
        for (int x = 0; x < n; x++) {
            A[y + x] = strtol(buffer_pointer, &buffer_pointer, 10);
            // fscanf(file, "%d", A + y + x);
        }
    }
    // read array B
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < size; y += n) {
            B[y + x] = strtol(buffer_pointer, &buffer_pointer, 10);
            // fscanf(file, "%d", B + y + x);
        }
    }
    free(buffer);
    // fclose(file);
    
#ifdef DEBUG
    printf("A\n"); 
    for (int y = 0; y < size; y += n) {
        for (int x = 0; x < n; x++) {
            printf("%d ", A[y + x]);
        }
        printf("\n");
    }
    printf("B\n"); 
    for (int y = 0; y < size; y += n) {
        for (int x = 0; x < n; x++) {
            printf("%d ", B[y + x]);
        }
        printf("\n");
    }
#endif
    // run
    // /*
    #pragma omp parallel for num_threads(thread_count) shared(C, A, B, size, n) schedule(static)
    for (int y_ = 0; y_ < size; y_ += n << CACHE_POWER) {
        for (int x_ = 0, yB_ = 0; x_ < n; x_ += CACHE_SIZE, yB_ += n << CACHE_POWER) {
            for (int y = y_; y < y_ + (n << CACHE_POWER) && y < size; y += n) {
                for (int x = x_, yB = yB_; x < x_ + CACHE_SIZE && x < n; x++, yB += n) {
                    for (int xA = 0; xA < n; xA++) {
                        C[y + x] += A[y + xA] * B[yB + xA];
                    }
                }
            }
        }
    }
    /*/
    #pragma omp parallel for num_threads(thread_count) collapse(2) shared(C, A, B, size, n) schedule(auto)
    for (int y = 0; y < size; y += n) {
        for (int x = 0; x < n; x++) {
            int yB = x * n;
            for (int xA = 0; xA < n; xA++) {
                C[y + x] += A[y + xA] * B[yB + xA];
            }
        }
    }
    //*/
    free(A);
    free(B);


    // output
#ifdef DEBUG
    printf("C\n"); 
    for (int y = 0; y < size; y += n) {
        for (int x = 0; x < n; x++) {
            printf("%d ", C[y + x]);
        }
        printf("\n");
    }
#endif

    int result = 0;
    // #pragma omp parallel for num_threads(thread_count) shared(C, size, n) reduction(^: result) schedule(static)
    for (int i = 0; i < size; i++) {
        result ^= C[i];
    }
    printf("%d", result);
    free(C);

    return 0;
}
