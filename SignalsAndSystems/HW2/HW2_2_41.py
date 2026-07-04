import numpy
from scipy.signal import deconvolve

x_n: numpy.ndarray = numpy.array((1, -1, 2, 4))
y_n: numpy.ndarray = numpy.array((2, 6, 4, 0, 8, 5, 12))

print(2.41)

# question (a)
def summation(in1: numpy.ndarray, in2: numpy.ndarray, n):
    in1_ = numpy.pad(in1, (0, max(0, n - len(in1))), "constant")
    in2_ = numpy.pad(in2, (0, max(0, n - len(in2))), "constant")
    
    sum_ = 0
    for k in range(0, n):
        in1_k = in1_[k]
        sum_ += in1_k * in2_[n - k]
    return sum_

def recursive_algorithm(x_n: numpy.ndarray, y_n: numpy.ndarray) -> numpy.ndarray:
    output = list()
    x0 = x_n[0]

    # n = 0..
    for n in range(0, len(y_n) - len(x_n) + 1):
        h_n = numpy.array(output)
        hn = (y_n[n] - summation(h_n, x_n, n)) / x0
        output.append(hn)

    return numpy.array(output)

a_h_n = recursive_algorithm(x_n, y_n)
print(f"(a) {a_h_n}")


# question (b)
b_h_n, remainder = deconvolve(y_n, x_n)
print(f"(b) {b_h_n}")
