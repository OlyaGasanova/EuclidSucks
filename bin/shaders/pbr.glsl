varying vec4 vCoord;
varying vec4 vNormal;
varying vec4 vColor;

#ifdef VS

attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec4 aTangent;
attribute vec2 aTexCoord;
attribute vec4 aColor;

uniform mat4 uViewProj;
uniform mat4 uModel;

void main() {
    vCoord = uModel * vec4(aPosition, 1.0);

    vNormal  = uModel * vec4(aNormal.xyz, 0.0);

    vColor      = aColor;
    gl_Position = uViewProj * vCoord;
}

#else

uniform samplerCube sEnvmap;

uniform vec4 uViewPos;

uniform vec4 uParams[2];

#define COLOR     uParams[0]
#define METALLIC  uParams[1].x
#define ROUGHNESS uParams[1].y

void main() {
    vec3 vv = normalize(uViewPos.xyz - vCoord.xyz);
    vec3 rv = reflect(-vv, normalize(vNormal.xyz));

    vec4 color = vColor * COLOR;
    vec4 env = textureLod(sEnvmap, normalize(rv), ROUGHNESS * 9.0);

    fragColor = mix(color, env, METALLIC);
    
}

#endif