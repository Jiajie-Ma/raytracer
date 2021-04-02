#ifndef MATERIAL_H
#define MATERIAL_H

#include <cmath>
#include "AGLM.h"
#include "ray.h"
#include "hittable.h"

class material {
public:
  virtual bool scatter(const ray& r_in, const hit_record& rec, 
     glm::color& attenuation, ray& scattered) const = 0;
  virtual ~material() {}
};

class lambertian : public material {
public:
  lambertian(const glm::color& a) : albedo(a) {}

  virtual bool scatter(const ray& r_in, const hit_record& rec, 
     glm::color& attenuation, ray& scattered) const override 
  {
     glm::vec3 unitn = normalize(rec.normal);
     glm::vec3 scatter_direction = unitn + random_unit_vector();
     if (near_zero(scatter_direction))
     {
        scatter_direction = unitn;
     }
     scattered = ray(rec.p, scatter_direction);
     attenuation = albedo;
     return true; //bounce!
  }

public:
  glm::color albedo;
};

class phong : public material {
public:
  phong(const glm::vec3& view) :
     diffuseColor(0,0,1), 
     specColor(1,1,1),
     ambientColor(.01f, .01f, .01f),
     lightPos(5,5,0),
     viewPos(view), 
     kd(0.45), ks(0.45), ka(0.1), shininess(10.0) 
  {}

  phong(const glm::color& idiffuseColor, 
        const glm::color& ispecColor,
        const glm::color& iambientColor,
        const glm::point3& ilightPos, 
        const glm::point3& iviewPos, 
        float ikd, float iks, float ika, float ishininess) : 
     diffuseColor(idiffuseColor), 
     specColor(ispecColor),
     ambientColor(iambientColor),
     lightPos(ilightPos),
     viewPos(iviewPos), kd(ikd), ks(iks), ka(ika), shininess(ishininess) 
  {}

  virtual bool scatter(const ray& r_in, const hit_record& rec, 
     glm::color& attenuation, ray& scattered) const override 
  {
     // ambient
     glm::color ambient = ka * ambientColor;

     // diffuse
     glm::vec3 unitn = normalize(rec.normal);
     glm::vec3 lightDir = normalize(lightPos - rec.p);
     glm::color diffuse = kd * max(glm::vec3(0), glm::dot(unitn, lightDir)) * diffuseColor;

     // specular
     glm::vec3 reflection = normalize(-glm::reflect(lightDir, unitn));
     glm::vec3 unitv = normalize(viewPos - rec.p);
     glm::color spec = ks * specColor * float(pow(glm::dot(unitv, reflection), shininess));

     attenuation = ambient + diffuse + spec;
     return false;
  }

public:
  glm::color diffuseColor;
  glm::color specColor;
  glm::color ambientColor;
  glm::point3 lightPos;
  glm::point3 viewPos; 
  float kd; 
  float ks;
  float ka; 
  float shininess;
};

class metal : public material {
public:
   metal(const glm::color& a, float f) : albedo(a), fuzz(glm::clamp(f,0.0f,1.0f)) {}

   virtual bool scatter(const ray& r_in, const hit_record& rec, 
      glm::color& attenuation, ray& scattered) const override 
   {
      glm::vec3 unitn = normalize(rec.normal);
      glm::vec3 scatter_direction = glm::reflect(normalize(r_in.direction()), unitn) + fuzz * random_unit_vector();
      scattered = ray(rec.p, scatter_direction);
      attenuation = albedo;
      return (glm::dot(scatter_direction, unitn) > 0); //bounce!
   }

public:
   glm::color albedo;
   float fuzz;
};

class dielectric : public material {
public:
  dielectric(float index_of_refraction) : ir(index_of_refraction) {}

  virtual bool scatter(const ray& r_in, const hit_record& rec, 
     glm::color& attenuation, ray& scattered) const override 
   {
      attenuation = glm::color(1.0, 1.0, 1.0);
      float refraction_ratio = rec.front_face ? (1.0/ir) : ir;

      glm::vec3 unit_direction = normalize(r_in.direction());
      glm::vec3 unitn = normalize(rec.normal);
      float cos_theta = fmin(dot(-unit_direction, unitn), 1.0);
      float sin_theta = sqrt(1.0 - cos_theta*cos_theta);

      bool cannot_refract = refraction_ratio * sin_theta > 1.0;
      glm::vec3 direction;

      if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_float())
      {
         direction = glm::reflect(unit_direction, unitn);
      }
      else{
         direction = glm::refract(unit_direction, unitn, refraction_ratio);
      }
      
      scattered = ray(rec.p, direction);
      return true;
   }

public:
  float ir; // Index of Refraction

private:
   static float reflectance(float cosine, float ref_idx) {
   // Use Schlick's approximation for reflectance.
   float r0 = (1-ref_idx) / (1+ref_idx);
   r0 = r0*r0;
   return r0 + (1-r0)*pow((1 - cosine),5);
   }
};

glm::vec3 refract(const glm::vec3& uv, const glm::vec3& n, float etai_over_etat) {
    float cos_theta = fmin(glm::dot(-uv, n), 1.0);
    glm::vec3 r_out_perp =  etai_over_etat * (uv + cos_theta*n);
    glm::vec3 r_out_parallel = float(-sqrt(fabs(1.0 - pow(glm::length(r_out_perp), 2)))) * n;
    return r_out_perp + r_out_parallel;
}


#endif

