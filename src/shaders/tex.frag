uniform sampler3D SamplerDataVolume;
uniform sampler1D SamplerTransferFunction;
uniform int lights;
uniform vec3 volExtentMax;
uniform vec3 V;

//uniform sampler2D BackTexture;
//uniform vec3 scaleTex;
//uniform vec3 lightPos;
//uniform vec3 ambient;
//uniform vec3 diffuse;
//uniform vec4 camera;
//uniform vec3 background; // background Color
//uniform vec3 kakskd; // Ka, Ks and Kd values
//uniform float stepsize;
//uniform vec3 volExtentMin;

//uniform vec3 viewVector;
//uniform vec3 cameraPos;

void main()
{
	// Get value from 3D Texture
	vec4 value = texture3D(SamplerDataVolume, gl_TexCoord[0].xyz);
	// DEBUG - gl_FragColor = value.wwww;
	
	vec4 optical = texture1D(SamplerTransferFunction, value.w);
	// DEBUG - gl_FragColor = optical;
	
	vec3 normal = (optical.xyz * 2.0) - 1.0;
	// DEBUG - gl_FragColor = vec4(value.xyz, optical.w);
	
	// DEBUG - gl_FragColor = vec4(V, value.w);
	// DEBUG - return;
	
	// SHADING =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	vec3 Ka = optical.xyz;
	vec3 Kd = optical.xyz;
	vec3 Ks = optical.xyz;
	vec3 ambientColor = gl_LightSource[0].ambient.xyz;
	vec3 finalColor = Ka * ambientColor;
	float n = 10.0;
	
	for (int i = 1; i < 4; i++)
	{
		if (i < lights)
		{			
			vec3 L = (gl_TexCoord[0].xyz * volExtentMax) - gl_LightSource[0].position.xyz;
			vec3 lightColor = gl_LightSource[i].ambient.xyz;
			vec3 H = normalize(L + V);
			vec3 N = normalize(value.xyz);

			float a = ( dot(L, N) > 0.0 ) ? dot(L, N) : 0.0;
			float b = ( dot(H, N) > 0.0 ) ? dot(H, N) : 0.0;
			finalColor += (Kd * lightColor * a) + (Ks * lightColor * pow(b, n));
		}
	}
	
	gl_FragColor = vec4(finalColor, optical.w);
	// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	
	// Cool effect in one of the axis
	//vec4 properties = texture1D(SamplerTransferFunction, gl_TexCoord[0].x);
	//gl_FragColor = properties;
	
	// Nice Colors
    //gl_FragColor = vec4(gl_TexCoord[0].xyz, 0.05);
}