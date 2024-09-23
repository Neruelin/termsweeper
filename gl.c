#include <GL/freeglut.h>
#include <stdio.h>

void main() {
    printf("openglut loaded\n");

    printf("glutInit\n");

    int pargc = 0;

    glutInit(&pargc, NULL);

    printf("create window\n");
    // glutCreateWindow("gl");

    scanf("\n");
}