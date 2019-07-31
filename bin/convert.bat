texconv -f BC7_UNORM ../res/textures/bricks_d.tga
texconv -f BC7_UNORM ../res/textures/bricks_n.tga
texconv -f BC1_UNORM ../res/textures/grid.tga
texconv -f BC7_UNORM ../res/textures/paving_d.tga
texconv -f BC7_UNORM ../res/textures/paving_n.tga

cd ../res/textures/
move *.dds ../../bin/textures
cd ../../bin/textures