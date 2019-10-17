attribute vec4 al_pos;
attribute vec2 al_texcoord;
uniform mat4 al_projview_matrix;

varying vec2 pos;
varying vec2 tex_uv;

void main()
{
    pos = al_pos.xy;
    tex_uv = al_texcoord;
    gl_Position = al_projview_matrix * al_pos;
}
