uniform sampler2D al_tex;
uniform float frame;
uniform float created_at;
varying vec2 pos;
varying vec2 tex_uv;

float time() {
    return (frame - created_at) * 3.14f / 5.f;
}

float pulse() {
    return clamp(abs(sin(time())), .5f, 1.f);
}

void main()
{
    gl_FragColor = vec4(1.f, 1.0f, .0f, .0f) * pulse();
}
