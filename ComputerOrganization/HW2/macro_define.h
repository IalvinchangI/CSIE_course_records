#define macro_fft_cycle_count fft_cycle_count = \
    add_cnt*add_CPI + sub_cnt*sub_CPI + mul_cnt*mul_CPI + div_cnt*div_CPI + \
    lw_cnt*lw_CPI + sw_cnt*sw_CPI + others_cnt*others_CPI + \
    fadd_cnt*fadd_CPI + fsub_cnt*fsub_CPI + fmul_cnt*fmul_CPI + fdiv_cnt*fdiv_CPI;

#define macro_fft_cpu_time fft_cpu_time = fft_cycle_count * cycle_time;

#define macro_calc_fft_ratio fft_ratio = (\
    add_cnt*add_CPI + sub_cnt*sub_CPI + mul_cnt*mul_CPI + div_cnt*div_CPI + \
    others_cnt*others_CPI + \
    fadd_cnt*fadd_CPI + fsub_cnt*fsub_CPI + fmul_cnt*fmul_CPI + fdiv_cnt*fdiv_CPI) / \
    (float)fft_cycle_count;


#define macro_arraymul_vector_cycle_count arraymul_vector_cycle_count = \
    add_cnt*add_CPI + sub_cnt*sub_CPI + mul_cnt*mul_CPI + div_cnt*div_CPI + \
    lw_cnt*lw_CPI + sw_cnt*sw_CPI + others_cnt*others_CPI + \
    fadd_cnt*fadd_CPI + fsub_cnt*fsub_CPI + fmul_cnt*fmul_CPI + fdiv_cnt*fdiv_CPI + \
    flw_cnt*flw_CPI + fsw_cnt*fsw_CPI;

#define macro_arraymul_vector_cpu_time arraymul_vector_cpu_time = arraymul_vector_cycle_count * cycle_time;


#define macro_arraymul_baseline_cycle_count arraymul_baseline_cycle_count = \
    add_cnt*add_CPI + sub_cnt*sub_CPI + mul_cnt*mul_CPI + div_cnt*div_CPI + \
    lw_cnt*lw_CPI + sw_cnt*sw_CPI + others_cnt*others_CPI + \
    fadd_cnt*fadd_CPI + fsub_cnt*fsub_CPI + fmul_cnt*fmul_CPI + fdiv_cnt*fdiv_CPI + \
    flw_cnt*flw_CPI + fsw_cnt*fsw_CPI;

#define macro_arraymul_baseline_cpu_time arraymul_baseline_cpu_time = arraymul_baseline_cycle_count * cycle_time;

#define macro_calc_arraymul_baseline_ratio arraymul_baseline_ratio = (\
    add_cnt*add_CPI + sub_cnt*sub_CPI + mul_cnt*mul_CPI + div_cnt*div_CPI + \
    others_cnt*others_CPI + \
    fadd_cnt*fadd_CPI + fsub_cnt*fsub_CPI + fmul_cnt*fmul_CPI + fdiv_cnt*fdiv_CPI) / \
    (float)arraymul_baseline_cycle_count;


#define macro_calc_arraymul_double_ratio arraymul_baseline_ratio = (\
    add_cnt*add_CPI + sub_cnt*sub_CPI + mul_cnt*mul_CPI + div_cnt*div_CPI + \
    others_cnt*others_CPI + \
    dadd_cnt*dadd_CPI + dsub_cnt*dsub_CPI + dmul_cnt*dmul_CPI + ddiv_cnt*ddiv_CPI) / \
    (float)arraymul_double_cycle_count;

#define macro_arraymul_double_cycle_count arraymul_double_cycle_count = \
    add_cnt*add_CPI + sub_cnt*sub_CPI + mul_cnt*mul_CPI + div_cnt*div_CPI + \
    lw_cnt*lw_CPI + sw_cnt*sw_CPI + others_cnt*others_CPI + \
    dadd_cnt*dadd_CPI + dsub_cnt*dsub_CPI + dmul_cnt*dmul_CPI + ddiv_cnt*ddiv_CPI + \
    dlw_cnt*dlw_CPI + dsw_cnt*dsw_CPI;

#define macro_arraymul_double_cpu_time arraymul_double_cpu_time = arraymul_double_cycle_count * cycle_time;

