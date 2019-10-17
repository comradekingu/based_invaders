varying vec2 pos;
uniform vec2 explosion_center;
uniform float maxradius;
uniform float progress;

void main()
{
    float radius = clamp(progress * 40.f, .0f, 1.f) * maxradius / 3.f
                                  + progress * maxradius * 2.f / 3.f;
    float dist = distance(pos, explosion_center) / radius;
    float intensity = (1.f - dist) * pow((1.f - progress), 5.f);
    intensity = clamp(intensity, .0f, .9f);

    float r = intensity;
    float g = (1.f - smoothstep(.0f, .5f, dist)) * intensity * .8f;
    float b = (1.f - smoothstep(.0f, .2f, dist)) * intensity * .6f;

    gl_FragColor = vec4(r, g, b, 0);
}
