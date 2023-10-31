from sympy import sympify

infix_expression = "(-2*X*(X - Y) - 01*X - 2*Y - 54)"

simplified_expression = sympify(infix_expression)

print(simplified_expression)

expanded_expression = simplified_expression.expand()

print(expanded_expression)
