/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*    Shader.hpp                                     oooooo       oooooo      */
/*                                                 oooooooooo   oooooooooo    */
/*                                                         o%%%%%o            */
/*                                                         %:::::%            */
/*                                                        %:::::::%           */
/*    This file is part of the                             %:::::%            */
/*    Lums library.                                         %%%%%             */
/*                                                                            */
/* ************************************************************************** */

#ifndef LUMS_SHADER_HPP
#define LUMS_SHADER_HPP

#include <Lums/OpenGL.hpp>

namespace lm
{
	class Shader
	{
	friend class ShaderProgram;
	
	public:
		enum Type
		{
			Vertex,
			Fragment
		};

		Shader(const char* path, Type type, bool resource = true);
		GLuint	get() const	{ return _shader; }
		static const Shader&	fragment();
		static const Shader&	vertex();
		~Shader();

	private:
		Shader();
		void	load(const char* data, GLenum type);
		GLuint	_shader;
	};
}

#endif