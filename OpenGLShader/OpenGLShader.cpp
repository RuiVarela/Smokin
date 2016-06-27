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

#include "OpenGLShader.h"

#include <cassert>
#include <fstream>

namespace Smokin
{

	Shader::Shader(Shader::ShaderType const type)
		:shader_type(type), loaded(false)
	{
	}

	Shader::Shader(std::string const& source, Shader::ShaderType const type)
		:shader_type(type), loaded(false)
	{
		loadFromSource(source);
	}

	Shader::~Shader()
	{
		unload();
	}

	bool Shader::loadFromFile(std::string const& filename)
	{
		unload();

		std::ifstream input;
		input.open(filename.c_str(), std::ios::binary);

		if(!input)
		{
			shader_log << ((shader_type == FragmentShader) ? "Fragment" : "Vertex" )<< " Shader Unable to open file : " << filename << std::endl;
			return false;
		}

		input.seekg(0, std::ios::end);
		int length = input.tellg();

		char *text = new char[length + 1];
		input.seekg(0, std::ios::beg);
		input.read(text, length);
		input.close();
		text[length] = '\0';

		std::string source(text);
		delete[](text);

		loadFromSource(source);

		return loaded;
	}

	bool Shader::loadFromSource(std::string const& source)
	{
		unload();

		char const* source_as_char= source.c_str();

		shader_id = glCreateShader( GLenum(shader_type));
		glShaderSource(shader_id, 1, &source_as_char, 0);
		glCompileShader(shader_id);
		
		GLint compiled = GL_FALSE;
		glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled);

		if (compiled)
		{
			loaded = true;
		}
		else
		{
			shader_log << ((shader_type == FragmentShader) ? "Fragment" : "Vertex" )<< " Shader Compilation Failed" << std::endl;
		}

		int infolog_length = 0;
	    int chars_written  = 0;
	    char *info_log;

		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &infolog_length);

	    if (infolog_length > 0)
	    {
	        info_log = new char[infolog_length + 1];
	        glGetShaderInfoLog(shader_id, infolog_length, &chars_written, info_log);
			shader_log << ((shader_type == FragmentShader) ? "Fragment" : "Vertex" )<< " Info Log :" << std::endl << info_log << std::endl;
	        delete[](info_log);
	    }

		return loaded;
	}

	void Shader::unload()
	{
		shader_log.str("");

		if (loaded)
		{
			glDeleteShader(shader_id); 
			loaded = false;
		}
	}

	//
	// Uniform
	//
	Uniform::Uniform(std::string const& name, float const value_1)
		:uniform_name(name), uniform_type(FloatUniform), numer_of_elements(1), component_size(1), integer_internal_data(false)
	{
		initData();
		set(value_1);
	}

	Uniform::Uniform(std::string const& name, float const value_1, float const value_2)			
		:uniform_name(name), uniform_type(FloatVec2Uniform), numer_of_elements(1), component_size(2), integer_internal_data(false)
	{
		initData();
		set(value_1, value_2);
	}

	Uniform::Uniform(std::string const& name, float const value_1, float const value_2, float const value_3)			
		:uniform_name(name), uniform_type(FloatVec3Uniform), numer_of_elements(1), component_size(3), integer_internal_data(false)
	{
		initData();
		set(value_1, value_2, value_3);
	}

	Uniform::Uniform(std::string const& name, float const value_1, float const value_2, float const value_3, float const value_4)			
		:uniform_name(name), uniform_type(FloatVec4Uniform), numer_of_elements(1), component_size(4), integer_internal_data(false)
	{
		initData();
		set(value_1, value_2, value_3, value_4);
	}

	Uniform::Uniform(std::string const& name, int const value_1)			
		:uniform_name(name), uniform_type(IntUniform), numer_of_elements(1), component_size(1), integer_internal_data(true)
	{
		initData();
		set(value_1);
	}

	Uniform::Uniform(std::string const& name, int const value_1, int const value_2)			
		:uniform_name(name), uniform_type(IntVec2Uniform), numer_of_elements(1), component_size(2), integer_internal_data(true)
	{
		initData();
		set(value_1, value_2);
	}

	Uniform::Uniform(std::string const& name, int const value_1, int const value_2, int const value_3)			
		:uniform_name(name), uniform_type(IntVec3Uniform), numer_of_elements(1), component_size(3), integer_internal_data(true)
	{
		initData();
		set(value_1, value_2, value_3);
	}

	Uniform::Uniform(std::string const& name, int const value_1, int const value_2, int const value_3, int const value_4)			
		:uniform_name(name), uniform_type(IntVec4Uniform), numer_of_elements(1), component_size(4), integer_internal_data(true)
	{
		initData();
		set(value_1, value_2, value_3, value_4);
	}

	void Uniform::set(float const value_1)
	{
		assert(float_data);
		assert(component_size >= 1);

		float_data[0] = value_1;
		upload();
	}

	void Uniform::set(float const value_1, float const value_2)
	{
		assert(float_data);
		assert(component_size >= 2);

		float_data[0] = value_1;
		float_data[1] = value_2;
		upload();
	}

	void Uniform::set(float const value_1, float const value_2, float const value_3)
	{
		assert(float_data);
		assert(component_size >= 3);

		float_data[0] = value_1;
		float_data[1] = value_2;
		float_data[2] = value_3;
		upload();
	}

	void Uniform::set(float const value_1, float const value_2, float const value_3, float const value_4)
	{
		assert(float_data);
		assert(component_size >= 4);

		float_data[0] = value_1;
		float_data[1] = value_2;
		float_data[2] = value_3;
		float_data[3] = value_4;
		upload();
	}

	void Uniform::set(int const value_1)
	{
		assert(int_data);
		assert(component_size >= 1);

		int_data[0] = value_1;
		upload();
	}

	void Uniform::set(int const value_1, int const value_2)
	{
		assert(int_data);
		assert(component_size >= 2);

		int_data[0] = value_1;
		int_data[1] = value_2;
		upload();
	}

	void Uniform::set(int const value_1, int const value_2, int const value_3)
	{
		assert(int_data);
		assert(component_size >= 3);

		int_data[0] = value_1;
		int_data[1] = value_2;
		int_data[2] = value_3;
		upload();
	}

	void Uniform::set(int const value_1, int const value_2, int const value_3, int const value_4)
	{
		assert(int_data);
		assert(component_size >= 4);

		int_data[0] = value_1;
		int_data[1] = value_2;
		int_data[2] = value_3;
		int_data[3] = value_4;
		upload();
	}

	void Uniform::initData()
	{
		uniform_id = 0;
		float_data = 0;
		int_data = 0;
		program = 0;

		if (integer_internal_data)
		{
			int_data = new int[component_size * numer_of_elements];
		}
		else
		{
			float_data = new float[component_size * numer_of_elements];
		}

	}

	void Uniform::upload()
	{
		if (program)
		{
			if (integer_internal_data)
			{
				switch(component_size)
				{
				case 1:
					glUniform1iv(uniform_id, numer_of_elements, int_data);
					break;
				case 2:
					glUniform2iv(uniform_id, numer_of_elements, int_data);
					break;
				case 3:
					glUniform3iv(uniform_id, numer_of_elements, int_data);
					break;
				case 4:
					glUniform4iv(uniform_id, numer_of_elements, int_data);
					break;
				}
			}
			else
			{
				switch(component_size)
				{
				case 1:
					glUniform1fv(uniform_id, numer_of_elements, float_data);
					break;
				case 2:
					glUniform2fv(uniform_id, numer_of_elements, float_data);
					break;
				case 3:
					glUniform3fv(uniform_id, numer_of_elements, float_data);
					break;
				case 4:
					glUniform4fv(uniform_id, numer_of_elements, float_data);
					break;
				}
			}
		}
	}

	Uniform::~Uniform()
	{
		delete[](float_data);
		delete[](int_data);
	}

	//
	// Program
	//

	Program::Program()
	{
		linked = false;
		program_id = 0;
	}

	Program::~Program()
	{
		unload();
	}

	void Program::addShader(SharedPointer<Shader> shader)
	{
		if (!program_id)
		{
			program_id = glCreateProgram();
		}

		glAttachShader(program_id, shader->id()); 

		program_shaders.push_back(shader);

		assert(program_id);
	}

	void Program::removeShader(Shader* shader)
	{
		ProgramShaders::iterator iterator = program_shaders.begin();

		while(iterator != program_shaders.end())
		{
			if (iterator->get() == shader)
			{
				assert(program_id);
				glDetachShader(program_id, (*iterator)->id() ); 
				iterator = program_shaders.erase(iterator);
			}
			else
			{
				++iterator;
			}
		}
	}

	void Program::addUniform(SharedPointer<Uniform> uniform)
	{
		assert(uniform.get());

		program_uniforms[uniform->name()] = uniform;

		if (linked)
		{
			use();

			OpenGlId id = glGetUniformLocation(program_id, uniform->name().c_str());

			if (id != -1)
			{
				uniform->uniform_id = id; 
				uniform->program = this;
				uniform->upload();
			}
			else
			{
				uniform->uniform_id = 0; 
				uniform->program = 0;
				program_log << "Uniform not found : " << uniform->name().c_str() << std::endl;
			}
		}
	}

	Uniform* Program::uniform(std::string const name)
	{
		ProgramUniforms::iterator found = program_uniforms.find(name);

		if (found == program_uniforms.end())
		{
			return 0;
		}

		return found->second.get();
	}

	bool Program::link()
	{
		if (program_id)
		{
			glLinkProgram(program_id);

			int link_status = GL_FALSE;
			glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);

			if (link_status)
			{
				linked = true;

				use();

				for (ProgramUniforms::iterator uniform_iterator = program_uniforms.begin();
					uniform_iterator != program_uniforms.end(); 
					++uniform_iterator)
				{
					OpenGlId id = glGetUniformLocation(program_id, uniform_iterator->second->name().c_str());

					if (id != -1)
					{
						uniform_iterator->second->uniform_id = id; 
						uniform_iterator->second->program = this;
						uniform_iterator->second->upload();
					}
					else
					{
						uniform_iterator->second->uniform_id = -1; 
						uniform_iterator->second->program = 0;

						program_log << "Uniform not found : " << uniform_iterator->second->name().c_str() << std::endl;
					}
				}
			}
			else
			{
				program_log << "Program Link Failed" << std::endl;
			}

			int infolog_length = 0;
			int chars_written  = 0;
			char *info_log;

			glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &infolog_length);

			if (infolog_length > 0)
			{
				info_log = new char[infolog_length + 1];
				glGetProgramInfoLog(program_id, infolog_length, &chars_written, info_log);
				program_log << "Program Info Log :" << std::endl << info_log << std::endl;
				delete[](info_log);
			}
		}

		return linked;
	}

	void Program::unload()
	{
		useFixedFunctionality();


		program_log.str("");

		ProgramShaders::iterator iterator = program_shaders.begin();

		while(iterator != program_shaders.end())
		{
			assert(program_id);
			glDetachShader(program_id, (*iterator)->id() ); 
			iterator = program_shaders.erase(iterator);
		}

		for (ProgramUniforms::iterator uniform_iterator = program_uniforms.begin();
			uniform_iterator != program_uniforms.end(); 
			++uniform_iterator)
		{
			uniform_iterator->second->program = 0;
			uniform_iterator->second->uniform_id = 0;
		}

		program_uniforms.clear();

		if (program_id)
		{
			glDeleteProgram(program_id);
		}

		linked = false;
	}

	void Program::use()
	{
		if (linked)
		{
			glUseProgram(program_id);
		}
	}

	void Program::useFixedFunctionality()
	{
		glUseProgram(0);
	}

};