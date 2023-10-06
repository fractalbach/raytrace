#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "material.h"
#include "vec3.h"

#include <cmath>
#include <memory>

class sphere : public hittable
{
public:
    sphere(point3 _center, double _radius, std::shared_ptr<material> _material)
        : center(_center), radius(_radius), mat(_material)
    {
    }

    bool hit(const ray & r, interval ray_t, hit_record & rec) const override
    {
        vec3 oc     = r.origin() - center;
        auto a      = r.direction().length_squared();
        auto half_b = dot(oc, r.direction());
        auto c      = oc.length_squared() - radius * radius;

        auto D = half_b * half_b - a * c;

        if (D < 0)
        {
            return false;
        }

        auto sqrtd = std::sqrt(D);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-half_b - sqrtd) / a;
        if (!ray_t.surrounds(root))
        {
            root = (-half_b + sqrtd) / a;
            if (!ray_t.surrounds(root))
            {
                return false;
            }
        }

        rec.t   = root;
        rec.p   = r.at(rec.t);
        rec.mat = mat;

        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);

        return true;
    }

private:
    point3                    center;
    double                    radius;
    std::shared_ptr<material> mat;
};

#endif