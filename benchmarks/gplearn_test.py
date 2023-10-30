
from gplearn.genetic import SymbolicRegressor
import numpy as np
import time

data = np.genfromtxt("data/simple_dataset_3_variables.csv", delimiter=",", skip_header=1)
X = data[:, 0:2]
y = data[:, 2]

#X as double 64
X = X.astype(np.float64)
y = y.astype(np.float64)

#Reshape to 2D
X = X.reshape(-1, 2)
y = y.reshape(-1, 1)

t0 = time.time()
generations = 500
population_size = 100
mutation_prob=0.5

est = SymbolicRegressor(population_size = population_size,generations = generations, p_crossover=mutation_prob)

est.fit(X, y)
print(est._program)

print(time.time()-t0)