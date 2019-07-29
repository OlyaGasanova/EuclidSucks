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
    # write shader file name
    writeString(file, "base.glsl")
    # write texture file names
    for i in range(2): # diffuse, normal, specular
        texName = ''
        slot = material.texture_slots[i]
        if slot != None:
            texName = bpy.path.basename(slot.texture.image.filepath)
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
    
    uvs = triMesh.uv_layers.active.data
    
    # get indices (array) and vertices (dictionary)
    for f in triMesh.polygons:
        for i in f.loop_indices:
            vIndex = triMesh.loops[i].vertex_index
            vertex = triMesh.vertices[vIndex]
            vc = MatGL * vertex.co
            vn = MatGL * triMesh.loops[i].normal
            vt = MatGL * triMesh.loops[i].tangent
            bs = triMesh.loops[i].bitangent_sign
            uv = uvs[i].uv
            
            vp = ( vc.x, vc.y, vc.z,    # position
                   vn.x, vn.y, vn.z,    # normal
                   vt.x, vt.y, vt.z,    # tangent
                   bs,                  # bitangent sign (b = cross(n, t) * bs)
                   uv.x, uv.y,          # texcoord
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
