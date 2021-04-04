#ifndef TRIANGLE_H_
#define TRIANGLE_H_


#include "hittable.h"
#include "AGLM.h"
#include "line.h"

class triangle : public hittable {
public:
   triangle() : a(glm::point3(1,0,0)), b(glm::point3(0,1,0)), c(glm::point3(0,0,1)), mat_ptr(0) {}
   triangle(const glm::point3& v0, const glm::point3& v1, const glm::point3& v2, 
      std::shared_ptr<material> m) : a(v0), b(v1), c(v2), mat_ptr(m) {
         assert(!near_zero(glm::cross(b - a, c - a)) && "The three vertices of a triangle cannot be  colinear!");
      };

   virtual bool hit(const ray& r, hit_record& rec) const override
   {
      // find the supporting plane of the triangle
      glm::vec3 ab = b - a;
      glm::vec3 ac = c - a;
      glm::vec3 bc = c - b;
      glm::vec3 n = glm::normalize(glm::cross(ab, ac));
      plane p (a, n, mat_ptr);

      // check if the ray hits the supporting plane. If so, save the relevant data to hit_record. Otherwise, return false
      if (!p.hit(r, rec))
      {
         return false;
      }

      float t = 0;
      // check if the ray hits the triangle. We break into two cases.
      // If t = 0; i.e. the origin of the ray is on the supporting plane
      if (near_zero(rec.t))
      {
         // check if the r.origin() is inside the triangle
         float u = glm::dot(glm::cross(ab, r.origin()-a), n);
         float v = glm::dot(glm::cross(bc, r.origin()-b), n);
         float w = glm::dot(glm::cross(-ac, r.origin()-c), n);

         if (u >= 0 && v >= 0 && w >= 0)
         {
            t = 0;
         }
         else{
            // if the origin is not inside the triangle, check if it hits an edge of the triangle
            line l1(a, b, 0);
            line l2(b, c, 0);
            line l3(a, c ,0);

            if (l1.hit(r, rec))
            {
               t = rec.t;
            }
            if (l2.hit(r, rec) && (rec.t < t || t == 0))
            {
               t = rec.t;
            }
            if (l3.hit(r, rec) && (rec.t < t || t == 0))
            {
               t = rec.t;
            }

            // if it does not hit an edge of the triangle, t remains to be 0 (otherwise, t > 0 since the origin is not inside the triangle)
            if (t == 0)
            {
               return false;
            }
         }
      }
      else{
         // let q be the intersection of r and p
         glm::vec3 q = r.origin() + r.direction() * rec.t;

         // check if the intersection is inside the triangle
         float u = glm::dot(glm::cross(ab, q-a), n);
         float v = glm::dot(glm::cross(bc, q-b), n);
         float w = glm::dot(glm::cross(-ac, q-c), n);

         // check if q is inside the triangle
         if (u >= 0 && v >= 0 && w >= 0)
         {
            t = rec.t;
         }
         else{
            return false;
         }
      }

      // save relevant data in hit record
      rec.t = t; // save the time when we hit the object
      rec.p = r.at(t); // ray.origin + t * ray.direction
      rec.mat_ptr = mat_ptr; 

      // save normal
      // make sure the normal follows right hand rule, i.e. it always points left or outside the screen
      if (n[0] > 0 || n[1] < 0 || n[2] < 0)
      {
         n = -n;
      }
      glm::vec3 outward_normal = normalize(n); // compute unit length normal
      rec.set_face_normal(r, outward_normal);

      return true;

/*
      // method 2
      glm::vec3 e1 = b - a;
      glm::vec3 e2 = c - a;
      glm::vec3 e3 = c - b;
      glm::vec3 q = glm::cross(r.direction(), e2);
      float d = glm::dot(e1, q);
      float t;

      if (near_zero(d))
      {
         glm::vec3 n = glm::normalize(glm::cross(e1, e2));
         float w1 = glm::dot(glm::cross(e1, r.origin()-a), n);
         float w2 = glm::dot(glm::cross(e3, r.origin()-b), n);
         float w3 = glm::dot(glm::cross(-e2, r.origin()-c), n);

         if (w1 >= 0 && w2 >= 0 && w3 >= 0)
         {
            t = 0;
         }
         else{
            return false;
         }
      }
      else{
         float f = 1.0f/d;
         glm::vec3 s = r.origin() - a;
         float u = f * (glm::dot(s, q));

         if (u < 0.0){
            std::cout << u << std::endl;
            return false;
         }

         glm::vec3 r1 = glm::cross(s, e1);
         float v = f * (glm::dot(r.direction(), r1));

         if (v < 0.0 || u + v > 1.0){
            std::cout << v << std::endl;
            return false;
         }

         t = f * (glm::dot(e2, r1));
         if (t < 0.0)
         {
            return false;
         }
      }
      
      // save relevant data in hit record
      rec.t = t; // save the time when we hit the object
      rec.p = r.at(t); // ray.origin + t * ray.direction
      rec.mat_ptr = mat_ptr; 

      glm::vec3 ab = b - a;
      glm::vec3 ac = c - a;
      glm::vec3 n = glm::normalize(glm::cross(ab, ac));

      // save normal
      // make sure the normal follows right hand rule, i.e. it always points left or outside the screen
      if (n[0] > 0 || n[1] < 0 || n[2] < 0)
      {
         n = -n;
      }
      glm::vec3 outward_normal = normalize(n); // compute unit length normal
      rec.set_face_normal(r, outward_normal);

      return true;
*/
   }

public:
   glm::point3 a;
   glm::point3 b;
   glm::point3 c;
   std::shared_ptr<material> mat_ptr;
};

#endif
