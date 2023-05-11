
// // This is the basic include file for the global variables in the program.  
// // Since all files need access to it, we define EXTERN as either blank or 
// // extern, depending on if included in the main program or not.  
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include "Camera.h"
#include "usefulStuff.h"
#include "Material.h"
#include "object.h"
#include "light.h"

#ifdef MAINPROGRAM 
#define EXTERN 
#else 
#define EXTERN extern 
#endif

//photo size
EXTERN int w, h;
EXTERN int maxDepth;
EXTERN std::string imageName;
const int defaultDepth = 5;

//camera
EXTERN int cameraCount;
EXTERN int maxCamera;
EXTERN Camera camera;

//objects
EXTERN int numVert;
EXTERN int numVertNorm ;
EXTERN int maxVert;
EXTERN int maxVertNorm; //default to 10
EXTERN std::vector<glm::vec3> vtxList;
EXTERN std::vector<glm::vec3> normVtxList;
EXTERN std::vector<glm::vec3> normList;
EXTERN std::vector<object *> objects; 

//light
EXTERN std::vector<light *> lights;
EXTERN glm::vec3 attenuation;

//material
EXTERN glm::vec3 ambient;
EXTERN Color diffuse;
EXTERN Color specular;
EXTERN Color emission;
EXTERN float shininess;


