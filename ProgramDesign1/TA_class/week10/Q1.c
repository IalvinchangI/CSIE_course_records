#include <stdio.h>
#include <string.h>

void sort(char array[], int len) {
	for (int i = len; i > 1; i--) {
		for (int j = 1; j < i; j++) {
			if (array[j - 1] > array[j]) {
				char temp = array[j - 1];
				array[j - 1] = array[j];
				array[j] = temp;
			}
		}
	}
}

void areAnagrams(char str1[], char str2[]) {
    int len1 = 0, len2 = 0;
    for (int i = 0; i < 100; i++) {
	    if (str1[i] == '\0') {
		    len1 = i;
		    break;
	    }
    }
    for (int i = 0; i < 100; i++) {
	    if (str2[i] == '\0') {
		    len2 = i;
		    break;
	    }
    }
    if (len1 != len2) {
	    printf("No");
	    return;
    }
    // sort 後會相同
    sort(str1, len1);
    sort(str2, len2);
    
    for (int i = 0; i < len1; i++) {
	    if (str1[i] != str2[i]) {
		    printf("No");
		    return;
	    }
    }
    printf("Yes");
}

int main() {
    char str1[100], str2[100];
    
    scanf("%s", str1);
    
    scanf("%s", str2);

    areAnagrams(str1, str2);

    return 0;
}
