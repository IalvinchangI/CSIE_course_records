#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


#define F_LIMIT 1000000

inline static int index2(int y, int x, int partial_width) {
    return y * partial_width + x;
}

inline static int next(int* ground, int partial_width, int x, int y) {
    int pos = index2(y, x, partial_width);
    return (*(ground + pos) >> 1) + ((
        *(ground + pos + 1) + 
        *(ground + pos - 1) + 
        *(ground + pos + partial_width) + 
        *(ground + pos - partial_width)
    ) >> 2);
}


int main(int argc, char* argv[]) {
    // MPI init
    int comm_sz;
    int rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);


    // get input file
    char filename[1000] = {0};
    if (rank == 0) {
        scanf("%s", filename);
    }    
    MPI_Bcast(filename, 1000, MPI_CHAR, 0, MPI_COMM_WORLD);

    // get input
    FILE* file = fopen(filename, "r");
    int n = 0;
    int t = 0;
    fscanf(file, "%d", &n);
    fscanf(file, "%d", &t);
    fgetc(file);  // clear \n
    int partial_width = (n + 2);
    int partial_height = (n + 2) >> 2;
    int y_interval[5] = {
        1,                                          // rank 0 start
        partial_height + 1,                         // rank 1 start
        (partial_height << 1) + 1,                  // rank 2 start
        (partial_height << 1) + partial_height + 1, // rank 3 start
        n + 1                                       // rank 4 start  // does not exist
    };
    int interval_length = (y_interval[rank+1] - y_interval[rank] >= 0) ? y_interval[rank+1] - y_interval[rank] : 0;
    MPI_Datatype MPI_VECTOR_N;
    int err = MPI_Type_contiguous(n, MPI_INT, &MPI_VECTOR_N);
    MPI_Type_commit(&MPI_VECTOR_N);

    // build ground
    int* ground0 = (int*) malloc((interval_length + 2) * partial_width * sizeof(int));
    int* ground1 = (int*) malloc((interval_length + 2) * partial_width * sizeof(int));
    int** current_ground = &ground0;
    int** next_ground = &ground1;

    // read file and fill ground
    for (int y = 1 + 1; y < y_interval[rank]; y++) {  // skip lines
        int ch = 0;
        while ((ch = fgetc(file)) != '\n' && ch != EOF);
    }
    // head
    for (int x = 1; x < partial_width; x++) {  // clear ground
        *((*current_ground) + index2(0, x, partial_width)) = 0;
        *((*next_ground) + index2(0, x, partial_width)) = 0;
    }
    if (rank != 0) {
        for (int x = 1; x <= n; x++) {  // fill data
            fscanf(file, "%d", (*current_ground) + index2(0, x, partial_width));
        }
    }
    // middle
    for (int y = 1; y <= interval_length; y++) {
        // clear ground
        *((*current_ground) + index2(y, 0, partial_width)) = 0;
        *((*current_ground) + index2(y, partial_width - 1, partial_width)) = 0;
        *((*next_ground) + index2(y, 0, partial_width)) = 0;
        *((*next_ground) + index2(y, partial_width - 1, partial_width)) = 0;
        for (int x = 1; x <= n; x++) {  // fill data
            fscanf(file, "%d", (*current_ground) + index2(y, x, partial_width));
        }
    }
    // tail
    for (int x = 1; x < partial_width; x++) {  // clear ground
        *((*current_ground) + index2(interval_length + 1, x, partial_width)) = 0;
        *((*next_ground) + index2(interval_length + 1, x, partial_width)) = 0;
    }
    if (rank != 3) {
        for (int x = 1; x <= n; x++) {  // fill data
            fscanf(file, "%d", (*current_ground) + index2(interval_length + 1, x, partial_width));
        }
    }
    fclose(file);
    // current_ground is t0 now

    // calculate
    MPI_Status mpi_status;
    for (int time = 1; time <= t; time++) {
        // update ground
        for (int y = 1; y <= interval_length; y++) {
            for (int x = 1; x < partial_width - 1; x++) {
                int temp = next(*current_ground, partial_width, x, y);
                *((*next_ground) + index2(y, x, partial_width)) = (temp > F_LIMIT) ? F_LIMIT : temp;
            }
        }

        // exchange head & tail
        // exchange 1's head & 0's tail
        // exchange 3's head & 2's tail
        // exchange 2's head & 1's tail
        // put in next_ground
        switch (rank) {
            case 0:
                MPI_Sendrecv(  // exchange 1's head & 0's tail
                        (*next_ground) + index2(interval_length, 1, partial_width), 1, MPI_VECTOR_N, 1, 0, 
                        (*next_ground) + index2(interval_length + 1, 1, partial_width), 1, MPI_VECTOR_N, 1, 0, 
                        MPI_COMM_WORLD, &mpi_status
                );
                break;
            case 1:
                MPI_Sendrecv(  // exchange 1's head & 0's tail
                        (*next_ground) + index2(1, 1, partial_width), 1, MPI_VECTOR_N, 0, 0, 
                        (*next_ground) + index2(0, 1, partial_width), 1, MPI_VECTOR_N, 0, 0, 
                        MPI_COMM_WORLD, &mpi_status
                );
                MPI_Sendrecv(  // exchange 2's head & 1's tail
                        (*next_ground) + index2(interval_length, 1, partial_width), 1, MPI_VECTOR_N, 2, 0, 
                        (*next_ground) + index2(interval_length + 1, 1, partial_width), 1, MPI_VECTOR_N, 2, 0, 
                        MPI_COMM_WORLD, &mpi_status
                );
                break;
            case 2:
                MPI_Sendrecv(  // exchange 3's head & 2's tail
                        (*next_ground) + index2(interval_length, 1, partial_width), 1, MPI_VECTOR_N, 3, 0, 
                        (*next_ground) + index2(interval_length + 1, 1, partial_width), 1, MPI_VECTOR_N, 3, 0, 
                        MPI_COMM_WORLD, &mpi_status
                );
                MPI_Sendrecv(  // exchange 2's head & 1's tail
                        (*next_ground) + index2(1, 1, partial_width), 1, MPI_VECTOR_N, 1, 0, 
                        (*next_ground) + index2(0, 1, partial_width), 1, MPI_VECTOR_N, 1, 0, 
                        MPI_COMM_WORLD, &mpi_status
                );
                break;
            case 3:
                MPI_Sendrecv(  // exchange 3's head & 2's tail
                        (*next_ground) + index2(1, 1, partial_width), 1, MPI_VECTOR_N, 2, 0, 
                        (*next_ground) + index2(0, 1, partial_width), 1, MPI_VECTOR_N, 2, 0, 
                        MPI_COMM_WORLD, &mpi_status
                );
                break;
        }

        // switch current_ground & next_ground
        int** temp = next_ground;
        next_ground = current_ground;
        current_ground = temp;
    }

    // sum
    unsigned long long sum = 0;
    for (int i = partial_width; i < (interval_length + 1) * partial_width; i++) {
        sum += *((*current_ground) + i);
    }
    unsigned long long total_sum = 0;
    MPI_Reduce(&sum, &total_sum, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        printf("%llu\n", total_sum);
    }

    MPI_Finalize();
    return 0;
}
