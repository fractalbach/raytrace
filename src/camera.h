#ifndef CAMERA_H
#define CAMERA_h

#include "color.h"
#include "hittable.h"
#include "utils.h"

class camera
{
public:
    double aspect_ratio      = 16.0 / 9.0; // Ratio of image width over height
    int    image_width       = 400;        // Rendered image width in pixel count
    int    samples_per_pixel = 16;         // Count of random samples used for antialiasing
    int    max_depth         = 8;          // maximum number of ray bounces allowed per raycast

    void render(const hittable & world)
    {
        init();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; ++j)
        {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i)
            {
                // Do 4 samples per pixel by dividing evenly
                // color pixel_color(0, 0, 0);
                // for (int sample = 0; sample < 4; sample++)
                // {
                //     ray r = get_ray_4sample(i, j, sample);
                //     pixel_color += ray_color(r, world);
                // }
                // write_color(std::cout, pixel_color, 4);

                // Do random sampling based on the number of samples
                color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; sample++)
                {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                write_color(std::cout, pixel_color, samples_per_pixel);
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
    color ray_color(const ray & r, int depth, const hittable & world) const
    {
        // break out if we've maxed our recursion depth
        if (depth <= 0)
        {
            return color(0, 0, 0);
        }

        // Check if the ray hit the object
        hit_record rec;

        if (world.hit(r, interval(0.001, infinity), rec))
        {
            // DIFFUSION
            vec3 direction = rec.normal + random_unit_vector();

            // custom neat hack: combines the normal vector coloring with the diffusion
            if (depth == max_depth)
                return 0.8 * (rec.normal + color(1, 1, 1)) * ray_color(ray(rec.p, direction), depth - 1, world);

            return 0.5 * ray_color(ray(rec.p, direction), depth - 1, world);

            // COLOR-CODE BY NORMAL VECTOR
            // return 0.5 * (rec.normal + color(1, 1, 1));
        }

        // Gradiant blue sky background
        vec3   u = unit_vector(r.direction()); // unit vector of our ray
        double a = 0.5 * (u.y() + 1.0);        // a is the intensity of the color
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }

    // returns a randomly adjusted ray within the bounds of the given pixel
    ray get_ray(int i, int j) const
    {
        auto pixel_center  = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto pixel_sample  = pixel_center + pixel_sample_square();
        auto ray_origin    = center;
        auto ray_direction = pixel_sample - ray_origin;
        return ray(ray_origin, ray_direction);
    }

    ray get_ray_4sample(int i, int j, int sample) const
    {
        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);

        auto offset_x = -0.5;
        auto offset_y = -0.5;
        if (sample == 1)
        {
            offset_x = 0.5;
        }
        else if (sample == 2)
        {
            offset_y = 0.5;
        }
        else if (sample == 3)
        {
            offset_x = 0.5;
            offset_y = 0.5;
        }
        auto offset_vec    = (offset_x * pixel_delta_u) + (offset_y * pixel_delta_v);
        auto pixel_sample  = pixel_center + offset_vec;
        auto ray_origin    = center;
        auto ray_direction = pixel_sample - ray_origin;
        return ray(ray_origin, ray_direction);
    }

    vec3 pixel_sample_square() const
    {
        auto px = -0.5 + utils::random_double();
        auto py = -0.5 + utils::random_double();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }
};

#endif