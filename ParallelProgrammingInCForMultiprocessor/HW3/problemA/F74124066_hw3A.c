#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>


#define THRESHOLD 1e-12
#define MAX_LINE_LENGTH 26000
//#define DEBUG


inline static double** find_great_pivot(double** augmented_matrix, int n, int rank, int current_row, int head_x) {
    int start = current_row + ((4 + rank - (current_row & 3)) & 3);
    double** current_max = augmented_matrix + start;
    if (*(*current_max + head_x) >= 1 || *(*current_max + head_x) <= -1) {
        return current_max;
    }
    for (int y = start + 4; y < n; y += 4) {
        if (augmented_matrix[y] != NULL) {
            double** target = augmented_matrix + y;
            if (*(*target + head_x) >= 1 || *(*target + head_x) <= -1) {
                return target;
            }
            if (*(*target + head_x) > *(*current_max + head_x)) {
                current_max = target;
            }
        }
    }
    return current_max;
}

inline static void exchange_row(double** augmented_matrix, double** row1, double** row2) {
    double* temp = *row1;
    *row1 = *row2;
    *row2 = temp;
}

int main(int argc, char* argv[]) {
    // MPI init
    int comm_sz;
    int rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Request request;


    // get input file
    char filename[1000] = {0};
    if (rank == 0) {
        scanf("%s", filename);
    }
    MPI_Bcast(filename, 1000, MPI_CHAR, 0, MPI_COMM_WORLD);


    // get input
    FILE* file = fopen(filename, "r");
    int n = 0;
    fscanf(file, "%d", &n);
    fgetc(file);  // clear \n
    int partial_width = n + 1;
    MPI_Datatype MPI_VECTOR_N;
    int err = MPI_Type_contiguous(partial_width, MPI_DOUBLE, &MPI_VECTOR_N);
    MPI_Type_commit(&MPI_VECTOR_N);

    double** augmented_matrix = (double**) calloc(n, sizeof(double*));
    char line_buffer[MAX_LINE_LENGTH] = {0};
    for (int y = 0; y < n; y++) {
        fgets(line_buffer, MAX_LINE_LENGTH, file);
        if ((rank & 3) != rank) {
            continue;
        }
        char* line_pointer = line_buffer;

        augmented_matrix[y] = (double*) malloc(partial_width * sizeof(double));
        for (int x = 0; x <= n; x++) {
            double* current = augmented_matrix[y] + x;
            *current = strtol(line_pointer, &line_pointer, 10);  // p
            if (*line_pointer == '/') {
                int q = strtol(line_pointer + 1, &line_pointer, 10);  // q
                *current /= q;
            }
        }
    }
    fclose(file);

    #ifdef DEBUG
    for (int y = 0; y < n; y++) {
        if ((y & 3) == rank) {
            for (int x = 0; x < n + 1; x++) {
                printf("% f ", augmented_matrix[y][x]);
            }
            printf("\n");
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    printf("\n");
    MPI_Barrier(MPI_COMM_WORLD);
    #endif

   // calculate strict triangular form
    double* buffer = (double*) malloc(partial_width * sizeof(double));
    for (int pivot_row = 0; pivot_row < n - 1; pivot_row++) {
        if (pivot_row + ((4 + rank - (pivot_row & 3)) & 3) >= n) {
            int choose_rank = 0;
            MPI_Bcast(&choose_rank, 1, MPI_INT, (pivot_row & 3), MPI_COMM_WORLD);
            if (choose_rank != -1)
                MPI_Bcast(buffer, 1, MPI_VECTOR_N, choose_rank, MPI_COMM_WORLD);
            continue;
        }
        double** top_row_pointer = augmented_matrix + pivot_row + ((4 + rank - (pivot_row & 3)) & 3);

        double** pivot_row_pointer = find_great_pivot(augmented_matrix, n, rank, pivot_row, pivot_row);
        if (pivot_row_pointer != top_row_pointer) {  // local exchange
            exchange_row(augmented_matrix, top_row_pointer, pivot_row_pointer);
        }
        int choose_rank = rank;
        double pivot = *(*top_row_pointer + pivot_row);
        if ((pivot_row & 3) == rank) {  // find suitable pivot row
            for (int i = 0; i < 4; i++) {
                if (i == rank) continue;
                if (pivot_row + ((4 + i - (pivot_row & 3)) & 3) >= n) {
                    continue;
                }
                double temp = 0;
                MPI_Recv(&temp, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                if (pivot < temp) {
                    pivot = temp;
                    choose_rank = i;
                }
            }
            if (pivot <= THRESHOLD && pivot >= -THRESHOLD) {  // skip because everyone are too small
                choose_rank = -1;
            }
        }
        else {  // send their pivot
            MPI_Send(&pivot, 1, MPI_DOUBLE, (pivot_row & 3), 0, MPI_COMM_WORLD);
        }
        MPI_Bcast(&choose_rank, 1, MPI_INT, (pivot_row & 3), MPI_COMM_WORLD);

        if (choose_rank == -1) {  // skip because everyone are too small
            if ((pivot_row & 3) != 0 && ((n - 1) & ~3) >= pivot_row) {
                if (rank == 0) {
                    augmented_matrix[pivot_row] = (double*) malloc(partial_width * sizeof(double));
                    MPI_Recv(augmented_matrix[pivot_row], 1, MPI_VECTOR_N, (pivot_row & 3), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                }
                else if (rank == (pivot_row & 3)) {
                    MPI_Send(*top_row_pointer, 1, MPI_VECTOR_N, 0, 0, MPI_COMM_WORLD);
                }
            }
            continue;
        }

        // get pivot row
        if (choose_rank == rank) {
            memcpy(buffer, *top_row_pointer, partial_width * sizeof(double));
        }
        MPI_Bcast(buffer, 1, MPI_VECTOR_N, choose_rank, MPI_COMM_WORLD);

        // global exchange
        if ((pivot_row & 3) != rank && choose_rank == rank) {
            MPI_Recv(*top_row_pointer, 1, MPI_VECTOR_N, (pivot_row & 3), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else if ((pivot_row & 3) == rank && choose_rank != rank) {
            MPI_Isend(*top_row_pointer, 1, MPI_VECTOR_N, choose_rank, 0, MPI_COMM_WORLD, &request);
            memcpy(*top_row_pointer, buffer, partial_width * sizeof(double));
        }
        if (rank == 0 && (pivot_row & 3) != 0) {
            augmented_matrix[pivot_row] = (double*) malloc(partial_width * sizeof(double));
            memcpy(augmented_matrix[pivot_row], buffer, partial_width * sizeof(double));
        }

        // eliminate
        pivot = buffer[pivot_row];
        int start = pivot_row + ((4 + rank - (pivot_row & 3)) & 3) + (((pivot_row & 3) == rank) ? 4 : 0);
        for (int y = start; y < n; y += 4) {
            double* current_row_head = augmented_matrix[y] + pivot_row;
            if (*current_row_head <= THRESHOLD && *current_row_head >= -THRESHOLD) {  // skip because this row is too small
                continue;
            }
            double ratio = *current_row_head / pivot;
            *current_row_head = 0;
            for (int x = pivot_row + 1; x <= n; x++) {
                double pivot_row_x_value = buffer[x];
                augmented_matrix[y][x] = augmented_matrix[y][x] - pivot_row_x_value * ratio;
            }
        }
        
        /*
        #ifdef DEBUG
        if (rank == 0) {
            printf("%d\n", pivot_row);
            for (int y = 0; y < n; y++) {
                if (augmented_matrix[y] != NULL) {
                    for (int x = 0; x < n + 1; x++) {
                        printf("% f ", augmented_matrix[y][x]);
                    }
                } 
                printf("\n");
            }
            printf("\n");
        }
        #endif
        */
    }
 
    // get row and terminate
    if (rank == 0) {
        for (int i = ((n - 1) & ~3) + 1 ; i < n; i++) {
            double* buffer = (double*) malloc(partial_width * sizeof(double));
            MPI_Recv(buffer, 1, MPI_VECTOR_N, (i & 3), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            free(augmented_matrix[i]);
            augmented_matrix[i] = buffer;
        }
        MPI_Type_free(&MPI_VECTOR_N);
    }
    else {
        if (((n - 1) & ~3) + rank < n) {
            MPI_Send(augmented_matrix[((n - 1) & ~3) + rank], 1, MPI_VECTOR_N, 0, 0, MPI_COMM_WORLD);
        }
        MPI_Type_free(&MPI_VECTOR_N);
        MPI_Finalize();
        return 0;
    }
    #ifdef DEBUG
    if (rank == 0) {
        printf("end\n");
        for (int y = 0; y < n; y++) {
            if (augmented_matrix[y] != NULL) {
                for (int x = 0; x < n + 1; x++) {
                    printf("% f ", augmented_matrix[y][x]);
                }
            } 
            printf("\n");
        }
        printf("\n");
    }
    printf("output done\n");
    #endif

    // singular or not
    if (augmented_matrix[n - 1][partial_width - 2] <= THRESHOLD && augmented_matrix[n - 1][partial_width - 2] >= -THRESHOLD) {
        // singular
        if (augmented_matrix[n - 1][partial_width - 1] <= THRESHOLD && augmented_matrix[n - 1][partial_width - 1] >= -THRESHOLD) {
            printf("Infinite Solutions\n");
            MPI_Finalize();
            return 0;
        }
        else {
            printf("No Solution\n");
            MPI_Finalize();
            return 0;
        }
    }

    // calcualte *x*
    double* vector_x = (double*) malloc(n * sizeof(double));
    for (int y = n - 1; y >= 0; y--) {
        double row_sum = augmented_matrix[y][partial_width - 1];
        for (int x = n - 1; x > y; x--) {
            row_sum -= vector_x[x] * augmented_matrix[y][x];
        }
        vector_x[y] = row_sum / augmented_matrix[y][y];
    }

    // output
    for (int i = 0; i < n - 1; i++) {
        printf("%8.8f ", vector_x[i]);
    }
    printf("%8.8f\n", vector_x[n - 1]);

    MPI_Finalize();
    return 0;
}
