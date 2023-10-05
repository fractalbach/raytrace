#include "camera.h"
#include "hittable_list.h"
#include "sphere.h"
#include "third_party/argparse.hpp"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

// SHOW macro prints a variable's name and then its value
#define SHOW(a) std::clog << #a << ": " << (a) << std::endl;

// int write_file()
// {
//     static const char * filepath_output_image = "out/test.ppm";
//     std::ofstream       f;
//     f.open(filepath_output_image, std::ios::out | std::ios::trunc);
//     if (!f.is_open())
//     {
//         std::cerr << "Unable to open file" << std::endl;
//         return 1;
//     }
//     render(f);
//     f.close();
//     return 0;
// }

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

    cam.samples_per_pixel = fancy ? 128 : 8;
    cam.max_depth         = fancy ? 64 : 8;

    world.add(std::make_shared<sphere>(point3(0, 0, -1), 0.5));        // front-most
    world.add(std::make_shared<sphere>(point3(1, 0, -2), 0.5));        // right sidekick
    world.add(std::make_shared<sphere>(point3(-1, 0, -2), 0.5));       // left sidekick
    world.add(std::make_shared<sphere>(point3(2, -0.25, -1.5), 0.25)); // smaller on the right
    world.add(std::make_shared<sphere>(point3(-1, 2, -2.5), 0.5));     // floater

    for (int i = 0; i < 20; i++)
    {
        double x = utils::random_double_range(-7.0, 7.0);
        double y = utils::random_double_range(0.0, 5.0);
        double z = utils::random_double_range(-15.0, -1.0);
        world.add(std::make_shared<sphere>(point3(x, y, z), 0.5));
    }

    world.add(std::make_shared<sphere>(point3(0, -100.5, -1), 100)); // acts like a ground

    cam.render(world);
}