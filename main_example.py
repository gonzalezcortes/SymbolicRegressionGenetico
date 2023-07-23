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
expressions = gsp.create_initial_population(100, 3)
#print(f"Initial population: {pop}")
print(expressions[0])
generations = 10
for gen in range(generations):
    best_expressions = gsp.get_best_expressions(expressions,0.10,"mse", x, y)

for i in range(99):
    combined_expr = gsp.merge_expressions(expressions[i],expressions[i+1])
    combined_values = gsp.evaluate_fx(combined_expr, x)
    mse_c = metrics.mse(y, combined_values)
    print(mse_c)



# Evaluate an expression
slowestMSE = float('inf')
for expr in expressions:
    values = gsp.evaluate_fx(expr, x)
    
    mse_1 = metrics.mse(y, values)
    ##print(f'expression {expr} mse_1: {mse_1}')

    if mse_1 < slowestMSE:
        slowestMSE = mse_1
        slowestExpr = expr
        y1 = values
    
print(f"Slowest MSE is {slowestMSE} with expression {slowestExpr}")

### Plot ###
plt.plot(x,y, label = "target")
plt.plot(x,y1, label = "predicted")
plt.legend()
plt.show()
