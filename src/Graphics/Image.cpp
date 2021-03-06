/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*    Image.cpp                                      oooooo       oooooo      */
/*                                                 oooooooooo   oooooooooo    */
/*                                                         o%%%%%o            */
/*                                                         %:::::%            */
/*                                                        %:::::::%           */
/*    This file is part of the                             %:::::%            */
/*    Lums library.                                         %%%%%             */
/*                                                                            */
/* ************************************************************************** */

#define _CRT_SECURE_NO_WARNINGS

#include <map>
#include <cmath>
#include <iostream>
#include <LumsInclude/Graphics/Image.hpp>
#include <LumsInclude/Graphics/Graphics.hpp>
#include <LumsInclude/OperatingSystem.hpp>

using namespace lm;

Image::Image()
: _width(0u)
, _height(0u)
, _bufferWidth(0u)
, _bufferHeight(0u)
, _xScaleHint(1u)
, _yScaleHint(1u)
, _data(nullptr)
, _scale(1.f)
{

}

Image::Image(Image&& rhs)
: _width(rhs._width)
, _height(rhs._height)
, _path(rhs._path)
{

}

Image&
Image::operator=(Image&& rhs)
{
    _width = rhs._width;
    _height = rhs._height;
    _path = rhs._path;
    return *this;
}

void
Image::load()
{
    FILE* file = fopen(_path.c_str(), "r");

    unload();

    if (!file)
    {
        std::cerr << "Image: could not open " << _path << std::endl;
        return;
    }
    _data = ImageFormat::png(file, _width, _height, _format);
    _bufferWidth = _width;
    _bufferHeight = _height;
    fclose(file);
    resize();
}

void
Image::unload()
{
    delete [] _data;
    _data = nullptr;
}

void
Image::setPath(const std::string path, bool resource)
{
    _path = resource ? resourcePath() + '/' + path : path;
}

void
Image::setBuffer(unsigned char* buffer, unsigned int w, unsigned int h, GLenum format, float scale)
{
    unload();
    _data = buffer;
    _width = w * scale;
    _height = h * scale;
    _bufferWidth = w;
    _bufferHeight = h;
    _format = format;
    resize();
}

void
Image::setScale(float scale)
{
    _scale = scale;
}

void
Image::setScaleHint(unsigned int x, unsigned int y)
{
    _xScaleHint = x;
    _yScaleHint = y;
}

Image::~Image()
{
    unload();
}

/* Private */

void
Image::resize()
{
    float scale = _scale * globalScale();
    if (scale == 1.f)
        return;

    unsigned int newWidth = std::ceil(_bufferWidth * scale);
    unsigned int newHeight = std::ceil(_bufferHeight * scale);

    if (newWidth % _xScaleHint)
        newWidth += _xScaleHint - (newWidth % _xScaleHint);
    if (newHeight % _yScaleHint)
        newHeight += _yScaleHint - (newHeight % _yScaleHint);

    float xScale = float(newWidth) / _bufferWidth;
    float yScale = float(newHeight) / _bufferHeight;

    const int pixelSize = (_format == GL_RGB) ? 3 : 4;

    unsigned char* buffer = new unsigned char[newWidth * newHeight * pixelSize];
    for (unsigned int i = 0; i < newWidth * newHeight; ++i)
    {
        const unsigned int x = i % newWidth;
        const unsigned int y = i / newWidth;
        unsigned char* dst = buffer + x * pixelSize + y * newWidth * pixelSize;
        const unsigned char* src = _data + int(x / xScale) * pixelSize + int(y / yScale) * _bufferWidth * pixelSize;

        std::memcpy(dst, src, pixelSize);
    }
    delete [] _data;
    _data = buffer;
    _bufferWidth = newWidth;
    _bufferHeight = newHeight;
}

