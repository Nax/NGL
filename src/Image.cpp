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
#include <png.h>
#include <Lums/Image.hpp>
#include <Lums/OperatingSystem.hpp>

using namespace lm;

Image::Image()
: _width(0)
, _height(0)
, _texture(0)
{

}

Image::Image(Image&& rhs)
: _width(rhs._width)
, _height(rhs._height)
, _texture(rhs._texture)
{
    rhs._texture = 0;
    _atlas = std::move(rhs._atlas);
}

Image&
Image::operator=(Image&& rhs)
{
    _width = rhs._width;
    _height = rhs._height;
    _texture = rhs._texture;
    rhs._texture = 0;
    _atlas = std::move(rhs._atlas);
    return *this;
}

Image&
Image::atlas(size_t w, size_t h)
{
    size_t len = w * h;
    double img_w = 1.0 / w;
    double img_h = 1.0 / h;

    _iwidth = _width / w;
    _iheight = _height / h;
    _atlas.resize(len);
    for (size_t i = 0; i < len; i++)
    {
        double x = (i % w) * img_w;
        double y = (i / w) * img_h;

        _atlas[i].x = x;
        _atlas[i].y = y;
        _atlas[i].w = img_w;
        _atlas[i].h = img_h;
        _atlas[i].offX = 0;
        _atlas[i].offY = 0;
    }
    return *this;
}

Image&
Image::atlas(const lm::FrameDescriptori* rect, size_t n)
{
    double w = _width;
    double h = _height;

    _iwidth = _width / rect[0].w;
    _iheight = _height / rect[0].h;
    _atlas.resize(n);
    for (size_t i = 0; i < n; i++)
    {
        _atlas[i].x = rect[i].x / w;
        _atlas[i].y = rect[i].y / h;
        _atlas[i].w = rect[i].w / w;
        _atlas[i].h = rect[i].h / h;
        _atlas[i].offX = rect[i].offX;
        _atlas[i].offY = rect[i].offY;
    }
    return *this;
}

void
Image::loadFile(const std::string path, bool resource)
{
    typedef void (Image::*imgptr_t)(std::string, bool);

    static const std::map<std::string, imgptr_t> extFuncs = {
        {"png", &Image::loadFilePNG}
    };

    std::string ext = path.substr(path.find_last_of('.') + 1);

    if (extFuncs.find(ext) != extFuncs.end())
    {
        (this->*(extFuncs.at(ext)))(path, resource);
        atlas();
    }
}

/*
 * This is KLUDGE
 */

void
Image::loadFilePNG(const std::string path, bool resource)
{
    std::string     file = resource ? resourcePath() + '/' + path : path;
    FILE*           f = fopen(file.c_str(), "rb");
    png_byte        header[8];

    if (!f)
    {
        std::cerr << "PNG: Could not open file '" << file << "'" << std::endl;
        return;
    }

    fread(header, 1, 8, f);
    if (png_sig_cmp(header, 0, 8))
        return;

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    png_init_io(png_ptr, f);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    int color;
    int bits;
    unsigned int w;
    unsigned int h;
    GLint format;

    png_get_IHDR(png_ptr, info_ptr, &w, &h, &bits, &color, NULL, NULL, NULL);

    switch (color)
    {
        case PNG_COLOR_TYPE_RGB:
            format = GL_RGB;
            break;

        case PNG_COLOR_TYPE_RGB_ALPHA:
            format = GL_RGBA;
            break;

        case PNG_COLOR_TYPE_PALETTE:
            format = GL_RGBA;
            png_set_packing(png_ptr);
            png_set_palette_to_rgb(png_ptr);
            break;

        default:
            std::cerr << "PNG: Invalid color : " << color << " (" << bits << " bits)" << std::endl;
            return;
    }

    _width = png_get_image_width(png_ptr, info_ptr);
    _height = png_get_image_height(png_ptr, info_ptr);

    png_read_update_info(png_ptr, info_ptr);

    int rows = png_get_rowbytes(png_ptr, info_ptr);
    rows += 3 - (rows - 1) % 4;

    png_byte* image = new png_byte[rows * _height];
    png_byte** image_ptr = new png_byte*[_height];

    for (size_t i = 0; i < _height; i++)
        image_ptr[i] = image + i * rows;

    png_read_image(png_ptr, image_ptr);

    delete [] image_ptr;

    fclose(f);
    gen(image, format);
}

void
Image::loadDescriptor(const ImageDescriptor& descriptor)
{
    loadFile(descriptor._path);
    if (descriptor._customAtlas)
        atlas(descriptor._atlas.custom.rect, descriptor._atlas.custom.n);
    else
        atlas(descriptor._atlas.normal.x, descriptor._atlas.normal.y);
    linear(descriptor._linear);
}

Image
Image::fromFile(const std::string path, bool resource)
{
    Image   a;

    a.loadFile(path, resource);
    return a;
}

Image::~Image()
{
    if (_texture)
        glDeleteTextures(1, &_texture);
}

/*
 * PRIVATE
 */

void
Image::gen(unsigned char* img, GLint format)
{
    if (_texture)
        glDeleteTextures(1, &_texture);
    glGenTextures(1, &_texture);
    linear();
    glTexImage2D(GL_TEXTURE_2D, 0, format, _width, _height, 0, format, GL_UNSIGNED_BYTE, img);
    delete img;
}
