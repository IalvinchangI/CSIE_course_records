#include<stdio.h>
#include<stdlib.h>

int main(void) {
	FILE *file = fopen("example.txt", "r");
	//逐行讀取
	char line[256];
    	while (fgets(line, sizeof(line), file) != NULL) {
        	printf("%s", line);
    	}
	return 0;
}
