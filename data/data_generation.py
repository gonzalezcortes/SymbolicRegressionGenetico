import numpy as np

# ######## simple_dataset_1_variable A & B ########

x = np.linspace(0, 4*np.pi, num=201)
y = np.sin(x) + np.cos(x + x) + np.random.normal(scale=0.2, size=201)

dataset = np.stack((x, y), axis=1)
np.savetxt("data/simple_dataset_1_variable_A.csv", dataset, delimiter=",", header="x,y", comments="")

x = np.linspace(0, 4*np.pi, num=201)
y = np.sin(x) + np.exp((3*x)/2) + np.cos(x + x) + np.random.normal(scale=0.2, size=201)

dataset = np.stack((x, y), axis=1)
np.savetxt("data/simple_dataset_1_variable_B.csv", dataset, delimiter=",", header="x,y", comments="")

######## simple_dataset_2_variables.csv ########

x = np.linspace(-10, 10, 50)
y = np.linspace(-10, 10, 50)
x, y = np.meshgrid(x, y)

z = x**2 + y**2

x_flat = x.flatten()
y_flat = y.flatten()
z_flat = z.flatten()


dataset = np.stack((x_flat, y_flat, z_flat), axis=1)
np.savetxt("data/simple_dataset_2_variables.csv", dataset, delimiter=",", header="x,y,z", comments="")

######## simple_dataset_3_variables.csv ########

x = np.linspace(-10, 10, 50)
y = np.linspace(-10, 10, 50)
x, y = np.meshgrid(x, y)

z = -3*x**2 - y**2 + 2*x*y

x_flat = x.flatten()
y_flat = y.flatten()
z_flat = z.flatten()


dataset = np.stack((x_flat, y_flat, z_flat), axis=1)
np.savetxt("data/simple_dataset_3_variables.csv", dataset, delimiter=",", header="x,y,z", comments="")


