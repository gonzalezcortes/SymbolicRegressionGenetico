import time
from genetico import parallel_test as prt

data = prt.generate_synthetic_data(5000000)

start = time.time()
prt.loop_1(data)
print("loop_1: ", time.time() - start)

start = time.time()
prt.loop_2(data)
print("loop_2: ", time.time() - start)


