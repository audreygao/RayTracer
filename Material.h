#include <algorithm>

class Color {
    public:
    Color(float red=0, float green=0, float blue=0): r(red), g(green), b(blue){}
    float r, g, b;
    Color operator+(const Color& otherColor) {
        float newr = std::min(this->r + otherColor.r, 1.f);
        float newg = std::min(this->g + otherColor.g, 1.f);
        float newb = std::min(this->b + otherColor.b, 1.f);
        return Color(newr, newg, newb);
    }
    Color operator*(float f) {
        float newr = std::min(this->r * f, 1.0f);
        float newg = std::min(this->g * f, 1.0f);
        float newb = std::min(this->b * f, 1.0f);
        return Color(newr, newg, newb);
    }
    Color operator*(const Color& otherColor) {
        float newr = std::min(this->r * otherColor.r, 1.0f);
        float newg = std::min(this->g * otherColor.g, 1.0f);
        float newb = std::min(this->b * otherColor.b, 1.0f);
        return Color(newr, newg, newb);
    }
};

class Material {
    public:
    Material(Color d, Color s, Color e, float sh, Color a) :
        diffuse(d), specular(s), emission(e), shininess(sh), ambient(a){}
    Color ambient;
    Color diffuse;
    Color specular;
    Color emission;
    float shininess;
};

class Intersection {
  public:
    Intersection(){Mtl=0; HitDistance=1e10;}
    ~Intersection() {
        if(Mtl!=0)
        delete Mtl;
    }
    // Results of intersection test
    float HitDistance; //shortest hit distance
    glm::vec3 Position; //position of intersection
    glm::vec3 Normal; //normal of intersection
    Material *Mtl;
};