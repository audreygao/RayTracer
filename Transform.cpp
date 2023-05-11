// Transform.cpp: implementation of the Transform class.

// Note: when you construct a matrix using mat4() or mat3(), it will be COLUMN-MAJOR
// Keep this in mind in readfile.cpp and display.cpp
// See FAQ for more details or if you're having problems.

#include "Transform.h"

// Helper rotation function.  Please implement this.  
mat3 Transform::rotate(const float degrees, const vec3& axis) 
{
  mat3 ret;

    float rad = glm::radians(degrees);
    vec3 a = normalize(axis);
    ret = (cos(rad) * mat3(1) + (1-cos(rad)) * glm::outerProduct(a, a) +
           sin(rad) * mat3(0, a[2], -a[1], -a[2], 0, a[0], a[1], -a[0], 0));
  return ret;
}

void Transform::left(float degrees, vec3& eye, vec3& up) 
{
    mat3 rotM = rotate(degrees, up);
    eye = rotM * eye;
}

void Transform::up(float degrees, vec3& eye, vec3& up) 
{

    vec3 ax = cross(eye, up);
    mat3 rotM = rotate(degrees, ax);
    eye = rotM * eye;
    up = sqrt(up[0]*up[0] + up[1]*up[1] + up[2]*up[2]) * normalize(cross(ax,eye));
}

mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) 
{
  mat4 ret;

    vec3 c = normalize(eye);
    vec3 b = normalize(up);
    vec3 a = cross(b,c);
    ret = mat4(a[0], b[0], c[0], 0, a[1], b[1], c[1], 0, a[2], b[2], c[2], 0,
               dot(-a, eye), dot(-b, eye), dot(-c, eye), 1);
  return ret;
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
  mat4 ret;

    float rad = 0.5 * glm::radians(fovy);
    float a = 1/(aspect * glm::tan(rad));
    float c = 0 - (zFar + zNear)/(zFar - zNear);
    float d = 0 - (2 * zFar * zNear)/(zFar - zNear);
    mat4 m = mat4(a, 0, 0, 0,
                  0, 1/(glm::tan(rad)), 0, 0,
                  0, 0, c, d,
                  0, 0, -1, 0);
    ret = glm::transpose(m);
  return ret;
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) 
{
  mat4 ret;

    ret = mat4(sx, 0, 0, 0,
               0, sy, 0, 0,
               0, 0, sz, 0,
               0, 0, 0, 1);
  return ret;
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) 
{
  mat4 ret;
  // YOUR CODE FOR HW2 HERE
    mat4 m = mat4(1,0,0,tx,
                  0,1,0,ty,
                  0,0,1,tz,
                  0,0,0,1);
    ret = glm::transpose(m);
  return ret;
}

// To normalize the up direction and construct a coordinate frame.  
// As discussed in the lecture.  May be relevant to create a properly 
// orthogonal and normalized up. 
// This function is provided as a helper, in case you want to use it. 
// Using this function (in readfile.cpp or display.cpp) is optional.  

vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) 
{
  vec3 x = glm::cross(up,zvec); 
  vec3 y = glm::cross(zvec,x); 
  vec3 ret = glm::normalize(y); 
  return ret; 
}


Transform::Transform()
{

}

Transform::~Transform()
{

}
