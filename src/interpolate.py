from scipy.interpolate import interp2d
from scipy.interpolate import griddata
import pandas as pd
import numpy as np
import sys
import heatmap

def interpolate(out_path):
    df = pd.read_csv("./raw_matrix.csv",header=None)
    rows = df.shape[0]
    cols = df.shape[1]
    arr = df.to_numpy()

    x = np.where(arr != 0)

    # Another approach; Not adopted.
    # points = np.column_stack((x[0],x[1]))
    # values = arr[x].flatten()

    # grid_x, grid_y = np.mgrid[0:5:6j, 0:4:5j]
    # grid_z = griddata(points, values, (grid_x, grid_y), method='nearest')
    # print(grid_z)

    f = interp2d(x[0], x[1], arr[x].flatten(), kind='linear')
    res = arr.copy()
    res[arr==0] = f(range(rows), range(cols)).T[arr==0]
    res[res>1] = 1
    # res[res<0] = 0
    print(res)

    np.savetxt(out_path,res,delimiter=',')

def main():
    out_path = sys.argv[1]
    l_ref = int(sys.argv[2])
    h_ref = int(sys.argv[3])
    l_mod = int(sys.argv[4])
    h_mod = int(sys.argv[5])
    interpolate(out_path)
    heatmap.showHeatMap(out_path, l_ref,h_ref,l_mod,h_mod)

if __name__ == "__main__":
    main()


