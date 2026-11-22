#include "ornithopter.h"

Ornithopter::Ornithopter(const Mesh *cube, ShaderProgram *shader)
    : cube(cube), shader(shader) {
    buildModel();
}

void Ornithopter::updateTransform() {
    global_transform = Matrix4D::translation(position) * Matrix4D::rotationY(yaw) *
                Matrix4D::rotationX(pitch) * Matrix4D::rotationZ(roll);
}

void Ornithopter::addWing(const Vector3D &pivot, float side) {
    wings.push_back({pivot, side,
                     Matrix4D::translation(Vector3D(0, 0, -7.5f / 2)) *
                         Matrix4D::scale(0.3f / 2, 0.1f / 2, 7.5f / 2)});
}

// animation function also translates the wings to their pivot point at the ornithopter.
Matrix4D Ornithopter::wingAnimation(const Wing &w) const {
    const float restYaw = to_radians(16.0f);
    return Matrix4D::translation(w.pivot) *
           Matrix4D::rotationZ(-w.side * wingFlapAngle) *
           Matrix4D::rotationX(wingFlipAngle) *
           Matrix4D::rotationY(w.side * (restYaw + wingOpenAngle));
}

void Ornithopter::buildModel() {
    // cockpit
    bodyParts.push_back(
        Matrix4D::rotationX(to_radians(8.0f)) *
        Matrix4D::scale(1.4f / 2, 1.4f / 2, 5.4f / 2));

    //tail
    bodyParts.push_back(
        Matrix4D::translation(Vector3D(0.0f, 0.5f, -5.4f))*
        Matrix4D::scale(0.8f / 2, 0.6f / 2, 6.0f / 2)
        );

    //Foot
    bodyParts.push_back(
        Matrix4D::translation(Vector3D(0.0f, -1.0f, -0.4f))*
        Matrix4D::scale(0.2f / 2, 1.0f / 2, 2.0f / 2)
        );
    bodyParts.push_back(
        Matrix4D::translation(Vector3D(0.0f, -1.5f, -0.4f))*
        Matrix4D::scale(1.4f / 2, 0.2f / 2, 3.0f / 2));

    // side +1 = right wings, side -1 = left wings
    addWing({-0.55f, 0.8f, -0.15f}, 1);
    addWing({-0.4f, 0.8f, -1.0f}, 1);
    addWing({-0.55f, -0.15f, 0.0f}, 1);
    addWing({-0.4f, -0.15f, -0.9f}, 1);
    addWing({0.55f, 0.8f, -0.15f}, -1);
    addWing({0.4f, 0.8f, -1.0f}, -1);
    addWing({0.55f, -0.15f, 0.0f}, -1);
    addWing({0.4f, -0.15f, -0.9f}, -1);
}


void Ornithopter::drawCube(const Matrix4D &m) const {
    shaderUniform(*shader, "uModel", m);
    glBindVertexArray(cube->vao);
    glDrawElements(GL_TRIANGLES, cube->size_ibo, GL_UNSIGNED_INT, nullptr);
}

void Ornithopter::draw() const {
    for (const Matrix4D &p : bodyParts) {
        drawCube(global_transform * p);
    }
    for (const Wing &w : wings) {
        drawCube(global_transform * wingAnimation(w) * w.mesh);
    }
}