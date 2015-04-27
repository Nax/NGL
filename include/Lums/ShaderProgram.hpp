/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*    ShaderProgram.hpp                              oooooo       oooooo      */
/*                                                 oooooooooo   oooooooooo    */
/*                                                         o%%%%%o            */
/*                                                         %:::::%            */
/*                                                        %:::::::%           */
/*    This file is part of the                             %:::::%            */
/*    Lums library.                                         %%%%%             */
/*                                                                            */
/* ************************************************************************** */

#ifndef LUMS_SHADER_PROGRAM_HPP
#define LUMS_SHADER_PROGRAM_HPP

#include <vector>
#include <Lums/GL.hpp>

namespace lm
{
	class Shader;

	class ShaderProgram
	{
	public:
		ShaderProgram();
		void	attach(const Shader& shader);
		void	link();
		void	use() const;
		static const ShaderProgram& defaultProgram();
		~ShaderProgram();

	private:
		GLuint				_program;
		std::vector<GLuint>	_shaders;

	};
}

#endif