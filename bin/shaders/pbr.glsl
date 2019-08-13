varying vec4 vCoord;
varying vec4 vNormal;
varying vec4 vColor;

#ifdef VS

attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec4 aColor;

uniform mat4 uViewProj;
uniform mat4 uModel;

void main() {
    vCoord      = uModel * vec4(aPosition, 1.0);
    vNormal     = uModel * vec4(aNormal.xyz, 0.0);
    vColor      = aColor;
    gl_Position = uViewProj * vCoord;
}

#else

uniform samplerCube sEnvmap;
uniform sampler2D   sLUT;

uniform vec4 uViewPos;

uniform vec4 uParams[2];

#define COLOR     uParams[0]
#define METALLIC  uParams[1].x
#define ROUGHNESS uParams[1].y
#define MAX_LOD   5

// OGL has flipped Y for 2D textures
vec4 fetch2D(sampler2D smp, vec2 tc) {
    return texture2D(sLUT, vec2(tc.x, 1.0 - tc.y));
}

// OGL has flipped -X and +X direction for cubemap textures
vec4 fetchCube(samplerCube smp, vec3 tc, float level) {
    return textureLod(smp, vec3(-tc.x, tc.y, tc.z), level);
}

void main() {
    vec3 n = normalize(vNormal.xyz);
    vec3 v = normalize(uViewPos.xyz - vCoord.xyz);
    vec3 r = reflect(-v, n);

    vec4 diffuse = vColor * COLOR;

    vec3 F0 = mix(vec3(0.04), diffuse.xyz, METALLIC);
    vec3 F = F0 + (max(vec3(1.0 - ROUGHNESS), F0) - F0) * pow(1.0 - dot(n, v), 5.0);

    diffuse.xyz  *= fetchCube(sEnvmap, n, MAX_LOD).xyz;
    vec3 specular = fetchCube(sEnvmap, r, ROUGHNESS * MAX_LOD).xyz;
    vec2 lut      = fetch2D(sLUT, vec2(max(0.0, dot(n, v)), ROUGHNESS)).xy;

    specular = specular * (F * lut.x + lut.y);

    vec3 kd = (vec3(1.0) - F) * (1.0 - METALLIC);

    vec3 color = kd * diffuse.xyz + specular;

    fragColor = vec4(color, diffuse.w);
}

#endif
