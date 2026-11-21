#include "ornithopter.h"

Ornithopter::Ornithopter(const Mesh *cube, ShaderProgram *shader)
    : cube(cube), shader(shader) {
    buildModel();
}

void Ornithopter::updateTransform() {
    transform = Matrix4D::translation(position) * Matrix4D::rotationY(yaw) *
                Matrix4D::rotationX(pitch) * Matrix4D::rotationZ(roll);
}

void Ornithopter::buildModel() {
    // cockpit
    bodyParts.push_back(
        Matrix4D::rotationX(to_radians(8.0f)) *
        Matrix4D::scale(1.4f / 2, 1.4f / 2, 5.4f / 2));
}

// Task 2
Matrix4D Ornithopter::wingAnimation(const Wing &w) const {
    return Matrix4D::identity();
}

void Ornithopter::drawCube(const Matrix4D &m) const {
    shaderUniform(*shader, "uModel", m);
    glBindVertexArray(cube->vao);
    glDrawElements(GL_TRIANGLES, cube->size_ibo, GL_UNSIGNED_INT, nullptr);
}

void Ornithopter::draw() const {
    for (const Matrix4D &p : bodyParts)
        drawCube(transform * p);
    for (const Wing &w : wings)
        drawCube(transform * w.pivot * wingAnimation(w) * w.local);
}