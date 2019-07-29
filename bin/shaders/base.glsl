#define MAX_LIGHTS 3

varying vec4 vCoord;
varying vec2 vTexCoord;
varying vec4 vColor;
varying vec4 vNormal;
varying vec4 vTangent;

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
    vTangent = uModel * vec4(aTangent.xyz, 0.0);
    vTangent.w = aTangent.w;

    vTexCoord   = aTexCoord;
    vColor      = aColor;
    gl_Position = uViewProj * vCoord;
}

#else

uniform sampler2D sDiffuse; // rgb - albedo, a - ambient occlusion
uniform sampler2D sNormal;  // rgb - normal, a - roughness

uniform vec4 uViewPos;
uniform vec4 uLightPos[MAX_LIGHTS];   // xyz - pos, w - 1/radius
uniform vec4 uLightColor[MAX_LIGHTS];

void main() {
    vec4 d = texture2D(sDiffuse, vTexCoord);
    vec4 n = texture2D(sNormal,  vTexCoord);

    vec3 N = normalize(vNormal.xyz);
    vec3 T = normalize(vTangent.xyz);
    vec3 B = cross(N, T) * vTangent.w;
    n.xyz = normalize(mat3(T, B, N) * (n.xyz * 2.0 - 1.0));

    vec3 light    = vec3(0.0);
    vec3 specular = vec3(0.0);
    
    vec3 vv = normalize(uViewPos.xyz - vCoord.xyz);
    vec3 rv = reflect(-vv, n.xyz);

    for (int i = 0; i < MAX_LIGHTS; i++) {
        vec3 lv = (uLightPos[i].xyz - vCoord.xyz) * uLightPos[i].w;

        vec3 att = max(0.0, 1.0 - dot(lv, lv)) * uLightColor[i].xyz;

        lv = normalize(lv);
        
        float lum = max(0.0, dot(n.xyz, lv)) * d.w;
        
        light += lum * att;
        specular += pow(max(0.0, dot(lv, rv)) * (1.0 - n.w), 8.0) * att;
    }

    light = light * 0.8 + 0.2; // apply ambient

    fragColor = vec4(n.xyz, 1.0);// vec4(d.xyz * light * vColor.xyz + specular, vColor.w);
}

#endif