#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec4 aInstanceCol0;
layout (location = 3) in vec4 aInstanceCol1;
layout (location = 4) in vec4 aInstanceCol2;
layout (location = 5) in vec4 aInstanceCol3;
layout (location = 6) in vec2 aTexCoord;

out vec3 color;
out vec2 TexCoord;
uniform mat4 view;
uniform mat4 projection;
void main()
{
	mat4 model = mat4(aInstanceCol0, aInstanceCol1, aInstanceCol2,
	                                                        aInstanceCol3);
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	color = aColor;
	TexCoord = aTexCoord;
}
