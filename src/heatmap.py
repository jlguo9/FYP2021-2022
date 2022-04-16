import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import sys
import numpy as np

class heatmap:
    def showHeatMap(path, l_ref, h_ref, l_mod, h_mod):
        df = pd.read_csv(path,header=None)
        data = df.to_numpy()
        XLabel = list(range(l_ref,h_ref+1))
        YLabel = list(range(l_mod,h_mod+1))
        fig = plt.figure()
        ax = fig.add_subplot(111)
        ax.set_xticks(range(len(XLabel)))
        ax.set_xticklabels(XLabel)
        ax.set_yticks(range(len(YLabel)))
        ax.set_yticklabels(YLabel[::-1])
        img = ax.imshow(data, cmap = plt.cm.jet)
        plt.colorbar(img)
        plt.title("JND Surface Visualization")
        plt.savefig(path+".png")
        plt.show()
        
if __name__ == "__main__":
    out_path = sys.argv[1]
    l_ref = int(sys.argv[2])
    h_ref = int(sys.argv[3])
    l_mod = int(sys.argv[4])
    h_mod = int(sys.argv[5])
    heatmap.showHeatMap(out_path, l_ref, h_ref, l_mod, h_mod)