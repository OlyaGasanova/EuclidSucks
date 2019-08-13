texconv -vflip -f BC7_UNORM ../res/textures/bricks_d.tga
texconv -vflip -f BC7_UNORM ../res/textures/bricks_n.tga
texconv -vflip -f BC1_UNORM ../res/textures/grid.tga
texconv -vflip -f BC7_UNORM ../res/textures/paving_d.tga
texconv -vflip -f BC7_UNORM ../res/textures/paving_n.tga

cd ../res/textures
move *.dds ../../bin/textures
cd ../../bin