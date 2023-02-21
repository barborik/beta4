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
#include <GL/glew.h>

int main()
{
    if (glewInit() != GLEW_OK)
    {
        return 1;
    }
    return 0;
}