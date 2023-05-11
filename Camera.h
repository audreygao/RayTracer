#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Camera {
  public:
    Camera() {}
    glm::vec3 pos, u, v, w;
    float fovy;
    float fovx;
};
