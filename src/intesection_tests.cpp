#include <cassert>
#include "AGLM.h"
#include "material.h"
#include "ray.h"
#include "sphere.h"
#include "box.h"
#include "plane.h"
#include "triangle.h"
#include "line.h"
#include "hittable.h"

using namespace glm;
using namespace std;

float eps = 0.0001f; // epsilon for comparing floats

template <class T>
bool vecEquals(T a, T b)
{
   return all(epsilonEqual(a, b, eps));
}

template <class T>
bool equals(T a, T b)
{
   return abs<T>(a - b) < eps;
}

void check(bool val, const std::string& message, const hit_record& hit, const ray& ray)
{
   if (!val)
   {
      cout << message << endl;
      cout << hit.str() << endl;
      cout << ray.str() << endl;
   }
   assert(val);
}

void test_sphere(const sphere& s, const ray& r, bool hits, const hit_record& desired) {
   hit_record hit;
   bool result = s.hit(r, hit);

   check(result == hits, "error: ray should/shouldn't hit", hit, r);
   if (hits) {
      check(vecEquals(hit.p, desired.p), "error: position incorrect:", hit, r);
      check(vecEquals(hit.normal, desired.normal), "error: normal incorrect:", hit, r);
      check(equals(hit.t, desired.t), "error: hit time incorrect", hit, r);
      check(hit.front_face == desired.front_face, "error: front facing incorrect", hit, r);
   }
}

void test_plane(const plane& p, const ray& r, bool hits, const hit_record& desired) {
   hit_record hit;
   bool result = p.hit(r, hit);

   check(result == hits, "error: ray should/shouldn't hit", hit, r);
   if (hits) {
      check(vecEquals(hit.p, desired.p), "error: position incorrect:", hit, r);
      check(vecEquals(hit.normal, desired.normal), "error: normal incorrect:", hit, r);
      check(equals(hit.t, desired.t), "error: hit time incorrect", hit, r);
      check(hit.front_face == desired.front_face, "error: front facing incorrect", hit, r);
   }
}

void test_triangle(const triangle& T, const ray& r, bool hits, const hit_record& desired) {
   hit_record hit;
   bool result = T.hit(r, hit);

   check(result == hits, "error: ray should/shouldn't hit", hit, r);
   if (hits) {
      check(vecEquals(hit.p, desired.p), "error: position incorrect:", hit, r);
      check(vecEquals(hit.normal, desired.normal), "error: normal incorrect:", hit, r);
      check(equals(hit.t, desired.t), "error: hit time incorrect", hit, r);
      check(hit.front_face == desired.front_face, "error: front facing incorrect", hit, r);
   }
}

void test_line(const line& l, const ray& r, bool hits, const hit_record& desired) {
   hit_record hit;
   bool result = l.hit(r, hit);

   check(result == hits, "error: ray should/shouldn't hit", hit, r);
   if (hits) {
      check(vecEquals(hit.p, desired.p), "error: position incorrect:", hit, r);
      check(vecEquals(hit.normal, desired.normal), "error: normal incorrect:", hit, r);
      check(equals(hit.t, desired.t), "error: hit time incorrect", hit, r);
      check(hit.front_face == desired.front_face, "error: front facing incorrect", hit, r);
   }
}

int main(int argc, char** argv)
{
   shared_ptr<material> empty = 0; 
   hit_record none = hit_record{ point3(0), point3(0), -1.0f, false, empty};

   sphere s(point3(0), 2.0f, empty);
   test_sphere(s, 
               ray(point3(0, 0, 3), vec3(0, 0, -1)), // ray outside/towards sphere
               true, 
               hit_record{vec3(0,0,2), vec3(0,0,1), 1.0, true, empty}); 

   test_sphere(s, 
               ray(point3(0, 0, 0), vec3(0, 0, -1)), // ray inside sphere
               true, 
               hit_record{ vec3(0,0,-2), vec3(0,0,1), 2.0, false, empty}); 

   test_sphere(s, 
               ray(point3(0, 0, 3), vec3(0, 0, 1)), // ray outside/away sphere
               false, 
               none); 

   test_sphere(s, 
               ray(point3(0, 0, 3), vec3(0, 5,-1)), // ray outside/towards sphere (miss)
               false, 
               none); 

   test_sphere(s, 
               ray(point3(0, 0, 3), vec3(0, 1,-3)), // ray outside/towards sphere (hit)
               true, 
               hit_record{vec3(0,0.3432f, 1.9703f), vec3(0,0.1716f, 0.9851f), 0.3432f, true, empty}); 

   // TODO: Your tests here
   // plane intersection tests (the direction of the normal follows the right-hand rule). Note that if a ray points towards the 
   // plane, it must hit the plane, so we have essentially 3 distinct cases.
   plane p(point3(0), vec3(0,3,0), empty);
   test_plane(p, 
               ray(point3(0, 2, 3), vec3(0, -2, 0)), // ray outside/towards sphere
               true, 
               hit_record{vec3(0,0,3), vec3(0,1,0), 1.0, true, empty}); 

   test_plane(p, 
               ray(point3(2, 0, 3), vec3(0, 6, 0)), // the origin of the ray is inside the sphere
               true, 
               hit_record{ vec3(2,0,3), vec3(0,-1,0), 0, false, empty}); 

   test_plane(p, 
               ray(point3(6, -8, 4), vec3(0, 2, 0)), // ray outside/towards sphere from the other hyperplane
               true, 
               hit_record{ vec3(6,0,4), vec3(0,-1,0), 4.0, false, empty}); 

   test_plane(p, 
               ray(point3(6, 0, 6), vec3(2, 0, 9)), // ray entirely inside the plane
               true, 
               hit_record{ vec3(6,0,6), vec3(0,-1,0), 0, false, empty}); 

   test_plane(p, 
               ray(point3(0, 2, 3), vec3(2, 0, 3)), // ray parallel to the plane
               false, 
               none); 

   test_plane(p, 
               ray(point3(1, -0.0001, 4), vec3(1, 0, 1)), // ray parallel to the plane
               false, 
               none); 
   
   // triangle intersection tests (the direction of the normal follows the right-hand rule).
   triangle T(point3(0,1,0), point3(0,0,1), point3(0,0,-1), empty);

   test_triangle(T,
                  ray(point3(0,1,0), vec3(2,4,6)), // the origin of the ray is on a vertex
                  true,
                  hit_record{ vec3(0,1,0), vec3(-1,0,0), 0, true, empty});

   test_triangle(T,
                  ray(point3(0,0.5,-0.5), vec3(-6,7,8)), // the origin of the ray is on an edge
                  true,
                  hit_record{ vec3(0,0.5,-0.5), vec3(1,0,0), 0, false, empty});

   test_triangle(T,
                  ray(point3(0,0.5,0), vec3(-6,7,8)), // the origin of the ray is inside the interior
                  true,
                  hit_record{ vec3(0,0.5,0), vec3(1,0,0), 0, false, empty});

   test_triangle(T,
                  ray(point3(0,2,0), vec3(-6,7,8)), // the origin of the ray is in the supporting plane but misses the triangle
                  false,
                  none);

   test_triangle(T,
                  ray(point3(0,0.5,0), vec3(0,7,8)), // the ray is entirely in the supporting plane and the origin is inside the interior
                  true,
                  hit_record{ vec3(0,0.5,0), vec3(1,0,0), 0, false, empty});

   test_triangle(T,
                  ray(point3(0,0.5,-0.5), vec3(0,-6,-6)), // the ray is entirely in the supporting plane and the origin is on an edge
                  true,
                  hit_record{ vec3(0,0.5,-0.5), vec3(1,0,0), 0, false, empty});

   test_triangle(T,
                  ray(point3(0,2,0), vec3(0,3,0)), // the ray is entirely in the supporting plane and the origin is outside the triangle. It misses the trianlge
                  false,
                  none);

   test_triangle(T,
                  ray(point3(0,6,0), vec3(0,1,100)), // the ray is entirely in the supporting plane and the origin is outside the triangle. It misses the trianlge
                  false,
                  none);

   test_triangle(T,
                  ray(point3(0,0,2), vec3(0,0,-0.5)), // the ray is entirely in the supporting plane and the origin is outside the triangle. It hits the triangle
                  true,
                  hit_record{ vec3(0,0,1), vec3(1,0,0), 2.0, false, empty});

   test_triangle(T,
                  ray(point3(0,1,1), vec3(0,-1,-1)), // the ray is entirely in the supporting plane and the origin is outside the triangle. It hits the trianlge
                  true,
                  hit_record{ vec3(0,0.5,0.5), vec3(1,0,0), 0.5f, false, empty});

   test_triangle(T,
                  ray(point3(2,1,1), vec3(1,1,1)), // the ray is outside the support plane and it points away from the triangle
                  false,
                  none);

   test_triangle(T,
                  ray(point3(6,0.5,0.5), vec3(-1,0,0)), // the ray is outside the support plane and it hits the triangle
                  true,
                  hit_record{ vec3(0,0.5,0.5), vec3(1,0,0), 6.0, false, empty});

   test_triangle(T,
                  ray(point3(-2,1,0), vec3(2,-0.5,0)), // the ray is outside the support plane and it hits the triangle
                  true,
                  hit_record{ vec3(0,0.5,0), vec3(-1,0,0), 1.0, true, empty});

   test_triangle(T,
                  ray(point3(2,1,1), vec3(-6,0,0)), // the ray is outside the support plane. It point towards the triangle but misses
                  false,
                  none);

   test_triangle(T,
                  ray(point3(-2,1,0), vec3(6,10,0)), // the ray is outside the support plane. It point towards the triangle but misses
                  false,
                  none);

   // line intersection tests
   line l(point3(0), point3(1,0,0), empty);
   line l2(point3(0,1,-1), point3(3,1,-1), empty);

   test_line(l, 
               ray(point3(0), vec3(1,0,1)), // the origin of the ray is on the line
               true,
               hit_record{ vec3(0,0,0), vec3(0,0,-1), 0, false, empty}
            );

   test_line(l, 
               ray(point3(0.5,0,0), vec3(0,6,0)), // the origin of the ray is on the line
               true,
               hit_record{ vec3(0.5,0,0), vec3(0,-1,0), 0, false, empty}
            );

   test_line(l, 
               ray(point3(0.5,0,0), vec3(6.6,0,0)), // the ray is colinear with the line and it hits the line
               true,
               hit_record{ vec3(0.5,0,0), vec3(0,0,-1), 0, false, empty}
            );

   test_line(l, 
               ray(point3(0,0,0), vec3(-6.6,0,0)), // the ray is colinear with the line and it hits the line
               true,
               hit_record{ vec3(0,0,0), vec3(0,0,-1), 0, false, empty}
            );

   test_line(l, 
               ray(point3(-1,0,0), vec3(0.5,0,0)), // the ray is colinear with the line and it hits the line
               true,
               hit_record{ vec3(0,0,0), vec3(0,0,-1), 2.0, false, empty}
            );

   test_line(l, 
               ray(point3(3,0,0), vec3(-0.5,0,0)), // the ray is colinear with the line and it hits the line
               true,
               hit_record{ vec3(1,0,0), vec3(0,0,-1), 4.0, false, empty}
            );

   test_line(l, 
               ray(point3(6,0,0), vec3(42,0,0)), // the ray is colinear with the line but it misses the line
               false,
               none
            );

   test_line(l, 
               ray(point3(-9,0,0), vec3(-6,0,0)), // the ray is colinear with the line but it misses the line
               false,
               none
            );

   test_line(l, 
               ray(point3(4,6,10), vec3(2,5,9)), // the ray is outside the line and it points away from the line
               false,
               none
            );

   test_line(l, 
               ray(point3(6,-2,-5), vec3(0,-42,0)), // the ray is outside the line and it points away from the line
               false,
               none
            );

   test_line(l, 
               ray(point3(0,1,0), vec3(0.5,-0.5,0)), // the ray is outside the line and it hits the line
               true,
               hit_record{ vec3(1,0,0), vec3(0,1,0), 2.0, false, empty}
            );

   test_line(l, 
               ray(point3(0.5,-3,-3), vec3(0,1,1)), // the ray is outside the line and it hits the line
               true,
               hit_record{ vec3(0.5,0,0), normalize(vec3(0,-1,-1)), 3.0, false, empty}
            );

   test_line(l, 
               ray(point3(0,6,0), vec3(-6,0,10)), // the ray is outside the line. It points towards the line but misses it
               false,
               none
            );
   
   test_line(l, 
               ray(point3(0,-2,-6), vec3(0,10,3)), // the ray is outside the line. It points towards the line but misses it
               false,
               none
            );
}
