from sympy import sympify

infix_expression = "2/0"

simplified_expression = sympify(infix_expression)

print(simplified_expression)