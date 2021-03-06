//
// Created by ryu19 on 2020/12/30.
//

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include <memory>
#include <iostream>
using namespace  std;
namespace rayt {
    class Image{
    public:
        struct rgb{
            unsigned char r;
            unsigned char g;
            unsigned char b;
        };
        Image(): m_pixels(nullptr){}
        Image(int w,int h) {
            m_width = w;
            m_height = h;
            m_pixels.reset(new rgb[m_width*m_height]);
        }
        int width() const {return m_width;}
        int height() const {return m_height;}
        void* pixels() const {return m_pixels.get();}
        void write(int x,int y,float r,float g,float b){
            int index = m_width*y + x;
            m_pixels[index].r = static_cast<unsigned char>(min(r*255.99f,255.0f));
            m_pixels[index].g = static_cast<unsigned char>(min(g*255.99f,255.0f));
            m_pixels[index].b = static_cast<unsigned char>(min(b*255.99f,255.0f));
        }

    private:
        int m_width;
        int m_height;
        unique_ptr<rgb[]> m_pixels;
    };
}
