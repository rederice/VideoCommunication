import sys

def psnr(a, size):
    return 10 * math.log10(255 * 255 * size / a)


def main():
    filename = sys.argv[1]
    ftype = sys.argv[2]
    count = 0
    if ftype == "CIF":
        frame_row = 352
        frame_col = 288
    elif ftype == "QCIF":
        frame_row = 176
        frame_col = 144
    else:
        perror("please input CIF or QCIF file.")
    frame_size = frame_row*frame_col
    _frame_size = int(frame_size / 4)
    y = []
    u = []
    v = []
    with open(filename, "rb") as fp:
        while True:
            y2 = fp.read(frame_size)
            if y2 == b"":
                break
            else:
                y.append(y2)
            u.append(fp.read(_frame_size))
            v.append(fp.read(_frame_size))
            count += 1
    print("Frame count:", count)
    print("done")

    with open("half_"+filename, "wb") as fp:
        with open("comp_"+filename, "wb") as fr:
            for i in range(len(y)):
                if i%2 == 0:
                    fp.write(y[i])
                    fp.write(u[i])
                    fp.write(v[i])
                else:
                    fr.write(y[i])
                    fr.write(u[i])
                    fr.write(v[i])

if __name__ == "__main__":
    main()
