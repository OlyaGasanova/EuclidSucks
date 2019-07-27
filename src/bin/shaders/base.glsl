varying vec2 vTexCoord;
varying vec4 vColor;

#ifdef VS

attribute vec3 aPosition;
attribute vec2 aTexCoord;
attribute vec4 aColor;

uniform mat4 uViewProj;

void main() {
    gl_Position = uViewProj * vec4(aPosition, 1.0);
    
    vTexCoord = aTexCoord;
    vColor    = aColor;
}

#else

uniform sampler2D sDiffuse;

void main() {
    fragColor = texture2D(sDiffuse, vTexCoord) * vColor;
}

#endif