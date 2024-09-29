#include <stdio.h>
#include <GL/freeglut.h>

static void display(void) {
    // const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	glClear(GL_COLOR_BUFFER_BIT);

    glRasterPos2f(0.5,0.5);

    printf("display func called\n");

    glutBitmapString(GLUT_BITMAP_8_BY_13, "Hello");

    glutSwapBuffers();
}

void initGraphics() {
    glutInitWindowSize(200,200);
    glutInitWindowPosition(40,40);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    int pargc = 0;
    glutInit(&pargc, NULL);
    int mainWindow = glutCreateWindow("gl");
    glutDisplayFunc(display);
}

void main() {
    printf("openglut loaded\n");

    printf("init window size init window position\n");

    glutInitWindowSize(200,200);
    glutInitWindowPosition(40,40);

    printf("init displayMode\n");

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    
    printf("glutInit\n");

    int pargc = 0;

    glutInit(&pargc, NULL);

    printf("create window\n");
    int mainWindow = glutCreateWindow("gl");
    
    glutDisplayFunc(display);

    // printf("enter main loop\n");
    // glutMainLoop();

    int running = 1;

    while (running) {
        glutMainLoopEvent();
        scanf("\n");
    }

}