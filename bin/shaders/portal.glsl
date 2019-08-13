#ifdef VS

attribute vec3 aPosition;

uniform mat4 uViewProj;
uniform mat4 uModel;

void main() {
    gl_Position = uViewProj * (uModel * vec4(aPosition, 1.0));
}

#else

void main() {
    fragColor = vec4(1.0);
}

#endif
