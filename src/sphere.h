// ray.h, from https://raytracing.github.io by Peter Shirley, 2018-2020
#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "AGLM.h"

class sphere : public hittable {
public:
   sphere() : radius(1), center(0), mat_ptr(0) {}
   sphere(const glm::point3& cen, float r, std::shared_ptr<material> m) : 
      center(cen), radius(r), mat_ptr(m) {
         assert(radius > 0 && "The radius of a sphere cannot be 0!");
      };

   virtual bool hit(const ray& r, hit_record& rec) const override;

public:
   glm::point3 center;
   float radius;
   std::shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray& r, hit_record& rec) const {
   /* Analytic method
   glm::vec3 oc = r.origin() - center;
   float a = glm::dot(r.direction(), r.direction());
   float half_b = glm::dot(oc, r.direction());
   float c = glm::length2(oc) - radius*radius;

   float discriminant = half_b*half_b - a*c;
   if (discriminant < 0) return false;
   float sqrtd = sqrt(discriminant);

   float t = (-half_b - sqrtd) / a;
   if (t < 0) t = (-half_b + sqrtd) / a;
   if (t < 0) return false;

   // save relevant data in hit record
   rec.t = t; // save the time when we hit the object
   rec.p = r.at(t); // ray.origin + t * ray.direction
   rec.mat_ptr = mat_ptr; 

   // save normal
   glm::vec3 outward_normal = normalize(rec.p - center); // compute unit length normal
   rec.set_face_normal(r, outward_normal);

   return true;
   */

   // Geometric method starts from here
   glm::vec3 el = center - r.origin();
   float len = glm::length(r.direction());
   glm::vec3 d = r.direction()/len;

   float s = glm::dot(el, d);
   float elSqr = glm::dot(el, el);
   float rSqr = radius * radius;
   if (s < 0 && elSqr > rSqr) return false;

   float mSqr = elSqr - s * s;
   if (mSqr > rSqr) return false;

   float q = sqrt(rSqr - mSqr);
   float t;
   if (elSqr > rSqr)
   {
      t = s - q;
   }
   else{
      t = s + q;
   }
   t = t/len;

   // save relevant data in hit record
   rec.t = t; // save the time when we hit the object
   rec.p = r.at(t); // ray.origin + t * ray.direction
   rec.mat_ptr = mat_ptr; 

   // save normal
   glm::vec3 outward_normal = normalize(rec.p - center); // compute unit length normal
   rec.set_face_normal(r, outward_normal);

   return true;
}

#endif

