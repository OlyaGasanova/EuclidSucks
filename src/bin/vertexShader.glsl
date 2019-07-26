#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) out vec4 vert_color;
uniform mat4 MVP;
  
  void main(){
    // Выходная позиция нашей вершины: MVP * position
    gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
	vert_color = gl_Position;
  }