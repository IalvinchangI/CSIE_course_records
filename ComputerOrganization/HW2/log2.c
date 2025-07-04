"addi %[add_cnt], %[add_cnt], 1\n\t"
"addi t1, x0, 1\n\t"

"Log2_LOOP:\n\t"
"addi %[others_cnt], %[others_cnt], 1\n\t"
    "bge t1, %[N], END_Log2_LOOP\n\t"

"addi %[others_cnt], %[others_cnt], 1\n\t"
    "srli %[N], %[N], 1\n\t"

"addi %[add_cnt], %[add_cnt], 1\n\t"
    "addi %[log], %[log], 1\n\t"

"addi %[others_cnt], %[others_cnt], 1\n\t"
    "jal x0, Log2_LOOP\n\t"
"END_Log2_LOOP:\n\t"
