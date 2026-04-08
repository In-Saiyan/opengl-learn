#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <format>
#include <iostream>


// Too lazy to type these again and again? Add here :)
using string = std::string;
using LocalTime = std::chrono::local_time<std::chrono::system_clock::duration>;

/*
 * Environment ENUM 
 */
enum class Environment {
    DEVELOPMENT,
    PRODUCTION
};

// ENV variables are for the weak, Keep all my variables in the code...
Environment CURRENT_ENV = Environment::DEVELOPMENT;

/*
 * This is the base ENUM that tells about the level of the LOGs
 */
enum class LOG_LEVEL{
  INFO,
  DEBUG,
  ERROR,
  WARNING,
  CRITICAL,
  NOTE,
};

std::string_view to_string(LOG_LEVEL level) {
  switch (level) {
    case LOG_LEVEL::INFO:       return "INFO";
    case LOG_LEVEL::DEBUG:      return "DEBUG";
    case LOG_LEVEL::ERROR:      return "ERROR";
    case LOG_LEVEL::WARNING:    return "WARNING";
    case LOG_LEVEL::CRITICAL:   return "CRITICAL";
    case LOG_LEVEL::NOTE:       return "NOTE";
  }
  return "HOW DID WE GET HERE??";
}

void LOG(LOG_LEVEL level, LocalTime time, string message) {
  if (CURRENT_ENV == Environment::PRODUCTION) {
    if (level == LOG_LEVEL::DEBUG || level == LOG_LEVEL::WARNING) {
      return;
    }
  }
  std::cout << std::format("{:%F | %X} | ", time) << "[" << to_string(level) << "] " << message << std::endl; 
}

LocalTime current_time () {
  return std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
}

void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main () {
  LOG(LOG_LEVEL::INFO, current_time(), "PROGRAM EXECUTION STARTED");

  if (!glfwInit()) {
    LOG(LOG_LEVEL::ERROR, current_time(), "COULD NOT INITIALIZE GLFW");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "Test Title", NULL, NULL);
  if (window == NULL) {
    LOG(LOG_LEVEL::ERROR, current_time(), "Failed to create a window");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    LOG(LOG_LEVEL::ERROR, current_time(), "Failed to create GLAD process");
    return -1;
  }

  LOG(LOG_LEVEL::INFO, current_time(), "GLAD started");

  glViewport(0, 0, 800, 600);


  while(!glfwWindowShouldClose(window)) {
    processInput(window);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}
