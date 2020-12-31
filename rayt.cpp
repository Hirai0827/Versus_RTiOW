#include <iostream>
#include "rayt.h"
#include "ray.h"
#include "vec3.h"
#include "math_utils.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include <omp.h>
#define NUM_THREAD 8
using namespace std;


color ray_color(const ray& r,const hittable& world,int depth){
    hit_record rec;
    if(depth <= 0){
        return color(0.0,0.0,0.0);
    }
    if(world.hit(r,0.0001,infinity,rec)){
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth-1);
        return color(0,0,0);
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0,1.0,1.0) + t * color(0.5,0.7,1.0);
}
int main() {
    //Image
    const auto aspect_ratio = 16.0/9.0;
    const int nx = 400;
    const int ny = static_cast<int>(nx / aspect_ratio);
    const int samples_per_pixel = 50;
    const int max_depth = 50;
    //World
    hittable_list world;
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_left = make_shared<dielectric>(1.5);
    auto material_center   = make_shared<metal>(color(0.8, 0.8, 0.8),0.3);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2),1.0);

    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
    //Camera
    point3 lookfrom(3,3,2);
    point3 lookat(0,0,-1);
    vec3 vup(0,1,0);
    auto dist_to_focus = (lookfrom-lookat).length();
    auto aperture = 2.0;

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    unique_ptr<rayt::Image> image(make_unique<rayt::Image>(nx,ny));
    #pragma omp parallel for schedule(dynamic, 1) num_threads(NUM_THREAD)
    for(int yi = 0; yi < ny; yi++){
        std::cerr << "\rScanlines remaining: " << yi << ' ' << std::flush;
        for(int xi = 0; xi < nx; xi++){
            color pixel_color(0,0,0);
            for(int i = 0; i < samples_per_pixel;i++){
                auto u = double(xi + random_double()) / (nx-1);
                auto v = double(yi + random_double()) / (ny-1);
                ray r = cam.get_ray(u,v);
                pixel_color += ray_color(r,world,max_depth);

            }
            //ガンマ補正書ける(=2.0として近似する(と実質sqrtになる))
            image->write(nx- xi-1,ny-yi-1,sqrt(pixel_color[0]/samples_per_pixel),sqrt(pixel_color[1]/samples_per_pixel),sqrt(pixel_color[2]/samples_per_pixel));
        }
    }
    std::cerr << "\nDone.\n";
    stbi_write_png("../render.png", nx, ny, sizeof(rayt::Image::rgb), image->pixels(),nx*sizeof(rayt::Image::rgb));

    return 0;

}
