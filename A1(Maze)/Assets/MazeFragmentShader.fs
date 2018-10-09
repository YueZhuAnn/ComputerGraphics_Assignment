#version 330

uniform sampler2D textureImg;
uniform vec3 colour;

in vec2 texturePos;
in vec3 normal_val;
in float distance;

out vec4 fragColor;

void main() {
	vec3 lightDir = normalize(vec3(-0.5f, -1.0f, 0.5f));
	vec4 lightCol = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	float lightPow = 10.0f;
	float cosine = 1.0f;

	fragColor = texture(textureImg, texturePos)*vec4(colour, 1);

	cosine = dot(lightDir, normal_val);
	if (cosine <= 0)
	{
		cosine = 0;
	}
	// AmbientColor
	vec4 ambitnentColor = vec4(0.5,0.5,0.5,1) * fragColor;

	// DiffuseColor
	vec4 DiffuseColor = fragColor * lightCol * lightPow * cosine /distance;
	
	fragColor = ambitnentColor + DiffuseColor;
}
