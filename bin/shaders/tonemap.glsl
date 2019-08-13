varying vec2 vTexCoord;

#ifdef VS

attribute vec3 aPosition;

void main() {
    gl_Position = vec4(aPosition.xyz, 1.0);
    vTexCoord = aPosition.xy * 0.5 + 0.5;
}

#else

uniform sampler2D sDiffuse;

// http://filmicworlds.com/blog/filmic-tonemapping-operators/

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

vec3 Uncharted2Tonemap(vec3 x)
{
   return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

vec3 Uncharted2(vec3 color) {
   float ExposureBias = 2.0;
   vec3 curr = Uncharted2Tonemap(ExposureBias * color);

   vec3 whiteScale = 1.0 / Uncharted2Tonemap(vec3(W));
   return curr * whiteScale;
}

vec3 Reinhard(vec3 color) {
    return color / (color + 1.0);
}

void main() {
    vec3 color = texture2D(sDiffuse, vTexCoord).xyz;

    //color = Reinhard(color);
    color = Uncharted2(color);
    color = pow(color, vec3(1.0/2.2)); // linear to gamma

    fragColor = vec4(color, 1.0);
}

#endif
