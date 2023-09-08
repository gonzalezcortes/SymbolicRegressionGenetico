from genetico import cuda_kernel
import numpy as np

data = [1, 2, 3, 4, 5]
cuda_kernel.square(data)

print(data)  # Output should be [1, 4, 9, 16, 25]
