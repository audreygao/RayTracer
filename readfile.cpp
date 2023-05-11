/*****************************************************************************/
// This file is readfile.cpp.  It includes helper functions for matrix 
// transformations for a stack (matransform) and to rightmultiply the 
// top of a stack.  These functions are given to aid in setting up the 
// transformations properly, and to use glm functions in the right way.  
// Their use is optional in your program.  


// The functions readvals and readfile do basic parsing.
// Basic includes to get this file to work.  
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include "Transform.h" 

using namespace std;
#include "variable.h" 
#include "readfile.h"
#include <cstring>
#include <string.h>

// You may not need to use the following two functions, but it is provided
// here for convenience

// The function below applies the appropriate transform to a 4-vector
void matransform(stack<mat4> &transfstack, float* values) 
{
  mat4 transform = transfstack.top(); 
  vec4 valvec = vec4(values[0],values[1],values[2],values[3]); 
  vec4 newval = transform * valvec; 
  for (int i = 0; i < 4; i++) values[i] = newval[i]; 
}

void rightmultiply(const mat4 & M, stack<mat4> &transfstack) 
{
  mat4 &T = transfstack.top(); 
  T = T * M; 
}

// Function to read the input data values
// Use is optional, but should be very helpful in parsing.  
bool readvals(stringstream &s, const int numvals, float* values) 
{
  for (int i = 0; i < numvals; i++) {
    s >> values[i]; 
    if (s.fail()) {
      cout << "Failed reading value " << i << " will skip\n"; 
      return false;
    }
  }
  return true; 
}

void readfile(const char* filename) 
{

  

  string str, cmd; 
  ifstream in;
  in.open(filename); 
  if (in.is_open()) {

    // need to implement a matrix stack to store transforms.  
    // This is done using standard STL Templates 
    stack <mat4> transfstack; 
    transfstack.push(mat4(1.0));  // identity

    int begin = 0;
    cameraCount = 0;

    //default values;
    imageName = "raytrace.png"; //default image file name
    maxDepth = defaultDepth; //default to 5
    attenuation = glm::vec3(1,0,0); 
    ambient = glm::vec3(0.2, 0.2, 0.2);

    getline (in, str); 
    while (in) {
      if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) {
        // Ruled out comment and blank lines 

        begin++;
        stringstream s(str);
        s >> cmd; 
        int i; 
        float values[10]; // Position and color for light, colors for others
        // Up to 10 params for cameras.  
        bool validinput; // Validity of input 

        //first command needs to be size
        if(begin==1) {
          if(cmd == "size") {
            validinput = readvals(s, 2, values); 
            if (validinput) {
              w = (int)values[0];
              h = (int)values[1];
            }
          } else {
            cerr << "Picture size needs to be first command.\n";
            break;
          }
        }
        else if(cmd == "maxdepth") {
          validinput = readvals(s, 1, values); 
            if (validinput) {
              maxDepth = values[0];
            }
        }
        else if(cmd == "output") {
          s >> imageName;  
        }

        // Process the light, add it to database.
        // Lighting Command
        else if (cmd == "directional" || cmd == "point") {
          validinput = readvals(s, 6, values); 
            if (validinput) {
              //transform the light
              vec3 p = glm::vec3(values[0], values[1], values[2]);
              vec4 tLight = transfstack.top() * glm::vec4(p, 1.f);

              Color c(values[3], values[4], values[5]);
              light * li = new light(vec3(tLight), c, attenuation.x, attenuation.y, attenuation.z);
              if(cmd=="directional") {li->type = lightType::directional;}
              else {li->type = lightType::point;}
              lights.push_back(li);
            }
        }
        else if(cmd == "attenuation") {
          validinput = readvals(s, 3, values); 
            if (validinput) {
              attenuation = glm::vec3(values[0], values[1], values[2]);
            }
        }

        else if (cmd == "ambient") {
          validinput = readvals(s, 3, values); // colors 
          if (validinput) {
            // for (i = 0; i < 3; i++) {
            //   ambient[i] = values[i]; 
            // }
            ambient = glm::vec3(values[0], values[1], values[2]);
          }
        } else if (cmd == "diffuse") {
          validinput = readvals(s, 3, values); 
          if (validinput) {
            diffuse = Color(values[0], values[1],values[2]);

          }
        } else if (cmd == "specular") {
          validinput = readvals(s, 3, values); 
          if (validinput) {
            specular = Color(values[0], values[1],values[2]);
          }
        } else if (cmd == "emission") {
          validinput = readvals(s, 3, values); 
          if (validinput) {
            emission = Color(values[0], values[1],values[2]);
          }
        } else if (cmd == "shininess") {
          validinput = readvals(s, 1, values); 
          if (validinput) {
            shininess = values[0]; 
          }
        } else if (cmd == "camera") {
          if(cameraCount < 1) {
            validinput = readvals(s,10,values); // 10 values eye cen up fov
            if (validinput) {

                glm::vec3 eyeinit = vec3(values[0], values[1],values[2]); //lookfrom
                glm::vec3 center = vec3(values[3], values[4], values[5]); //lookat
                vec3 v = vec3(values[6], values[7], values[8]); //up
                //glm::vec3 upinit = Transform::upvector(v, eyeinit);
                glm::vec3 upinit = glm::normalize(v);

                camera.pos = eyeinit;
                camera.w = glm::normalize(eyeinit - center);
                camera.u = glm::normalize(glm::cross(upinit, camera.w));
                camera.v = glm::normalize(glm::cross(camera.w, camera.u));
                float aspect = (float)w/h;
                camera.fovy = glm::radians(values[9]);
                camera.fovx = 2 * glm::atan(glm::tan(camera.fovy/2)*aspect);
            }
          } else {
            cerr << "Too many cameras\n";
          }
        }
        else if (cmd == "maxverts" || cmd == "maxvertsnorms") {
          // validinput = readvals(s, 1, values); 
          // if (validinput) {
          //   if(cmd == "maxverts") {maxVert = values[0];}
          //   else {maxVertNorm = values[0];}
          // }
        }
        else if (cmd == "vertex") {
          validinput = readvals(s, 3, values); 
          if (validinput) {
            vtxList.push_back(glm::vec3(values[0], values[1], values[2]));
          }
        }
        else if (cmd == "vertexnormal") {
          validinput = readvals(s, 6, values); 
          if (validinput) {
            normVtxList.push_back(glm::vec3(values[0], values[1], values[2]));
            normList.push_back(glm::vec3(values[3], values[4], values[5]));
          }
        }
        else if (cmd == "sphere") {
          validinput = readvals(s, 4, values); 
          if (validinput) {
            sphere * s = new sphere();
            s->center = glm::vec3(values[0], values[1], values[2]);
            s->radius = values[3];
            s->Mtl = new Material(diffuse, specular, emission, shininess, Color(ambient.x, ambient.y, ambient.z));
            s->transform = transfstack.top(); 
            objects.push_back(s);
          }
        }
        else if (cmd == "tri" || cmd == "trinormal") {
          validinput = readvals(s, 3, values); 
          if (validinput) {
            Material * m = new Material(diffuse, specular, emission, shininess, Color(ambient.x, ambient.y, ambient.z));
            Triangle * tri = new Triangle();
            if(cmd == "tri") {
              for(int i = 0; i < 3; i++) {
                if(values[i] < vtxList.size()) {
                  glm::vec4 v = transfstack.top() * glm::vec4(vtxList[values[i]],1.f);
                  tri->Vtx[i] = glm::vec3(v);

                }
              }
              //calculate and store normal for the flat triangle
              tri->theNormal = glm::normalize(glm::cross(tri->Vtx[1] - tri->Vtx[0], tri->Vtx[2] - tri->Vtx[0]));
            }
            else { //trinormal
              for(int i = 0; i < 3; i++) {
                if(values[i] < normVtxList.size()) {
                  glm::vec4 v = transfstack.top() * glm::vec4(normVtxList[values[i]],1.f);
                  tri->Vtx[i] = glm::vec3(v);
                  glm::vec4 vNorm = transfstack.top() * glm::vec4(normList[values[i]],1.f);
                  tri->VtxNorm[i] = glm::vec3(vNorm);
                }
              }
              //calculate normal using the vertex normals and store
            }
            tri->Mtl = m;
            tri->transform = transfstack.top(); 
            objects.push_back(tri);
          }
        }

        //transformations
        else if (cmd == "translate") {
          validinput = readvals(s,3,values); 
          if (validinput) {
              mat4 M = Transform::translate(values[0],values[1],values[2]);
              rightmultiply(M, transfstack);
          }
        }
        else if (cmd == "scale") {
          validinput = readvals(s,3,values); 
          if (validinput) {

              mat4 M = Transform::scale(values[0],values[1],values[2]);
              rightmultiply(M, transfstack);

          }
        }
        else if (cmd == "rotate") {
          validinput = readvals(s,4,values); 
          if (validinput) {
              vec3 a = vec3(values[0], values[1], values[2]);
              mat4 M = mat4(Transform::rotate(values[3], a));
              rightmultiply(M, transfstack);

          }
        }

        // I include the basic push/pop code for matrix stacks
        else if (cmd == "pushTransform") {
          transfstack.push(transfstack.top()); 
        } else if (cmd == "popTransform") {
          if (transfstack.size() <= 1) {
            cerr << "Stack has no elements.  Cannot Pop\n"; 
          } else {
            transfstack.pop(); 
          }
        }

        else {
          cerr << "Unknown Command: " << cmd << " Skipping \n"; 
        }
      }
      getline (in, str); 
    }

  } else {
    cerr << "Unable to Open Input Data File " << filename << "\n"; 
    throw 2; 
  }
}
