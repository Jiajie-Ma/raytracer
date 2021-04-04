#ifndef PLANE_H_
#define PLANE_H_

#include "hittable.h"
#include "AGLM.h"

class plane : public hittable {
public:
   plane() : a(0), n(glm::vec3(1,0,0)), mat_ptr(0) {}
   plane(const glm::point3& p, const glm::vec3& normal, 
      std::shared_ptr<material> m) : a(p), n(normal), mat_ptr(m) {
         assert(glm::length(n) > 0 && "The normal vector of a plane cannot be 0!");
         // make sure the normal follows right hand rule, i.e. it always points left or outside the screen
         if (n[0] > 0 || n[1] < 0 || n[2] < 0)
         {
            n = -n;
         }
      };

   virtual bool hit(const ray& r, hit_record& rec) const override
   {
      // necessary dot product computations 
      float d = glm::dot(r.direction(), n);
      glm::vec3 u = a - r.origin();
      float q = glm::dot(u, n);

      // the ray is parallel with the plane and it is not on the plane
      if (near_zero(d) && !near_zero(q)) return false;
      float t;

      // the ray is inside the plane
      if (near_zero(d) && near_zero(q))
      {
         t = 0;
      }
      else{
         // the ray is not parallel with the plane. Then we compute t using the formula with dot product
         t = q/d;
         // the ray points away from the plane if t is negative
         if (t < 0)
         {
            return false;
         }
      }

      // save relevant data in hit record
      rec.t = t; // save the time when we hit the object
      rec.p = r.at(t); // ray.origin + t * ray.direction
      rec.mat_ptr = mat_ptr; 
      
      // save normal
      glm::vec3 outward_normal = normalize(n); // compute unit length normal
      rec.set_face_normal(r, outward_normal);
      
      return true;
   }

public:
   glm::vec3 a;
   glm::vec3 n;
   std::shared_ptr<material> mat_ptr;
};

#endif
