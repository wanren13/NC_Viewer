#version 400 core

struct Light
{
    bool on;
    vec4 position;
    vec3 spotDirection;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float spotCutoff;
    float spotExponent;
    vec3 attenuation;  //  [0] = constant, [1] = linear, [2] = quadratic
};

struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 emission;
    float shininess;
};

in vec4 position;
in vec4 color;
in vec3 normal;
in vec2 tex_coord;

vec4 texcolor;

uniform Light Lt[10];
uniform Material Mat;
uniform vec3 eye;
uniform vec4 GlobalAmbient;
uniform int numLights;
uniform bool useTexture;
uniform mat2 texTrans;

uniform sampler2D tex1;


out vec4 fColor;

void main()
{
//  get the texture fragment color.
    texcolor = texture(tex1, texTrans*tex_coord);

//  get the material fragment color.
    float deg = 0.017453292519943296;
    bool useLights = false;
    vec4 acumcol = vec4(0.0);

    vec4 globalAmbientPortion = Mat.ambient*GlobalAmbient;

    for (int i = 0; i < numLights; i++)
    {
        if (Lt[i].on)
        {
            useLights = true;
            vec3 l = vec3(Lt[i].position)-vec3(position);
            float lightDistance =length(l);
            l = normalize(l);
            vec3 r = normalize(2.0*dot(l,normal)*normal - l);
            vec3 v = normalize(eye-vec3(position));

            float dfang = max(0.0, dot(l, normal));
            float specang = max(0.0, dot(r, v));

            vec4 ambientPortion = Mat.ambient*Lt[i].ambient;

            float attenuation = 1.0 / (Lt[i].attenuation[0] +
                                       Lt[i].attenuation[1] * lightDistance +
                                       Lt[i].attenuation[2] * lightDistance * lightDistance);

            float spotCos = dot(l, -normalize(Lt[i].spotDirection));
            float SpotCosCutoff = cos(Lt[i].spotCutoff*deg);

            float spotFactor = 1.0;
            if (spotCos < SpotCosCutoff)
                spotFactor = 0.0;
            else
                spotFactor *= pow((spotCos+1)/2, Lt[i].spotExponent);

            vec4 diffusePortion;
            if (dfang <= 0.000001)
                diffusePortion = vec4(0.0);
            else
                diffusePortion = Mat.diffuse*Lt[i].diffuse*dfang*attenuation*spotFactor;

            vec4 specularPortion;
            if (specang <= 0.000001)
                specularPortion = vec4(0.0);
            else if (dfang <= 0.000001)
                specularPortion = vec4(0.0);
            else
                specularPortion = Mat.specular*Lt[i].specular*pow(specang, Mat.shininess)*attenuation*spotFactor;

            vec4 c = ambientPortion + diffusePortion + specularPortion + Mat.emission;

            acumcol += min(c, vec4(1.0));
        }
    }

    acumcol = min(acumcol + globalAmbientPortion, vec4(1.0));

    if (useLights)
        fColor = acumcol;
    else if (useTexture)
        fColor = texcolor;
    else
        fColor = color;

    if (useTexture)
    {
        //  Mix the texture and material fragment colors.
        fColor = 0.5*texcolor + 0.5*fColor;     // same as, fColor = mix(texcolor, fColor, 0.5);
        fColor = min(fColor, vec4(1.0));
    }
}


