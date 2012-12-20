uniform sampler2D colormap;
//uniform sampler2D bumpmap;
//varying vec2  TexCoord;
//uniform int maxHeight;
varying vec4 diffuse,ambient;
varying vec3 normal,halfVector;
void main(void) {
	
	/* first transform the normal into eye space and
	 normalize the result */
    normal = normalize(gl_NormalMatrix * gl_Normal);
	
    /* pass the halfVector to the fragment shader */
    halfVector = gl_LightSource[0].halfVector.xyz;
	
    /* Compute the diffuse, ambient and globalAmbient terms */
    diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
    ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
    ambient += gl_LightModel.ambient * gl_FrontMaterial.ambient;
    gl_Position = ftransform();
	
//	gl_FrontColor =  gl_Color * NdotL * diffuse;
	
//	TexCoord = gl_MultiTexCoord0.st;
//
//	vec4 bumpColor = texture2D(bumpmap, TexCoord);
//	float df = 0.30*bumpColor.x + 0.59*bumpColor.y + 0.11*bumpColor.z;
//	vec4 newVertexPos = vec4(gl_Normal * df * float(maxHeight), 0.0) + gl_Vertex;
//
//	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	//--------------------------------------------
//	gl_PointSize = 10.0;
	
//	gl_Position = ftransform();
	gl_FrontColor = gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}