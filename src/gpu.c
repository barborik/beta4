#define GLEW_STATIC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <zahl.h>

#include "part.h"

typedef struct
{
    part_t best;
    part_t start;
    part_t end;
} ival_t;

/*

int subsize = 1 << size;
int start_i = subsize * i;
int end_i = start_i + subsize - 1;

for (int j = 0; j < size; j++)
{
    start->buf[j] = (start_i >> (size - j - 1)) & 1;
    end->buf[j] = (end_i >> (size - j - 1)) & 1;
}

*/
void split(part_t *start, part_t *end, int n, int i)
{
    z_t tmp, one, start_i, end_i, subsize;

    zinit(tmp);
    zinit(one);
    zinit(start_i);
    zinit(end_i);
    zinit(subsize);

    zsets(one, "1");
    zlsh(start_i, one, size);

    zseti(tmp, n);

    zdiv(subsize, start_i, tmp);

    zseti(tmp, i);
    zmul(start_i, subsize, tmp);

    zadd(end_i, start_i, subsize);
    zsub(end_i, end_i, one);

    for (int j = 0; j < size; j++)
    {
        zrsh(tmp, start_i, size - j - 1);
        zand(tmp, tmp, one);
        zzero(tmp) ? (start->buf[j] = 0) : (start->buf[j] = 1);

        zrsh(tmp, end_i, size - j - 1);
        zand(tmp, tmp, one);
        zzero(tmp) ? (end->buf[j] = 0) : (end->buf[j] = 1);
    }

    zfree(tmp);
    zfree(one);
    zfree(start_i);
    zfree(end_i);
    zfree(subsize);
}

void software()
{
    // old code
}

int gfxinit()
{
    GLFWwindow *window;

    if (!glfwInit())
    {
        return 0;
    }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    window = glfwCreateWindow(1, 1, "", NULL, NULL);
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
    printf("compilation successful\n");

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
    printf("linking successful\n");

    // get max number of work groups
    GLint groups;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &groups);
    z_t one, nivals, zgroups;
    zinit(one);
    zinit(nivals);
    zinit(zgroups);
    zsets(one, "1");
    zlsh(nivals, one, size);
    zseti(zgroups, size);
    if (zcmp(zgroups, nivals) < 0)
    {
        zsub(nivals, nivals, one);
        groups = (1 << size) - 1;
    }

    if (groups > 1024)
    {
        groups = 1024;
    }
    printf("groups: %d\n", groups);

    /*int nthreads;
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &nthreads);
    printf("XYZ: %d\n", nthreads);*/

    // set up input buffer
    /*int input_values = 21;
    GLuint input_buffer;
    glGenBuffers(1, &input_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, input_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int), &input_value, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, input_buffer);*/

    // set buffer
    GLuint set_buffer;
    glGenBuffers(1, &set_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, set_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, (size + 1) * sizeof(int), NULL, GL_DYNAMIC_COPY);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int), &size);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 4, sizeof(int) * size, set);
    /*for (int i = 1; i < size + 1; i++)
    {
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 4 + i * sizeof(int), sizeof(int), &set[i]);
    }*/
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, set_buffer);

    // intervals buffer
    GLuint ivals_buffer;
    glGenBuffers(1, &ivals_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ivals_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 268 * 2 * groups, NULL, GL_DYNAMIC_COPY);
    for (int i = 0; i < groups * 2; i += 2)
    {
        part_t start, end;
        pinit(&start);
        pinit(&end);
        split(&start, &end, groups, i / 2);

        /*glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 268 + 0 * 4, sizeof(int), &start.sum1);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 268 + 1 * 4, sizeof(int), &start.sum2);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 268 + 2 * 4, sizeof(int), &start.diff);*/
        for (int j = 0; j < size; j++)
        {
            int tmp = start.buf[j];
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 268 + 3 * 4 + j * 4, sizeof(int), &tmp);
        }

        /*glBufferSubData(GL_SHADER_STORAGE_BUFFER, (i + 1) * 268 + 0 * 4, sizeof(int), &end.sum1);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, (i + 1) * 268 + 1 * 4, sizeof(int), &end.sum2);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, (i + 1) * 268 + 2 * 4, sizeof(int), &end.diff);*/
        for (int j = 0; j < size; j++)
        {
            int tmp = end.buf[j];
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, (i + 1) * 268 + 3 * 4 + j * 4, sizeof(int), &tmp);
        }

        free(start.buf);
        free(end.buf);
    }
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ivals_buffer);

    // bests buffer
    GLuint bests_buffer;
    glGenBuffers(1, &bests_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bests_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 268 * groups, NULL, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, bests_buffer);

    part_t *bests = malloc(sizeof(part_t) * groups);
    for (int i = 0; i < groups; i++)
    {
        pinit(&bests[i]);
    }

    // run the compute shader¨
    struct timeval start, end;
    glUseProgram(program);
    printf("DISPATCHING\n");
    gettimeofday(&start, NULL);
    glDispatchCompute(groups, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // get bests
    for (int i = 0; i < groups; i++)
    {
        bests[i].sum1 = 0;
        bests[i].sum2 = 0;
        bests[i].diff = 0;
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 268 + 0 * 4, sizeof(unsigned int), &bests[i].sum1);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 268 + 1 * 4, sizeof(unsigned int), &bests[i].sum2);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 268 + 2 * 4, sizeof(unsigned int), &bests[i].diff);
        for (int j = 0; j < size; j++)
        {
            int tmp;
            glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, i * 268 + 3 * 4 + j * 4, sizeof(int), &tmp);
            bests[i].buf[j] = tmp;
        }
    }
    printf("DONE\n");

    // compare bests
    part_t best;
    pinit(&best);
    for (int i = 0; i < groups; i++)
    {
        if (bests[i].diff < best.diff)
        {
            copy(&best, &bests[i]);
        }

        if (!best.diff)
        {
            break;
        }
    }

    gettimeofday(&end, NULL);

    printp(&best);
    int elapsed = ((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec);
    printf("total elapsed time:\n%d microseconds\n%d milliseconds\n%f seconds\n", elapsed, elapsed / 1000, (double)elapsed / 1000000);
    exit(0);
}

int main()
{
    init(27);

    jmp_buf env;
    if (setjmp(env))
    {
        return 1;
    }
    zsetup(env);

    if (!gfxinit())
    {
        printf("OpenGL not initialized, GPU won't be used\n");
        software();
        return 0;
    }

    hardware();
    return 0;
}