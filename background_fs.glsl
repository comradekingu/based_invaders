varying vec2 pos;
uniform vec2 dimensions;

void main()
{
    vec2 norm = pos / dimensions;
    float curvature = norm.y + .025f * sin(3.14f * norm.x);

    float sky = smoothstep(.0f, .75f, curvature) * .8f;
    vec3 skyblue = vec3(0, sky / 3.f, sky);
    
    float earth = smoothstep(.7f, .9f, curvature) * .4f;
    vec3 earthgreen = vec3(0, earth, sky / 6.f);

    vec3 color = (skyblue + earthgreen) / 2.f;

    gl_FragColor = vec4(color, 0);
}
