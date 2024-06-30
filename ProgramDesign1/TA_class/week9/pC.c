#include<stdio.h>
#include<stdbool.h>

void permutation(int [], int, bool [], int);

int main(void) {
	int n = 0;
	scanf("%d", &n);
	int temp[8] = {0};
	bool in_TF[8] = {false};
	permutation(temp, n, in_TF, n);
	return 0;
}

void permutation(int array[], int layer, bool in_TF[], int len) {
	for (int i = 1; i <= len; i++) {
		if (in_TF[i - 1]) {
			continue;
		}

		int index = len - layer;
		in_TF[i - 1] = true;
		array[index] = i;
		if (layer == 1) {  // print
			for (int i = 0; i < len; i++) {
				printf("%d", array[i]);
			}
			printf("\n");
			in_TF[i - 1] = false;
			return;
		}
		// recursion
		permutation(array, layer - 1, in_TF, len);

		// clear in_TF
		in_TF[i - 1] = false;
	}
}
