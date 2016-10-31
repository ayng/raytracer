from random import random

size = 3

with open('blanket.obj', 'w') as f:
    for j in range(size):
        for i in range(size):
            f.write('v {} {} {}\n'.format(\
                    float(i) / (size-1), \
                    float(j) / (size-1), \
                    0.4 * (random() - 0.5)))
                    #0))

    for i in range(size-1):
        for j in range(size-1):
            print i, j
            tri1 = (i+j*size, i+(j+1)*size, (i+1)+(j+1)*size)
            tri2 = ((i+1)+j*size, i+j*size, (i+1)+(j+1)*size)
            f.write('f {} {} {}\n'.format(*map(lambda x: x+1, reversed(tri1))))
            f.write('f {} {} {}\n'.format(*map(lambda x: x+1, reversed(tri2))))

