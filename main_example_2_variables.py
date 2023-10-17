import matplotlib.pyplot as plt
import numpy as np
import time
from genetico import geneticSymbolicRegression as gsp
from genetico import geneticSymbolicRegressionRN as gspRN
from genetico import geneticSymbolicRegressionRN_MV as gspRNMV
from genetico import metrics 
np.random.seed(8)

t0 = time.time()

data = np.genfromtxt("data/simple_dataset_2_variables.csv", delimiter=",", skip_header=1)

trainer = gspRNMV.Training()

print(len(data))

X = data[:, 0:2]
target = data[:, 2]

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

population_size = 100
depth = 3
generations = 100 #100
elite_perc = 0.10
mutation_prob = 0.10
grow_prob = 0.01 #0.001
metric = "mse"


print(f"Running with a population size of {population_size} and {generations} generations.")

print("*------------*------------*------------")
t2 = time.time()

## Input Data
rows_x, cols_x = X.shape
rows_y, cols_y = y.shape
#gspRNMV.Training.set_matrix_x(X.ravel().tolist(), rows_x, cols_x)
#gspRNMV.Training.set_matrix_y(y.ravel().tolist(), rows_y, cols_y)

arr = np.array([1.0, 2.0, 3.0, 4.0], dtype=np.float64)

trainer.set_matrix_x_from_numpy(X)

#xx = np.array([1,2,3], dtype=np.float64)

#list of 3 valus List[float] numpy declare it as float64

#gspRNMV.Training.print_array(xx)
#gspRNMV.Training.set_matrix_x_from_numpy(X_t)




## Parameters

gspRNMV.Training.set_binary_operators = [ "+", "-", "*", "/" ]
gspRNMV.Training.set_unary_operators = ["sin", "cos" , "exp"]
gspRNMV.Training.set_terminals = ["X"]
gspRNMV.Training.set_constants = ["1", "2", "3", "4", "5", "6", "7", "8", "9", "10"]


#sorted_expressions_2 = gspRNMV.genetic_training(population_size, depth, generations, metric, elite_perc, mutation_prob, grow_prob, X, target)

#y2B =  gspRN.evaluate_fx(sorted_expressions_2[0][1], x)
#print("Running time rpn", time.time()-t2)

#print("*------------*------------*------------")
#print("Expression A: ", sorted_expressions_2[0][1], "mse ", metrics.mse(y2B,y))
#print("Running time total", time.time()-t0)


