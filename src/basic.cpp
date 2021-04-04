// Raytracer framework from https://raytracing.github.io by Peter Shirley, 2018-2020
// alinen 2021, modified to use glm and ppm_image class

#include "ppm_image.h"
#include "AGLM.h"
#include "ray.h"
#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "line.h"
#include "camera.h"
#include "material.h"
#include "hittable_list.h"

using namespace glm;
using namespace agl;
using namespace std;

// counterclockwise rotation of a vector by theta on the xz plane
glm::point3 xy_rotation(glm::point3& p, float theta)
{
   glm::point3 q;
   // apply the 2d rotation matrix
   q[0] = cos(theta) * p[0] - sin(theta) * p[1];
   q[1] = sin(theta) * p[0] + cos(theta) * p[1];
   q[2] = q[2];
   return q;
}

// translate a point p by a vector v
glm::point3 translation(glm::point3& p, glm::vec3 v)
{
   glm::point3 q;
   // apply the translation
   q[0] = p[0] + v[0];
   q[1] = p[1] + v[1];
   q[2] = p[2] + v[2];
   return q;
}

// rotate a point by an euler angle a
glm::point3 rotation(glm::point3& p, glm::vec3 a)
{
   glm::quat Q = glm::quat(a);
   glm::mat3 M = glm::toMat3(Q);
   return M*p;
}

// create a circle with center c and radius r in the scene
void circle(const glm::point3& c, float r, std::shared_ptr<material> m, glm::vec3 a, hittable_list& world)
{
   // angle of each slice
   float dtheta = 2*M_PI/static_cast<float>(10);
   // starting point
   glm::point3 p(r, 0, 0);

   for (int i =0; i < 10; i++)
   {
      // find the two points that define the current slice
      float theta1 = static_cast<float>(i) * dtheta;
      float theta2 = static_cast<float>(i+1) * dtheta;
      glm::point3 v1 = xy_rotation(p, theta1);
      glm::point3 v2 = xy_rotation(p, theta2);
      v1 = rotation(v1, a);
      v2 = rotation(v2, a);
      glm::point3 p1 = translation(v1, c);
      glm::point3 p2 = translation(v2, c);

      world.add(make_shared<triangle>(c, p1, p2, m));
   }
}

void tetrahedron(std::shared_ptr<material> m1, std::shared_ptr<material> m2, std::shared_ptr<material> m3, std::shared_ptr<material> m4, hittable_list& world)
{
   // tetrahedron in the unique image
   glm::point3 p1 = point3(13,-16,-20);
   glm::point3 p2 = point3(8,-28,-40);
   glm::point3 p3 = point3(30,-28,-40);
   glm::point3 p4 = point3(13, -10, -30);

   // tetrahedron demonstration
   /*glm::point3 p1 = point3(0,-0.8, -1);
   glm::point3 p2 = point3(2,-1,-2);
   glm::point3 p3 = point3(-2,-1,-2);
   glm::point3 p4 = point3(0,1,-1.5);*/

   world.add(make_shared<triangle>(p1, p2, p3, m1));
   world.add(make_shared<triangle>(p1, p2, p4, m1));
   world.add(make_shared<triangle>(p1, p3, p4, m1));
   world.add(make_shared<triangle>(p4, p2, p3, m1));
   
}

// create a planet in the scene
void planet(const glm::point3& c, float r, float d, std::shared_ptr<material> m1, std::shared_ptr<material> m2, glm::vec3 a, hittable_list& world)
{

   world.add(make_shared<sphere>(c, r, m1));
   circle(c, r+d, m2, a, world);

}

color ray_color(const ray& r, const hittable_list& world, int depth)
{
   hit_record rec;
   if (depth <= 0)
   {
      return color(0);
   }

   if (world.hit(r, 0.001f, infinity, rec))
   {
      ray scattered;
      color attenuation;
      if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
      {
         color recurseColor = ray_color(scattered, world, depth - 1);
         return attenuation * recurseColor;
      }
      return attenuation;
   }
   vec3 unit_direction = normalize(r.direction());
   //auto t = 0.5f * (unit_direction.y + 1.0f);
   //return (1.0f - t) * color(1,1,1) + t * color(0.5f, 0.7f, 1.0f);
   auto t = 0.5f * (unit_direction.y + 1.0f);
   return (1.0f - t) * color(1.0f/255.0f, 5.0f/255.0f, 14.0f/255.0f) + t * color(1.0f/255.0f, 5.0f/255.0f, 14.0f/255.0f);
}

color normalize_color(const color& c, int samples_per_pixel)
{
   // todo: implement me!
   float scale = 1.0f / samples_per_pixel;
   float r = std::min(0.999f, std::max(0.0f, c.r * scale));
   float g = std::min(0.999f, std::max(0.0f, c.g * scale));
   float b = std::min(0.999f, std::max(0.0f, c.b * scale));

   // apply gamma correction 
   r = sqrt(r);
   g = sqrt(g);
   b = sqrt(b);

   return color(r, g, b);
}

void ray_trace(ppm_image& image)
{
   // Image
   int height = image.height();
   int width = image.width();
   float aspect = width / float(height);
   int samples_per_pixel = 10; // higher => more anti-aliasing
   int max_depth = 10; // higher => less shadow acne

   // World
   vec3 camera_pos(0,0,0);
   shared_ptr<material> planetm = make_shared<lambertian>(color(237.0f/255.0f, 219.0f/255.0f, 173.0f/255.0f));
   shared_ptr<material> circlem = make_shared<lambertian>(color(252.0f/255.0f, 238.0f/255.0f, 173.0f/255.0f));
   shared_ptr<material> varus = make_shared<lambertian>(color(98.0f/255.0f, 174.0f/255.0f, 231.0f/255.0f));
   shared_ptr<material> yellow = make_shared<lambertian>(color(246.0f/255.0f, 255.0f/255.0f, 104.0f/255.0f));
   shared_ptr<material> gray = make_shared<lambertian>(color(140.0f/255.0f, 140.0f/255.0f, 148.0f/255.0f));
   
   
   shared_ptr<material> lwall = make_shared<phong>(color(49.0f/255.0f, 38.0f/255.0f, 96.0f/255.0f), 
     color(1,1,1),
     color(.01f, .01f, .01f),
     vec3(0,100,-1000),
     camera_pos, 
     0.45, 0.45, 0.1, 20.0);
   shared_ptr<material> rwall = make_shared<phong>(color(49.0f/255.0f, 38.0f/255.0f,96.0f/255.0f), 
     color(1,1,1),
     color(.01f, .01f, .01f),
     vec3(0,100,-1000),
     camera_pos, 
     0.45, 0.45, 0.1, 20.0);
     shared_ptr<material> floor = make_shared<phong>(color(88.0f/255.0f, 98.0f/255.0f,100.0f/255.0f), 
     color(1,1,1),
     color(.01f, .01f, .01f),
     vec3(0,03,-10000),
     camera_pos, 
     0.45, 0.45, 0.1, 20.0);
     shared_ptr<material> ceiling = make_shared<phong>(color(0.0f/255.0f, 0.0f/255.0f,0.0f/255.0f), 
     color(1,1,1),
     color(.01f, .01f, .01f),
     vec3(0,3,-10000),
     camera_pos, 
     0.45, 0.45, 0.1, 20.0);
   shared_ptr<material> glass = make_shared<dielectric>(1.5f);
   shared_ptr<material> metalBlue = make_shared<lambertian>(color(211.0f/255.0f, 236.0f/255.0f, 230.0f/255.0f));

   hittable_list world;
   world.add(make_shared<plane>(point3(0,-4,0), vec3(0,1,-0.6), floor));
   world.add(make_shared<plane>(point3(6,0,0), vec3(1,0,0.7), lwall));
   world.add(make_shared<plane>(point3(-6,0,0), vec3(1,0,-0.7), rwall));
   world.add(make_shared<plane>(point3(0,4,0), vec3(0,1,0.6), ceiling));

   world.add(make_shared<plane>(point3(0,0,-100), vec3(0,0,1), glass));
   planet(point3(0,0,-120), 20.0f, 20.0f, planetm, circlem, vec3(-0.45 * M_PI,0, 0.1 * M_PI), world);
   world.add(make_shared<sphere>(point3(-30, 30, -200), 3.0f, yellow));
   world.add(make_shared<sphere>(point3(25, 18, -120), 5.0f, gray));
   world.add(make_shared<sphere>(point3(30, -30, -160), 10.0f, varus));

   tetrahedron(metalBlue, metalBlue, metalBlue, metalBlue, world);

   // planet
   /*hittable_list world;
   shared_ptr<material> planetm = make_shared<lambertian>(color(0.5f));
   shared_ptr<material> circlem = make_shared<lambertian>(color(252.0f/255.0f, 238.0f/255.0f, 173.0f/255.0f));

   planet(point3(0,0,-3.0), 1.0f, 0.8f, planetm, circlem, vec3(-0.45 * M_PI,0, 0.1 * M_PI), world);*/

   // tetrahedron
   /*hittable_list world;
   shared_ptr<material> rwall = make_shared<phong>(color(49.0f/255.0f, 38.0f/255.0f,96.0f/255.0f), 
     color(1,1,1),
     color(.01f, .01f, .01f),
     vec3(1,0,0),
     camera_pos, 
     0.45, 0.45, 0.1, 20.0);
   shared_ptr<material> planetm = make_shared<lambertian>(color(0.5f));
   shared_ptr<material> circlem = make_shared<lambertian>(color(252.0f/255.0f, 238.0f/255.0f, 173.0f/255.0f));
   tetrahedron(rwall, rwall, rwall, rwall, world);*/
   
   
   //world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, gray));

   // Camera
   /*
   point3 lookfrom(-5,0,0);
   point3 lookat(20,0,-120);
   vec3 vup(0,1,0);
   auto dist_to_focus = (lookfrom-lookat).length();
   auto aperture = 0.0;
   camera cam(lookfrom, lookat, vup, 90, aspect, aperture, dist_to_focus);*/

   point3 lookfrom(0,0,0);
   point3 lookat(0,0,-120);
   vec3 vup(0,1,0);
   auto dist_to_focus = (lookfrom-lookat).length();
   auto aperture = 0.0;
   camera cam(lookfrom, lookat, vup, 90, aspect, aperture, dist_to_focus);


   // Ray trace
   for (int j = 0; j < height; j++)
   {
      for (int i = 0; i < width; i++)
      {
         color c(0, 0, 0);
         for (int s = 0; s < samples_per_pixel; s++) // antialias
         {
            float u = float(i + random_float()) / (width - 1);
            float v = float(height - j - 1 - random_float()) / (height - 1);

            ray r = cam.get_ray(u, v);
            c += ray_color(r, world, max_depth);
         }
         c = normalize_color(c, samples_per_pixel);
         image.set_vec3(j, i, c);
      }
   }

   image.save("basicblur.png");
}