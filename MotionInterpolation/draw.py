import math
import matplotlib.pyplot as plt
import numpy as np
import sys

def psnr(a, size):
    if a == 0:
        return 0.0
    return 10 * math.log10(255 * 255 * size / a)

def findpsnr(y1, y2, row, col, name):
    print(len(y1),len(y2))
    graph = []
    for frame in range(len(y2)):
        diff = 0.0
        for i in range(col):
            for j in range(row):
                diff += (y1[frame][i*row+col] - y2[frame][i*row+col])**2
        graph.append(psnr(diff, row*col))
    x = np.linspace(1,len(y2),len(y2))
    plt.plot(x, graph)
    plt.title("interpolated PSNR graph")
    plt.xlabel("Frame")
    plt.ylabel("PSNR")
    plt.savefig(name+"_psnr.png")
    print("psnr graph plotted.")

def main():
    file1 = sys.argv[1]
    file2 = sys.argv[2]
    ftype = sys.argv[3]
    frame_row = 0
    frame_col = 0
    if ftype == "CIF":
        frame_row = 352
        frame_col = 288
    elif ftype == "QCIF":
        frame_row = 176
        frame_col = 144
    else:
        print("please input CIF or QCIF file.")
        exit(1)
    frame_size = frame_row*frame_col
    _frame_size = int(frame_size / 4)
    y = []
    u = []
    v = []
    with open(file1, "rb") as fp:
        while True:
            y2 = fp.read(frame_size)
            if y2 == b"":
                break
            else:
                y.append(y2)
            u.append(fp.read(_frame_size))
            v.append(fp.read(_frame_size))
    y_comp = []
    u_comp = []
    v_comp = []
    cnt = 0
    with open(file2, "rb") as fp:
        while True:
            cnt+=1
            if cnt <= len(y)-1:
                yyy = fp.read(frame_size)
                if yyy == b"":
                    break
                else:
                    y_comp.append(yyy)
                u_comp.append(fp.read(_frame_size))
                v_comp.append(fp.read(_frame_size))
            else:
                break
    findpsnr(y, y_comp, frame_row, frame_col, file1)

if __name__ == "__main__":
    main()