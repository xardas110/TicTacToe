#version 460 core
out vec4 FragColor;
  
uniform vec3 color;
uniform vec3 camPos;
in vec3 fragWS;
in vec3 norm;
void main()
{
	vec3 sunDir = vec3(0.15, 0.10, 1.0);
	sunDir = normalize(sunDir);
	vec3 sunColor = vec3(1.0, 1.0, 1.0);
	
	vec3 fragToCam = camPos - fragWS;
	fragToCam = normalize(fragToCam);
	float diffuseStrength = max(dot(sunDir, norm), 0);
//pow(NDotH, material.Shininess) * (material.Shininess + 2.f) / 8.f
	float specularStrength = pow(max(dot(fragToCam, reflect(-sunDir, norm)), 0), 8) * (8.0 + 2.0)/8.0; 
	vec3 diffuse = sunColor * diffuseStrength * 0.8;
	vec3 spec = sunColor * specularStrength;
	vec3 lightColor = (diffuse + spec);
	vec3 finalColor = color * lightColor;
	FragColor = vec4(finalColor.x, finalColor.y, finalColor.z, 1.0);
}
