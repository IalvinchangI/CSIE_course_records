#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <utility>
#include <functional>
#include <stdlib.h>
#include <pthread.h>
using namespace std;


const long long POS_INF = 1e18;
vector<vector<pair<int, int>>> graph;
			// weight, end node

vector<long long> result;

int n = 0;
int* interval = NULL;



long long Dijkstra(int start) {
	priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<pair<long long, int>>> min_heap;
	vector<long long> shortest(n + 1, POS_INF);
	long long farthest_length = -1;
	
	shortest[start] = 0;
					// length, node
	min_heap.push(make_pair(0, start));
	while (min_heap.empty() == false) {
		pair<long long, int> current = min_heap.top();
		min_heap.pop();
		
		if (shortest[current.second] < current.first) {  // has found a short path
			continue;
		}
		if (current.first > farthest_length && current.second != start) {  // record the farthest length
			farthest_length = current.first;
		}
		for (const pair<int, int>& element : graph[current.second]) {  // update length
			if (current.first + element.first >= shortest[element.second]) {  // has found a short path
				continue;
			}
			shortest[element.second] = current.first + element.first;
			min_heap.push(make_pair(shortest[element.second], element.second));
		}
	}
	
	return farthest_length;
}

void* thread_function(void *argv) {
	int rank = *(int*)argv;
	
	for (int i = interval[rank]; i < interval[rank+1]; i++) {
		result[i] = Dijkstra(i);
	}
	
    return NULL;
}


int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);

    // read file
    string filename;
    cin >> filename;
    ifstream file(filename);

    int m = 0;
    file >> n >> m;
    graph.resize(n + 1);
    result.resize(n + 1);
    
    for (int i = 0; i < m; i++) {
        int start = 0;
        int end = 0;
        int weight = 0;
    	file >> start >> end >> weight;
    	graph[start].push_back(make_pair(weight, end));
    	graph[end].push_back(make_pair(weight, start));
    }
    file.close();


    // init thread & run thread
	int thread_count = atoi(argv[1]);

    int unit = n / thread_count + 1;
    interval = (int*)malloc((thread_count + 1) * sizeof(int));
    interval[0] = 1;
    interval[thread_count] = n + 1;
    for (int i = 1; i < thread_count; i++) {
        interval[i] = i * unit;
        if (interval[i] > n) {
            interval[i] = n + 1;
        }
    }
    
    pthread_t* handler = (pthread_t*)malloc(thread_count * sizeof(pthread_t));
    int* thread_args = (int*)malloc(thread_count * sizeof(int));
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
    free(interval);


    // output
    for (int i = 1; i <= n; i++) {
        if (i == n) {
            cout << result[i];
        } else {
            cout << result[i] << "\n";
        }
    }

    return 0;
}
