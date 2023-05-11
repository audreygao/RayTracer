#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class shapeType {sphere, triangle};

class object {
    public:
        object(shapeType t) : type(t){Mtl=0;}
        ~object() {
            if(Mtl != 0)
            delete Mtl;
        }
        shapeType type;
        glm::mat4 transform;
        Material * Mtl;
};

class Triangle : public object{
  public:
    Triangle(): object(shapeType::triangle){}
    //bool Intersect(const Ray &ray, Intersection &hit);
    glm::vec3 Vtx[3];
    glm::vec3 VtxNorm[3];
    glm::vec3 theNormal;
};

// class flatTriangle : public object{
//     public:
//         flatTriangle(): object(shapeType::flatTriangle){}
//         //bool Intersect(const Ray &ray, Intersection &hit);
//     glm::vec3 Vtx[3];
//     glm::vec3 VtxNorm[3];
// };

class sphere : public object{
    public:
        sphere(): object(shapeType::sphere){}
        bool Intersect(const Ray &ray, Intersection &hit);
    glm::vec3 center;
    float radius;
};