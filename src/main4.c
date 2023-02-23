/*#include <stdio.h>

#include "netlib.h"

int con = 0;

int main(int argc, char *argv[])
{
    sock_t this, other;

    nlib_mksock(&this, argv[1], argv[2], NLIB_SERVER, NLIB_TCP);
    printf("TEST\n");
    nlib_mksock(&other, NULL, argv[4], NLIB_CLIENT, NLIB_TCP);

    printf("TEST\n");

    nlib_noblock(&this);
    nlib_noblock(&other);

    while (!nlib_accept(&this, &other))
    {
        if (!con)
        {
            if (nlib_connect(&this, argv[3], argv[4]))
            {
                con = 1;
            }
            printf("%s\n", nlib_msg);
        }
        sleep(10);
    }

    printf("done\n");

    return 0;
}
*/

const char *shaderCode =
    "#version 430\n"
    "layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
    "layout(std430, binding = 0) buffer Output {\n"
    "    uint result;\n"
    "} output;\n"
    "void main() {\n"
    "    output.result = 42;\n"
    "}\n";

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int gfxinit()
{
    GLFWwindow *window;

    if (!glfwInit())
    {
        return 0;
    }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    window = glfwCreateWindow(1, 1, "Headless Window", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return 0;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        return 0;
    }

    return 1;
}

void software()
{
    // old code
}

void hardware()
{
    // load the shader
    FILE *srcf = fopen("shader.comp", "r");
    fseek(srcf, 0, SEEK_END);
    int len = ftell(srcf);
    fseek(srcf, 0, SEEK_SET);
    char *src = malloc(len);
    fread(src, 1, len, srcf);
    fclose(srcf);

    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    // check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("shader compilation error: %s\n", infoLog);
        return;
    }

    // create the shader program
    GLuint program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);

    // check for linking errors
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("shader linking error: %s\n", infoLog);
        return;
    }

    // run the compute shader
    glUseProgram(program);
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int), NULL, GL_DYNAMIC_DRAW);
    glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    unsigned int result;
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int), &result);
    printf("result: %u\n", result);

    // cleanup
    free(src);
    glDeleteShader(shader);
    glDeleteProgram(program);
    glDeleteBuffers(1, &buffer);
    glfwTerminate();
}

int main()
{
    if (!gfxinit())
    {
        printf("OpenGL not initialized, GPU won't be used\n");
        software();
        return 0;
    }

    hardware();
    return 0;
}