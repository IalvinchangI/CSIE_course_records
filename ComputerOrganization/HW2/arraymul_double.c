"slli %[arr_size], %[arr_size], 3\n\t"
"addi %[others_cnt], %[others_cnt], 1\n\t"
"add %[arr_size], %[arr_size], %[h]\n\t"
"addi %[add_cnt], %[add_cnt], 1\n\t"

"addi t0, x0, 1\n\t"
"addi %[add_cnt], %[add_cnt], 1\n\t"
"fcvt.d.w %[result], t0\n\t"
"addi %[others_cnt], %[others_cnt], 1\n\t"

"arraymul_baseline_LOOP:\n\t"
    "addi %[others_cnt], %[others_cnt], 1\n\t"
    "bge %[h], %[arr_size], END_arraymul_baseline_LOOP\n\t"

    "fld f0, 0(%[h])\n\t"
    "addi %[dlw_cnt], %[dlw_cnt], 1\n\t"
    "fld f1, 0(%[x])\n\t"
    "addi %[dlw_cnt], %[dlw_cnt], 1\n\t"

    "fmul.d f2, f0, f1\n\t"
    "addi %[dmul_cnt], %[dmul_cnt], 1\n\t"
    "fmul.d %[result], %[result], f2\n\t"
    "addi %[dmul_cnt], %[dmul_cnt], 1\n\t"


    "addi %[h], %[h], 8\n\t"
    "addi %[add_cnt], %[add_cnt], 1\n\t"
    "addi %[x], %[x], 8\n\t"
    "addi %[add_cnt], %[add_cnt], 1\n\t"

    "addi %[others_cnt], %[others_cnt], 1\n\t"
    "jal x0, arraymul_baseline_LOOP\n\t"
"END_arraymul_baseline_LOOP:\n\t"
