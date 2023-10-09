#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "third_party/argparse.hpp"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

// SHOW macro prints a variable's name and then its value
#define SHOW(a) std::clog << #a << ": " << (a) << std::endl;

int main(int argc, char * argv[])
{
    // ========================================
    // ARGUMENT PARSING
    // ========================================

    argparse::ArgumentParser program("raytrace");

    program.add_argument("-f", "--fancy")
        .help("sets higher samples per pixel and ray depth count")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("-r", "--randomize")
        .help("seeds the RNG with the current time")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("-s", "--samples")
        .help("sets the number of samples per pixel")
        .default_value(16)
        .metavar("INT")
        .scan<'i', int>();

    program.add_argument("-d", "--depth")
        .help("sets maximum number of ray bounces")
        .default_value(8)
        .metavar("INT")
        .scan<'i', int>();

    program.add_argument("--seed")
        .help("seeds the RNG with an unsigned integer you provide")
        .metavar("UINT")
        .scan<'i', unsigned int>();

    program.add_argument("--fov")
        .help("Camera's Vertical Field of View")
        .default_value(20)
        .metavar("INT")
        .scan<'i', int>();

    program.add_argument("--frame")
        .help("SPECIAL: frame number to use when making animations")
        .metavar("INT")
        .scan<'i', int>();

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error & err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    // ========================================
    // RANDOM NUMBER GENERATOR SETTINGS
    // ========================================

    if (program.is_used("seed"))
    {
        unsigned int seed = program.get<unsigned int>("seed");
        std::clog << "Enabled: Custom RNG seed provided: " << seed << std::endl;
        utils::randomize(seed);
    }
    else if (program.is_used("randomize") && program.get<bool>("randomize"))
    {
        std::clog << "Enabled: Randomize the RNG seed" << std::endl;
        utils::randomize(); // seed the randomizer with current time to get a different image each time
    }

    // ========================================
    // SET UP THE CAMERA AND WORLD
    // ========================================

    hittable_list world; // the list of all objects in our world
    camera        cam;   // how we view this world

    // cam.image_width       = 1200;
    cam.samples_per_pixel = 16;
    cam.max_depth         = 8;
    cam.vfov              = 20;
    cam.lookfrom          = point3(13, 2, 3);
    cam.lookat            = point3(0, 0, 0);
    cam.vup               = vec3(0, 1, 0);
    cam.defocus_angle     = 0.6;
    cam.focus_dist        = 10.0;

    if (program.is_used("fancy") && program.get<bool>("fancy"))
    {
        cam.samples_per_pixel = 128;
        cam.max_depth         = 32;
    }

    if (program.is_used("samples"))
        cam.samples_per_pixel = program.get<int>("samples");

    if (program.is_used("depth"))
        cam.max_depth = program.get<int>("depth");

    if (program.is_used("fov"))
        cam.vfov = program.get<int>("fov");

    if (program.is_used("frame"))
    {
        int    frame  = program.get<int>("frame");
        double period = 60.0;
        double t      = 2.0 * pi * frame / period;
        double x      = 13.0 * cos(t) + 0.1;
        double z      = 13.0 * sin(t) + 0.1;
        cam.lookfrom  = point3(x, 2, z);
    }

    // ========================================
    // DEBUGGING INFO
    // ========================================
    // SHOW(cam.samples_per_pixel);
    // SHOW(cam.max_depth);
    // SHOW(cam.lookfrom);

    // ========================================
    // DEFINE THE MATERIALS AND SPHERES
    // ========================================

    // auto material_ground = std::make_shared<lambertian>(color(0.1, 0.8, 0.2)); // green
    // auto material_center = std::make_shared<dielectric>(1.5);
    // auto material_left   = std::make_shared<dielectric>(1.5);

    // auto material_gold   = std::make_shared<metal>(color(0.8, 0.6, 0.2), 0.7);
    // auto material_red    = std::make_shared<lambertian>(color(0.7, 0.3, 0.3));
    // auto material_silver = std::make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);

    // world.add(std::make_shared<sphere>(point3(0.0, -100.5, -3.0), 100.0, material_ground)); // GROUND
    // world.add(std::make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_red));         // MIDDLE
    // world.add(std::make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.5, material_left));      // LEFT
    // world.add(std::make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_gold));        // RIGHT

    // for (int i = 0; i < 20; i++)
    // {
    //     double x   = utils::random_double_range(-7.0, 7.0);
    //     double y   = utils::random_double_range(0.0, 0.0);
    //     double z   = utils::random_double_range(-5.0, -2.0);
    //     auto   c   = color(utils::random_double(), utils::random_double(), utils::random_double());
    //     auto   mat = std::make_shared<lambertian>(c);
    //     world.add(std::make_shared<sphere>(point3(x, y, z), 0.25, mat));
    // }

    // ========================================
    // THE BOOKS VERSION OF THE WORLD
    // ========================================

    using namespace std;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            auto   choose_mat = utils::random_double();
            point3 center(a + 0.9 * utils::random_double(), 0.2, b + 0.9 * utils::random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9)
            {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8)
                {
                    // diffuse
                    auto albedo     = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95)
                {
                    // metal
                    auto albedo     = color::random(0.5, 1);
                    auto fuzz       = utils::random_double_range(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else
                {
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

    cam.render(world);
}