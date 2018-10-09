#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 L;
uniform mat4 R;
uniform float scale;

in vec3 position;
in vec2 tex;
in vec3 normal;

out vec2 texturePos;
out vec3 normal_val;
out vec3 light_val;
out float cosine;
out float distance;

void main() {

	vec3 light = vec3(16.0f, 10.0f, 16.0f);
	gl_Position = P * V * M * vec4(position, 1.0);

	vec4 ptr_Position = M * vec4(position, 1.0);
	vec4 light_Position = L * vec4(light, 1.0);
	vec4 normal_Direction = R * vec4(normal, 1.0);
	
	// Convert to vec3
	vec3 ptr_Position_v3 = vec3(ptr_Position[0], ptr_Position[1], ptr_Position[2]);
	vec3 light_Position_v3 = vec3(light_Position[0], light_Position[1], light_Position[2]);
	vec3 normal_Direction_v3 = vec3(normal_Direction[0], normal_Direction[1], normal_Direction[2]);

	distance = length(light_Position_v3 - ptr_Position_v3) / scale;

	texturePos = tex;
	normal_val = normalize(normal_Direction_v3);
}
