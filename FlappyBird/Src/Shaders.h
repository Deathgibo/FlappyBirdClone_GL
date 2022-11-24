#pragma once

//GLSL 3.3+
std::string vshaderSource =
//"#version 460									\n"
"layout(location = 0) in vec3 position;			\n"
"layout(location = 1) in vec2 uv;				\n"

"out vec2 fraguv;			                    \n"

"uniform mat4 mvp;								\n"

"void main()									\n"
"{												\n"
"  gl_Position = mvp * vec4(position, 1.0);		\n"
"  fraguv = uv;									\n"
"}												\n";

std::string fshaderSource =
//"#version 460									\n"
"layout(location = 0) out vec4 out_color;		\n"

"in vec2 fraguv;			                    \n"

"uniform sampler2D color_texture;				\n"
"uniform vec4 color_val;						\n"
"uniform int use_texture;						\n"
"uniform int use_alpha;						    \n"

"void main()									\n"
"{												\n"
"  vec4 color;									\n"
"  if (use_texture == 1)						\n"
"    color = texture(color_texture, fraguv);	\n"
"  else											\n"
"    color = color_val;							\n"
"  if (use_alpha == 1 && color.a > .1) 		\n"
"    color.a = color_val.a;						\n"
"  out_color = color;							\n"
"}												\n";


//GLSL 1.3, 1.4, 1.5
std::string vshaderSource_old =
//"#version 460									\n"
"attribute vec3 position;						\n"
"attribute vec2 uv;								\n"

"varying vec2 fraguv;			                \n"

"uniform mat4 mvp;								\n"

"void main()									\n"
"{												\n"
"  gl_Position = mvp * vec4(position, 1.0);		\n"
"  fraguv = uv;									\n"
"}												\n";

std::string fshaderSource_old =
//"#version 460									\n"
"varying vec2 fraguv;			                \n"

"uniform sampler2D color_texture;				\n"
"uniform vec4 color_val;						\n"
"uniform int use_texture;						\n"
"uniform int use_alpha;						    \n"

"void main()									\n"
"{												\n"
"  vec4 color;									\n"
"  if (use_texture == 1)						\n"
"    color = texture(color_texture, fraguv);	\n"
"  else											\n"
"    color = color_val;							\n"

"  if (use_alpha == 1 && color.a > .1) 		    \n"
"    color.a = color_val.a;						\n"
"  gl_FragColor = color;						\n"
"}												\n";