import pandas as pd
import matplotlib.pyplot as plt


# search for the local maxima in perceptual quality.
def find_local_optimal(threshold):
    df = pd.read_csv("./combined.csv",header=None)
    rows = df.shape[0]
    cols = df.shape[1]

    # To view the whole dataframe column by column with respect to ref
    # for (colname, colval) in df.iteritems():
        #print(colname, colval.values)

    i = 0
    flag = 1
    local_max = 0
    # Corollary: For any given δ, Q(x) is the local maximum in [x, x + δ]
    # if P(x, y) ≤ 0.5 for all 0 ≤ y ≤ δ
    # Q(x) is not the local maximum in [x, x + δ] if P(x, y) > 0.5
    # for any 0 ≤ y ≤ δ.
    while 1:
        for j in range(min(len(df[i]), int(threshold))):
            if df[i][j] > 0.5:
                flag = 0
        if flag == 1:
            local_max = i
            break
        if i == rows - 1:
            break
        i += 1
        flag = 1
    print("Local maximum ref: ", local_max)


# In practice, we like to find the local maximum within the largest
# possible range, and the largest δ we can identify in the JND surface
# is the range of the increase ymax. Therefore, we discard any ref that
# does not satisfy P(ref, y) ≤ 0.5, where 0 ≤ y ≤ ymax, until we
# find the first ref that satisfies the condition

def find_global_optimal():
    df = pd.read_csv("./combined.csv",header=None)
    rows = df.shape[0]
    cols = df.shape[1]

    # To view the whole dataframe column by column with respect to ref
    # for (colname, colval) in df.iteritems():
        # print(colname, colval.values)

    i = 0
    flag = 1
    global_max = 0

    while 1:
        for j in range(len(df[i])):
            if df[i][j] > 0.5:
                flag = 0
        if flag == 1:
            global_max = i
            break
        if i == rows - 1:
            break
        i += 1
        flag = 1
    print("Global maximum ref: ", global_max)


def show_normalized(ref):
    df = pd.read_csv("./combined.csv",header=None)
    x=[]
    y=[]
    for i in range(len(df[ref])):
        x.append(ref+i)
        y.append(df[i][ref])

    plt.plot(x, y, label="testing")
    plt.xlabel("Normalized Control Input")
    plt.ylabel("Perceptual Quality (Normalized MOS)")
    plt.ylim(0, 1)
    plt.xlim(0, 1)
    plt.legend()
    plt.show()


def main():
    print("Please input the threshold to find the local maximum")
    threshold = int(input())
    find_local_optimal(threshold)

    # As there may be multiple local mxima in perceptual quality depending
    # on δ, we can reduce δ from ymax to find other ref if necessary

    find_global_optimal()
    ref = 4
    show_normalized(ref)


if __name__ == "__main__":
    main()


