/*
 * Author : Alvin Chang
 * Created on Tue Dec 19 12:24:16 2023
 * NCKU PD1 Homework11
 */

#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>

// =============================== linked_list =============================== //

typedef struct _prefix {
    unsigned int ip;
    unsigned char len;
    struct _prefix* next;
} prefix;

void insert(prefix**, prefix*);  // put prefix into list, order them
void delete(prefix**, const prefix*);  // find data and delete it
bool contain(const prefix*, const prefix*);  // find data; retrun TF



// =============================== tool =============================== //

void print_list(const prefix*);
void print_group(const prefix**, int);

unsigned long long int begin,end;

unsigned long long int rdtsc()//32-bit
{
	unsigned long long int x;
	__asm__   volatile ("rdtsc" : "=A" (x));
	return x;
}

unsigned long long int rdtsc_64bits()//64-bit
{
   unsigned long long int x;
   unsigned a, d;

   __asm__ volatile("rdtsc" : "=a" (a), "=d" (d));

   return ((unsigned long long)a) | (((unsigned long long)d) << 32);;
}



// =============================== homework =============================== //

int input(char*, prefix**);  // infinite fscanf from file_path; input file_path; out head of list; retrun fscanf_count
void length_distribution(const prefix*);  // compute the number of prefixes with prefix length
void segment(int, prefix**, prefix**, int*, bool);  // seperate all data into groups by d; input d, list; out grouped list, how many element in each group
void print_segment(const int*, int);  // input an array which store how many element in each group, length
void prefix_insert(char*, prefix**, int*, int);  // infinite fscanf from file_path, put into groups; input file_path, groups, group_count, d
void prefix_delete(char*, prefix**, int*, int);  // infinite fscanf from file_path, delete prefix in group; input file_path, groups, group_count, d
void search(char*, prefix**, int);  // infinite fscanf from file_path, search prefix in group; input file_path, groups, d



int main(int argc, char *argv[]) {
    // input
    prefix *list = NULL;
    printf("The total number of prefixes in the input file is : %d.\n", input(argv[1], &list));  // input routing_table
    // length_distribution(list);
    // print_list(list);

    // segment
    int d = atoi(argv[5]);
    int length = ((1U << d) + 1);
    prefix **groups = calloc(length, sizeof(prefix*));
    int *group_count = calloc(length, sizeof(int));
    segment(d, &list, groups, group_count, false);
    // printf("============= segment =============\n");
    // print_segment(group_count, length);

    // prefix_insert
    // printf("============= prefix_insert =============\n");
    prefix_insert(argv[2], groups, group_count, d);

    // prefix_delete
    // printf("============= prefix_delete =============\n");
    prefix_delete(argv[3], groups, group_count, d);

    // // search
    printf("============= search =============\n");
    search(argv[4], groups, d);

    return 0;
}




// =============================== homework =============================== //

int input(char *file_path, prefix **out) {
    // infinite fscanf from file_path; input file_path; out head of list; retrun fscanf_count
    int count = 0;
    // file
    FILE *file = fopen(file_path, "r");
    if (file == NULL) exit(1);

    // list
    prefix **current_node = out;

    // infinite fscanf
    while (1) {
        unsigned int ip[4] = {0};
        int len = 0;
        int scan_count = fscanf(file, "%d.%d.%d.%d/%d", ip, ip + 1, ip + 2, ip + 3, &len);
        switch (scan_count) {
            char skip;
            case 4:  // for exception
                skip = 1;
                for (int i = 0; i < 4; i++) {
                    if (ip[i]) {
                        len += 8 * skip;
                        skip = 1;
                    } else skip++;
                }
            case 5:  // store data in list
                // set up
                *current_node = malloc(sizeof(prefix));
                (*current_node) -> ip = ip[0];
                for (int i = 1; i < 4; i++) (*current_node) -> ip = ((*current_node) -> ip << 8) | ip[i];
                (*current_node) -> len = len;

                // for next node
                current_node = &((*current_node) -> next);
                count++;
                continue;
        }
        *current_node = NULL;
        break;
    }
    fclose(file);
    return count;
}

void length_distribution(const prefix *head) {
    // compute the number of prefixes with prefix length
    int length_count[33] = {0};
    while (head != NULL) {  // count
        length_count[head -> len] += 1;
        head = head -> next;
    }
    for (int i = 0; i < 33; i++) {  // print out the result
        printf("the number of prefixes with prefix length %d = %d\n", i, length_count[i]);
    }
}

void segment(int d, prefix **head, prefix **out_groups, int *out_count, bool clock_TF) {
    // seperate all data into groups by d; out grouped list, how many element in each group
    prefix *current = *head;
    *head = NULL;  // reset pointer outside
    char rshift = 32 - d;  // ip >> rshift
    while (current != NULL) {
	    // if (clock_TF) begin = rdtsc();
        prefix *next = current -> next;  // temp

        unsigned int index;
        if ((current -> len) < d) {index = 1U << d;}  // special group
        else {index = (current -> ip) >> rshift;}  // normal group
        
        insert(out_groups + index, current);
        out_count[index]++;
        current = next;
	    // if (clock_TF) end = rdtsc();
	    // if (clock_TF) printf("%llu\n", (end - begin));
    }
}

void print_segment(const int *group_count, int length) {
    // input an array which store how many element in each group, length
    for (int i = 0; i < length - 1; i++) printf("The number of prefixes in group %d = %d\n", i, group_count[i]);  // normal group
    printf("The number of prefixes in special group = %d\n", group_count[length - 1]);  // special group
}

void prefix_insert(char *file_path, prefix **groups, int *group_count, int d) {
    // infinite fscanf from file_path, put into groups; input file_path, groups, group_count, d
    prefix *list = NULL;
    input(file_path, &list);
    segment(d, &list, groups, group_count, true);
}

void prefix_delete(char *file_path, prefix **groups, int *group_count, int d) {
    // infinite fscanf from file_path, delete prefix in group; input file_path, groups, group_count, d
    prefix *list = NULL;
    input(file_path, &list);

    char rshift = 32 - d;  // ip >> rshift
    while (list != NULL) {
	    // begin = rdtsc();
        prefix *next = list -> next;  // temp

        unsigned int index;
        if ((list -> len) < d) {index = 1U << d;}  // special group
        else {index = (list -> ip) >> rshift;}  // normal group
        
        delete(groups + index, list);
        group_count[index]--;
        free(list);
        list = next;
        // end = rdtsc();
	    // printf("%llu\n", (end - begin));
    }
}

void search(char *file_path, prefix **groups, int d) {
    // infinite fscanf from file_path, search prefix in group; input file_path, groups, d
    prefix *list = NULL;
    input(file_path, &list);

    char rshift = 32 - d;  // ip >> rshift
    while (list != NULL) {
	    begin = rdtsc();
        prefix *next = list -> next;  // temp

        unsigned int index;
        if ((list -> len) < d) {index = 1U << d;}  // special group
        else {index = (list -> ip) >> rshift;}  // normal group
        
        bool result = contain(groups[index], list);
        // printf(result ? "successful\n" : "failed\n");
        free(list);
        list = next;
        end = rdtsc();
	    printf("%llu\n", (end - begin));
    }
}



// =============================== linked_list =============================== //

void insert(prefix** head, prefix* node) {
    // put prefix into list, reset node, order them
    while (*head != NULL) {  // find
        if (((*head) -> ip) > (node -> ip)) break;
        if (((*head) -> ip) == (node -> ip) && ((*head) -> len) >= (node -> len)) break;
        head = &((*head) -> next);
    }
    // insert
    node -> next = *head;
    *head = node;
}

void delete(prefix** head, const prefix* node) {
    // find data and delete it
    while (*head != NULL && (((*head) -> ip) != (node -> ip) || ((*head) -> len) != (node -> len))) {  // find
        if (((*head) -> ip) > (node -> ip)) return;
        head = &((*head) -> next);
    }
    if (*head == NULL) return 0;
    // delete
    prefix *del = *head;
    *head = (*head) -> next;
    free(del);
}

bool contain(const prefix *head, const prefix *node) {
    // find data; retrun TF
    while (head != NULL) {  // find
        if ((head -> ip) == (node -> ip) && (head -> len) == (node -> len)) return true;
        if ((head -> ip) > (node -> ip)) return false;
        head = head -> next;
    }
    return false;
}



// =============================== tool =============================== //

void print_list(const prefix *head) {
    while (head != NULL) {
        for (int i = 3; i > 0; i--) printf("%d.", (head -> ip >> i * 8) & 255);
        printf("%d/%d -> ", (head -> ip) & 255, head -> len);
        head = head -> next;
    }
    printf("NULL\n");
}

void print_group(const prefix **groups, int length) {
    for (int i = 0; i < length; i++) {
        if (groups[i]) {
            printf("%d : ", i);
            print_list(groups[i]);
        }
    }
}
