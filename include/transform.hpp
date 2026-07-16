#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP
#include <glm/glm.hpp>

// GLM matrix calculation helper managing model transforms and widescreen scales
class Transform
{
public:
    glm::mat4 trans = glm::mat4(1.0f); // Stored transformation matrix

    // Axis enum
    enum Lines {X, Y, Z};

    // Revert matrix state back to standard Identity state
    void reset();

    // Rotations
    void rotate(float rotation, Lines line);
    void rotate(float rotation, Lines line1, Lines line2);
    void rotate(float rotation);

    // Placement translation
    void translate(glm::vec3 transform);

    // Perspective mapping calculation
    void perspective(float fov, float widht, float height, float near, float far);
    
    // Direct aspect ratio modifier adjusting widescreen distortions
    void setValue(int line, int row, float value);

    // Returns computed matrix data
    glm::mat4 getTransform();
};

#endif