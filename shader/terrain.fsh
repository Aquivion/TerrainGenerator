#version 130

in vec2 fragTexCoord;
in float posY;

in float depth;

out vec4 color;

uniform sampler2D normalTex;
uniform sampler2D seamlessTex;
uniform sampler2D stoneTex;
uniform sampler2D stoneDetailTex;

uniform vec3 lightDir;
uniform float max = 0.0;
uniform float brightness = 1.0;
uniform bool seamlessTexEnabled = true;

void main(){
	vec3 stone = texture(stoneTex, fragTexCoord).xyz;
	vec3 stoneDetail = texture(stoneDetailTex, fragTexCoord*64.0).xyz;
	float grey = dot(texture(normalTex, fragTexCoord).xyz, lightDir);
	if(seamlessTexEnabled){
		if(depth < 50.0){
			float greyDetail = 1.0-dot(texture(seamlessTex, fragTexCoord*8.0).xyz, lightDir);
			grey = grey * (depth/50.0) + greyDetail * (1.0 - (depth / 50.0));
		}
	}
	color = vec4((grey * stone * stoneDetail * (posY/max)) * brightness, 1.0);
	//color = vec4(0.0, 1.0, 0.0, 1.0);
}
