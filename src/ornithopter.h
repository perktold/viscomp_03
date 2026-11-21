#pragma once
#include "mygl/base.h"
#include "mygl/mesh.h"
#include "mygl/shader.h"
#include <vector>

struct Wing {
    Matrix4D pivot; // hinge position/orientation on the body
    Matrix4D local; // scale + offset of the wing cube relative to the pivot
};

class Ornithopter {
public:
    Ornithopter(const Mesh *cube, ShaderProgram *shader);
    void draw() const;

    // ornithopter transform and animation state, only change this externally (this is for pitch/roll/yaw for the movement etc)
    Matrix4D transform = Matrix4D::identity();
    float wingOpenAngle = 0.0f, wingFlipAngle = 0.0f, wingFlapAngle = 0.0f;

private:
    void buildModel();
    Matrix4D wingAnimation(const Wing &w) const;
    void drawCube(const Matrix4D &m) const;

    const Mesh *cube;
    ShaderProgram *shader;
    std::vector<Matrix4D> bodyParts; // cockpit, tail boom, strut, landing plate
    std::vector<Wing> wings;         // 8 wings
};