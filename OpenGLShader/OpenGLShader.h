/*
	Smokin::ShaderLib - OpengGL Shader support.

	Software License Agreement (MIT License)

	Copyright (c) 2008 Rui Varela <rui.filipe.varela@gmail.com>

	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __SMOKIN_OPENGL_SHADER_H__
#define __SMOKIN_OPENGL_SHADER_H__

#include "..\Pointer\SharedPointer.h"

#include <GL/glew.h>
#include <sstream>
#include <string>
#include <list>
#include <map>
#include <vector>

namespace Smokin
{
	class Shader
	{
	public:
		typedef int OpenGlId;
		
		enum ShaderType
		{
			FragmentShader = GL_FRAGMENT_SHADER,
			VertexShader = GL_VERTEX_SHADER
		};

		Shader(ShaderType const type = VertexShader);
		Shader(std::string const& source, ShaderType const type = VertexShader);
		~Shader();

		bool loadFromFile(std::string const& filename);
		bool loadFromSource(std::string const& source);
		void unload();

		bool isLoaded() { return loaded; }
		ShaderType const& type() const { return shader_type; }
		OpenGlId const& id() const { return shader_id; }

		std::string log() const { return shader_log.str(); }
	private:
		Shader(Shader const & copy) {} // you should not copy a shader
		Shader& operator=(Shader const & copy) {} // you should not copy a shader

		ShaderType shader_type;
		std::stringstream shader_log;
		bool loaded;

		OpenGlId shader_id;
	};

	class Program;

	class Uniform
	{
	public:
		typedef Shader::OpenGlId OpenGlId;

		enum UniformType
		{
			FloatUniform = GL_FLOAT, 
			FloatVec2Uniform = GL_FLOAT_VEC2,
			FloatVec3Uniform = GL_FLOAT_VEC3,
			FloatVec4Uniform = GL_FLOAT_VEC4,
			IntUniform = GL_INT,
			IntVec2Uniform = GL_INT_VEC2,
			IntVec3Uniform = GL_INT_VEC3,
			IntVec4Uniform = GL_INT_VEC4,

			// TODO arrays / matrix (multi - elements)
		};

		Uniform(std::string const& name, float const value_1);
		Uniform(std::string const& name, float const value_1, float const value_2);
		Uniform(std::string const& name, float const value_1, float const value_2, float const value_3);
		Uniform(std::string const& name, float const value_1, float const value_2, float const value_3, float const value_4);
		Uniform(std::string const& name, int const value_1);
		Uniform(std::string const& name, int const value_1, int const value_2);
		Uniform(std::string const& name, int const value_1, int const value_2, int const value_3);
		Uniform(std::string const& name, int const value_1, int const value_2, int const value_3, int const value_4);

		void set(float const value_1);
		void set(float const value_1, float const value_2);
		void set(float const value_1, float const value_2, float const value_3);
		void set(float const value_1, float const value_2, float const value_3, float const value_4);

		void set(int const value_1);
		void set(int const value_1, int const value_2);
		void set(int const value_1, int const value_2, int const value_3);
		void set(int const value_1, int const value_2, int const value_3, int const value_4);

		~Uniform();

		OpenGlId const& id() const { return uniform_id; }
		UniformType const& type() const { return uniform_type; }
		std::string const& name() const { return uniform_name; }

		unsigned int numberOfElements() const { return numer_of_elements; }
		unsigned int numberOfComponens() const { return component_size; }
		float* floatData() { return float_data; }
		float* intData() { return float_data; }

		void upload();
	private:
		UniformType uniform_type;
		std::string uniform_name;

		unsigned int numer_of_elements;
		unsigned int component_size;
		bool integer_internal_data;

		void initData();

		float *float_data;
		int *int_data;

		OpenGlId uniform_id;
		Program* program;

		friend class Program;
	};

	class Program
	{
	public:
		typedef Shader::OpenGlId OpenGlId;

		typedef std::list< SharedPointer<Shader> > ProgramShaders;
		typedef std::map< std::string, SharedPointer<Uniform> > ProgramUniforms;

		Program();
		~Program();

		void addShader(SharedPointer<Shader> shader);
		void removeShader(Shader* shader);
		ProgramShaders const& shaders() const { return program_shaders; }

		void addUniform(SharedPointer<Uniform> uniform);
		Uniform* uniform(std::string const name);
		ProgramUniforms const& uniforms() const { return program_uniforms; }

		bool link();
		void use();
		static void useFixedFunctionality(); 
		void unload();

		bool isLinked() const { return linked; }
		OpenGlId const& id() const { return program_id; }

		std::string log() const { return program_log.str(); }
	private:
		Program(Program const& copy) {} // you should not copy a program
		Program& operator=(Program const & copy) {} // you should not copy a program

		ProgramShaders program_shaders;
		ProgramUniforms program_uniforms;

		OpenGlId program_id;
		bool linked;
		std::stringstream program_log;
	};

}; //namespace Smokin

#endif //__SMOKIN_OPENGL_SHADER_H__