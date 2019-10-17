uniform sampler2D al_tex;
uniform float frame;
uniform float created_at;
uniform float speed;

varying vec2 pos;
varying vec2 tex_uv;

vec4 diffuse(float d, vec2 offset) {
    return (texture2D(al_tex, tex_uv + offset) +
            texture2D(al_tex, tex_uv + offset + vec2(-d, -d)) +
            texture2D(al_tex, tex_uv + offset + vec2(-d, d)) +
            texture2D(al_tex, tex_uv + offset + vec2(d, d)) +
            texture2D(al_tex, tex_uv + offset + vec2(d, -d))) / 5.f;
}

vec4 multi_diffuse(float start, float step, int iters, vec2 offset) {
    vec4 ret = vec4(.0f, .0f, .0f, .0f);
    for (int i = 0; i < iters; i++) {
        ret += diffuse(start + float(i) * step, offset) / float(iters);
    }
    return ret;
}

vec4 sharp() {
    return texture2D(al_tex, tex_uv);
}

void main()
{
    vec2 glow_offset = vec2(.0f, .05f);
    vec4 dcol = multi_diffuse(.005f, .005f, 10, glow_offset);

    vec3 glow = dcol.a * vec3(1.f, 0.f, 0.f);
    vec3 col = mix(glow, sharp().rgb, dcol.a);

    gl_FragColor = vec4(col, dcol.a);
}
