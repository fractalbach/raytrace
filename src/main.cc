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
        .default_value(90)
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

    bool fancy     = program.get<bool>("fancy");
    bool randomize = program.get<bool>("randomize");
    bool seed_used = program.is_used("seed");
    int  fov       = program.get<int>("fov");
    int  samples   = program.get<int>("samples");
    int  depth     = program.get<int>("depth");

    if (seed_used)
    {
        unsigned int seed = program.get<unsigned int>("seed");
        std::clog << "Enabled: Custom RNG seed provided: " << seed << std::endl;
        utils::randomize(seed);
    }
    else if (randomize)
    {
        std::clog << "Enabled: Randomize the RNG seed" << std::endl;
        utils::randomize(); // seed the randomizer with current time to get a different image each time
    }

    if (fancy)
    {
        std::clog << "Enabled: Fancy output" << std::endl;
    }

    // ========================================
    // RENDER THE WORLD
    // ========================================

    hittable_list world; // the list of all objects in our world
    camera        cam;   // how we view this world

    cam.samples_per_pixel = fancy ? 128 : 16;
    cam.max_depth         = fancy ? 32 : 8;

    if (program.is_used("samples"))
        cam.samples_per_pixel = samples;

    if (program.is_used("depth"))
        cam.max_depth = depth;

    cam.vfov     = fov;
    cam.lookfrom = point3(-2, 2, 1);
    cam.lookat   = point3(0, 0, -1);
    cam.vup      = vec3(0, 1, 0);

    auto material_ground = std::make_shared<lambertian>(color(0.1, 0.8, 0.2)); // green
    auto material_center = std::make_shared<dielectric>(1.5);
    auto material_left   = std::make_shared<dielectric>(1.5);

    auto material_gold   = std::make_shared<metal>(color(0.8, 0.6, 0.2), 0.7);
    auto material_red    = std::make_shared<lambertian>(color(0.7, 0.3, 0.3));
    auto material_silver = std::make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);

    world.add(std::make_shared<sphere>(point3(0.0, -100.5, -3.0), 100.0, material_ground)); // GROUND
    world.add(std::make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_red));         // MIDDLE
    world.add(std::make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.5, material_left));      // LEFT
    world.add(std::make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_gold));        // RIGHT

    for (int i = 0; i < 20; i++)
    {
        double x   = utils::random_double_range(-7.0, 7.0);
        double y   = utils::random_double_range(0.0, 0.0);
        double z   = utils::random_double_range(-5.0, -2.0);
        auto   c   = color(utils::random_double(), utils::random_double(), utils::random_double());
        auto   mat = std::make_shared<lambertian>(c);
        world.add(std::make_shared<sphere>(point3(x, y, z), 0.25, mat));
    }

    using namespace std;

    // Copy of Book's world for testing

    // auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    // auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    // auto material_left   = make_shared<dielectric>(1.5);
    // auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);
    // world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
    // world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
    // world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
    // world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.4, material_left));
    // world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

    cam.render(world);
}