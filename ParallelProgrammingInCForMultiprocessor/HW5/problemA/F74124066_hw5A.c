#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//#define DEBUG
#define CACHE_SIZE 64
#define CACHE_POWER 6


int n = 0;
long long *A = NULL;
long long *B = NULL;
long long *C = NULL;
int* interval = NULL;
int size = 0;


void* thread_function(void *argv) {
    int rank = *(int*)argv;
    //*
    const int y_end = interval[rank+1] * n;
    for (int y_ = interval[rank] * n; y_ < y_end; y_ += n << CACHE_POWER) {
        for (int x_ = 0, yB_ = 0; x_ < n; x_ += CACHE_SIZE, yB_ += n << CACHE_POWER) {
            for (int y = y_; y < y_ + (n << CACHE_POWER) && y < y_end; y += n) {
                for (int x = x_, yB = yB_; x < x_ + CACHE_SIZE && x < n; x++, yB += n) {
                    for (int xA = 0; xA < n; xA++) {
                        C[y + x] += A[y + xA] * B[yB + xA];
                    }
                }
            }
        }
    }
    /*/
    const int y_end = interval[rank+1] * n;
    for (int y = interval[rank] * n; y < y_end; y += n) {
        for (int x = 0, yB = 0; x < n; x++, yB += n) {
            for (int xA = 0; xA < n; xA++) {
                C[y + x] += A[y + xA] * B[yB + xA];
            }
        }
    }
    //*/
    return NULL;
}


int main(int argc, char* argv[]) {
    // read file
    char filename[1000] = {0};
    scanf("%s", filename);

    FILE* file = fopen(filename, "r");
    fscanf(file, "%d", &n);

    // init array memory
    size = n * n;
    A = malloc(size * sizeof(long long));
    B = malloc(size * sizeof(long long));
    C = calloc(size, sizeof(long long));

    // read array A
    for (int y = 0; y < size; y += n) {
        int coef = 0;
        int bias = 0;
        int div = 0;
        fscanf(file, "%lld %d %d %d", A + y, &coef, &bias, &div);
        for (int x = 1; x < n; x++) {
            A[y + x] = (coef * A[y + x - 1] + bias) % div;
        }
    }
    // read array B
    //for (int y = 0; y < size; y += n) {
    for (int x = 0; x < n; x++) {
        int coef = 0;
        int bias = 0;
        int div = 0;
        //fscanf(file, "%lld %d %d %d", B + y, &coef, &bias, &div);
        fscanf(file, "%lld %d %d %d", B + x, &coef, &bias, &div);
        //for (int x = 1; x < n; x++) {
        for (int y = n; y < size; y += n) {
            //B[x + y] = (coef * B[x + y - 1] + bias) % div;
            B[x + y] = (coef * B[x + y - n] + bias) % div;
        }
    }
    fclose(file);
    
#ifdef DEBUG
    printf("A\n"); 
    for (int y = 0; y < size; y += n) {
        for (int x = 0; x < n; x++) {
            printf("%lld ", A[y + x]);
        }
        printf("\n");
    }
    printf("B\n"); 
    for (int y = 0; y < size; y += n) {
        for (int x = 0; x < n; x++) {
            printf("%lld ", B[y + x]);
        }
        printf("\n");
    }
#endif


    // init thread & run thread
    int thread_count = atoi(argv[1]);

    int unit = n / thread_count + 1;
    interval = malloc((thread_count + 1) * sizeof(int));
    interval[0] = 0;
    interval[thread_count] = n;
    for (int i = 1; i < thread_count; i++) {
        interval[i] = i * unit;
        if (interval[i] >= n) {
            interval[i] = n;
        }
    }

    pthread_t* handler = malloc(thread_count * sizeof(pthread_t));
    int* thread_args = malloc(thread_count * sizeof(int));
    for (int rank = 0; rank < thread_count; rank++) {
        thread_args[rank] = rank;
        pthread_create(
                handler + rank, 
                NULL, 
                thread_function, 
                thread_args + rank);
    } 


    // wait and collect data
    for (int rank = 0; rank < thread_count; rank++) {
        pthread_join(handler[rank], NULL);
    }
    free(handler);
    free(thread_args);
    free(A);
    free(B);


    // output
#ifdef DEBUG
    printf("C\n"); 
    for (int y = 0; y < size; y += n) {
        for (int x = 0; x < n; x++) {
            printf("%lld ", C[y + x]);
        }
        printf("\n");
    }
#endif

    for (int y = 0; y < size; y += n) {
        long long result = C[y];
        for (int x = 1; x < n; x++) {
            result ^= C[y + x];
        }
        
        if (y + n == size) {
            printf("%lld", result);
        } else {
            printf("%lld\n", result);
        }
    }
    free(C);

    return 0;
}
