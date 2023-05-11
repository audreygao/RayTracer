#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

 class Ray {
  public:
    Ray(glm::vec3 p, glm::vec3 d) : pos(p), dir(d) {}
    glm::vec3 pos;
    glm::vec3 dir;
};
