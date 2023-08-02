
from gplearn.genetic import SymbolicRegressor
import numpy as np
import time

t0 = time.time()
generations = 1000
est = SymbolicRegressor(generations=generations)

x = np.linspace(0, 4*np.pi, num=201)
y = np.sin(x) + np.cos(x + x) + np.random.normal(scale=0.2, size=201)


est.fit(x, y)
y_pred = est.predict(x)

print(time.time()-t0)