#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <format>
#include <iostream>

using string = std::string;
using LocalTime = std::chrono::local_time<std::chrono::system_clock::duration>;

/*
 * Environment ENUM 
 */
enum class Environment {
    DEVELOPMENT,
    PRODUCTION
};

Environment CURRENT_ENV = Environment::DEVELOPMENT;

/*
 * This is the base ENUM that tells about the leve of the LOGs
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
  switch (level) {
    case LOG_LEVEL::INFO : {
      std::cout << std::format("{:%F | %X} | ", time) << "[" << to_string(level) << "] " << message << std::endl; 
      break;
    }
    case LOG_LEVEL::ERROR : {
      std::cout << std::format("{:%F | %X} | ", time) << "[" << to_string(level) << "] " << message << std::endl; 
      break;
    }
  }
}

LocalTime current_time () {
  return std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
}

int main () {
  LocalTime now = current_time();
  LOG(LOG_LEVEL::INFO, now, "STARTED");
  if (!glfwInit()) {
    LOG(LOG_LEVEL::ERROR, current_time(), "COULD NOT INITIALIZE GLFW");
  }
}
