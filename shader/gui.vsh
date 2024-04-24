#version 130

in vec2 position;

const vec2 tex_Coord[4] = vec2[](
	vec2(0.0, 1.0),
	vec2(0.0, 0.0),
	vec2(1.0, 1.0),
	vec2(1.0, 0.0)
);

out vec2 fm_TexCoord;

void main(){
	gl_Position = vec4(position, 0.4, 1.0);
	fm_TexCoord = tex_Coord[gl_VertexID];
}
