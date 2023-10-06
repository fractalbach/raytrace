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
        .help("Produces a more detailed image. Takes more time.")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("-r", "--randomize")
        .help("Seeds the RNG with the current time.")
        .default_value(false)
        .implicit_value(true);

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

    if (randomize)
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

    auto material_ground = std::make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<lambertian>(color(0.7, 0.3, 0.3));
    auto material_left   = std::make_shared<metal>(color(0.8, 0.8, 0.8));
    auto material_right  = std::make_shared<metal>(color(0.8, 0.6, 0.2));

    world.add(std::make_shared<sphere>(point3(0.0, -100.5, -3.0), 100.0, material_ground));
    world.add(std::make_shared<sphere>(point3(0.0, 0.0, -1.5), 0.5, material_center));
    world.add(std::make_shared<sphere>(point3(-1.0, 0.0, -1.5), 0.5, material_left));
    world.add(std::make_shared<sphere>(point3(1.0, 0.0, -1.5), 0.5, material_right));

    cam.samples_per_pixel = fancy ? 128 : 8;
    cam.max_depth         = fancy ? 64 : 8;

    // world.add(std::make_shared<sphere>(point3(0, 0, -1), 0.5));        // front-most
    // world.add(std::make_shared<sphere>(point3(1, 0, -2), 0.5));        // right sidekick
    // world.add(std::make_shared<sphere>(point3(-1, 0, -2), 0.5));       // left sidekick
    // world.add(std::make_shared<sphere>(point3(2, -0.25, -1.5), 0.25)); // smaller on the right
    // world.add(std::make_shared<sphere>(point3(-1, 2, -2.5), 0.5));     // floater
    // world.add(std::make_shared<sphere>(point3(0, -100.5, -1), 100)); // acts like a ground

    for (int i = 0; i < 20; i++)
    {
        double x   = utils::random_double_range(-7.0, 7.0);
        double y   = utils::random_double_range(0.0, 3.0);
        double z   = utils::random_double_range(-5.0, 1.0);
        auto   c   = color(utils::random_double(), utils::random_double(), utils::random_double());
        auto   mat = std::make_shared<lambertian>(c);
        world.add(std::make_shared<sphere>(point3(x, y, z), 0.5, mat));
    }

    cam.render(world);
}