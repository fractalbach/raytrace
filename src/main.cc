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

    hittable_list world; // the list of all objects in our world
    camera        cam;   // how we view this world

    world.add(std::make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(std::make_shared<sphere>(point3(1, 0, -2), 0.5));
    world.add(std::make_shared<sphere>(point3(-1, 0, -2), 0.5));
    world.add(std::make_shared<sphere>(point3(-1, 2, -2.5), 0.5));

    world.add(std::make_shared<sphere>(point3(0, -100.5, -1), 100)); // acts like a ground

    cam.render(world);
}