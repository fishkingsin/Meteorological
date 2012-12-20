varying vec4 ProjTexCoord;
void main()
{
    mat4 textureMat;
    gl_Position = ftransform();
	
    gl_TexCoord[0] = gl_MultiTexCoord0;
    textureMat=mat4(0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.5,0.5,0.5,1.0);
	
    ProjTexCoord=(textureMat * gl_ModelViewProjectionMatrix)*gl_Vertex;
}