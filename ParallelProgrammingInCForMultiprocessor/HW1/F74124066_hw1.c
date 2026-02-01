#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define CODE_LENGTH 10
#define CODE_PERIOD 30

typedef char Code[CODE_LENGTH+1];


static inline int code_index(int time) {
    return time / CODE_PERIOD;
}


int main()
{
    int N = 0, Q = 0;
    scanf("%d %d", &N, &Q);

    Code* code_list = (Code*) malloc(N * sizeof(Code));
    if (code_list == NULL) {
        return 1;
    }
    for (int index = 0; index < N; index++) {
        scanf("%s", code_list[index]);
    }


    int result = 0; 
    for (int i = 0; i < Q; i++) {
        int time = 0;
        Code user_input = {0};
        scanf("%d %s", &time, user_input);
        time--;
        
        if (time >= CODE_PERIOD * N) {
            continue;
        }
        if (strcmp(
                user_input, 
                code_list[code_index(time)]
        ) == 0) {
            result++;
        }
    }
    
    printf("%d\n", result);

    return 0;
}
