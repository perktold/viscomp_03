#include <cstdlib>
#include <iostream>

#include "mygl/camera.h"
#include "mygl/geometry.h"
#include "mygl/mesh.h"
#include "mygl/shader.h"

#include "ground.h"
#include "ornithopter.h"

// Helper functions you may need for the wing flapping animation.
struct WingFlapAnimation {
  // t stores the time since the engine start / flapping start.
  float t = 0.0f;

  // The flapping speed increases over time, meaning the flapping cycle time
  // decreases over time.
  // T(t) = (T_start - T_end) * e^(-0.5*t) + T_end.
  float cycleTimeStart = 3.0f;
  float cycleTimeEnd = 0.1f;
  float cycleTime() const {
    return (cycleTimeStart - cycleTimeEnd) * std::exp(-0.5f * t) + cycleTimeEnd;
  }

  // The rotation of the wings during the flapping animation is modelled by a
  // Sinus function: a * sin(2*pi*t/T(t)).
  float angleZMax = to_radians(3.0f);
  float angleZ() const {
    return angleZMax *
           std::sin(2.0f * static_cast<float>(M_PI) * t / cycleTime());
  }
};

// Struct holding all necessary state variables for scene.
struct {
  // Camera.
  Camera camera;
  bool cameraFollowOrnithopter = false;
  float zoomSpeedMultiplier;

  // Game objects.
  Ground ground;

  // Ornithopter :)
  Ornithopter *ornithopter;
  WingFlapAnimation sFlap;
  // Template objects, transformation matrices and animation properties.
  Mesh cubeMesh;

  // Shader.
  ShaderProgram shaderColor;
  ShaderProgram shaderOrnithopter;
} sScene;

// Struct holding all state variables for input.
struct {
  bool mouseLeftButtonPressed = false;
  Vector2D mousePressStart;
  bool keyPressed[8] = {false, false, false, false, false, false, false, false};
} sInput;

// GLFW callback function for keyboard events.
void callbackKey(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  // Close window on escape.
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  // Make screenshot and save in work directory.
  if (key == GLFW_KEY_P && action == GLFW_PRESS) {
    screenshotToPNG("screenshot.png");
  }

  // Input for camera control.
  if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
    sScene.cameraFollowOrnithopter = false;
    sScene.camera.lookAt = {0.0f, 0.0f, 0.0f};
    cameraUpdateOrbit(sScene.camera, {0.0f, 0.0f}, 0.0f);
  }
  if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
    sScene.cameraFollowOrnithopter = true;
  }

  // Input for ornithopter control.
  if (key == GLFW_KEY_W) {
    sInput.keyPressed[0] = (action == GLFW_PRESS || action == GLFW_REPEAT);
  }
  if (key == GLFW_KEY_S) {
    sInput.keyPressed[1] = (action == GLFW_PRESS || action == GLFW_REPEAT);
  }
  if (key == GLFW_KEY_A) {
    sInput.keyPressed[2] = (action == GLFW_PRESS || action == GLFW_REPEAT);
  }
  if (key == GLFW_KEY_D) {
    sInput.keyPressed[3] = (action == GLFW_PRESS || action == GLFW_REPEAT);
  }
  if (key == GLFW_KEY_Q) {
    sInput.keyPressed[4] = (action == GLFW_PRESS || action == GLFW_REPEAT);
  }
  if (key == GLFW_KEY_E) {
    sInput.keyPressed[5] = (action == GLFW_PRESS || action == GLFW_REPEAT);
  }
  if (key == GLFW_KEY_SPACE) {
    sInput.keyPressed[6] = (action == GLFW_PRESS || action == GLFW_REPEAT);
  }
  if (key == GLFW_KEY_LEFT_SHIFT) {
    sInput.keyPressed[7] = (action == GLFW_PRESS || action == GLFW_REPEAT);
  }
  if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
    sScene.cameraFollowOrnithopter = false;
  }
  if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
    sScene.cameraFollowOrnithopter = true;
  }
}

// GLFW callback function for mouse position events.
void callbackMousePos(GLFWwindow *window, double x, double y) {
  if (sInput.mouseLeftButtonPressed && !sScene.cameraFollowOrnithopter) {
    Vector2D diff = sInput.mousePressStart - Vector2D(x, y);
    cameraUpdateOrbit(sScene.camera, diff, 0.0f);
    sInput.mousePressStart = Vector2D(x, y);
  }
}

// GLFW callback function for mouse button events.
void callbackMouseButton(GLFWwindow *window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    sInput.mouseLeftButtonPressed =
        (action == GLFW_PRESS || action == GLFW_REPEAT);

    double x, y;
    glfwGetCursorPos(window, &x, &y);
    sInput.mousePressStart = Vector2D(x, y);
  }
}

// GLFW callback function for mouse scroll events.
void callbackMouseScroll(GLFWwindow *window, double xoffset, double yoffset) {
  cameraUpdateOrbit(sScene.camera, {0, 0},
                    -sScene.zoomSpeedMultiplier * yoffset);
}

// GLFW callback function for window resize event.
void callbackWindowResize(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  sScene.camera.width = width;
  sScene.camera.height = height;
}

// Function to setup and initialize the whole scene.
void sceneInit(float width, float height) {
  sScene.ground =
      groundCreate({150.0f / 255.0f, 95.0f / 255.0f, 55.0f / 255.0f},
                   {226.0f / 255.0f, 180.0f / 255.0f, 120.0f / 255.0f});

  // Template objects.
  sScene.cubeMesh =
      meshCreate(cube::vertices, cube::indices, GL_STATIC_DRAW, GL_STATIC_DRAW);

  // init ornithopter
  sScene.shaderOrnithopter = shaderLoad("shader/gray.vert", "shader/gray.frag");
  sScene.ornithopter = new Ornithopter(&sScene.cubeMesh, &sScene.shaderOrnithopter);

  // Initialize camera.
  sScene.camera = cameraCreate(width, height, to_radians(45.0f), 0.01f, 500.0f,
                               {12.0f, 4.0f, -12.0f});
  sScene.cameraFollowOrnithopter = false;
  sScene.zoomSpeedMultiplier = 0.05f;

  // Load shader from file.
  sScene.shaderColor = shaderLoad("shader/default.vert", "shader/default.frag");
}

// Function to move and update objects in scene. TODO: move this into ornithopter class?
void sceneUpdate(float dt) {
  Ornithopter &o = *sScene.ornithopter;
  const float speed = 10.0f, turnSpeed = 1.5f;
  const float maxTilt = to_radians(15.0f);
  const float tiltRate = 5.0f; // how fast does the ornithopter reach max roll/pitch/yaw
  const float accelRate = 7.0f; // how fast does the ornithopter reach max speed

  // Wing animation
  const float openMax = to_radians(74.0f);
  const float flipMax = to_radians(180.0f);
  if (o.wingOpenAngle < openMax) {
    o.wingOpenAngle = std::min(openMax, o.wingOpenAngle + float(to_radians(20.0f)) * dt);
  } else if (o.wingFlipAngle < flipMax) {
    o.wingFlipAngle = std::min(flipMax, o.wingFlipAngle + float(to_radians(160.0f)) * dt);
  } else {
    sScene.sFlap.t += dt;
    o.wingFlapAngle = sScene.sFlap.angleZ();
  }

  // 1 means forward / right / counterclockwise / upwards, -1 means opposite
  float fwdIn  = float(sInput.keyPressed[0]) - float(sInput.keyPressed[1]); // W/S
  float sideIn = float(sInput.keyPressed[3]) - float(sInput.keyPressed[2]); // D/A
  float turnIn = float(sInput.keyPressed[4]) - float(sInput.keyPressed[5]); // Q/E
  float vertIn = float(sInput.keyPressed[6]) - float(sInput.keyPressed[7]); // Space/Shift

  // ease velocities
  float a = 1.0f - std::exp(-accelRate * dt);
  o.vFwd  += (fwdIn  - o.vFwd)  * a;
  o.vSide += (sideIn - o.vSide) * a;
  o.vVert += (vertIn - o.vVert) * a;
  o.vTurn += (turnIn - o.vTurn) * a;

  // Movement relative to current heading.
  o.yaw += o.vTurn * turnSpeed * dt;
  Vector3D forward = {std::sin(o.yaw), 0.0f, std::cos(o.yaw)};
  Vector3D right = {-std::cos(o.yaw), 0.0f, std::sin(o.yaw)};
  o.position += (forward * o.vFwd + right * o.vSide+ Vector3D(0, o.vVert, 0)) * (speed * dt);

  // Ease tilt toward target: nose down when moving forward, lean into sideways movement.
  float k = 1.0f - std::exp(-tiltRate * dt);
  o.pitch += (o.vFwd * maxTilt - o.pitch) * k;
  o.roll += (o.vSide * maxTilt - o.roll) * k;

  o.updateTransform();
  if (sScene.cameraFollowOrnithopter) {
    sScene.camera.lookAt = o.position;
  }
}

// Function to draw all objects in the scene.
void sceneDraw() {
  // Clear framebuffer color.
  glClearColor(135.0f / 255, 206.0f / 255, 235.0f / 255, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Render scene — use shader and set uniforms (names match the ones in the
  // shader).
  {
    glUseProgram(sScene.shaderColor.id);
    shaderUniform(sScene.shaderColor, "uProj", cameraProjection(sScene.camera));
    shaderUniform(sScene.shaderColor, "uView", cameraView(sScene.camera));

    // Draw ground.
    shaderUniform(sScene.shaderColor, "uModel", Matrix4D::identity());
    glBindVertexArray(sScene.ground.mesh.vao);
    glDrawElements(GL_TRIANGLES, sScene.ground.mesh.size_ibo, GL_UNSIGNED_INT,
                   nullptr);

    glUseProgram(sScene.shaderOrnithopter.id);
    shaderUniform(sScene.shaderOrnithopter, "uProj", cameraProjection(sScene.camera));
    shaderUniform(sScene.shaderOrnithopter, "uView", cameraView(sScene.camera));
    sScene.ornithopter->draw();
  }
  glCheckError();

  // Cleanup OpenGL state.
  glBindVertexArray(0);
  glUseProgram(0);
}

int main(int argc, char **argv) {
  // Create window/context.
  int width = 1280;
  int height = 720;
  GLFWwindow *window = windowCreate(
      "Assignment 3 - Transformations, User Input and Camera", width, height);
  if (!window) {
    return EXIT_FAILURE;
  }

  // Set window callbacks.
  glfwSetKeyCallback(window, callbackKey);
  glfwSetCursorPosCallback(window, callbackMousePos);
  glfwSetMouseButtonCallback(window, callbackMouseButton);
  glfwSetScrollCallback(window, callbackMouseScroll);
  glfwSetFramebufferSizeCallback(window, callbackWindowResize);

  // Init OpenGL.
  glEnable(GL_DEPTH_TEST);

  // Setup scene.
  sceneInit(width, height);

  // Main loop.
  double timeStamp = glfwGetTime();
  double timeStampNew = 0.0;

  while (!glfwWindowShouldClose(window)) {
    // Poll and process input and window events.
    glfwPollEvents();

    // Update camera and model matrices.
    timeStampNew = glfwGetTime();
    sceneUpdate(timeStampNew - timeStamp);
    timeStamp = timeStampNew;

    // Draw all objects in the scene.
    sceneDraw();

    // Swap front and back buffer.
    glfwSwapBuffers(window);
  }

  // Cleanup — delete OpenGL shader and buffers.
  shaderDelete(sScene.shaderOrnithopter);
  shaderDelete(sScene.shaderColor);
  groundDelete(sScene.ground);

  // Cleanup GLFW/GL context.
  windowDelete(window);

  delete sScene.ornithopter;

  return EXIT_SUCCESS;
}
