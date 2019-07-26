#version 330 core
out vec3 color;
layout(location = 1) in vec4 vercolor;
void main(){
    color = vercolor.xyz+vec3(0.8,0,0);//vec3(1,0,0);
}