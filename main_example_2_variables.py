import matplotlib.pyplot as plt
import numpy as np
import time
from genetico import geneticSymbolicRegression as gsp
#from genetico import geneticSymbolicRegressionRN as gspRN
from genetico import geneticSymbolicRegressionRN_MV as gspRNMV
from genetico import metrics 
np.random.seed(8)



data = np.genfromtxt("data/simple_dataset_2_variables.csv", delimiter=",", skip_header=1)

#data = np.genfromtxt("data/simple_dataset_1_variable_A.csv", delimiter=",", skip_header=1)

X = data[:, 0:2]
y = data[:, 2]

#X as double 64
X = X.astype(np.float64)
y = y.astype(np.float64)

#Reshape to 2D
X = X.reshape(-1, 2)
y = y.reshape(-1, 1)


t0 = time.time()

trainer = gspRNMV.Training()


population_size = 100#100
depth = 3
generations = 100 #100
elite_perc = 0.2 #0.2
mutation_prob = 0.5 #0.5
grow_prob = 0.1 #0.1
prune_prob = 0.1 #0.1
metric = "mse"
verbose = False
early_stop = 0.01

print(f"Running with a population size of {population_size} and {generations} generations.")

print("*------------*------------*------------")


trainer.set_matrix_x_from_numpy(X)
trainer.set_matrix_y_from_numpy(y)

## Parameters

trainer.set_binary_operators([ "+", "-", "*", "/","^"]) #"+", "-", "*", "/", "^"
#trainer.set_unary_operators(["sin","cos"])
trainer.set_terminals(["X","Y"])
trainer.set_constants(["0","1"])

#z = x^2 + y^2

##trainer.print_operators()



## Training

sorted_expressions_2 = trainer.genetic_training(population_size, depth, generations, metric, elite_perc, mutation_prob, grow_prob, prune_prob, early_stop, verbose)
print("Running time", time.time()-t0)


print("*------------*------------*------------")
print(sorted_expressions_2[0][1], " mse: ", sorted_expressions_2[0][0])
