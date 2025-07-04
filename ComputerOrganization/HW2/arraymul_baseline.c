"addi %[others_cnt], %[others_cnt], 1\n\t"
"slli %[arr_size], %[arr_size], 2\n\t"
"addi %[add_cnt], %[add_cnt], 1\n\t"
"add %[arr_size], %[arr_size], %[h]\n\t"

"arraymul_baseline_LOOP:\n\t"
    "addi %[others_cnt], %[others_cnt], 1\n\t"
    "bge %[h], %[arr_size], END_arraymul_baseline_LOOP\n\t"

    "addi %[flw_cnt], %[flw_cnt], 1\n\t"
    "flw f0, 0(%[h])\n\t"
    "addi %[flw_cnt], %[flw_cnt], 1\n\t"
    "flw f1, 0(%[x])\n\t"

    "addi %[fmul_cnt], %[fmul_cnt], 1\n\t"
    "fmul.s f0, f0, f1\n\t"
    "addi %[fadd_cnt], %[fadd_cnt], 1\n\t"
    "fadd.s f0, f0, %[id]\n\t"

    "addi %[fsw_cnt], %[fsw_cnt], 1\n\t"
    "fsw f0, 0(%[y])\n\t"

    "addi %[add_cnt], %[add_cnt], 1\n\t"
    "addi %[h], %[h], 4\n\t"
    "addi %[add_cnt], %[add_cnt], 1\n\t"
    "addi %[x], %[x], 4\n\t"
    "addi %[add_cnt], %[add_cnt], 1\n\t"
    "addi %[y], %[y], 4\n\t"

    "addi %[others_cnt], %[others_cnt], 1\n\t"
    "jal x0, arraymul_baseline_LOOP\n\t"
"END_arraymul_baseline_LOOP:\n\t"
