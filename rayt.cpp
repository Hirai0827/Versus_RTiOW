#include <iostream>
#include "rayt.h"
#include "ray.h"
#include "vec3.h"
#include "math_utils.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "moving_sphere.h"
#include "aarect.h"
#include "box.h"
#include <omp.h>
#include "constant_medium.h"
#define NUM_THREAD 4
using namespace std;

hittable_list cornell_box() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto blue = make_shared<lambertian>(color(.12, .15, .45));
    auto light = make_shared<diffuse_light>(color(8, 8, 8));
    auto glass = make_shared<dielectric>(1.5);

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(100, 555-100, 100, 555-100, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, blue));
    //objects.add(make_shared<box>(point3(130, 0, 65), point3(295, 165, 230), white));
    //objects.add(make_shared<box>(point3(265, 0, 295), point3(430, 330, 460), white));
    objects.add(make_shared<sphere>(point3(265, 200.0, 295), 100.0, glass));
    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    objects.add(box1);

    shared_ptr<hittable> box2 = make_shared<box>(point3(0,0,0), point3(555,555,555), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(0,0,0));
   // objects.add(make_shared<constant_medium>(box2, 0.001, color(1,1,1)));
    return objects;
}

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));

    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0,.5), 0);
                    world.add(make_shared<moving_sphere>(
                            center, center2, 0.0, 1.0, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));
    return world;
}

color ray_color(const ray& r,const color& background,const hittable& world,int depth){
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth-1);
}

int main() {
    //Image
    auto aspect_ratio = 16.0/9.0;
    int nx = 400;
    int ny = static_cast<int>(nx / aspect_ratio);
    int samples_per_pixel = 150;
    int max_depth = 100;
    //World
    hittable_list world;
    auto material_ground = make_shared<lambertian>(make_shared<checker_texture>(color(0.8, 0.8, 0.0),color(1.0, 1.0, 1.0)));
    auto material_left = make_shared<dielectric>(1.5);
    auto material_center   = make_shared<diffuse_light>(color(0.8, 0.8, 0.8));
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2),1.0);

    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    world.add(make_shared<sphere>(point3( 2.0,    0.0, -2.0),   0.5, material_center));
    world.add(make_shared<sphere>(point3( -2.0,    0.0, -2.0),   0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
    world = cornell_box();
    //Camera
    point3 lookfrom(0,2,5);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto aperture = 0.0;
    color background(0,0,0);
    world = cornell_box();
    aspect_ratio = 1.0;
    ny = static_cast<int>(nx / aspect_ratio);
    samples_per_pixel = 200;
    background = color(0,0,0);
    lookfrom = point3(278, 278, -800);
    lookat = point3(278, 278, 0);
    auto dist_to_focus = (lookfrom - lookat).length_squared();
    double vfov = 40.0;

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus,0.0,1.0);

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
                pixel_color += ray_color(r,background,world,max_depth);

            }
            //ガンマ補正書ける(=2.0として近似する(と実質sqrtになる))
            image->write(nx- xi-1,ny-yi-1,sqrt(pixel_color[0]/samples_per_pixel),sqrt(pixel_color[1]/samples_per_pixel),sqrt(pixel_color[2]/samples_per_pixel));
        }
    }
    std::cerr << "\nDone.\n";
    stbi_write_png("../render.png", nx, ny, sizeof(rayt::Image::rgb), image->pixels(),nx*sizeof(rayt::Image::rgb));

    return 0;

}
