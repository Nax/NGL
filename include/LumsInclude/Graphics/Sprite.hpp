/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*    Sprite.hpp                                     oooooo       oooooo      */
/*                                                 oooooooooo   oooooooooo    */
/*                                                         o%%%%%o            */
/*                                                         %:::::%            */
/*                                                        %:::::::%           */
/*    This file is part of the                             %:::::%            */
/*    Lums library.                                         %%%%%             */
/*                                                                            */
/* ************************************************************************** */

#ifndef LUMS_SPRITE_HPP
#define LUMS_SPRITE_HPP

#include <LumsInclude/Graphics/Texture.hpp>
#include <LumsInclude/ExportDll.hpp>
#include <LumsInclude/Math/Vector.hpp>
#include <iostream>

namespace lm
{
    class Sprite
    {
    public:
        Sprite();
        
        Texture&
        texture() const
        {
            return *_texture;
        }

        int
        width() const
        {
            return _texture->atlas(_currentFrame).size.x * _texture->width() * _scale.x;
        }

        int
        height() const
        {
            return _texture->atlas(_currentFrame).size.y * _texture->height() * _scale.y;
        }

        std::size_t
        atlas() const
        {
            return _currentFrame;
        }

        const lm::Vector2f&
        scale() const
        {
            return _scale;
        }

        int
        frame() const
        {
            return _currentFrame - _baseFrame;
        }

        void
        setTexture(Texture& texture)
        {
            _texture = &texture;
            setImageInAtlas(0);
        }

        void
        setAnimation(std::size_t image, std::size_t length, std::size_t speed, bool loop = true)
        {
            setSpeed(speed);
            _length = length;
            _loop = loop;
            _finished = false;
            setImageInAtlas(image);
        }

        void
        setSpeed(std::size_t speed)
        {
            _acc = 0;
            _speed = speed;
        }

        void
        setImageInAtlas(std::size_t i)
        {
            _baseFrame = _currentFrame = i;
            _acc = 0;
        }

        void
        setScale(float scale)
        {
            setScale(scale, scale);
        }

        void
        setScale(float sx, float sy)
        {
            _scale = {sx, sy};
        }

        bool
        finished() const
        {
            return _finished;
        }

        LUMS_EXPORTED void  update();

        lm::Vector2f    pos;
        lm::Vector2b    flip;

    private:
        Texture*        _texture;
        lm::Vector2f    _scale;
        std::size_t     _baseFrame;
        std::size_t     _currentFrame;
        std::size_t     _length;
        std::size_t     _acc;
        std::size_t     _speed;
        bool            _loop;
        bool            _finished;

    };
}

#endif