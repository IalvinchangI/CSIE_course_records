#include<stdio.h>
#include<stdbool.h>

#define max_y 10
#define max_x 10


void connected_zone(int [max_y][max_x], int , int);

int count = 0;
bool check_TF[max_y][max_x] = {{false}};
int height = 0, width = 0;

int main() {
    int matrix[max_y][max_x] = {{0}};
    int max_count = 0;

    printf("Enter the height and width of the matrix: ");
    scanf("%d %d", &height, &width);

    printf("Enter the matrix values:\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            scanf("%d", &matrix[i][j]);
        }
    }
    // find
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
	    if (matrix[y][x] == 0) {
		    continue;
	    }
            if (check_TF[y][x]) {
		    continue;
	    }
	    // count connected zone
	    count = 0;
	    connected_zone(matrix, x, y);
	    if (count > max_count) {
		    max_count = count;
	    }
        }
    }
    printf("%d", max_count);
    return 0;
}

void connected_zone(int matrix[max_y][max_x], int x, int y) {
	if (check_TF[y][x]) {
		return;
	}
	count += 1;
	check_TF[y][x] = true;
	int pos = -1;
	if ((pos = y - 1) >= 0 && matrix[pos][x])
		connected_zone(matrix, x, pos);
	if ((pos = x + 1) < width && matrix[y][pos])
		connected_zone(matrix, pos, y);
	if ((pos = y + 1) < height && matrix[pos][x])
		connected_zone(matrix, x, pos);
	if ((pos = x - 1) >= 0 && matrix[y][pos])
		connected_zone(matrix, pos, y);
}

