import matplotlib.pyplot as plt
import numpy as np
import time
from genetico import geneticSymbolicRegression as gsp
#from genetico import geneticSymbolicRegressionRN as gspRN
from genetico import geneticSymbolicRegressionRN_MV as gspRNMV
from genetico import metrics 
np.random.seed(8)

t0 = time.time()

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





print(X.shape)

trainer = gspRNMV.Training()


#trainer.testCombineLikeTerms()


print(len(data))

#X = data[:, 0:2]
#target = data[:, 2]
"""
x = X[:, 0].reshape(50, 50)
y = X[:, 1].reshape(50, 50)
z = target.reshape(50, 50)

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.plot_surface(x, y, z, cmap='viridis')

ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('f(x, y)')
ax.set_title('3D Surface Plot of f(x, y) = x^2 + y^2')

#plt.show()
"""

population_size = 100#100
depth = 3
generations = 100 #100
elite_perc = 0.10 #0.10
mutation_prob = 0.10 #0.10
grow_prob = 0 #0.001
metric = "mse"


print(f"Running with a population size of {population_size} and {generations} generations.")

print("*------------*------------*------------")


trainer.set_matrix_x_from_numpy(X)
trainer.set_matrix_y_from_numpy(y)

## Parameters

trainer.set_binary_operators([ "+", "-", "*", "/", "^"]) #"+", "-", "*", "/", "^"
trainer.set_unary_operators(["cos", "sin"])
#trainer.set_unary_operators(["sin"])
trainer.set_terminals(["X","Y"])
trainer.set_constants(["0.5","1","2","3","4","5","6","7","8"])

#z = x^2 + y^2

##trainer.print_operators()

t2 = time.time()

## Training
t1 = time.time()
sorted_expressions_2 = trainer.genetic_training(population_size, depth, generations, metric, elite_perc, mutation_prob, grow_prob)
print("Running time", time.time()-t1)


print("*------------*------------*------------")
print(sorted_expressions_2[0][1], " mse: ", sorted_expressions_2[0][0])
