#include<stdio.h>

int main(void) {
	int days, start_day;

	printf("Enter number of days in month: \n");
	scanf("%d", &days);
	printf("Enter starting day of the week (1=Sun, 7=Sat): \n");
	scanf("%d", &start_day);

	start_day -= 2;
	for (int day = -start_day, i = 1; day <= days; day++, i++) {
		if (day <= 0) {
			printf("  ");
		} else {
			printf("%2d", day);
		}
		
		if (day != days) {
			if (i % 7 == 0) {
				printf("\n");
			} else {
				printf(" ");
			}
		}
	}
	return 0;
}
