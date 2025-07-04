"addi %[add_cnt], %[add_cnt], 1\n\t"
"add t1, %[N], x0\n\t"

"addi %[add_cnt], %[add_cnt], 1\n\t"
"addi t2, x0, 1\n\t"

"addi %[others_cnt], %[others_cnt], 1\n\t"
"fcvt.s.w f1, t2\n\t"

"PI_LOOP:\n\t"
"addi %[others_cnt], %[others_cnt], 1\n\t"
    "beq t1, x0, END_PI_LOOP\n\t"

"addi %[others_cnt], %[others_cnt], 1\n\t"
    "fcvt.s.w f2, t2\n\t"

"addi %[fdiv_cnt], %[fdiv_cnt], 1\n\t"
    "fdiv.s f2, f1, f2\n\t"

"addi %[fadd_cnt], %[fadd_cnt], 1\n\t"
    "fadd.s %[pi], %[pi], f2\n\t"

"addi %[add_cnt], %[add_cnt], 1\n\t"
    "addi t1, t1, -1\n\t"

"addi %[add_cnt], %[add_cnt], 1\n\t"
    "addi t2, t2, 2\n\t"


"addi %[others_cnt], %[others_cnt], 1\n\t"
    "beq t1, x0, END_PI_LOOP\n\t"

"addi %[others_cnt], %[others_cnt], 1\n\t"
    "fcvt.s.w f2, t2\n\t"

"addi %[fdiv_cnt], %[fdiv_cnt], 1\n\t"
    "fdiv.s f2, f1, f2\n\t"

"addi %[fsub_cnt], %[fsub_cnt], 1\n\t"
    "fsub.s %[pi], %[pi], f2\n\t"

"addi %[add_cnt], %[add_cnt], 1\n\t"
    "addi t1, t1, -1\n\t"

"addi %[add_cnt], %[add_cnt], 1\n\t"
    "addi t2, t2, 2\n\t"


"addi %[others_cnt], %[others_cnt], 1\n\t"
    "jal x0, PI_LOOP\n\t"
"END_PI_LOOP:\n\t"
