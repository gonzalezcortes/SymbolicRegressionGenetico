from sympy import sympify

infix_expression = "(-2*X ^ 2 + 2*X*Y - X*(1 - X) + 6*X + 2*Y - 5)"

simplified_expression = sympify(infix_expression)

print(simplified_expression)