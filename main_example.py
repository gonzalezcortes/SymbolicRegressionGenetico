import matplotlib.pyplot as plt
import numpy as np
import time
from genetico import geneticSymbolicRegression as gsp
from genetico import metrics 

###### First Equation to test ######
x = np.linspace(0, 3*np.pi, num=201)
y = np.sin(2*x+x) + np.cos(x)

pop_size = 10000
depth = 5

x_s =  list([1,2,3,4,5,6,7,8,0])

single_expr = gsp.generate_random_expr(3)

# Create an initial population
expressions = gsp.create_initial_population(10, 3)
#print(f"Initial population: {pop}")

# Evaluate an expression
for expr in expressions:
    value = gsp.evaluate_fx(expr, x)
    mse_1 = metrics.mse(y, value)
    print(f'expression {expr} mse_1: {mse_1}')

print(gsp.modify_binary_operators(expr))
    