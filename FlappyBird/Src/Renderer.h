#pragma once
#include "Shaders.h"
#include "ShaderCompiler.h"
#include "ThirdParty/stb_image.h"

//textures
GLuint texture_logomenu,
texture_pause, texture_play, texture_menu, texture_background,
texture_ground, texture_pipes, texture_score, texture_start,
texture_gameover, texture_overback, texture_ok, texture_share, texture_sharedark, texture_recent, texture_top,
texture_dot, texture_default, texture_getready, texture_flappybird, texture_tap, texture_studios;
std::array<GLuint, 10> texture_numbers;
std::array<GLuint, 4> texture_medals;
std::array<GLuint, 3> texture_bird;

std::unordered_map<std::string, GLuint*> distribution_map; //maps file name to its internal type
class Renderer
{
private:
	struct GPUINFO
	{
		GLint maxFrameBufferWidth = -1;
		GLint maxFrameBufferHeight = -1;
		GLint maxViewPortDimensions[2] = { -1, -1 };
		GLint maxSamples = 0;
		GLint maxTextureSize = -1;

		GLint MinorVersion = 0;
		GLint MajorVersion = 0;

		const GLubyte* gl_Vendor = nullptr;
		const GLubyte* gl_Renderer = nullptr;
		const GLubyte* gl_Context_Version = nullptr;
		const GLubyte* gl_ShadingLanguageVersion = nullptr;

	};

	/* OpenGL features required for this renderer:
		None
	*/

	/* OpenGL formats required for this renderer:
		None
	*/

	/* OpenGL extensions required for this renderer:
		None
	*/

	/* OpenGL API versions supported in this renderer
		every API from 3.0 to 4.6 is supported. 
		if its under 3.2 then multisampling will be disabled
	*/

	/* GLSL versions supported
		shaders support every version of GLSL unless its like 1.0
	*/

	GPUINFO GPU_info;
	GLuint program_main;
	GLuint program_main_mvp, program_main_color, program_main_use_texture, program_main_use_alpha;
	GLuint rect_vbo, rect_ibo, rect_vao;
	GLuint fbo_main;
	GLuint color_attachment;
	GLsizei sample_count = 1;

	bool fbo_failed = false;
	bool fbo_failed_once = false;
	bool init = false;
public:

	void CleanUp()
	{
		if (!init)
			return;
		glDeleteBuffers(1, &rect_vbo);
		glDeleteBuffers(1, &rect_ibo);
		glDeleteVertexArrays(1, &rect_vao);

		glDeleteProgram(program_main);

		glDeleteTextures(1, &color_attachment);
		glDeleteFramebuffers(1, &fbo_main);

		checkError("Cleaning Up Renderer");
	}

	void Begin()
	{
		glUseProgram(program_main);
		if(!fbo_failed)
			glBindFramebuffer(GL_FRAMEBUFFER, fbo_main);
		else
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindVertexArray(rect_vao);
		checkError("End of Begin loop");
	}

	void End()
	{
		if (!fbo_failed)
		{
			glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_main);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

			glBlitFramebuffer(0, 0, Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, 0, 0, Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		}

		checkError("End of render loop");
	}

	void RenderRect(const GLuint& texture_id, const glm::mat4& mat, const glm::vec4& color = glm::vec4(0, 0, 0, 0), int use_texture = true, int use_alpha = false)
	{
		glUniformMatrix4fv(program_main_mvp, 1, GL_FALSE, glm::value_ptr(mat));
		glUniform1i(program_main_use_texture, use_texture);
		glUniform1i(program_main_use_alpha, use_alpha);
		if (use_texture)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture_id);
		}
		if (use_alpha || !use_texture)
		{
			glUniform4fv(program_main_color, 1, glm::value_ptr(color));
		}

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	void ExportTexture(const char* fileLocation, bool alpha, bool flip = true)
	{
		stbi_set_flip_vertically_on_load(flip);
		int width, height, bitdepth;
		unsigned char *texData = stbi_load(fileLocation, &width, &height, &bitdepth, (alpha) ? STBI_rgb_alpha : 0);
		if (!texData)
		{
			PRINT(std::cout << "failed to export texture: " << fileLocation << std::endl;)
			return;
		}
		bitdepth = (alpha) ? 4 : 3;

		//write to the asset file: fileLocation=texData|
		std::string file_name = fileLocation;
		file_name.resize(64);
		if (file_name.size() > 64)
			PRINT(std::cout << "file name larger than 64 bytes!\n");
		
		size_t filename_size = 64 * sizeof(std::string::value_type);
		int _width = width;
		int _height = height;
		int texture_size = width*height*bitdepth;
		
		size_t size = 0;
		size += filename_size;  //file name
		size += sizeof(int) * 3; //width and height and texture_size
		size += texture_size; //texture data

		unsigned char* binary_data = new unsigned char[size];
		size_t offset = 0;
	
		for (int i = 0; i < file_name.size(); i++)
		{
			file_name[i] = (file_name[i] + 100) % 256;
		}
		std::memcpy(binary_data + offset, file_name.data(), filename_size);
		offset += filename_size;

		std::memcpy(binary_data + offset, InttoU(_width).data(), sizeof(int));
		offset += sizeof(int);
		
		std::memcpy(binary_data + offset, InttoU(_height).data(), sizeof(int));
		offset += sizeof(int);

		std::memcpy(binary_data + offset, InttoU(texture_size).data(), sizeof(int));
		offset += sizeof(int);

		std::memcpy(binary_data + offset, texData, texture_size);

		WriteAssetFile(binary_data, size);
		PRINT(std::cout << "exported: " << fileLocation << std::endl;)
	}

	void CreateTexture(GLuint& textureid, const char* fileLocation, bool alpha, bool flip = true)
	{
		if (Config::GENERATE_DISTRIBUTION_DATA)
		{
			ExportTexture(fileLocation, alpha, flip);
			return;
		}

		unsigned char* texData = nullptr;
		int width, height, bitdepth;
		if (Config::LOAD_FROM_DISTRIBUTION)
		{
			texData = ReadTextureFromAssetFile(fileLocation, width, height);
		}
		else
		{
			stbi_set_flip_vertically_on_load(flip);
			texData = stbi_load(fileLocation, &width, &height, &bitdepth, (alpha) ? STBI_rgb_alpha : 0);
		}
		if (!texData)
		{
			PRINT(std::cout << "failed to load texture: " << fileLocation << std::endl;)
			WriteErrorFile("failed to load texture: " + std::string(fileLocation));
			textureid = texture_default;
			return;
		}
		checkError("clear texture load");

		glGenTextures(1, &textureid);
		glBindTexture(GL_TEXTURE_2D, textureid);
		//format and type have nothing to do with the internal image, its just the properties of the pixel data you are binding to it!!
		glTexImage2D(GL_TEXTURE_2D, 0, (alpha) ? GL_RGBA : GL_RGB, width, height, 0, (alpha) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, texData);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);


		GLint filter = (Config::TEXTUREQUALITY == TEXTURE_QUALITY::HIGH) ? GL_LINEAR : GL_NEAREST;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

		if (!Config::LOAD_FROM_DISTRIBUTION)
			stbi_image_free(texData);
		else
			delete [] texData;
		if (checkError("created texture"))
		{
			PRINT(std::cout << "loaded file but failed to create texture: " << fileLocation << std::endl;)
			WriteErrorFile("loaded file but failed to create texture: " + std::string(fileLocation));
			textureid = texture_default;
			return;
		}
	}

	bool Initialize()
	{
		//initialize GLEW
		if (glewInit() != GLEW_OK)
		{
			PRINT(std::cout << "GLEW could not initialize!";)
			WriteErrorFile("GLEW failed to initialize. (This is needed for OpenGL so game cannot be played)");
			return false;
		}

		//Print Extensions
		if (false)
		{
			GLint extension_count = 0;
			glGetIntegerv(GL_NUM_EXTENSIONS, &extension_count);
			for (int i = 0; i < extension_count; i++)
				std::cout << glGetStringi(GL_EXTENSIONS, i) << std::endl;
		}

		//get gpu information
		GPU_info.gl_Vendor = glGetString(GL_VENDOR);
		GPU_info.gl_Renderer = glGetString(GL_RENDERER);
		GPU_info.gl_Context_Version = glGetString(GL_VERSION);
		GPU_info.gl_ShadingLanguageVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

		glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &GPU_info.maxFrameBufferWidth);
		glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &GPU_info.maxFrameBufferHeight);
		glGetIntegerv(GL_MAX_VIEWPORT_DIMS, GPU_info.maxViewPortDimensions);
		glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &GPU_info.maxSamples);
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &GPU_info.maxTextureSize);
		glGetIntegerv(GL_MAJOR_VERSION, &GPU_info.MajorVersion);
		glGetIntegerv(GL_MINOR_VERSION, &GPU_info.MinorVersion);

		PRINT(std::cout << glGetString(GL_VENDOR) << " " << glGetString(GL_RENDERER) << " " << glGetString(GL_VERSION) << " Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;)

		sample_count = std::min(Config::MULTISAMPLE_COUNT, GPU_info.maxSamples);

		//Multisampling only supported on openGL 3.2 and higher
		if (GPU_info.MajorVersion <= 3 && GPU_info.MinorVersion < 2)
			sample_count = 1;

		PRINT(std::cout << "MAX COLOR SAMPLES SUPPORTED: " << GPU_info.maxSamples << std::endl;)
		PRINT(std::cout << "Color samples being used: " << sample_count << std::endl;)

		if (GPU_info.MajorVersion < 3)
			WriteErrorFile("Need to have OpenGL 3.0 or greater supported. Try updating Graphics Driver.");
		
		//set opengl state
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_SCISSOR_TEST);
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glClearColor(0.7, 0.7, 0.7, 1.0);

		if (Config::MULTISAMPLE_COUNT > 1)
			glEnable(GL_MULTISAMPLE);
		else
			glDisable(GL_MULTISAMPLE);

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		checkError("end of initialize");


		//set GLSL version based off current openGL version. anything over 3.2 maps 1 to 1. else 3.0->1.3, 3.1->1.4, 3.2->1.5
		std::string glsl_version = "#version 460\n";
		if (GPU_info.MajorVersion >= 4 || (GPU_info.MajorVersion >= 3 && GPU_info.MinorVersion >= 3))
		{
			std::string major = std::to_string(GPU_info.MajorVersion);
			std::string minor = std::to_string(GPU_info.MinorVersion);
			glsl_version = "#version " + major + minor + "0\n";
		}
		else if (GPU_info.MajorVersion >= 3)
		{	
			std::string major = "1";
			std::string minor = "3";
			if (GPU_info.MinorVersion == 2)
				minor = "5";
			else if (GPU_info.MinorVersion == 1)
				minor = "4";
			glsl_version = "#version " + major + minor + "0\n";
		}
		PRINT(std::cout << "GLSL Version: " << glsl_version << std::endl;)

		std::string vertex_source = vshaderSource;
		std::string fragment_source = fshaderSource;
		//if GLSL version is 100 series then switch to other shaders

		//if its equal or higher than 3.3 if(major >=3 && minor >=3
		if (GPU_info.MajorVersion < 4 && GPU_info.MinorVersion < 3)
		{
			vertex_source = vshaderSource_old;
			fragment_source = fshaderSource_old;
		}

		vertex_source = glsl_version + vertex_source;
		fragment_source = glsl_version + fragment_source;
		
		//main shader
		if (!CompileGLShader(vertex_source, fragment_source, program_main))
		{
			PRINT(std::cout << "main shaders failed to compile" << std::endl;)
			WriteErrorFile("Shaders failed to compile. (Need shaders so game cannot go on)");
			return false;
		}
		glUseProgram(program_main);
		program_main_mvp = glGetUniformLocation(program_main, "mvp");
		program_main_color = glGetUniformLocation(program_main, "color_val");
		program_main_use_texture = glGetUniformLocation(program_main, "use_texture");
		program_main_use_alpha = glGetUniformLocation(program_main, "use_alpha");
		glUseProgram(0);
		checkError("After setup main shader");
		
		//creating vbo/ibo/vao
		std::vector<float> rect =
		{
			//position      //uv
			-1.0,1.0, 0.0,   0.0,1.0,  //tl
			-1.0,-1.0,0.0,   0.0,0.0,  //bl
			1.0,-1.0, 0.0,   1.0,0.0,  //br
			1.0,1.0,  0.0,   1.0,1.0   //tr
		};

		std::vector<uint32_t> rect_index;
		rect_index.push_back(3);
		rect_index.push_back(0);
		rect_index.push_back(1);
		rect_index.push_back(3);
		rect_index.push_back(1);
		rect_index.push_back(2);

		glGenVertexArrays(1, &rect_vao);
		glBindVertexArray(rect_vao);
		glGenBuffers(1, &rect_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, rect_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * rect.size(), rect.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0); //position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3)); //uv attribute
		glEnableVertexAttribArray(1);
		glGenBuffers(1, &rect_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rect_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t)*rect_index.size(), rect_index.data(), GL_STATIC_DRAW);

		if (checkError("created vbo,ibo,vao"))
		{
			PRINT("error creating vbo,ibo,vao\n";)
			WriteErrorFile("error creating vbo,ibo,vao. (geometry might now be messed up)");
		}

		glGenFramebuffers(1, &fbo_main);
		ResizeFBO();

		checkError("End of renderer initialization");
		init = true;

		return true;
	}

	void ResizeFBO()
	{
		checkError("ResizeFBO top----");
		GLenum default_format = GL_RGB8;
		if (fbo_failed_once)
			default_format = GL_RGB;

		glDeleteTextures(1, &color_attachment);
		glGenTextures(1, &color_attachment);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo_main);

		//clamp resolution to max values
		GLsizei resolution_w = Config::SCREEN_WIDTH;
		GLsizei resolution_h = Config::SCREEN_HEIGHT;
		if (GPU_info.maxFrameBufferWidth != -1)
			resolution_w = std::min(resolution_w, GPU_info.maxFrameBufferWidth);
		if (GPU_info.maxFrameBufferHeight != -1)
			resolution_h = std::min(resolution_h, GPU_info.maxFrameBufferHeight);

		if (Config::MULTISAMPLE_COUNT > 1)
		{
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, color_attachment);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sample_count, default_format, resolution_w, resolution_h, GL_TRUE);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, color_attachment, 0);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, color_attachment);
			glTexImage2D(GL_TEXTURE_2D, 0, default_format, resolution_w, resolution_h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment, 0);
		}

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			PRINT(std::cout << "Frame buffer failed to be complete!\n";)
			WriteErrorFile("Frame buffer failed to be created. (Using default framebuffer)");
			fbo_failed = true;
			fbo_failed_once = true;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		checkError("ResizeFBO");
	}

	void ReCreateFBO()
	{
		glDeleteFramebuffers(1, &fbo_main);
		glGenFramebuffers(1, &fbo_main);
		fbo_failed = false;
	}

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		if (width <= 0) width = 1;
		if (height <= 0) height = 1;

		Config::SCREEN_WIDTH = width;
		Config::SCREEN_HEIGHT = height;
		PRINT(std::cout << "Window Resize: "<<width << " x " << height << std::endl;)

		GLsizei viewport_width = width;
		GLsizei viewport_height = height;
		if (GPU_info.maxViewPortDimensions[0] != -1)
			viewport_width = std::min(viewport_width, GPU_info.maxViewPortDimensions[0]);
		if (GPU_info.maxViewPortDimensions[1] != -1)
			viewport_height = std::min(viewport_height, GPU_info.maxViewPortDimensions[1]);

		glViewport(0, 0, viewport_width, viewport_height);
		
		if (fbo_failed)
			ReCreateFBO();
		ResizeFBO();
	}
};

Renderer RenderManager;

/*
-----gpu driver independance-----

Usually for GPU there are:
1.) Features, that are true of false that gpus use or not. Like compute shader, depth clamp, blending, geometry shader, etc. These are API independant features
2.) Limits, which are just limit values that you can't go over for that GPU
3.) Formats, whenever you created images, buffers, fbos they require formats well these tell you what formats are actually supported by the GPU
4.) API versions supported.
5.) extensions, does this GPU supported different api extensions
6.) shaders, have different versions that could use different calls and support different things
Each of these can change from GPU to GPU so query this information and deal with it appropriately

glGetString: tells you opengl api and shader versions
glGet: tells you limits and other info
features: to query features theres no openGL option, but features are directly linked with api versions. So check to see what api that feature it supported on
formats: for format, you set the interlformat parameter which requests a format, but it can be switched. So I don't think formats can actually fail, theres no way to query them anyways, 
maybe see if texture failed to create. NOTE: you can use proxy textures that "test" create texture to see if its valid or not.
Opengl Specification tells you what calls are supported on what API version
For GLSL https://github.com/mattdesl/lwjgl-basics/wiki/GLSL-Versions shows how OpenGL versions map to GLSL

-driver bugs-
there could be very specific driver bugs but they are harder to catch and are more to deal with when they happen
*/

/* OpenGL calls used in this renderer:
		glGetIntegerv			   - 2.0 to 4.6
		glGetStringi			   - 3.0 to 4.6
		glGetString				   - 2.0 to 4.6
		glDisable				   - 2.0 to 4.6
		glEnable				   - 2.0 to 4.6
		glPolygonMode			   - 2.0 to 4.6
		glClearColor			   - 2.0 to 4.6
		glBlendEquation            - 2.0 to 4.6
		glBlendFunc                - 2.0 to 4.6
		glUseProgram               - 2.0 to 4.6
		glGetUniformLocation       - 2.0 to 4.6
		glGenVertexArrays          - 3.0 to 4.6
		glBindVertexArray		   - 3.0 to 4.6
		glGenBuffers			   - 2.0 to 4.6
		glBindBuffer			   - 2.0 to 4.6
		glBufferData			   - 2.0 to 4.6
		glVertexAttribPointer	   - 3.0 to 4.6
		glEnableVertexAttribArray  - 2.0 to 4.6
		glGenFramebuffers		   - 3.0 to 4.6
		glDeleteTextures		   - 2.0 to 4.6
		glGenTextures			   - 2.0 to 4.6
		glBindFramebuffer		   - 3.0 to 4.6
		glTexImage2DMultisample	   - 3.2 to 4.6
		glBindTexture			   - 2.0 to 4.6
		glFramebufferTexture2D	   - 3.0 to 4.6
		glTexImage2D			   - 2.0 to 4.6
		glTexParameteri			   - 2.0 to 4.6
		glCheckFramebufferStatus   - 3.0 to 4.6
		glViewport				   - 2.0 to 4.6
		glUniformMatrix4fv		   - 2.0 to 4.6
		glUniform1i				   - 2.0 to 4.6
		glActiveTexture			   - 2.0 to 4.6
		glUniform4fv			   - 2.0 to 4.6
		glDrawElements			   - 2.0 to 4.6
		glBlitFramebuffer		   - 3.0 to 4.6 *
		glDeleteBuffers			   - 2.0 to 4.6
		glDeleteProgram			   - 2.0 to 4.6
		glClear					   - 2.0 to 4.6
		glGetError				   - 2.0 to 4.6
		glCreateShader			   - 2.0 to 4.6
		glShaderSource			   - 2.0 to 4.6
		glCompileShader			   - 2.0 to 4.6
		glGetShaderiv			   - 2.0 to 4.6
		glGetShaderInfoLog		   - 2.0 to 4.6
		glGetProgramInfoLog		   - 2.0 to 4.6
		glGetProgramiv			   - 2.0 to 4.6
		glCreateProgram			   - 2.0 to 4.6
		glAttachShader			   - 2.0 to 4.6
		glLinkProgram			   - 2.0 to 4.6
	*/