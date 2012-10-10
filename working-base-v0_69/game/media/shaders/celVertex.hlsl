varying vec3 Vertex;
varying vec3 Normal;

void main(void)
{
    // Front color
    gl_FrontColor = gl_Color;

    // Determine the normal of the vertex
    Vertex = vec3(gl_ModelViewMatrix * gl_Vertex);
    Normal = normalize(gl_NormalMatrix * gl_Normal);

    // Textures coordinates
    gl_TexCoord[0] = gl_MultiTexCoord0;

    // The position of the vertex
    gl_Position = ftransform();
}
