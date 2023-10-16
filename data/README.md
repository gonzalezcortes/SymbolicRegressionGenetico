<h2> Datasets <h2>

<!-- html list --->
* simple_dataset_3_variables.csv

Generated to get an equation of 2 variables.

<code> 
import numpy as np
x = np.linspace(-10, 10, 50)
y = np.linspace(-10, 10, 50)
x, y = np.meshgrid(x, y)
z = x**2 + y**2
x_flat = x.flatten()
y_flat = y.flatten()
z_flat = z.flatten()
dataset = np.stack((x_flat, y_flat, z_flat), axis=1)
np.savetxt("data/simple_dataset_3_variables.csv", dataset, delimiter=",", header="x,y,z", comments="")

</code>