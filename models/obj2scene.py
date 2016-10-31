vertices = []
triangles = []
with open('bunny.obj', 'r') as f:
    for line in f:
        if line.startswith('v'):
            prefix, v0, v1, v2 = line.split(' ')
            vertices.append(map(float, (v0, v1, v2)))
        if line .startswith('f'):
            prefix, t0, t1, t2 = line.split(' ')
            triangles.append([vertices[int(t)-1] for t in (t0, t1, t2)])


with open('bunny_obj.txt', 'w') as f:
    for tri in triangles:
        vert = [' '.join(map(str,v)) for v in tri]
        line = ' '.join(vert)
        line = 'tri ' + line + '\n'
        f.write(line)

