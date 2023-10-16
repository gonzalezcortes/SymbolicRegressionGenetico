## Datasets

- **simple_dataset_3_variables.csv**: This dataset was generated to represent an equation of two variables.

### Description

The dataset contains three columns:
- `x`: Represents the x-coordinate.
- `y`: Represents the y-coordinate.
- `z`: Represents the z-coordinate, calculated using the equation: 

- \begin{equation}
  z = x^2 + y^2
\end{equation}
\( z = x^2 + y^2 \).

### Generation

Here is the Python code snippet used for generating the dataset:

```python
import numpy as np

# Generate a grid of x and y values
x = np.linspace(-10, 10, 50)
y = np.linspace(-10, 10, 50)
x, y = np.meshgrid(x, y)

# Calculate the corresponding z values
z = x**2 + y**2

# Flatten the arrays
x_flat = x.flatten()
y_flat = y.flatten()
z_flat = z.flatten()

# Stack the flattened arrays into a single dataset
dataset = np.stack((x_flat, y_flat, z_flat), axis=1)

# Save the dataset to a CSV file within a 'data' directory
np.savetxt("data/simple_dataset_3_variables.csv", dataset, delimiter=",", header="x,y,z", comments="")
