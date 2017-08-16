import matplotlib.pyplot as plt
# %matplotlib inline

def load(fname):
    with open(fname, 'r') as f:
        n = int(f.readline())
        segs = []
        for i in range(n):
            segs.append(list(map(lambda x: int(x), f.readline().split(' '))))
        start = list(map(lambda x: int(x), f.readline().split(' ')))
        finish = list(map(lambda x: int(x), f.readline().split(' ')))
    return segs, start, finish

def visualize(segs, title):
    left, bottom, right, top = segs[0]
    for seg in segs:
        plt.plot([seg[0], seg[2]], [seg[1], seg[3]])
        left = min(left, seg[0], seg[2])
        right = max(right, seg[0], seg[2])
        bottom = min(bottom, seg[1], seg[3])
        top = max(top, seg[1], seg[3])
        
    plt.title(title)
    plt.axis([left-1, right+1, bottom-1, top+2])
    plt.show()    

# folder = 'cmake-build-debug/'
# fname = 'input.txt'
# segs, start, finish = load(folder + fname)
# plt.scatter([start[0], finish[0]], [start[1], finish[1]])
# visualize(segs, fname)

folder = 'tests/input/'
for i in range(1, 16):
    fname = ('00%d.txt'%i)[-7:]
    segs, start, finish = load(folder + fname)
    plt.scatter([start[0], finish[0]], [start[1], finish[1]])
    visualize(segs, fname)

