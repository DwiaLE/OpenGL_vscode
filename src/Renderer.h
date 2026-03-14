#pragma once

#include <glew.h>
#include <glfw3.h>

#define ASSERT(x) do { if (!(x)) __builtin_trap(); } while (0)
#define GLCall(x) do { \
    GLClearError(); \
    x; \
    ASSERT(GLLogCall(#x, __FILE__, __LINE__)); \
} while (0)

void GLClearError();
bool GLLogCall(const char* function , const char* file , int line);

