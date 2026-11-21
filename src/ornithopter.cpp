#include "ornithopter.h"

Ornithopter::Ornithopter(const Mesh *cube, ShaderProgram *shader)
    : cube(cube), shader(shader) {
    buildModel();
}

void Ornithopter::updateTransform() {
    transform = Matrix4D::translation(position) * Matrix4D::rotationY(yaw) *
                Matrix4D::rotationX(pitch) * Matrix4D::rotationZ(roll);
}

Wing leftWing;
Wing rightWing;

void Ornithopter::buildModel() {
    // cockpit
    bodyParts.push_back(
        Matrix4D::rotationX(to_radians(8.0f)) *
        Matrix4D::scale(1.4f / 2, 1.4f / 2, 5.4f / 2));

    //tail
    bodyParts.push_back(
        Matrix4D::translation(Vector3D(0.0f, 0.0f, -5.4f))*
        Matrix4D::scale(0.8f / 2, 0.6f / 2, 6.0f / 2)
        );

    //Foot
    bodyParts.push_back(
        Matrix4D::translation(Vector3D(0.0f, -1.0f, -0.4f))*
        Matrix4D::scale(0.2f / 2, 1.0f / 2, 2.0f / 2)
        );
    bodyParts.push_back(
        Matrix4D::translation(Vector3D(0.0f, -1.5f, -0.4f))*
        Matrix4D::scale(0.2f / 2, 0.2f / 2, 3.0f / 2));

    //wings left
    leftWing.pivot =
        Matrix4D::translation(Vector3D(0.0f, 1.0f, -0.4f)) *
        Matrix4D::rotationY(to_radians(16.0f));

    leftWing.local =
        Matrix4D::translation(Vector3D(0.0f, 0.0f, -7.5f / 2)) *
        Matrix4D::scale(0.3f / 2, 0.1f / 2, 7.5f / 2);
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