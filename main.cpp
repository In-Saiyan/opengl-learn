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

// Syntactic Sugar ahh
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

  // Set OpenGL versions(3.3 in this case)
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "Test Title", NULL, NULL); // Create Window
  if (window == NULL) {
    LOG(LOG_LEVEL::ERROR, current_time(), "Failed to create a window");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window); // Apply OpenGL commands from this program to this window specifically

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    LOG(LOG_LEVEL::ERROR, current_time(), "Failed to create GLAD process");
    return -1;
  }

  LOG(LOG_LEVEL::INFO, current_time(), "GLAD started");

  glViewport(0, 0, 800, 600); // Set viewport in that window, first two: offset + other two: dimensions
  
  // VBO(Vertex Buffer Object) is like an object that holds data
  // VAO(Vertex Array Object) is the rule manual that tells GPU how to read the VBO
  unsigned int VBO, VAO; // IDs for objects below
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO); // Bind the VAO first - telling OpenGL start recording the setup instructions...
  
  float vertices[] = { // Out tirangal
    -0.5f, -0.5f, 0.0f, // left  
    0.5f, -0.5f, 0.0f, // right 
    0.0f,  0.5f, 0.0f  // top   
  };

  // Bind the VBO and push the vertex data onto GPU vram
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Tell GPU the method to read the raw data 
  // 0 : index (can be linked to a shader)
  // 3: size of each vertex 
  // GL_FLOAT: 32-bit floats 
  // 3 * sizeof(float): "stride" - how much space is between the start of one vertex and the start of the next...
  // (void*) 0: the offset of the buffer data (currently we are at the start)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*) 0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  // Graphics Pipeline (Shaders)
  // 1. Vertex Shaders: Handles the Positioning of the vertex 
  // 2. Fragment Shaders: Handles the pixel clolors (calcs the "fragments")
  const char *vertexShaderSrc = "#version 330 core\n\
                                 layout (location = 0) in vec3 aPos; \n\
                                 void main()\n\
                                 {\n\
                                   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n\
                                 }\n\0";  

  const char *fragShaderSrc = "#version 330 core\n\
                               out vec4 FragColor;\n\
                               void main()\n\
                               {\n\
                                 FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n\
                               }\n\0";

  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];

  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    LOG(LOG_LEVEL::ERROR, current_time(), string("Vertex Shader Compilation Failed: ") + infoLog);
  }

  LOG(LOG_LEVEL::INFO, current_time(), "Compiled Vertex Shaders");

  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragShaderSrc, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    LOG(LOG_LEVEL::ERROR, current_time(), string("Failed to compile Fragment Shader: ") + infoLog);
  }

  LOG(LOG_LEVEL::INFO, current_time(), "Fragment Shader Successfully Compiled");

  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  // Check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    LOG(LOG_LEVEL::ERROR, current_time(), string("Shader Linking Failed: ") + infoLog);
  }

  // Clean up the individual shader objects (we don't need them once linked)
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  LOG(LOG_LEVEL::INFO, current_time(), "Shaders successfully attached");

  while(!glfwWindowShouldClose(window)) { // While the OS doesn't signals to close the window this is essentially just a while(true) loop
    processInput(window);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Wipe out the color
    glClear(GL_COLOR_BUFFER_BIT); // Only clear the color? Need some clarification.

    // Activate our shader program
    glUseProgram(shaderProgram);
    // Bind our VAO (the instruction manual for our triangle)
    glBindVertexArray(VAO);
    // DRAW!
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window); // Swap front(on screen) and back buffers since its better than drawing inplace which can cause visual artifacts and make up for a significantly worse experience.
    glfwPollEvents(); // Polls for events like input(from mouse, keyboard etc...)
  }


  return 0;
}
