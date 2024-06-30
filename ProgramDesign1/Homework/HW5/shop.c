/*
 * Author : Alvin Chang
 * Created on Fri Oct 20 10:36:25 2023
 * NCKU PD1 Homework5
 */

#include<stdio.h>

int main(void) {
    // welcome
    printf("//////////Welcome to NCKU-PD1-Kirby-Shop\\\\\\\\\\\\\\\\\\\\\n");
    
    // clear buffer var
    char trash = 0;

    // option var
    int option = 0;

    // Openning var
    char open_TF = 0;

    // List var
    int sum[5] = {0};  // {Kirby-A, Kirby-B, Kirby-C, sum_Kirby, sum_Money}
    
    while (1) {
        // option
        printf("Options: (1)Openning (2)List (3)Adding (4)Exit\n");
        int input_check = scanf("%d", &option);
        
        // clear buffer & check the end of input
        int error_TF = 0;
        while ((trash = getchar()) != '\n' && trash != EOF) {
            if (trash != ' ' || trash != '\t') {
                error_TF = 1;
            }
        }

        if (input_check == 1 && !error_TF) {  // input is number
            switch (option) {
                case 1:  // Openning
                    if (open_TF) {  // already opened
                        printf("Your Kirby shop has already opened :(\n");
                    } else {  // has not opened
                        open_TF = 1;
                        printf("Your shop is openning!\n");
                    }
                    continue;
                case 2:  // List
                    printf("===================================\n");
                    printf("Kirby-A: %d\n", sum[0]);
                    printf("Kirby-B: %d\n", sum[1]);
                    printf("Kirby-C: %d\n", sum[2]);
                    printf("Kirby: %d, Money: %d\n", sum[3], sum[4]);
                    printf("===================================\n");
                    continue;
                case 3:  // Adding
                    if (!open_TF) {  // has not opened
                        printf("Your Kirby shop has not opened\n");
                        continue;
                    }
                    int error_times = 0;
                    printf("Please enter three numbers: ");
                    while (1) {
                        // enter number
                        int once[5] = {0};  // {Kirby-A, Kirby-B, Kirby-C, Kirby, Price}
                        int input_check = scanf("%d %d %d", &once[0], &once[1], &once[2]);

                        // clear buffer & check the end of input
                        int error_TF = 0;
                        while ((trash = getchar()) != '\n' && trash != EOF) {
                            if (trash != ' ' || trash != '\t') {
                                error_TF = 1;
                            }
                        }

                        if (input_check == 3 && !error_TF) {
                            if (error_times > 0 && once[0] == -1 && once[1] == -1 && once[2] == -1) {  // enter '-1 -1 -1' to make new options
                                break;
                            }
                            if (once[0] >= 0 && once[1] >= 0 && once[2] >= 0) {  // no error
                                // gcd
                                int a = once[0], b = once[1], c = once[2];  // temp
                                while (a != 0 && b != 0) (a > b) ? (a %= b) : (b %= a);  // A B
                                int boxes = (a == 0) ? b : a;
                                while (boxes != 0 && c != 0) (boxes > c) ? (boxes %= c) : (c %= boxes);  // gcd(AB) C
                                boxes = (c == 0) ? boxes : c;

                                // print
                                once[3] = once[0] + once[1] + once[2];  // Kirby
                                once[4] = once[3] - 2 * boxes;  // Price
                                printf("Divide these Kirbys into %d boxes, with the number of Kirby in each box being %d, %d, and %d\n", boxes, once[0] / boxes, once[1] / boxes, once[2] / boxes);
                                printf("Price: %d\n", once[4]);
                                // store
                                for (int i = 0; i < 5; i++) {
                                    sum[i] += once[i];
                                }
                                break;
                            // else: <E> try again
                            }
                        }
                        error_times++;
                        printf("Error: Please try again or enter '-1 -1 -1' to make a new options: ");
                    }
                    continue;
                case 4:  // Exit
                    return 0;
                default:  // <E> try again
                    break;
            }
        }
        printf("Error: Please try again!\n");
    }
}