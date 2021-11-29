from scipy.interpolate import interp2d
from scipy.interpolate import griddata
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import sys

df = pd.read_csv("./raw_matrix.csv",header=None)
rows = df.shape[0]
cols = df.shape[1]
arr = df.to_numpy()

x = np.where(arr != 0)

# points = np.column_stack((x[0],x[1]))
# values = arr[x].flatten()

# grid_x, grid_y = np.mgrid[0:5:6j, 0:4:5j]
# grid_z = griddata(points, values, (grid_x, grid_y), method='nearest')
# print(grid_z)

f = interp2d(x[0], x[1], arr[x].flatten(), kind='linear')
res = arr.copy()
res[arr==0] = f(range(rows), range(cols)).T[arr==0]
res[res>1] = 1
res[res<0] = 0
print(res)

np.savetxt("interpolated_matrix.csv",res,delimiter=',')



