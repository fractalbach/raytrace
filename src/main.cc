#include "color.h"
#include "ray.h"
#include "vec3.h"

#include <fstream>
#include <iostream>

color ray_color(const ray & r)
{
    vec3   u = unit_vector(r.direction());
    double a = 0.5 * (u.y() + 1.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

void render(std::ostream & out)
{
    static const double aspect_ratio = 16.0 / 9.0;
    static const int    image_width  = 400;
    static const int    image_height = static_cast<int>(image_width / aspect_ratio);

    std::clog << "aspect_ratio: " << aspect_ratio << std::endl;
    std::clog << "image_width : " << image_width << std::endl;
    std::clog << "image_height: " << image_height << std::endl;

    // ensures that image_height is at least 1
    // image_height                     = (image_height < 1) ? 1 : image_height;

    // Camera
    double focal_length    = 1.0;
    double viewport_height = 2.0;
    double viewport_width  = viewport_height * (static_cast<double>(image_width) / image_height);
    point3 camera_center   = point3(0, 0, 0);

    // vectors that represent to the viewport edges (right and down)
    vec3 viewport_u = vec3(viewport_width, 0, 0);
    vec3 viewport_v = vec3(0, -viewport_height, 0);

    // calculate horizontal andvertical delta vectors from pixel to pixel
    vec3 pixel_delta_u = viewport_u / image_width;
    vec3 pixel_delta_v = viewport_u / image_height;

    // calculate location of upper-left pixel
    vec3 viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    vec3 pixel00_loc         = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // print the PPM image header
    out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    // print the pixels
    for (int j = 0; j < image_height; ++j)
    {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i)
        {
            auto pixel_center  = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;

            ray   r(camera_center, ray_direction);
            color pixel_color = ray_color(r);

            write_color(out, pixel_color);
        }
    }
    std::clog << "\rDone.                    \n"; // Progress Indicator End
}

int write_file()
{
    static const char * filepath_output_image = "out/test.ppm";
    std::ofstream       f;
    f.open(filepath_output_image, std::ios::out | std::ios::trunc);
    if (!f.is_open())
    {
        std::cerr << "Unable to open file" << std::endl;
        return 1;
    }
    render(f);
    f.close();
    return 0;
}

int main()
{
    render(std::cout);
    return 0;
}