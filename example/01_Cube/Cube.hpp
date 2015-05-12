/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*    Cube.hpp                                       oooooo       oooooo      */
/*                                                 oooooooooo   oooooooooo    */
/*                                                         o%%%%%o            */
/*                                                         %:::::%            */
/*                                                        %:::::::%           */
/*    This file is part of the                             %:::::%            */
/*    Lums library.                                         %%%%%             */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUBE_HPP
#define CUBE_HPP

#include <Lums/Lums.hpp>

class Cube : public lm::GameState
{
public:
    Cube();
    void    load();
    void    update();
    void    handleEvent(const lm::Event& event);
    void    render();
    void    unload();
    
private:
    lm::Vector2a    		_angle;
    lm::Vector2f    		_speed;
    lm::Projection			_proj;
    lm::VertexBufferP3C4	_vbo;
    lm::ShaderProgram		_shader;
};

#endif
