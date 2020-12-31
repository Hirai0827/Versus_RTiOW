#include <iostream>
#include "rayt.h"
using namespace std;
int main() {
    int nx = 200;
    int ny = 100;
    unique_ptr<rayt::Image> image(make_unique<rayt::Image>(nx,ny));
    for(int yi = 0; yi < ny; yi++){
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for(int xi = 0; xi < nx; xi++){
            float r = float(xi)/float(nx);
            float g = float(yi)/float(ny);
            float b = 0.5f;
            image->write(xi,yi,r,g,b);
        }
    }
    std::cerr << "\nDone.\n";
    stbi_write_bmp("render.bmp", nx, ny, sizeof(rayt::Image::rgb), image->pixels());

    return 0;

}
