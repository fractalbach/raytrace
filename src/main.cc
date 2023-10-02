#include "color.h"
#include "ray.h"
#include "vec3.h"

#include <fstream>
#include <iostream>

// SHOW macro prints a variable's name and then its value
#define SHOW(a) std::clog << #a << ": " << (a) << std::endl;

// given a sphere in 3d space, and a raycast, does this ray hit the sphere?
// returns a time t where the ray hits the sphere, but if the there's no hit, then the second pair returns false.
std::pair<double, bool> hit_sphere(const point3 & center, double radius, const ray & r)
{
    vec3   oc = r.origin() - center;
    double a  = dot(r.direction(), r.direction());
    double b  = 2.0 * dot(oc, r.direction());
    double c  = dot(oc, oc) - radius * radius;
    double D  = b * b - 4 * a * c; // discriminant
    if (D < 0)
    {
        return std::make_pair(-1.0, false); // the ray did not hit the sphere
    }
    else
    {
        double t = (-b - sqrt(D)) / (2.0 * a); // where the ray first contacts the sphere
        return std::make_pair(t, true);        // sphere was hit, so return a value and a true
    }
}

// ray_color will directly give a color output for a single raycast.
color ray_color(const ray & r)
{
    // Check if the ray hit the sphere
    auto   result = hit_sphere(point3(0, 0, -1), 0.5, r);
    double t      = result.first;
    bool   ok     = result.second;

    // If the sphere was hit, draw the normal vector
    if (ok)
    {
        vec3 n = unit_vector(r.at(t) - vec3(0, 0, -1));      // calculate the normal
        return 0.5 * color(n.x() + 1, n.y() + 1, n.z() + 1); // make it look fancy
    }

    // Gradiant blue sky background
    vec3   u = unit_vector(r.direction()); // unit vector of our ray
    double a = 0.5 * (u.y() + 1.0);        // a is the intensity of the color
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

void render(std::ostream & out)
{
    double aspect_ratio = 16.0 / 9.0;
    int    image_width  = 400;
    int    image_height = static_cast<int>(image_width / aspect_ratio);

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

    SHOW(viewport_height);
    SHOW(viewport_width);

    // A viewport is like a window into the 3d world. It exists in the 3d world, but has a grid of pixels, just like our
    // output image. We map the viewport (u,v) -> image (x,y).

    // vectors that represent to the viewport edges (right anddown)
    vec3 viewport_u = vec3(viewport_width, 0, 0);
    vec3 viewport_v = vec3(0, -viewport_height, 0);

    SHOW(viewport_u);
    SHOW(viewport_v);

    // calculate horizontal andvertical delta vectors from pixel to pixel
    vec3 pixel_delta_u = viewport_u / image_width;
    vec3 pixel_delta_v = viewport_v / image_height;

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