import matplotlib.pyplot as plt
import numpy as np
import time
from genetico import geneticSymbolicRegression as gsp
from genetico import geneticSymbolicRegressionRN as gspRN
from genetico import metrics 
np.random.seed(8)

t0 = time.time()

x = np.linspace(-10, 10, 50)
y = np.linspace(-10, 10, 50)
x, y = np.meshgrid(x, y)

# Calculate the function values based on the grid
z = x**2 + y**2

# Flatten the x, y, and z arrays
x_flat = x.flatten()
y_flat = y.flatten()
z_flat = z.flatten()

# Stack them into a single dataset
dataset = np.stack((x_flat, y_flat, z_flat), axis=1)

# Save the dataset to a CSV file
np.savetxt("data/simple_dataset_3_variables.csv", dataset, delimiter=",", header="x,y,z", comments="")


fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.plot_surface(x, y, z, cmap='viridis')

ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('f(x, y)')
ax.set_title('3D Surface Plot of f(x, y) = x^2 + y^2')

plt.show()



# Get initial values
population_size = 100
depth = 3
generations = 100 #100
elite_perc = 0.10
mutation_prob = 0.10
grow_prob = 0.01 #0.001
metric = "mse"


print(f"Running with a population size of {population_size} and {generations} generations.")

print("*------------*------------*------------")
t1 = time.time()
sorted_expressions_1 = gsp.genetic_training(population_size, depth, generations, metric, elite_perc, mutation_prob, grow_prob, x, y)
y2 =  gsp.evaluate_fx(sorted_expressions_1[0][1], x)
print("Running time", time.time()-t1)

print("*------------*------------*------------")
t2 = time.time()
sorted_expressions_2 = gspRN.genetic_training(population_size, depth, generations, metric, elite_perc, mutation_prob, grow_prob, x, y)
y2B =  gspRN.evaluate_fx(sorted_expressions_2[0][1], x)
print("Running time rpn", time.time()-t2)

print("*------------*------------*------------")
print("Expression A: ", sorted_expressions_1[0][1], "mse ", metrics.mse(y2,y))
print("Expression B: ", sorted_expressions_2[0][1], "mse ", metrics.mse(y2B,y))


print("Running time total", time.time()-t0)


### Plot ###
plt.plot(x,y, label = "target")
plt.plot(x,y2, label = "predicted")
plt.legend()
plt.show()

