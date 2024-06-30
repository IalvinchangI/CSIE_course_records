/*
 * Author : Alvin Chang
 * Created on Fri Nov 2 20:10:08 2023
 * NCKU PD1 Homework7
 */

#include<stdio.h>
#include<stdbool.h>
#include<math.h>

#define max_file_count 21
// #define max_file_count 5


void store_file(int, int);  // store file into hard drives
void delete_file(int, int);  // find what files should delete and delete them
void find_suitable(int, int, int, int, int [], int [], int*);  // recursion
int address2index(int*);

void print_files(void);  // debug

bool in_file_sys(int);


// store
int file_count = 0;
int file[max_file_count][2] = {{0}};  // file store in hard drive  // {name, size}
int file_length = sizeof(file[0]);
int *file_address[max_file_count] = {0};  // address of file in hard drive --> header


int main(void) {
    while (1) {  // option
        int option = 0;
        printf("Options: ");
        scanf("%d", &option);
        int file_name = 0, file_size = 0;
        switch (option) {
            case 1:  // Add some files
                printf("Please input file name and file size: ");
                scanf("%d %d", &file_name, &file_size);

                // input file
                store_file(file_name, file_size);

                // delete file
                if (file_count >= max_file_count) {
                    int delete_count = 0, delete_sum_size = 0;
                    printf("Hard drive exceeds its capacity, please enter the number of files to be deleted: ");
                    scanf("%d %d", &delete_count, &delete_sum_size);
                    delete_file(delete_count, delete_sum_size);
                }
                break;
            case 2:  // Query the file
                printf("Please input the file name: ");
                scanf("%d", &file_name);
                printf(in_file_sys(file_name) ? "YES\n" : "NO\n");
                break;
            case 3:  // Exit the process
                return 0;
            default:  // <E>
                // print_files();
                break;
        }
    }
}


void store_file(int name, int size) {
    // store file into hard drives
    for (int i = 0; i < max_file_count; i++) {
        if (file[i][0] == 0) {  // can store
            file[i][0] = name;
            file[i][1] = size;
            file_address[file_count++] = file[i];
            break;
        }
    }
}

void delete_file(int count, int sum_size) {
    // find what files should delete and delete them
    int final_delete[max_file_count] = {0};  // index of file in file_address
    int smallest_gap = 2147483647;
    int temp_delete[max_file_count] = {0};  // index of file in file_address
    
    // find what files should delete
    find_suitable(max_file_count, count, count, sum_size, temp_delete, final_delete, &smallest_gap);

    //delete them
    for (int i = 0, index = 0; i < count; i++) {
        index = address2index(file_address[final_delete[i]]);
        (i != count - 1) ? printf("%d ", file[index][0]) : printf("%d\n", file[index][0]);
        file_address[final_delete[i]] = 0;
        file[index][0] = 0;
    }

    // fill empty spaces in file_address
    for (int i = final_delete[0] + 1, shift = 1; i < max_file_count; i++) {
        if (i == final_delete[shift]) {
            shift++;
            continue;
        }
        file_address[i - shift] = file_address[i];
        file_address[i] = 0;
    }

    file_count -= count;
}

void find_suitable(int n, int layer, int count, int match, int temp[], int final[], int *final_gap) {
    // recursion // max_file_count(n) C count(layer)
    for (int index = max_file_count - n; index < max_file_count - layer + 1; index++) {
        // index is the index of file_adress
        temp[count - layer] = index;
        if (layer == 1) {  // stop
            int sum = 0;
            for (int i = 0; i < count; i++) {
                sum += *(file_address[temp[i]] + 1);
            }
            int gap = abs(match - sum);
            if (gap < *final_gap) {  // exchange
                *final_gap = gap;
                for (int i = 0; i < count; i++) {
                    final[i] = temp[i];
                }
            }
        } else {
            find_suitable(max_file_count - index - 1, layer - 1, count, match, temp, final, final_gap);
        }
    }
}

int address2index(int *address) {
    return ((int) address - (int) file) / file_length;
}


void print_files(void) {
    // show what hard drive store
    printf("  num   name\tsize\taddress\n");
    for (int i = 0; i < max_file_count; i++) {
        printf("%5d : %-d\t%-d\t%d\n", i + 1, file[i][0], file[i][1], file_address[i]);
    }
}

bool in_file_sys(int file_name) {
    // check if the file_name exists
    for (int i = 0; i < max_file_count; i++) {
        if (file[i][0] == file_name) {
            return true;
        }
    }
    return false;
}
