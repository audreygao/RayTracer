/******************************************************************************/
/* This is the program skeleton for homework 2 in CSE167 by Ravi Ramamoorthi  */
/* Extends HW 1 to deal with shading, more transforms and multiple objects    */
/******************************************************************************/

// You shouldn't have to edit this file at all!

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <chrono>

#include "Transform.h"
#include <FreeImage.h>

#include <algorithm>
#include <math.h>


using namespace std; 

// Main variables in the program.  
#define MAINPROGRAM 
#include "variable.h"
#include "readfile.h" // prototypes for readfile.cpp  

void deleteAll() {
  for(auto li: lights) delete li;
  for(auto obj: objects) delete obj;
}

Ray * rayThruPixel(int i, int j) {
  float alpha = glm::tan(camera.fovx / 2) * ( ((j+0.5)-(float)(w/2)) / (float)(w/2) );
  float beta = glm::tan(camera.fovy / 2) * ( ((i+0.5)-(float)(h/2)) / (float)(h/2) );
  vec3 d = glm::normalize(alpha * camera.u + beta * camera.v - camera.w);
  return new Ray(camera.pos, d);
}

bool intersectTriangle(Ray ray, Triangle * tri, float & dist) {
  mat4 m(tri->Vtx[0].x, tri->Vtx[0].y, tri->Vtx[0].z, 1.f, 
        tri->Vtx[1].x, tri->Vtx[1].y, tri->Vtx[1].z, 1.f,
        tri->Vtx[2].x, tri->Vtx[2].y, tri->Vtx[2].z, 1.f,
        (-1.f)*ray.dir.x, (-1.f)*ray.dir.y, (-1.f)*ray.dir.z, 0.f);
  mat4 m2 = glm::inverse(m);
  vec4 v(ray.pos, 1);
  vec4 vec = m2 * v;
  if(vec.x >= 0 && vec.y >= 0 && vec.z >= 0 && vec.w >= 0) {
    dist = vec.w;
    return true;
  }
  return false;
}

float magnitudeSq(glm::vec3 v) {
  return v.x * v.x + v.y * v.y + v.z * v.z;;
}

bool intersectSphere(Ray ray, sphere * sp, float & dist) {
  float a = magnitudeSq(ray.dir);
  float b = 2 * glm::dot(ray.dir, ray.pos - sp->center);
  float c = magnitudeSq(ray.pos - sp->center) - sp->radius * sp->radius;
  float discriminate = b * b - 4 * a * c;

  //don't intersect
  if(discriminate < 0) {return false;}

  //one root
  if(discriminate == 0) {
    dist = (-1 * b) / (2 * a);
    return true;
  }

  //two roots
  float t1, t2;
  t1 = (-1 * b + sqrt(discriminate)) / (2 * a);
  t2 = (-1 * b - sqrt(discriminate)) / (2 * a);
  if(t1 < 0 && t2 < 0) return false;
  else if(t1 < 0 && t2 >= 0) dist = t2;
  else if(t1 >=0 && t2 < 0) dist = t1;
  else if(t1 >= 0 && t2 >= 0) dist = std::min(t1, t2);
  return true;
}

bool intersect(Ray ray, Intersection &i) {
  float mindist = 1e10;
  object * hitobject;
  float dist;
  glm::vec3 normal;

  for(auto * obj : objects) {
    if(obj->type == shapeType::sphere) {
      sphere * sp = (sphere *)obj;

      //transform the ray to the model space
      glm::vec4 scr = glm::inverse(sp->transform) * glm::vec4(ray.pos, 1.f);
      glm::vec3 direction = glm::mat3(glm::inverse(sp->transform)) * ray.dir;
      Ray tRay = Ray(glm::vec3(scr), direction);

      //intersect ray and sphere in model space
      if(intersectSphere(tRay, sp, dist)) {
        vec3 tPos = tRay.pos + dist * tRay.dir;
        vec3 newPos = glm::vec3(sp->transform * glm::vec4(tPos, 1.f));
        glm::vec3 tNormal = glm::normalize(tPos - sp->center);
        dist = glm::dot(newPos - ray.pos, ray.dir);

        if(dist < mindist) {
          mindist = dist;
          hitobject = obj;
          normal = glm::normalize(glm::inverse(glm::transpose(glm::mat3(sp->transform))) * tNormal);
        }
      } 
    } else {
      Triangle * tri = (Triangle *)obj;
      if(intersectTriangle(ray, tri, dist)) {
        if(dist < mindist) {
          mindist = dist;
          hitobject = obj;
          //normal = glm::normalize(glm::cross(tri->Vtx[1] - tri->Vtx[0], tri->Vtx[2] - tri->Vtx[0]));
          normal = tri->theNormal;
        }
      } 
    }
  }
  if(mindist < 1e10) {
    i.HitDistance = mindist;
    i.Mtl = hitobject->Mtl;
    i.Position = ray.pos + (mindist-0.02f) * ray.dir;
    i.Normal = normal;
  return true;
  } 
  
  return false;
}


Color computeLight(vec3 d, Ray& toLight, light li, Intersection &i) {
  float nDotL = glm::dot(i.Normal, toLight.dir);
  Color lambert = i.Mtl->diffuse * std::max(nDotL, 0.f) ;

  vec3 dDir = glm::normalize(toLight.dir - d);
  float nDotD = glm::dot(i.Normal, dDir) ;
  Color phong = i.Mtl->specular * pow(std::max(nDotD, 0.0f), i.Mtl->shininess) ;

  float distToLightRt = toLight.dir.x * toLight.dir.x + toLight.dir.y * toLight.dir.y + toLight.dir.z * toLight.dir.z;
  float atten = 1.f / (li.attenuation[0] + li.attenuation[1] * sqrt(distToLightRt) + li.attenuation[2] * distToLightRt);

  return li.color * atten * (lambert + phong);
}

void RaytraceRecursive(Ray &ray, int depth, int maxRecursionDepth, Color &color) {
  if(depth > maxRecursionDepth) return;
  Intersection * i = new Intersection();

  if(intersect(ray, *i) == false) return;
    color = (i->Mtl->ambient + i->Mtl->emission);

  for(auto li: lights) {
    glm::vec3 dirVec;
    if(li->type==lightType::point) dirVec = glm::normalize(glm::vec3(li->posDir - i->Position));
    else dirVec = glm::normalize(li->posDir);
    Ray toLight = Ray(i->Position, dirVec);
    Intersection * blockLight = new Intersection();
    bool hit = intersect(toLight, *blockLight);

    //determine if point light is blocked by  object
    float objDist = blockLight->HitDistance;
    bool block = hit;
    if(li->type==lightType::point && hit) {
      float lightDist = sqrt(magnitudeSq(li->posDir - i->Position));
      if(objDist < lightDist) {
        block = true;
      } else block = false;
    }
    if(!block) {
      color = color + computeLight(ray.dir, toLight, *li, *i);      
    }
  }

  //recursively raytrace the reflected ray
  glm::vec3 negDir = ray.dir * -1.f;
      float f = 2.f * glm::dot(negDir, i->Normal);
      glm::vec3 reflectDir = ray.dir + f * glm::normalize(i->Normal);
      Ray reflectedRay(i->Position, reflectDir);
      Color reflectedColor = Color(0,0,0);
      RaytraceRecursive(reflectedRay, depth+1, maxRecursionDepth, reflectedColor);
      color = color + reflectedColor * i->Mtl->specular;
  
}

void Raytrace(int width, int height){ 
  int pix = width * height;
  BYTE *pixels = new BYTE[3*pix];	
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

  #pragma omp parallel for collapse(2)
  for (int i=0; i<height; i++) {
    for (int j=0; j<width; j++){

      Ray * ray = rayThruPixel(i, j);
      Color color = Color(0,0,0);
      RaytraceRecursive(*ray, 1, maxDepth, color);
      int index = i*width + j;
      pixels[index * 3] = 255 * color.b;
      pixels[index * 3 + 1] = 255 * color.g;
      pixels[index * 3 + 2] = 255 * color.r;
      
      //progress bar (messed up with openMP)
      int percent = 100 * (index) / (pix)+1;
      printf("\r%d%% done.", percent);
    }
  }
  printf("\nDone setting all pixels. %s generated.\n", imageName.c_str());
  FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, width, height, width * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);

  FreeImage_Save(FIF_PNG, img, imageName.c_str(), 0);
  delete[] pixels;

  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  std::cout << "Time elapsed = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]" << std::endl;
}

int main(int argc, char* argv[]) {

  if (argc < 2) {
    cerr << "Usage: transforms testfile\n"; 
    exit(-1); 
  }

  readfile(argv[1]) ;

  FreeImage_Initialise();
  Raytrace(w, h);
  FreeImage_DeInitialise();

  deleteAll();
  return 0;
}
