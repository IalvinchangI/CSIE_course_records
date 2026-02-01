#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>


#define DIFFICULTY_LIMIT 1001
#define FILE_NAME_BUFFER_SIZE 1000



int main(int argc, char *argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
	
    // MPI init
    int comm_sz;
    int rank_ID = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_ID);


    // input
    char file_name[FILE_NAME_BUFFER_SIZE] = {0};
    std::ifstream file;
    if (rank_ID == 0) {
        std::cin >> file_name;
        MPI_Send(file_name, FILE_NAME_BUFFER_SIZE, MPI_CHAR, 2, 0, MPI_COMM_WORLD);
    } else if (rank_ID == 2) {
        MPI_Recv(file_name, FILE_NAME_BUFFER_SIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }


    // split
    if ((rank_ID & 1) == 0) {
    file.open(file_name);


    // data
    int n = 0, m = 0;
    int colten_ability = 0;
    int ability[DIFFICULTY_LIMIT] = {0};
    int hard_problem_count = 0;
    int problem[DIFFICULTY_LIMIT] = {0};
    int temp = 0;

    file >> n >> m;
    file >> colten_ability;
    switch (rank_ID) {
        case 2:
            for (int i = 1; i < n; i++) {  // ability
                file >> temp;
                ability[temp]++;
            }
            for (int i = DIFFICULTY_LIMIT - 2; i > colten_ability; i--) {  // ability (accumulate)
                ability[i] += ability[i + 1];
            }
            /*
            for (int i = DIFFICULTY_LIMIT - 1; i >= 1; i--) {
                if (ability[i] != 0) std::cout << i << "\t" << ability[i] << std::endl;
            }
            */
            MPI_Send(ability, DIFFICULTY_LIMIT, MPI_INT, 0, 0, MPI_COMM_WORLD);
            break;
        case 0:
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            for (int i = 0; i < m; i++) {  // problem
                file >> temp;
                if (colten_ability < temp) {  // not pass => hard problem
                    hard_problem_count++;
                    problem[temp]++;
                }
            }
            /*
            for (int i = DIFFICULTY_LIMIT - 1; i >= 1; i--) {
                if (problem[i] != 0) std::cout << i << "\t" << problem[i] << std::endl;
            }
            */
            MPI_Recv(ability, DIFFICULTY_LIMIT, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            break;
        default:
            break;
    }
    file.close();


    if (rank_ID == 0) {
        // table
        int* problem2rank = (int*) malloc(sizeof(int) * hard_problem_count);
        for (int i = DIFFICULTY_LIMIT - 1, count = hard_problem_count - 1; count >= 0; i--) {
            for (int j = 0; j < problem[i]; j++) {
                problem2rank[count - j] = ability[i];
            }
            count -= problem[i]; 
        }

        // calculate
        for (int k = 1; k <= m; k++) {
            long long output = m / k;  // problem count
            int start = m - output * k;
            for (int i = start; i < hard_problem_count; i += k) {
                output += problem2rank[i];
            }
            std::cout << output;
            if (k == m) {
                std::cout << std::endl;
            } else {
                std::cout << " ";
            }
        }
        free(problem2rank);
    }
    } // end split

    MPI_Finalize();

    return 0;
}

