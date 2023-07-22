def evaluate(expr, x):
    expr = expr.replace('x', str(x))

    try:
        result = eval(expr)
    except Exception as e:
        print(f"Error evaluating expression: {e}")
        return None

    return result

def evaluate_n_variables(expr, vars):
    try:
        result = eval(expr, {}, vars)
    except Exception as e:
        print(f"Error evaluating expression: {e}")
        return None

    return result




expr = "((3 - (x * 1)) + x)"
x = 2.0
print(evaluate(expr, x))  


expr = "((a - (b * c)) + d)"
vars = {'a': 3, 'b': 2, 'c': 1, 'd': 2}
print(evaluate_n_variables(expr, vars)) 

print(eval("2+1"))