#version 130

in vec3 position;
in vec2 texCoord;

out vec2 fragTexCoord;
out float posY;
out float depth;

uniform mat4 mvp;
uniform vec3 cameraPos;

void main(){
	gl_Position = mvp * vec4(position, 1.0);
	depth = distance(cameraPos, position);
	fragTexCoord = texCoord;
	posY = position.y;

}
