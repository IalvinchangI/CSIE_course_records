#include <stdio.h>
#include <math.h>
#include <mpi.h>


#define BOUNDARY 1000000007
#define DIVIDE2  500000004

typedef unsigned long long Number;

int main(int argc, char *argv[]) {
        // MPI init
        int comm_sz;
        int rank_ID;
        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank_ID);


        // input
        Number n = 0;
        char file_name[1000] = {0};
        FILE* file = NULL;
        switch (rank_ID) {
            case 0:
                scanf("%s", file_name);
                file = fopen(file_name, "r");
                fscanf(file, "%llu", &n);
                fclose(file);
                for (int rank = 1; rank < comm_sz; rank++) {
                    MPI_Send(&n, 1, MPI_UNSIGNED_LONG_LONG, rank, 0, MPI_COMM_WORLD);
                }
                break;
            case 1:
            case 2:
            case 3:
                MPI_Recv(&n, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                break;
            default:
                break;
        }


        // split
        // sqrt(N) / 2, sqrt(N), 2 * sqrt(N)
        Number sqrt_n = sqrt(n);
        Number split_list[3] = {sqrt_n >> 1, sqrt_n, sqrt_n << 1};

        Number rank_start = (rank_ID == 0) ? 1 : (n / (n / split_list[rank_ID - 1]) + 1);
        Number rank_end   = (rank_ID == 3) ? n : (n / (n / split_list[rank_ID]));

        // printf("<%d> n=%-20llu start=%-15llu end=%-20llu\n", rank_ID, n, rank_start, rank_end);
        // printf("<%d> n=%-20llu start=%-15llu end=%-20llu\n", rank_ID, n, (rank_ID == 0) ? 1 : split_list[rank_ID - 1], (rank_ID == 3) ? n : split_list[rank_ID]);


        // calculate
	Number output = 0;
	for (Number start = rank_start; start <= rank_end; ) {
		Number height = n / start;
		Number end = n / height;
		
		output = (output + (
                    (
                        (
                            (
                                (((start + end) % BOUNDARY) * ((end - start + 1) % BOUNDARY)) % BOUNDARY
                            ) * (height % BOUNDARY)
                        ) % BOUNDARY
                    ) * DIVIDE2
                ) % BOUNDARY) % BOUNDARY;

		start = end + 1;
	}


        // merge
        Number other_output = 0;
        switch (rank_ID) {
            case 0:
                for (int rank = 1; rank < comm_sz; rank++) {
                    MPI_Recv(&other_output, 1, MPI_UNSIGNED_LONG_LONG, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    output += other_output;
                    other_output = 0;
                }
                printf("%llu\n", output % BOUNDARY);
                break;
            case 1:
            case 2:
            case 3:
                MPI_Send(&output, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
                break;
            default:
                break;
        }

        MPI_Finalize();
	
	
	return 0;
}

