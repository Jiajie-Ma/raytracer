// camera.h, from https://raytracing.github.io by Peter Shirley, 2018-2020
// modified 2021, by alinen, to support glm/float

#ifndef CAMERA_H
#define CAMERA_H

#include "AGLM.h"

class camera 
{
public:
   camera() : origin(0), horizontal(2, 0, 0), vertical(0, 2, 0)
   {
      lower_left_corner = origin - horizontal * 0.5f - vertical * 0.5f - glm::vec3(0,0,1);
   }

   camera(glm::point3 pos, float viewport_height, float aspect_ratio, float focal_length) 
   {
      origin = pos;
      float viewport_width = aspect_ratio * viewport_height;
      horizontal = viewport_width * glm::vec3(1, 0, 0);
      vertical = viewport_height * glm::vec3(0, 1, 0);
      lower_left_corner = origin - horizontal * 0.5f - vertical * 0.5f - glm::vec3(0,0,focal_length);
   }

   camera(glm::point3 lookfrom,
          glm::point3 lookat,
          glm::vec3   vup,
          float vfov, // vertical field-of-view in degrees
          float aspect_ratio,
          float aperture,
          float focus_dist) 
   {
      // todo
      float theta = (2*M_PI/360.0) * vfov ;
      float h = tan(theta/2);
      float viewport_height = 2.0f * h;
      float viewport_width = aspect_ratio * viewport_height;

      w = normalize(lookfrom - lookat);
      u = normalize(glm::cross(vup, w));
      v = glm::cross(w, u);

      origin = lookfrom;
      horizontal = focus_dist * viewport_width * u;
      vertical = focus_dist * viewport_height * v;
      lower_left_corner = origin - horizontal * 0.5f - vertical * 0.5f - focus_dist*w;

      lens_radius = aperture * 0.5f;
   }

   virtual ray get_ray(float s, float t) const 
   {
      glm::vec3 rd = lens_radius * random_unit_disk();
      glm::vec3 offset = u * rd[0] + v * rd[1];

      return ray(
         origin + offset,
         lower_left_corner + s*horizontal + t*vertical - origin - offset
      );
   }

protected:
  glm::point3 origin;
  glm::point3 lower_left_corner;
  glm::vec3 horizontal;
  glm::vec3 vertical;
  glm::vec3 u,v,w;
  float lens_radius;
};
#endif

glm::vec3 random_in_unit_disk() {
    while (true) {
        glm::vec3 p = glm::vec3(random_float(-1,1), random_float(-1,1), 0);
        if (pow(glm::length(p),2) >= 1) continue;
        return p;
    }
}

