uniform sampler2D Texture0;
uniform vec3 LightDir;

uniform float silhouetteThreshold;

varying vec3 Vertex;
varying vec3 Normal;

vec4 CelShading ( vec4 color )
{
    float Intensity = dot( LightDir , normalize(Normal) );
    float factor = 1.0;
    if ( Intensity < 0.5 ) factor = 0.5;
    color *= vec4 ( factor, factor, factor, 1.0 );

    return color;
}

void main (void)
{
    vec4 silhouetteColor = vec4(0.0, 0.0, 0.0, 1.0);

    vec3 eyePos = normalize(-Vertex);
    vec3 EyeVert = normalize(eyePos - Vertex);

    float sil = max(dot(Normal,EyeVert), 0.0);
    if( sil < silhouetteThreshold )
        gl_FragColor = silhouetteColor;
    else {
        vec4 color = texture2D( Texture0 , vec2( gl_TexCoord[0] ) );

        color = CelShading ( color );

        gl_FragColor = color;
    }
}
