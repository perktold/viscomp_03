#include "ornithopter.h"

Ornithopter::Ornithopter(const Mesh *cube, ShaderProgram *shader)
    : cube(cube), shader(shader) {
    buildModel();
}

void Ornithopter::updateTransform() {
    global_transform = Matrix4D::translation(position) * Matrix4D::rotationY(yaw) *
                Matrix4D::rotationX(pitch) * Matrix4D::rotationZ(roll);
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

    //wings right:
    wings.push_back(
        Matrix4D::translation(Vector3D(-0.55f, 0.8f, -0.15f))*
        Matrix4D::rotationY(to_radians(16.0f)) *
        Matrix4D::translation(Vector3D(0.0f, 0.0f, -7.5f/2))*
        Matrix4D::scale(0.3f / 2, 0.1f / 2, 7.5f / 2));

    wings.push_back(
        Matrix4D::translation(Vector3D(-0.4f, 0.8f, -1.0f))*
        Matrix4D::rotationY(to_radians(16.0f)) *
        Matrix4D::translation(Vector3D(0.0f, 0.0f, -7.5f/2))*
        Matrix4D::scale(0.3f / 2, 0.1f / 2, 7.5f / 2));

    wings.push_back(
        Matrix4D::translation(Vector3D(-0.55f, -0.15f, 0.0f))*
        Matrix4D::rotationY(to_radians(16.0f)) *
        Matrix4D::translation(Vector3D(0.0f, 0.0f, -7.5f/2))*
        Matrix4D::scale(0.3f / 2, 0.1f / 2, 7.5f / 2));

    wings.push_back(
        Matrix4D::translation(Vector3D(-0.4f, -0.15f, -0.9f))*
        Matrix4D::rotationY(to_radians(16.0f)) *
        Matrix4D::translation(Vector3D(0.0f, 0.0f, -7.5f/2))*
        Matrix4D::scale(0.3f / 2, 0.1f / 2, 7.5f / 2));

    wings.push_back(
        Matrix4D::translation(Vector3D(0.55f, 0.8f, -0.15f))*
        Matrix4D::rotationY(to_radians(-16.0f)) *
        Matrix4D::translation(Vector3D(0.0f, 0.0f, -7.5f/2))*
        Matrix4D::scale(0.3f / 2, 0.1f / 2, 7.5f / 2));

    wings.push_back(
        Matrix4D::translation(Vector3D(0.4f, 0.8f, -1.0f))*
        Matrix4D::rotationY(to_radians(-16.0f)) *
        Matrix4D::translation(Vector3D(0.0f, 0.0f, -7.5f/2))*
        Matrix4D::scale(0.3f / 2, 0.1f / 2, 7.5f / 2));

    wings.push_back(
        Matrix4D::translation(Vector3D(0.55f, -0.15f, 0.0f))*
        Matrix4D::rotationY(to_radians(-16.0f)) *
        Matrix4D::translation(Vector3D(0.0f, 0.0f, -7.5f/2))*
        Matrix4D::scale(0.3f / 2, 0.1f / 2, 7.5f / 2));

    wings.push_back(
        Matrix4D::translation(Vector3D(0.4f, -0.15f, -0.9f))*
        Matrix4D::rotationY(to_radians(-16.0f)) *
        Matrix4D::translation(Vector3D(0.0f, 0.0f, -7.5f/2))*
        Matrix4D::scale(0.3f / 2, 0.1f / 2, 7.5f / 2));
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
    for (const Matrix4D &w : wings) {
        drawCube(global_transform * w);
    }
}