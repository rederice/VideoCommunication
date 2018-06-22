import math
import sys
BLOCK = 8
_STEP_ = 8


def expand_uv_frame_420(matrix, COL_SIZE, ROW_SIZE):
    new_matrix = []
    for i in range(len(matrix)):
        tmp_matrix = [0 for i in range(COL_SIZE * ROW_SIZE)]
        for col in range(COL_SIZE):
            for row in range(ROW_SIZE):
                tmp_matrix[col * ROW_SIZE + row] = matrix[i][(col // 2) * (ROW_SIZE // 2) + row // 2]
        new_matrix.append(tmp_matrix)
    return new_matrix


def shrink_uv_frame_420(matrix, COL_SIZE, ROW_SIZE):
    new_matrix = []
    for i in range(len(matrix)):
        tmp_matrix = [0 for _ in range(COL_SIZE // 2 * ROW_SIZE // 2)]
        for col in range(COL_SIZE):
            for row in range(ROW_SIZE):
                tmp_matrix[(col // 2) * (ROW_SIZE // 2) + row // 2] += matrix[i][col * ROW_SIZE + row]
                if tmp_matrix[col // 2 * ROW_SIZE // 2 + row // 2] > 256 * 4:
                    print(col, row)
        tmp_matrix = [i // 4 for i in tmp_matrix]
        new_matrix.append(tmp_matrix)
    return new_matrix


def create_new_frame(y_matrix, u_matrix, v_matrix, new_y_frame, new_u_frame, new_v_frame,
                     frame, interpolation_motion_vectors, COL_SIZE, ROW_SIZE):
    tmp_y_frame = [0 for i in range(COL_SIZE * ROW_SIZE)]
    tmp_u_frame = [0 for i in range(COL_SIZE * ROW_SIZE)]
    tmp_v_frame = [0 for i in range(COL_SIZE * ROW_SIZE)]
    pixel_write_count = [0 for i in range(COL_SIZE * ROW_SIZE)]
    # Write pixels with motion vector.
    for col in range(COL_SIZE//BLOCK):
        for row in range(ROW_SIZE//BLOCK):
            for c in range(BLOCK):
                for r in range(BLOCK):
                    x = ROW_SIZE * col * BLOCK + BLOCK * row + c
                    y = ROW_SIZE * col * BLOCK + BLOCK * row + r
                    new_x = x + interpolation_motion_vectors[frame][col][row][0]
                    new_y = y + interpolation_motion_vectors[frame][col][row][1]
                    if new_x < 0 or new_x > ROW_SIZE-1 or new_y < 0 or new_y > COL_SIZE-1:
                        continue
                    tmp_y_frame[new_y * ROW_SIZE + new_x] += y_matrix[frame][x + y * ROW_SIZE]
                    tmp_u_frame[new_y * ROW_SIZE + new_x] += u_matrix[frame][x + y * ROW_SIZE]
                    tmp_v_frame[new_y * ROW_SIZE + new_x] += v_matrix[frame][x + y * ROW_SIZE]
                    pixel_write_count[new_y * ROW_SIZE + new_x] += 1
    # Process empty pixels and overlapped pixels.
    for col in range(COL_SIZE):
        for row in range(ROW_SIZE):
            if pixel_write_count[col * ROW_SIZE + row] == 0:
                tmp_y_frame[col * ROW_SIZE + row] = \
                    (y_matrix[frame][row + col * ROW_SIZE] + y_matrix[frame + 1][row + col * ROW_SIZE]) // 2
                tmp_u_frame[col * ROW_SIZE + row] = \
                    (u_matrix[frame][row + col * ROW_SIZE] + u_matrix[frame + 1][row + col * ROW_SIZE]) // 2
                tmp_v_frame[col * ROW_SIZE + row] = \
                    (v_matrix[frame][row + col * ROW_SIZE] + v_matrix[frame + 1][row + col * ROW_SIZE]) // 2
            else:
                tmp_y_frame[col * ROW_SIZE + row] = \
                    tmp_y_frame[col * ROW_SIZE + row] // pixel_write_count[col * ROW_SIZE + row]
                tmp_u_frame[col * ROW_SIZE + row] = \
                    tmp_u_frame[col * ROW_SIZE + row] // pixel_write_count[col * ROW_SIZE + row]
                tmp_v_frame[col * ROW_SIZE + row] = \
                    tmp_v_frame[col * ROW_SIZE + row] // pixel_write_count[col * ROW_SIZE + row]
    new_y_frame.append(tmp_y_frame)
    new_u_frame.append(tmp_u_frame)
    new_v_frame.append(tmp_v_frame)



def tss(y_matrix, row, col, motion_vector):
    row_block = row // BLOCK
    col_block = col // BLOCK
    matrix1 = [[0 for x1 in range(row)] for y1 in range(col)]
    matrix2 = [[0 for x1 in range(row)] for y1 in range(col)]
    for frame in range(len(y_matrix) - 1):
        sys.stdout.write("\rFinding motion vectors(TSS)... Frame: " + str(frame+2))
        sys.stdout.flush()
        for c in range(col):
            for r in range(row):
                matrix2[c][r] = int(y_matrix[frame][c * row + r])
                matrix1[c][r] = int(y_matrix[frame + 1][c * row + r])

        block_motion_vector = [[0 for x1 in range(row_block)] for y1 in range(col_block)]
        for idx_x in range(row_block):
            for idx_y in range(col_block):
                min_sad = 1e9
                x = idx_x * BLOCK
                y = idx_y * BLOCK
                min_x = x
                min_y = y
                step = _STEP_
                while step >= 1:
                    step_x = [min_x - step, min_x - step, min_x - step, min_x, min_x, min_x, min_x + step, min_x + step,
                              min_x + step]
                    step_y = [min_y - step, min_y, min_y + step, min_y - step, min_y, min_y + step, min_y - step, min_y,
                              min_y + step]
                    for srh in range(9):
                        if step_x[srh] < 0 \
                                or step_x[srh] + BLOCK >= row or step_y[srh] < 0 \
                                or step_y[srh] + BLOCK >= col:
                            continue
                        tmp_sad = 0
                        for run_i in range(BLOCK):
                            for run_j in range(BLOCK):
                                tmp_sad += abs(
                                    matrix1[y + run_j][x + run_i] - matrix2[step_y[srh] + run_j][step_x[srh] + run_i])
                        if tmp_sad < min_sad:
                            min_sad = tmp_sad
                            min_x = step_x[srh]
                            min_y = step_y[srh]
                    step //= 2
                block_motion_vector[idx_y][idx_x] = [min_x - x, min_y - y]
        motion_vector.append(block_motion_vector)
    print(" done")


def main():
    filename = sys.argv[1]
    ftype = sys.argv[2]
    count = 0
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
    motion_vector = []
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
    tss(y, frame_row, frame_col, motion_vector)
    print("vector done.")

    u_frame = expand_uv_frame_420(u, frame_col, frame_row)
    v_frame = expand_uv_frame_420(v, frame_col, frame_row)

    # interpolation_motion_vectors is half of motion_vectors
    interpolation_motion_vectors = \
        [[[[num // 2 for num in sub1] for sub1 in sub2] for sub2 in sub3] for sub3 in motion_vector]
    new_y_frame = []
    new_u_frame = []
    new_v_frame = []
    for frame in range(count-1):
        sys.stdout.write("\rCreating interpolated frames... Frame: " + str(frame+1))
        sys.stdout.flush()
        create_new_frame(y, u_frame, v_frame, new_y_frame, new_u_frame, new_v_frame, frame, interpolation_motion_vectors, frame_col, frame_row)
    
    final_u = shrink_uv_frame_420(new_u_frame, frame_col, frame_row)
    final_v = shrink_uv_frame_420(new_v_frame, frame_col, frame_row)

    # Reconstruct frames.
    with open("result_"+filename, "wb") as fp:
        for frame in range(len(y)):
            sys.stdout.write("\rCreating video... Frame: " + str(frame * 2 + 1))
            sys.stdout.flush()
            fp.write(bytes(y[frame]))
            fp.write(bytes(u[frame]))
            fp.write(bytes(v[frame]))
            if frame >= count - 1:
                continue
            sys.stdout.write("\rCreating video... Frame: " + str(frame * 2 + 2))
            sys.stdout.flush()
            
            fp.write(bytes(new_y_frame[frame]))
            fp.write(bytes(final_u[frame]))
            fp.write(bytes(final_v[frame]))

if __name__ == "__main__":
    main()
