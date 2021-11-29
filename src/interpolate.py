from scipy.interpolate import interp2d
from scipy.interpolate import griddata
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import sys

def interpolate():
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
    res[res<0] = 0
    print(res)

    np.savetxt("interpolated_matrix.csv",res,delimiter=',')

def showHeatMap(l_ref,h_ref,l_mod,h_mod):
    df = pd.read_csv("./interpolated_matrix.csv",header=None)
    data = df.to_numpy()
    XLabel = list(range(l_ref,h_ref+1))
    YLabel = list(range(l_mod,h_mod+1))

    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.set_xticks(range(len(XLabel)))
    ax.set_xticklabels(XLabel)
    ax.set_yticks(range(len(YLabel)))
    ax.set_yticklabels(YLabel)
    img = ax.imshow(data, cmap = plt.cm.jet)
    plt.colorbar(img)
    plt.title("JND Surface Visualization")
    plt.savefig("./JND_Surface.png")
    plt.show()


def main():
    l_ref = int(sys.argv[1])
    h_ref = int(sys.argv[2])
    l_mod = int(sys.argv[3])
    h_mod = int(sys.argv[4])
    interpolate()
    showHeatMap(l_ref,h_ref,l_mod,h_mod)

if __name__ == "__main__":
    main()


