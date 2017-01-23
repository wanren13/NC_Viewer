#version 400 core

layout(location = 0) in vec4 vposition;
layout(location = 1) in vec4 vcolor;
layout(location = 2) in vec3 vnormal;
layout(location = 3) in vec2 in_tex_coord;

uniform mat4 ProjModelView;
uniform mat4 ModelView;
uniform mat3 NormalMatrix;

out vec4 color;
out vec4 position;
out vec3 normal;
out vec2 tex_coord;

///////////////////////////////////////
uniform float minY;
uniform float maxY;
uniform float transparency;
uniform float yScale;
uniform bool  enableGradientColor;

vec4 num2color();

void main()
{
    normal = normalize(NormalMatrix * vnormal);
    position = ModelView * vposition;

    if(enableGradientColor){
        color = num2color();
        //flat the surface
        gl_Position = ProjModelView * vec4(vposition.x, vposition.y * 0, vposition.zw);
    }
    else{
        color = vcolor;
        gl_Position = ProjModelView * vposition;
    }

    tex_coord = in_tex_coord;
}

// function for calculating gradient color
vec4 num2color()
{
    float y = vposition.y - minY;
    float range = maxY-minY;

    float fraction;
    if(range == 0)
        fraction = 0;
    else
        fraction = y/(maxY-minY);

    if(fraction < 0) fraction = 0;
    if(fraction > 1) fraction = 1;

    if(fraction <= 0.5)
        return vec4(0, 2*fraction, 1-2*fraction, transparency);
    else
        return vec4(2*fraction-1, 2-2*fraction, 0, transparency);
}
