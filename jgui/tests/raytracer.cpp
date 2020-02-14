/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "jgui/japplication.h"
#include "jgui/jwindow.h"
#include "jmath/jvector.h"

#include <vector>
#include <cassert>

typedef jmath::jvector_t<3, double> v3f;
typedef jmath::jvector_t<4, double> v4f;

struct Light {
  v3f position;
  double intensity;
};

struct Material {
  double refractive_index {1.0f};
  v4f albedo {1.0f, 0.0f, 0.0f, 0.0f};
  v3f diffuse_color {0.0f, 0.0f, 0.0f};
  double specular_exponent {0.0f};
};

struct Sphere {
  v3f center;
  double radius;
  Material material;

  bool ray_intersect(const v3f &orig, const v3f &dir, double &t0) const 
  {
    v3f L = center - orig;
    double tca = L.Scalar(dir);
    double d2 = L.Scalar(L) - tca*tca;

    if (d2 > radius*radius) {
      return false;
    }

    double thc = sqrtf(radius*radius - d2);
    double t1 = tca + thc;
    
    t0 = tca - thc;

    if (t0 < 0) {
      t0 = t1;
    }

    if (t0 < 0) {
      return false;
    }
    
    return true;
  }
};

v3f reflect(const v3f &I, const v3f &N) 
{
  return I - N*2.f*(I.Scalar(N));
}

v3f refract(const v3f &I, const v3f &N, const double eta_t, const double eta_i=1.0f) // Snell's law
{ 
  double cosi = - std::max(-1.0, std::min(1.0, I.Scalar(N)));

  if (cosi<0) {
    return refract(I, -N, eta_i, eta_t); // if the ray comes from the inside the object, swap the air and the media
  }

  double eta = eta_i / eta_t;
  double k = 1 - eta*eta*(1 - cosi*cosi);
  
  // k<0 = total reflection, no ray to refract. I refract it anyways, this has no physical meaning
  return k<0 ? v3f{1.0f, 0.0f, 0.0f} : I*eta + N*(eta*cosi - sqrtf(k)); 
}

bool scene_intersect(const v3f &orig, const v3f &dir, const std::vector<Sphere> &spheres, v3f &hit, v3f &N, Material &material) 
{
  double spheres_dist = std::numeric_limits<double>::max();
  
  for (size_t i=0; i < spheres.size(); i++) {
    double dist_i;

    if (spheres[i].ray_intersect(orig, dir, dist_i) && dist_i < spheres_dist) {
      spheres_dist = dist_i;
      hit = orig + dir*dist_i;
      N = (hit - spheres[i].center).Normalize();
      material = spheres[i].material;
    }
  }

  double checkerboard_dist = std::numeric_limits<double>::max();

  if (fabs(dir[1])>1e-3) {
    double d = -(orig[1] + 4)/dir[1]; // the checkerboard plane has equation y = -4
    v3f pt = orig + dir*d;

    if (d > 0 && fabs(pt[0]) < 10 && pt[2] < -10 && pt[2] > -30 && d<spheres_dist) {
      checkerboard_dist = d;
      hit = pt;
      N = v3f{0,1,0};
      material.diffuse_color = (int(0.5f*hit[0] + 1000) + int(0.5f*hit[2])) & 1 ? v3f{0.3f, 0.3f, 0.3f} : v3f{0.3f, 0.2f, 0.1f};
    }
  }
  
  return std::min(spheres_dist, checkerboard_dist) < 1000;
}

v3f cast_ray(const v3f &orig, const v3f &dir, const std::vector<Sphere> &spheres, const std::vector<Light> &lights, size_t depth=0) 
{
  v3f point, N;
  Material material;

  if (depth>4 || !scene_intersect(orig, dir, spheres, point, N, material)) {
    return v3f {0.2, 0.7, 0.8}; // background color
  }

  v3f reflect_dir = (reflect(dir, N)).Normalize();
  v3f refract_dir = (refract(dir, N, material.refractive_index)).Normalize();
  v3f reflect_orig = reflect_dir.Scalar(N) < 0 ? point - N*1e-3 : point + N*1e-3; // offset the original point to avoid occlusion by the object itself
  v3f refract_orig = refract_dir.Scalar(N) < 0 ? point - N*1e-3 : point + N*1e-3;
  v3f reflect_color = cast_ray(reflect_orig, reflect_dir, spheres, lights, depth + 1);
  v3f refract_color = cast_ray(refract_orig, refract_dir, spheres, lights, depth + 1);

  double diffuse_light_intensity = 0, specular_light_intensity = 0;

  for (size_t i=0; i<lights.size(); i++) {
    v3f light_dir = (lights[i].position - point).Normalize();
    double light_distance = (lights[i].position - point).EuclidianNorm();

    v3f shadow_orig = light_dir.Scalar(N) < 0 ? point - N*1e-3 : point + N*1e-3; // checking if the point lies in the shadow of the lights[i]
    v3f shadow_pt, shadow_N;
    Material tmpmaterial;

    if (scene_intersect(shadow_orig, light_dir, spheres, shadow_pt, shadow_N, tmpmaterial) && (shadow_pt-shadow_orig).EuclidianNorm() < light_distance) {
      continue;
    }

    diffuse_light_intensity += lights[i].intensity * std::max(0.0, light_dir.Scalar(N));
    specular_light_intensity += powf(std::max(0.0, -reflect(-light_dir, N).Scalar(dir)), material.specular_exponent)*lights[i].intensity;
  }

  return material.diffuse_color * diffuse_light_intensity * material.albedo[0] + v3f{1., 1., 1.}*specular_light_intensity * material.albedo[1] + reflect_color*material.albedo[2] + refract_color*material.albedo[3];
}

class RayTracer : public jgui::Window {

  private:
    Material ivory {1.0f, v4f{0.6f, 0.3f, 0.1f, 0.0f}, v3f{0.4f, 0.4f, 0.3f}, 50.0f};
    Material glass {1.5f, v4f{0.0f, 0.5f, 0.1f, 0.8f}, v3f{0.6f, 0.7f, 0.8f}, 125.0f};
    Material red_rubber {1.0f, v4f{0.9f, 0.1f, 0.0f, 0.0f}, v3f{0.3f, 0.1f, 0.1f}, 10.0f};
    Material mirror {1.0f, v4f{0.0f, 10.0f, 0.8f, 0.0f}, v3f{1.0f, 1.0f, 1.0f}, 1425.0f};
    std::vector<Sphere> spheres;
    std::vector<Light> lights;

  public:
    RayTracer():
      jgui::Window({720, 480})
  {
    spheres.push_back(Sphere{v3f{-3.0f, 0.0f, -16.0f}, 2.0f, ivory});
    spheres.push_back(Sphere{v3f{-1.0f, -1.5f, -12.0f}, 2.0f, glass});
    spheres.push_back(Sphere{v3f{ 1.5f, -0.5f, -18.0f}, 3.0f, red_rubber});
    spheres.push_back(Sphere{v3f{ 7.0f, 5.0f, -18.0f}, 4.0f, mirror});

    lights.push_back(Light({v3f{-20.0f, 20.0f, 20.0f}, 1.5f}));
    lights.push_back(Light({v3f{ 30.0f, 50.0f, -25.0f}, 1.8f}));
    lights.push_back(Light({v3f{ 30.0f, 20.0f, 30.0f}, 1.7f}));
  }

    virtual ~RayTracer()
    {
    }

    void Paint(jgui::Graphics *g) 
    {
      jgui::jsize_t
        size = GetSize();

      const int width = size.width;
      const int height = size.height;
      const double fov = M_PI/3.0f;
      std::vector<v3f> framebuffer(width*height);

#pragma omp parallel for
      for (int j=0; j<height; j++) { // actual rendering loop
        for (int i=0; i<width; i++) {
          double dir_x = (i + 0.5f) - width/2.0f;
          double dir_y = -(j + 0.5f) + height/2.0f; // this flips the image at the same time
          double dir_z = -height/(2.0f*tan(fov/2.0f));
          
          framebuffer[i+j*width] = cast_ray(v3f{0.0f, 0.0f, 0.0f}, v3f{dir_x, dir_y, dir_z}.Normalize(), spheres, lights);
        }
      }

      for (int i = 0; i < height*width; ++i) {
        v3f &c = framebuffer[i];

        g->SetRGB(uint32_t(jgui::jcolor_t<double>{c[0], c[1], c[2], 1.0f}.Normalize()), {i%width, i/width});
      }
    }

};

int main(int argc, char **argv)
{
  jgui::Application::Init(argc, argv);

  RayTracer app;

  app.SetTitle("RayTracer");
  app.Exec();

  jgui::Application::Loop();

  return 0;
}
