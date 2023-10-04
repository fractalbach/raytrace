#ifndef CAMERA_H
#define CAMERA_h

#include "color.h"
#include "hittable.h"

class camera
{
public:
    double aspect_ratio = 16.0 / 9.0;
    int    image_width  = 400;

    void render(const hittable & world)
    {
        init();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; ++j)
        {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i)
            {
                auto pixel_center  = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
                auto ray_direction = pixel_center - center;

                ray   r(center, ray_direction);
                color pixel_color = ray_color(r, world);

                write_color(std::cout, pixel_color);
            }
        }
        std::clog << "\rDone.                    \n"; // Progress Indicator End
    }

private:
    int    image_height;  // Rendered image height
    point3 center;        // Camera center
    point3 pixel00_loc;   // Location of pixel 0, 0
    vec3   pixel_delta_u; // Offset to pixel to the right
    vec3   pixel_delta_v; // Offset to pixel below

    void init()
    {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height; // ensure the image height is at least 1

        center = point3(0, 0, 0);

        // Determine viewport dimensions.
        double focal_length    = 1.0;
        double viewport_height = 2.0;
        double viewport_width  = viewport_height * (static_cast<double>(image_width) / image_height);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        vec3 viewport_u = vec3(viewport_width, 0, 0);
        vec3 viewport_v = vec3(0, -viewport_height, 0);

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        vec3 viewport_upper_left = center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc              = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    // ray_color will directly give a color output for a single raycast.
    color ray_color(const ray & r, const hittable & world) const
    {
        // Check if the ray hit the object
        hit_record rec;

        if (world.hit(r, interval(0, infinity), rec))
        {
            return 0.5 * (rec.normal + color(1, 1, 1));
        }

        // Gradiant blue sky background
        vec3   u = unit_vector(r.direction()); // unit vector of our ray
        double a = 0.5 * (u.y() + 1.0);        // a is the intensity of the color
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }
};

#endif