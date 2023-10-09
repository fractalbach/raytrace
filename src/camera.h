#ifndef CAMERA_H
#define CAMERA_h

#include "color.h"
#include "hittable.h"
#include "material.h"
#include "utils.h"

class camera
{
  public:
    double aspect_ratio      = 16.0 / 9.0;       // Ratio of image width over height
    int    image_width       = 400;              // Rendered image width in pixel count
    int    samples_per_pixel = 16;               // Count of random samples used for antialiasing
    int    max_depth         = 8;                // maximum number of ray bounces allowed per raycast
    double vfov              = 90;               // Vertical view angle (field of view)
    point3 lookfrom          = point3(0, 0, -1); // Point camera is looking from
    point3 lookat            = point3(0, 0, 0);  // Point camera is looking at
    vec3   vup               = vec3(0, 1, 0);    // Camera-relative "up" direction
    double defocus_angle     = 0;                // Variation angle of rays through each pixel
    double focus_dist        = 10;               // Distance from camera lookfrom point to plane of perfect focus

    void render(const hittable & world)
    {
        init();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; ++j)
        {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i)
            {
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
    int    image_height;   // Rendered image height
    point3 center;         // Camera center
    point3 pixel00_loc;    // Location of pixel 0, 0
    vec3   pixel_delta_u;  // Offset to pixel to the right
    vec3   pixel_delta_v;  // Offset to pixel below
    vec3   u, v, w;        // Camera frame basis vectors
    vec3   defocus_disk_u; // Defocus disk horizontal radius
    vec3   defocus_disk_v; // Defocus disk vertical radius

    void init()
    {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height; // ensure the image height is at least 1

        center = lookfrom;

        // Determine viewport dimensions.
        auto   theta           = utils::degrees_to_radians(vfov);
        auto   h               = tan(theta / 2);
        auto   viewport_height = 2 * h * focus_dist;
        double viewport_width  = viewport_height * (static_cast<double>(image_width) / image_height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        vec3 viewport_u = viewport_width * u;   // Vector across viewport horizontal edge
        vec3 viewport_v = viewport_height * -v; // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc              = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * tan(utils::degrees_to_radians(defocus_angle / 2));
        defocus_disk_u      = u * defocus_radius;
        defocus_disk_v      = v * defocus_radius;
    }

    // ray_color will directly give a color output for a single raycast.
    color ray_color(const ray & r, int depth, const hittable & world) const
    {
        // break out if we've maxed our recursion depth
        if (depth <= 0)
            return color(0, 0, 0);

        // Check if the ray hit the object
        hit_record rec;

        if (world.hit(r, interval(0.001, infinity), rec))
        {
            ray   scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_color(scattered, depth - 1, world);
            return color(0, 0, 0);

            // // DIFFUSION
            // vec3 direction = rec.normal + random_unit_vector();
            // // custom hack to aded nice colors
            // return 0.5 * (rec.normal + color(1, 1, 1)) * ray_color(ray(rec.p, direction), depth - 1, world);
            // return 0.5 * ray_color(ray(rec.p, direction), depth - 1, world);

            // COLOR-CODE BY NORMAL VECTOR
            // return 0.5 * (rec.normal + color(1, 1, 1));
        }

        // Gradiant blue sky background
        vec3   u = unit_vector(r.direction()); // unit vector of our ray
        double a = 0.5 * (u.y() + 1.0);        // a is the intensity of the color
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }

    // Get a randomly-sampled camera ray for the pixel at location i,j, originating from the camera defocus disk.
    ray get_ray(int i, int j) const
    {
        auto pixel_center  = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto pixel_sample  = pixel_center + pixel_sample_square();
        auto ray_origin    = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;
        return ray(ray_origin, ray_direction);
    }

    vec3 pixel_sample_square() const
    {
        auto px = -0.5 + utils::random_double();
        auto py = -0.5 + utils::random_double();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    // Returns a random point in the camera defocus disk.
    point3 defocus_disk_sample() const
    {
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }
};

#endif