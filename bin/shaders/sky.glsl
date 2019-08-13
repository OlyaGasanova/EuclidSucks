varying vec4 vCoord;

#ifdef VS

attribute vec3 aPosition;

uniform mat4 uViewProj;
uniform vec4 uViewPos;

void main() {
    vCoord = vec4(aPosition, 1.0);
    gl_Position = uViewProj * vec4(aPosition + uViewPos.xyz, 1.0);
}

#else

uniform samplerCube sEnvmap;

// OGL has flipped -X and +X direction for cubemap textures
vec4 fetchCube(samplerCube smp, vec3 tc, float level) {
    return textureLod(smp, vec3(-tc.x, tc.y, tc.z), level);
}

void main() {
    fragColor = fetchCube(sEnvmap, vCoord.xyz, 0.0);
}

#endif