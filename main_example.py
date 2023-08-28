import matplotlib.pyplot as plt
import numpy as np
import time
from genetico import geneticSymbolicRegression as gsp
from genetico import metrics 

###### First Equation to test ######
#x = np.linspace(0, 3*np.pi, num=201)
#y = np.sin(2*x+x) + np.cos(x)

#x = np.linspace(0, 4*np.pi, num=201)
#y = np.sin(x) + np.cos(x + x) + np.random.normal(scale=0.2, size=201)

t0 = time.time()
x = np.linspace(0, 4*np.pi, num=201)
y = np.sin(x) + np.exp((3*x)/2) + np.cos(x + x) + np.random.normal(scale=0.2, size=201)

# Get initial values
population_size = 100
depth = 3
generations = 2 #100
elite_perc = 0.10
mutation_prob = 0.10
grow_prob = 0.01 #0.001
metric = "mse"

qq = "(sin((2)))"
print(qq)
print(gsp.simplify_expr(qq))

print(f"Running with a population size of {population_size} and {generations} generations.")
sorted_expressions_1 = gsp.genetic_training(population_size, depth, generations, metric, elite_perc, mutation_prob, grow_prob, x, y)

y2 =  gsp.evaluate_fx(sorted_expressions_1[0][1], x)

print("Expression ", sorted_expressions_1[0][1], "mse ", metrics.mse(y2,y))

print("Running time", time.time()-t0)


### Plot ###
plt.plot(x,y, label = "target")
plt.plot(x,y2, label = "predicted")
plt.legend()
plt.show()

print(gsp.simplify_expr(sorted_expressions_1[0][1]))

