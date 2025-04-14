uniform sampler2D texture;
uniform sampler2D mask;

void main()
{
    vec2 texCoord = gl_TexCoord[0].xy;

    vec4 color = texture2D(texture, texCoord);
    vec4 maskColor = texture2D(mask, texCoord);
    
    if (maskColor.r < 0.5)
        discard;

    gl_FragColor = color;
}
