// ray.h, from https://raytracing.github.io by Peter Shirley, 2018-2020
#ifndef LINE_H
#define LINE_H

#include "hittable.h"
#include "AGLM.h"

class line : public hittable {
public:
   line() : a(0), b(1,0,0), normal(0,1,0), mat_ptr(0) {}
   line(const glm::point3& v0, const glm::point3& v1,
      std::shared_ptr<material> m) : a(v0), b(v1), mat_ptr(m) {
          // make sure the line is not degenerated as a point
          assert(a == b && "The endpoints of a line cannot be the same!");
          // let the normal be the cross product with (1,0,0). If the line is parallal with (1,0,0), let the normal be (0,1,0)
          if (near_zero(glm::cross(b-a, glm::vec3(1,0,0))))
          {
              normal = glm::vec3(0,1,0);
          }
          else{
              normal = glm::cross(b-a, glm::vec3(1,0,0));
          }
      };

   virtual bool hit(const ray& r, hit_record& rec) const override;

public:
   glm::point3 a;
   glm::point3 b;
   glm::vec3 normal;
   std::shared_ptr<material> mat_ptr;
};

bool line::hit(const ray& r, hit_record& rec) const {
    // annotations beside the variables are consistent with the notations done on a scratch paper
    // find the plane containing r.orgin(), a and b
    glm::vec3 v1 = b - a; // r
    glm::vec3 v2 = r.origin() - a; // q-p
    glm::vec3 n = glm::cross(v2, v1); // (q-p) * r

    // make sure a, b, r.origin() are not colinear. We will handle the colinear case below.
    if (!near_zero(n)){
        // check if another point of the ray is also on the plane. If not, then the ray does not hit the line
        glm::vec3 p = r.origin() + r.direction() * 42.0f;
        if (!near_zero(glm::dot(a-p, n))){
            return false;
        }
    }

    // at this point, we know that ab and r are on a 2d plane. Compute the necessary cross products to proceed
    glm::vec3 c1 = glm::cross(v1, r.direction()); // r * s
    glm::vec3 c2 = glm::cross(v2, r.direction()); // (q-p) * s

    float t;
    // check if r and ab are colinear
    if (near_zero(c1) && near_zero(n))
    {
        // check if the r.origin() is on ab
        float u = v2[0]/v1[0];
        if (0.0 <= u <= 1.0)
        {
            // if so, let t = 0
            t = 0;
        }
        else{
            // otherwise, check if the ray hits either a or b
            float t0 = -a[0]/r.direction()[0];
            float t1 = -b[0]/r.direction()[0];

            if (0.0 <= t0 <= 1.0 || 0.0 <= t1 <= 1.0)
            {
                if (t0 < t1)
                {
                    t = t0;
                }
                else{
                    t = t1;
                }
            }
            else{
                return false;
            }
        }
    }
    else if (near_zero(c1))
    {
        // in this case, r is parallel with ab and they do not intersect
        return false;
    }
    else
    {
        // otherwise, r is not parallel with ab
        float u = c2[0]/c1[0]; // (q-p)*s/r*s
        t = n[0]/c1[0]; // (q-p)*r/r*s

        // check if r misses ab
        if (t < 0 || u < 0 || u > 1)
        {
            return false;
        }
    }
    
    // save relevant data in hit record
    rec.t = t; // save the time when we hit the object
    rec.p = r.at(t); // ray.origin + t * ray.direction
    rec.mat_ptr = mat_ptr; 

    // save normal
    glm::vec3 outward_normal = normalize(normal); // compute unit length normal
    rec.set_face_normal(r, outward_normal);

    return true;
}

#endif

