import matplotlib.pyplot as plt
import numpy as np
import time
from genetico import geneticSymbolicRegression as gsp

###### First Equation to test ######
x = np.linspace(0, 3*np.pi, num=201)
y = np.sin(2*x+x) + np.cos(x)
pop_size = 10000
depth = 5

expr = gsp.generate_random_expr(3)
print(f"Generated expression: {expr}")

# Create an initial population
pop = gsp.create_initial_population(10, 3)
#print(f"Initial population: {pop}")


for i in range(10):
    print(pop[i])

# Evaluate an expression
###value = gsp.evaluateFX(expr, 3)
