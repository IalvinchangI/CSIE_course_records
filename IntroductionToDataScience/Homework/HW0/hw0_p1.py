# hw0 p1 Multiplication of Polynomial Texts
# author IalvinchangI


ORDER_STR = "^"
MULTI_STR = "*"
ADD_STR   = "+"
SUB_STR   = "-"

START_STR = "("
END_STR   = ")"

UNKNOWN = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
NUMBER = "0123456789"


class Term():
    def __init__(self) -> None:
        self.__order = dict()
        self.coefficient = 1
    
    def __getitem__(self, variable: str):
        if variable not in self.__order:
            return 0
        return self.__order[variable]

    def __setitem__(self, variable: str, order: int):
        self.__order[variable] = order

    def __eq__(self, value: object) -> bool:
        """ if the variable and order are equal, then return true """
        if isinstance(value, Term) == False:
            return False
        return value.get_variable() == self.get_variable()

    def __lt__(self, value: object) -> bool:  # self < value == True
        if isinstance(value, Term) == False:
            raise TypeError(f"{value.__class__} is not Term")

        # total order
        self_total_order  = sum(self.__order.values())
        value_total_order = sum(value.__order.values())
        if self_total_order != value_total_order:
            return self_total_order < value_total_order
        
        # unknown order
        for unknown in sorted(list(self.__order.keys() | value.__order.keys())):
            if self[unknown] != value[unknown]:
                return self[unknown] < value[unknown]
        return False  # equal

    def __add__(self, other):
        if not (self == other):
            raise AttributeError()

        output = Term()
        output.__order = self.__order.copy()
        output.coefficient = self.coefficient + other.coefficient
        return output
    
    def __mul__(self, other):
        output = Term()

        output.__order = self.__order.copy()
        for order in other.__order:
            if order in output.__order:
                output[order] += other.__order[order]
            else:
                output[order] = other.__order[order]

        output.coefficient = self.coefficient * other.coefficient

        return output
        
    
    def get_variable(self, use_operators_TF: bool = False):
        output = ""
        for variable in sorted(list(self.__order.keys())):
            output += variable
            
            order = self.__order[variable]
            if order > 1:
                if use_operators_TF == True:
                    output += ORDER_STR
                output += str(order)
        
        return output

    def to_string(self, use_operators_TF: bool):
        if self.coefficient == 0:
            return ""
        
        variable = self.get_variable(use_operators_TF)
        if variable == "":
            return str(self.coefficient)
        
        # variable != ""
        output = ""
        if self.coefficient == -1:
            output += "-"
        elif self.coefficient != 1:  # self.coefficient != 1, -1, 0
            output += str(self.coefficient)
            if use_operators_TF == True:
                output += MULTI_STR
        output += variable

        return output


def detect(polynomial: str, start: int) -> tuple[int]:
    """
    detect the next word is unknown, number or the end of term

    return: ({0: end, 1: unknown, 2: number}, pos)
    """
    end = start
    while True:
        if polynomial[end] == ADD_STR or polynomial[end] == SUB_STR or polynomial[end] == END_STR:
            return 0, end
        if polynomial[end] in UNKNOWN:
            return 1, end
        if polynomial[end] in NUMBER:
            return 2, end
        # else continue

        end += 1

def read_number(polynomial: str, start: int) -> tuple[int]:
    """ !! the char at start must be in the NUMBER """
    end = start
    while True:
        end += 1
        if polynomial[end] not in NUMBER:
            break
        
    return int(polynomial[start:end]), end

def read_term(term: Term, expression: str) -> str:
    """ read the first term in the expression """
    index = 0

    # sign of coefficient
    if expression[index] == SUB_STR:
        term.coefficient *= -1
        index += 1
    if expression[index] == ADD_STR:
        index += 1

    while True:
        # order and variable
        result, index = detect(expression, index)

        if result == 0:  # end
            break
        if result == 1:  # unknown
            unknown = expression[index]
            result, index = detect(expression, index + 1)  # assume the count of the char of the unknown is 1
            if result == 2:  # the order of unknown
                term[unknown], index = read_number(expression, index)
            else:
                term[unknown] = 1
        elif result == 2:  # number (coefficient)
            number, index = read_number(expression, index)
            term.coefficient *= number

    return expression[index:]

def read_polynomial(parse_polynomial: list, expression: str) -> str:
    """ read the expression until reaching the END_STR """
    while True:
        if expression[0] == END_STR:
            break
        
        term = Term()
        expression = read_term(term, expression)
        parse_polynomial.append(term)
    
    return expression[1:]


def multiply(polynomial1: list[Term], polynomial2: list[Term]) -> list[Term]:
    """ multiplication of polynomial """
    output_polynomial = list()
    for term1 in polynomial1:
        for term2 in polynomial2:
            output_polynomial.append(term1 * term2)
    
    return output_polynomial


def print_polynomial(polynomial: list[Term], use_operators_TF: bool):
    output = ""
    if len(polynomial) > 0:
        output += polynomial[0].to_string(use_operators_TF)
        for i in range(1, len(polynomial)):
            if polynomial[i].coefficient > 0:
                output += ADD_STR
            output += polynomial[i].to_string(use_operators_TF)
    if output == "":
        output = "0"
    print(output)




############################# main #############################
# input
expression = input("Input the polynomials: ")

# read
parse_polynomial: list[Term] = list()
while True:
    if len(expression) == 0:
        break
    if expression[0] == START_STR:
        parse_polynomial.append(list())
        expression = read_polynomial(parse_polynomial[-1], expression[1:])
        continue
    expression = expression[1:]

# multiplication
index = 1
while index < len(parse_polynomial):
    parse_polynomial.append(multiply(parse_polynomial[index - 1], parse_polynomial[index]))
    index += 2

# simplify
final_polynomial: list[Term] = sorted(parse_polynomial[-1], reverse=True)
index = 1
while index < len(final_polynomial):
    if final_polynomial[index - 1] == final_polynomial[index]:
        final_polynomial[index - 1] = final_polynomial[index - 1] + final_polynomial.pop(index)
    else:
        index += 1

# output
print("Output Result: ", end="")
print_polynomial(final_polynomial, True)
print("Output Result: ", end="")
print_polynomial(final_polynomial, False)
