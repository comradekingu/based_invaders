attribute vec4 al_pos;
uniform mat4 al_projview_matrix;
varying vec2 pos;

void main()
{
   pos = al_pos.xy;
   gl_Position = al_projview_matrix * al_pos;
}
