#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <utility>
#include <functional>
#include <cmath>
#include <limits>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <random>
#include <omp.h>
using namespace std;

#define DEFAULT_THREAD_COUNT 8
#define BLOCK_SIZE 64
#define TIME_THRESHOLD 9.8

#define ANT_COUNT (DEFAULT_THREAD_COUNT * 5)
#define ITERATION 10000

#define EVAPORATION 0.7f
#define ALPHA 1.0f
#define BETA 3.8f
#define PHEROMONE_DROP 1000.0f


static unsigned int random_init_int = time(NULL);
// static const unsigned int random_init_int = 135208918;

#define square(a) ((long long)(a) * (a))
inline static double get_distance(pair<int, int> a, pair<int, int> b) {
    return sqrt(square(a.first - b.first) + square(a.second - b.second));
}

static int random_int() {
    static thread_local mt19937 random_generator(omp_get_thread_num() + random_init_int);
    static thread_local uniform_int_distribution distribution(0, RAND_MAX - 1);
    return distribution(random_generator);
    // return rand();
}


int main(int argc, char* argv[]) {
    double start_time = omp_get_wtime();
    int thread_count = DEFAULT_THREAD_COUNT;
    if (argc >= 2) {
        thread_count = atoi(argv[1]);
    }
    // srand(start_time_int);
    ios_base::sync_with_stdio(false);
    cin.tie(0);

    // read file
    string filename;
    cin >> filename;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file." << endl;
        return 1;
    }

    int n = 0;
    file >> n;

    vector<pair<int, int>> position(n);
    for (int i = 0; i < n; i++) {
        int x = 0;  // 0 ~ 1000
        int y = 0;  // 0 ~ 1000
        file >> x >> y;
        position[i] = make_pair(x, y);
    }
    file.close();

    vector<double> pheromone_matrix(n * n, 1.0);

    // init dist_matrix & eta_matrix
    vector<double> dist_matrix(n * n);
    vector<double> eta_matrix(n * n);
    #pragma omp parallel for num_threads(thread_count) collapse(2) shared(dist_matrix, eta_matrix, n) schedule(static, BLOCK_SIZE)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) {
                dist_matrix[i * n + j] = 0;
                eta_matrix[i * n + j] = 0;
                continue;
            }

            // dist_matrix
            double d = get_distance(position[i], position[j]);
            dist_matrix[i * n + j] = d;

            // eta_matrix
            if (d < 1e-9) d = 1e-5;
            eta_matrix[i * n + j] = pow(1.0 / d, BETA);
        }
    }

    double global_best_length = numeric_limits<double>::max();
    vector<int> global_best_path;

    // calculate
    vector<vector<int>> all_ant_paths(ANT_COUNT, vector<int>(n));
    vector<double> all_ant_lengths(ANT_COUNT);
    for (int iter = 0; iter < ITERATION; iter++) {
        double iter_start_time = omp_get_wtime();
        // for (int i = 0; i < n; i++) {
        //     for (int j = 0; j < n; j++) {
        //         cout << pheromone_matrix[i * n + j] << " ";
        //     }
        //     cout << endl;
        // }

        // ants
        #pragma omp parallel num_threads(thread_count) shared(all_ant_paths, all_ant_lengths, n)
        {
            vector<double> probs(n-1);
            vector<int> candidates(n-1);
            vector<char> visited(n);

            #pragma omp for schedule(static, BLOCK_SIZE)
            for (int k = 0; k < ANT_COUNT; k++) {
                vector<int>& path = all_ant_paths[k];
                for (int i = 0; i < n; i++) {
                    visited[i] = 0;
                }
                
                // pick start point
                int start_node = random_int() % n;
                int current = start_node;
                
                path[0] = current;
                visited[current] = 1;
                double path_len = 0.0;

                // go through all cities
                for (int step = 1; step < n; step++) {
                    int city_count = n - step;
                    double sum_prob = 0.0;

                    // candidate & probability
                    int candidates_index = 0;
                    for (int next = 0; next < n; next++) {
                        if (!visited[next]) {
                            // P = (tau^alpha) * (eta^beta)
                            double tau = pheromone_matrix[current * n + next];
                            double eta = eta_matrix[current * n + next];
                            double p = eta;
                            if (ALPHA == 1.0f) {
                                p *= tau;
                            }
                            else {
                                p *= pow(tau, ALPHA);
                            }
                            
                            // record
                            probs[candidates_index] = p;
                            candidates[candidates_index] = next;
                            candidates_index++;
                            sum_prob += p;
                        }
                    }

                    // random select
                    int next_city = -1;
                    if (sum_prob == 0) {
                        next_city = candidates[random_int() % city_count];
                    } else {
                        double r = (double)random_int() / RAND_MAX * sum_prob;
                        double accumulated = 0.0;
                        for (int i = 0; i < city_count; i++) {
                            accumulated += probs[i];
                            if (accumulated >= r) {
                                next_city = candidates[i];
                                break;
                            }
                        }
                        if (next_city == -1) {
                            next_city = candidates[candidates_index - 1];
                        }
                    }

                    // next
                    visited[next_city] = true;
                    path_len += dist_matrix[current * n + next_city];
                    path[step] = next_city;
                    current = next_city;
                }

                // back to start
                path_len += dist_matrix[current * n + start_node];
                
                // record the path for update the pheromone_matrix
                all_ant_lengths[k] = path_len;
            }
        }

        // 1. Evaporation (update pheromone_matrix)
        // #pragma omp parallel for num_threads(thread_count) shared(pheromone_matrix, n) schedule(static, BLOCK_SIZE)
        for (int i = 0; i < n * n; i++) {
            pheromone_matrix[i] *= (1.0 - EVAPORATION);
            if(pheromone_matrix[i] < 1e-4) pheromone_matrix[i] = 1e-4; 
        }

        // 2. Deposit (update pheromone_matrix) & Update Best
        int best_index = -1;
        for (int k = 0; k < ANT_COUNT; k++) {
            double path_len = all_ant_lengths[k];
            double deposit = PHEROMONE_DROP / path_len;
            const vector<int>& path = all_ant_paths[k];

            // best solution
            if (path_len < global_best_length) {
                global_best_length = path_len;
                best_index = k;
            }
            
            // Deposit
            for (int i = 0; i < n; i++) {
                int from = path[i];
                int to = path[((i + 1) == n) ? 0 : (i + 1)];
                
                pheromone_matrix[from * n + to] += deposit;
                pheromone_matrix[to * n + from] += deposit;
            }
        }
        if (best_index != -1) {
            global_best_path = all_ant_paths[best_index];
        }

        // early stop
        double current_time = omp_get_wtime();
        // cout << ">> " << (current_time - start_time) << "\t" << (current_time - start_time + current_time - iter_start_time) << endl;
        if ((current_time - start_time + current_time - iter_start_time) > TIME_THRESHOLD) {
            break;
        }
    }

    // output
    cout << fixed << setprecision(6);
    cout << global_best_length << "\n";
    for (int i = 0; i < n; i++) {
        cout << global_best_path[i] << (i == n - 1 ? "" : " ");
    }
    cout << flush;

    return 0;
}