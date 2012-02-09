uniform sampler3D SamplerDataVolume;
uniform sampler1D SamplerTransferFunction;
uniform sampler2D BackTexture;
uniform vec3 scaleTex;
uniform vec3 lightPos;
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec4 camera;
uniform vec3 background; // background Color
uniform vec3 kakskd; // Ka, Ks and Kd values
uniform float stepsize;
uniform vec3 volExtentMin;
uniform vec3 volExtentMax;
uniform vec3 viewVector;
uniform vec3 cameraPos;

void main()
{
	vec4 value;
	vec3 direction;
	if (camera.b < 1.0) // Parallel
	{
		float len = length(((gl_TexCoord[0].xyz * scaleTex) - cameraPos));
		direction = viewVector * len;
	}
	else // Perspective
		direction = ((gl_TexCoord[0].xyz * scaleTex) - cameraPos);

    vec3 position = gl_TexCoord[0].xyz;
    position = position * scaleTex;

	// Initialize destination color
    vec4 dst = vec4(0.0, 0.0, 0.0, 0.0);
    
    for (int i = 0; i < 2000; i++) 
    {
		value = texture3D(SamplerDataVolume, position / scaleTex);
		
		if ((length(value.rgb) > camera.r) && (value.a > 0.1))
		{
			vec4 src = texture1D(SamplerTransferFunction, value.a);
		
			// Shading ---------------------------------------------------------------------
			vec3 ka = kakskd.xxx;
			vec3 ks = kakskd.yyy;
			vec3 kd = kakskd.zzz;
			float n = 50.0;
			vec3 V = viewVector;
			vec3 L = normalize(lightPos - position);
			vec3 N = value.rgb;
				
			vec3 H = normalize(L + V);
			vec3 ambientF = ka * ambient;
			
			float diffuseLight = max(dot(L, N), 0.0);
//			vec3 diffuseF = kd * diffuse * diffuseLight;
			vec3 diffuseF = kd * src.rgb * diffuseLight;
			
			float specularLight = pow(max(dot(H, N), 0.0), n);
			if (diffuseLight <= 0.0)
				specularLight = 0.0;
//			vec3 specular = (ks * diffuse * specularLight);
			vec3 specular = (ks * src.rgb * specularLight);
			
			vec4 final = src.rgbr * (ambientF + diffuseF + specular).rgbr;
			final.a = src.a;
			// ----------------------------------------------------------------------------

//			dst = (1.0 - dst.a) * src + dst;       
//			dst = (1.0 - dst.a) * specular + dst;
			dst = (1.0 - dst.a) * final + dst;
			// Position is outside the volume?	        
			//vec3 tmp1 = sign(position - volExtentMin.xyzx).xyz;
			vec3 tmp1 = sign(position);
			vec3 tmp2 = sign(volExtentMax - position);
			float inside = dot(tmp1, tmp2);
			
			if (position.z < 0.0)
				break;
			if (inside < 3.0) 
				break;
			//if (dst.a > camera.g)
			//	break;
		}
		position = position + (direction * stepsize);
    }
    //gl_FragColor = position.rgbr / scaleTex.rgbr;
	dst = (1.0 - dst.a) * vec4(0.7, 0.7, 0.5, 1.0) + dst;           
    gl_FragColor = dst;
}