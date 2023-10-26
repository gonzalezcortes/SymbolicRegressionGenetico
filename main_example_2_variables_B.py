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


trainer.testCombineLikeTerms()

