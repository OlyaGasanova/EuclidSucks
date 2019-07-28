import bpy
import struct
import bmesh
import mathutils

from mathutils import Matrix

# blender -> OpenGL coord system transform
MatGL = Matrix(([-1, 0, 0, 0],
                [ 0, 0, 1, 0],
                [ 0, 1, 0, 0],
                [ 0, 0, 0, 1]))
InvMatGL = MatGL.inverted()

def writeTransform(file, obj):
    m = MatGL * obj.matrix_local * InvMatGL
    m.transpose()
    # write 16 floats
    for row in m:
        for value in row:
            file.write(struct.pack('f', value))

def writeString(file, str):
    bstr = bytes(str, 'cp1252')
    # write size (1 byte) + string data (size bytes)
    file.write(struct.pack('B', len(bstr)))
    for b in bstr:
        file.write(struct.pack('B', b))

def writeMaterial(file, material):
    texName = ''
    slot = material.texture_slots[0]
    if slot != None:
        texName = bpy.path.basename(slot.texture.image.filepath)
    # write shader file name
    writeString(file, "base.glsl")
    # write texture file name
    writeString(file, texName)

def writeMesh(file, mesh):
    # triangulate the mesh first
    bm = bmesh.new()
    bm.from_mesh(mesh)
    bmesh.ops.triangulate(bm, faces = bm.faces)
    triMesh = bpy.data.meshes.new(name = '~temp~')
    bm.to_mesh(triMesh)
    triMesh.calc_tessface()
    triMesh.calc_tangents()
    bm.free()
    
    indices = []
    dict = {}
    
    uvs = triMesh.tessface_uv_textures
    
    # get indices (array) and vertices (dictionary)
    for f in triMesh.tessfaces:
        for i in range(3):
            vIndex = f.vertices[i]
            vertex = triMesh.vertices[vIndex]
            vc = MatGL * vertex.co
            vn = MatGL * vertex.normal
            t0 = uvs[0].data[f.index].uv[i]
            
            vp = ( vc.x, vc.y, vc.z,    # position
                   vn.x, vn.y, vn.z,    # normal
                   t0[0], t0[1],        # texcoord
                   1.0, 1.0, 1.0, 1.0 ) # color
                    
            index = dict.setdefault(vp, len(dict))
            
            indices.append(index)
    
    bpy.data.meshes.remove(triMesh)
    
    # write indices
    file.write(struct.pack('i', len(indices)))
    for i in indices:
        file.write(struct.pack('H', i))

    # vertices dictionary -> array    
    vertices = [None] * len(dict)
    for key in dict.keys():
        vertices[dict.get(key)] = key
        
    # write vertices
    file.write(struct.pack('i', len(vertices)))
    for vp in vertices:
        for v in vp:
            file.write(struct.pack('f', v))

def export(path):
    print('convert scene')
    
    objects = []
    
    for obj in bpy.data.scenes[0].objects:
        type = -1
        if obj.type == 'MESH' and len(obj.material_slots) > 0 and len(obj.material_slots[0].material.texture_slots) > 0:
            type = 0
        if obj.type == 'LAMP':
            type = 1
        if obj.name.startswith('#start'):
            type = 2
        if type == -1:
            continue
        objects.append( ( type, obj ) )
    
    file = open(path, 'wb')
    
    # write objects count
    file.write(struct.pack('i', len(objects)))
    
    # write objects
    for item in objects:
        type = item[0]
        obj  = item[1]
        print(" ", obj.name)
        file.write(struct.pack('i', type))
        writeTransform(file, obj)
        if type == 0:
            writeMaterial(file, obj.material_slots[0].material)
            writeMesh(file, obj.data);

    file.close()
    
    print('done')
    
export('C:/Projects/EuclidSucks/bin/scenes/scene1.scn')
