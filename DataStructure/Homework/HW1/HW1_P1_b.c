#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>


int main(void) {
    char input[101] = {0};
    char tempAddSub = 0;
    char tempMulDiv = 0;
    
    // read user input
    scanf("%s", input);

    // print the output
    for (int i = 0; input[i] != 0; i++) {
        switch (input[i]) {
            // add sub
            case '+':
            case '-':
                if (tempMulDiv != 0) {
                    // if previous operator is mul or div, we should do it first
                    // eg: a+b-c*d/e+f = ab+cd*e/-f+
                    // to make /- possible
                    printf("%c", tempMulDiv);
                    tempMulDiv = 0;
                }
                if (tempAddSub != 0) {
                    printf("%c", tempAddSub);
                }
                tempAddSub = input[i];
                break;

            // mul div
            case '*':
            case '/':
                if (tempMulDiv != 0) {
                    printf("%c", tempMulDiv);
                }
                tempMulDiv = input[i];
                break;
            
            default:  // variable
                printf("%c", input[i]);
                break;
        }
    }

    // clean temp
    if (tempMulDiv != 0) {
        printf("%c", tempMulDiv);
    }
    if (tempAddSub != 0) {
        printf("%c", tempAddSub);
    }

    return 0;
}
