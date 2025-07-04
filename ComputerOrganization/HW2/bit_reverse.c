"addi %[add_cnt], %[add_cnt], 1\n\t"
"addi t0, x0, 1\n\t"

"addi %[others_cnt], %[others_cnt], 1\n\t"
"sll t0, t0, %[m]\n\t"

"addi %[others_cnt], %[others_cnt], 1\n\t"
"srli t0, t0, 1\n\t"

"addi %[add_cnt], %[add_cnt], 1\n\t"
"addi t2, x0, 0\n\t"

"bit_reverse_LOOP:\n\t"
"addi %[others_cnt], %[others_cnt], 1\n\t"
    "beq t0, x0, END_bit_reverse_LOOP\n\t"

"addi %[others_cnt], %[others_cnt], 1\n\t"
    "and t1, t0, %[b]\n\t"

"addi %[others_cnt], %[others_cnt], 1\n\t"
    "beq t1, x0, bit_reverse_SKIP\n\t"
"addi %[add_cnt], %[add_cnt], 1\n\t"
        "addi t1, x0, 1\n\t"

"addi %[others_cnt], %[others_cnt], 1\n\t"
        "sll t1, t1, %[m]\n\t"

"addi %[others_cnt], %[others_cnt], 1\n\t"
        "or t2, t2, t1\n\t"
    "bit_reverse_SKIP:\n\t"

"addi %[others_cnt], %[others_cnt], 1\n\t"
    "srli t2, t2, 1\n\t"

"addi %[others_cnt], %[others_cnt], 1\n\t"
    "srli t0, t0, 1\n\t"

"addi %[others_cnt], %[others_cnt], 1\n\t"
    "jal x0, bit_reverse_LOOP\n\t"
"END_bit_reverse_LOOP:\n\t"

"addi %[add_cnt], %[add_cnt], 1\n\t"
"add %[b], t2, x0\n\t"
