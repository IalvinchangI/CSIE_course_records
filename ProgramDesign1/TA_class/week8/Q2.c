#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Using rand() to generate a random number between 1 and 6.
int rollDice() {
    return (rand() % 6) + 1;
}

int main() {
    
    int totalRolls = 10000; // you could modify the totalRolls
    int sum = 0;
    double expectedValue = 0;

    srand((unsigned) time(NULL));
    // Calculate expectedValue using rollDice()
    for (int i = 0; i < totalRolls; i++) {
	    sum += rollDice();
    }

    expectedValue = (double) sum / totalRolls;
    printf("The Expected Value is %lf\n", expectedValue);

    return 0;
}
