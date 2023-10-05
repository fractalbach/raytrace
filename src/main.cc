#include "camera.h"
#include "hittable_list.h"
#include "sphere.h"

#include <fstream>
#include <iostream>
#include <memory>

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

int main()
{
    // utils::randomize(); // seed the randomizer with current time to get a different image each time

    hittable_list world; // the list of all objects in our world
    camera        cam;   // how we view this world

    // Turn this on to get an epic render that will take ages to produce
    bool fancy = false;

    cam.samples_per_pixel = fancy ? 128 : 8;
    cam.max_depth         = fancy ? 128 : 8;

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