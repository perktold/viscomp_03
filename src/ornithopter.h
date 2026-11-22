#pragma once
#include "mygl/base.h"
#include "mygl/mesh.h"
#include "mygl/shader.h"
#include <vector>

class Ornithopter {
public:
    Ornithopter(const Mesh *cube, ShaderProgram *shader);

    void updateTransform();

    void draw() const;

    // ornithopter transform and animation state, only change this externally (this for the movement etc)
    Vector3D position = {0.0f, 5.0f, 0.0f};
    float yaw = 0.0f, pitch = 0.0f, roll = 0.0f; // rad
    Matrix4D transform = Matrix4D::identity();
    float wingOpenAngle = 0.0f, wingFlipAngle = 0.0f, wingFlapAngle = 0.0f;
    float vFwd = 0.0, vSide = 0.0, vVert = 0.0, vTurn = 0.0;

private:
    void buildModel();
    //Matrix4D wingAnimation(const Wing &w) const;
    void drawCube(const Matrix4D &m) const;

    const Mesh *cube;
    ShaderProgram *shader;
    std::vector<Matrix4D> bodyParts; // cockpit, tail boom, strut, landing plate
    //std::vector<Wing> wings;         // 8 wings
};