#include<stdlib.h>
#include<stdio.h>
#define N 100

int read_line(char str[], int n) {
	  int ch, i = 0;

	  while ((ch = getchar()) != '\n')
	    if (i < n)
	      str[i++] = ch;
	  str[i] = '\0';   /* terminates string */
	  return i;        /* number of characters stored */
}


int CutString(char** substr, char* str, char* cut){
	
        int index = 0;
	int same_TF = 1;
	int last_cut = 0;
	for (int str_index = 0; ; str_index++) {
		if (str[str_index] == '\0') {  // last substr
			substr[index] = (char*)malloc(sizeof(char) * (str_index - last_cut + 1));
			for (int i = 0; i < str_index - last_cut; i++) {
				substr[index][i] = str[last_cut + i];
			}
			index++;
			break;
		}
		
		// check cut TF
		int len = 0;
		for (; cut[len] != '\0' && str_index + len < N; len++) {
			if (str[str_index + len] != cut[len]) {
				same_TF = 0;
				break;
			}
		}
		if (same_TF) {  // cut
			substr[index] = (char*)malloc(sizeof(char) * (str_index - last_cut + 1));
			int i = 0;
			for (; i < str_index - last_cut; i++) {
				substr[index][i] = str[last_cut + i];
			}
			substr[index][i] = '\0';	
			str_index += len - 1;
			last_cut = str_index + 1;
			index++;
		} else {
			same_TF = 1;
		}
	}
        return index;
}

int main(){
	
	char str[N];
	char cut[N];
	char* substr[N];
	
	read_line(str, N);
	read_line(cut, N);
	
	int n = CutString(substr, str, cut);
	
	for(int i = 0 ; i < n ; i++)
		printf("%s\n", substr[i]);
		
	
	return 0;
}
