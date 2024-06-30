#include<stdio.h>

void SpiralMatrix(int n,  int (*matrix)[n]){
	for (int layer = 0, number = 1, length = n - 1; layer < (n + 1) / 2; layer++, length -= 2) {
		if (length == 0) {
			matrix[layer][layer] = number;
			break;
		}
		int x = layer, y = layer;
		for (; x < length + layer; x++, number++) {
			matrix[y][x] = number;
		}
		for (; y < length + layer; y++, number++) {
			matrix[y][x] = number;
		}
		for (; x > layer; x--, number++) {
			matrix[y][x] = number;
		}
		for (; y > layer; y--, number++) {
			matrix[y][x] = number;
		}
	}
}


int main(){
	int n;
	
	scanf("%d", &n);
	
	int matrix[n][n];

	for(int i = 0 ; i < n ; i++)
		for(int j = 0 ; j < n ; j++)
			matrix[i][j] = -1;
	
	SpiralMatrix(n, matrix);
    
	for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) 
                printf("%3d ", matrix[i][j]);
            printf("\n");
	}
    
	return 0;
}
