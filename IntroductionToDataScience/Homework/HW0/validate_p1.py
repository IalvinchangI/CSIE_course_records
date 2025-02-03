# hw0 p1 Multiplication of Polynomial Texts <validate>
# author IalvinchangI

import sympy
import subprocess
import re


INPUT_TITLE  = "Input the polynomials: "
OUTPUT_TITLE = "Output Result: "


def change_to_sympy(expression: str) -> str:
    expression = expression.replace("^", "**")
    
    match_numder_unknown = re.findall("[+-]?\d+[a-zA-Z]", expression)    # -2
    match_unknown_order = re.findall("[a-zA-Z]\d+", expression)          # 1
    match_unknown_unknown = re.findall("(?=([a-zA-Z]{2}))", expression)  # -2

    for replace_to in (
        [(match_str, match_str.replace(match_str[0], match_str[0] + "**")) for match_str in match_unknown_order] + 
        [(match_str, match_str.replace(match_str[-1], "*" + match_str[-1])) for match_str in match_numder_unknown + match_unknown_unknown]
    ):
        expression = expression.replace(replace_to[0], replace_to[1], 1)

    expression = expression.replace(")(", ")*(")
    return expression


with open("testcase_p1.txt", mode="r") as testcase_file:
    for expression in testcase_file:
        expression = expression.strip()
        if expression == "":
            continue

        sympy_expression = change_to_sympy(expression)

        # sympy's result
        sympy_result = sympy.expand(sympy_expression)

        # my result
        result = subprocess.run(["python", "hw0_p1.py"], input=expression, capture_output=True, text=True)
        my_raw_result = result.stdout.replace(INPUT_TITLE + OUTPUT_TITLE, "").split(OUTPUT_TITLE)
        my_result1 = sympy.sympify(change_to_sympy(my_raw_result[0]))
        my_result2 = sympy.sympify(change_to_sympy(my_raw_result[1]))

        # print(result.stdout)
        if my_result1 != sympy_result or my_result2 != sympy_result:
            print("=" * 50 + " error " + "=" * 50)
            print(f"expression: {expression}\n" + 
                  f"origin1:{my_raw_result[0].strip()}\n" + 
                  f"origin2:{my_raw_result[1].strip()}\n" + 
                  f"my1:\t{my_result1}\n" + 
                  f"my2:\t{my_result2}\n" + 
                  f"sympy:\t{sympy_result}")
            print("=" * 107)
    print("finish")
