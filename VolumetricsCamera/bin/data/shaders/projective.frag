uniform sampler2DRect tex0;
uniform sampler2DRect tex1;
varying vec4 ProjTexCoord;

void main()
{
    vec4 color;
    vec4 colProj;
	
    color = texture2D(tex0, gl_TexCoord[0].xy);
    colProj = texture2DRectProj(tex1, ProjTexCoord);
    color = color * colProj;
	
    gl_FragColor = color;
}
