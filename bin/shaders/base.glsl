varying vec2 vTexCoord;
varying vec4 vColor;

#ifdef VS

attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec4 aTangent;
attribute vec2 aTexCoord;
attribute vec4 aColor;

uniform mat4 uViewProj;
uniform mat4 uModel;
uniform vec4 uLightDir;
uniform vec4 uLightColor;

void main() {
    gl_Position = uViewProj * uModel * vec4(aPosition, 1.0);
    
    vec3 n = (uModel * vec4(aNormal, 0.0)).xyz;
    float lum = max(0.0, dot(normalize(n), uLightDir.xyz));
    vec3 light = (lum * uLightColor.xyz) * 0.8 + 0.2;
    
    vTexCoord = aTexCoord;
    vColor    = vec4(aColor.xyz * light, aColor.w);
}

#else

uniform sampler2D sDiffuse;

void main() {
    fragColor = texture2D(sDiffuse, vTexCoord) * vColor;
}

#endif