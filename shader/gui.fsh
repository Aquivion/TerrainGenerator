#version 130

in vec2 fm_TexCoord;
out vec4 fm_Color;

uniform vec4 color = vec4(1.0);
uniform bool useTexture = false;
uniform float brightness;

uniform sampler2D tex;

void main(){
	if(useTexture){
		fm_Color = texture(tex, fm_TexCoord)*brightness;
	}
	else{
		fm_Color = color*brightness;
	}
}