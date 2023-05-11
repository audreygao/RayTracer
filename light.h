
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class lightType {point, directional};

class light {
    public:
        light(glm::vec3 p, Color c, float c1, float c2, float c3): 
            posDir(p), color(c){
                attenuation[0] = c1;
                attenuation[1] = c2;
                attenuation[2] = c3;
            }
        lightType type;
        glm::vec3 posDir;
        float attenuation[3];
        Color color;
};