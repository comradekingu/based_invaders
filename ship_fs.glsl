uniform sampler2D al_tex;
uniform float frame;
uniform float created_at;
uniform float projectile_cooldown;
varying vec2 pos;
varying vec2 tex_uv;

vec4 sharp() {
    return texture2D(al_tex, tex_uv);
}

float centerdist()
{
    return distance(
        vec2(.5f, .5f),
        tex_uv
    );
}

float cooldown() {
    return clamp(.0f, 1.f, projectile_cooldown);
}

void main()
{
    vec4 tc = sharp();
    vec4 shipcol = vec4(
        tc.r,
        0,
        0,
        tc.a
    );
    vec3 glow = (1.f - cooldown()) * vec3(.9f, .6f, .1f) * (0.5f - centerdist());
    float gradient = sin((tex_uv.y - .5f) * 3.14f) * 2.f * clamp(.0f, 1.f, tex_uv.y);
    vec3 col = mix(glow * gradient, shipcol.rgb, tc.a);

    gl_FragColor = vec4(col, tc.a);
}
