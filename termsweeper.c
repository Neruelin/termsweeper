#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/freeglut.h>

#define GRIDWIDTH 20
#define GRIDHEIGHT 20
#define CHARWIDTH 8
#define CHARHEIGHT 13
#define CELLWIDTH 25
#define CELLHEIGHT 25
#define SCREENWIDTH (GRIDWIDTH * CELLWIDTH)
#define SCREENHEIGHT (GRIDHEIGHT * CELLHEIGHT)
#define THRESHOLD 85
#define MEMDEBUG 0

#define COLOR_BLACK 0.0,0.0,0.0
#define COLOR_WHITE 1.0,1.0,1.0
#define COLOR_GREY 0.3,0.3,0.3
#define COLOR_RED 1.0,0.0,0.0
#define COLOR_BLUE 0.0,0.0,1.0

int longestDigits = 0;

int initDigits() {
    for (int temp = 1; temp < GRIDWIDTH; temp *= 10) {
        longestDigits++;
    }
}

#if MEMDEBUG == 1
int mallocsCount = 0;
int freesCount = 0;
#endif

void* mmalloc(size_t size) {
#if MEMDEBUG == 1
    mallocsCount++;
#endif
    return malloc(size);
}

void mfree(void* ptr) {
#if MEMDEBUG == 1
    freesCount++;
#endif
    free(ptr);
}

// STRUCTS

typedef struct {
    char displayData[GRIDHEIGHT][GRIDWIDTH];
    int update[GRIDHEIGHT][GRIDWIDTH];
    int mineData[GRIDHEIGHT][GRIDWIDTH];
    int adjMineData[GRIDHEIGHT][GRIDWIDTH];
    int minesClicked;
    int tilesRevealed;
    int totalMines;
    int cursorGridX;
    int cursorGridY;
    int running;
    double lastTime;
    int repaint;
} GameData;

typedef struct LLNode {
    int gr;
    int gc;
    struct LLNode* next;
} LLNode;

typedef struct {
    LLNode* front;
    LLNode* back;
} Deque;

// STRUCT FUNCTIONS

void GameData_initDisplayData(GameData* gd) {
    for (int i = 0; i < GRIDHEIGHT; i++) {
        for (int j = 0; j < GRIDWIDTH; j++) {
            gd->displayData[i][j] = ' ';
        }
    }
}

void GameData_initGameData(GameData* gd) {
    for (int i = 0; i < GRIDHEIGHT; i++) {
        for (int j = 0; j < GRIDWIDTH; j++) {
            gd->mineData[i][j] = (rand() % 100 > THRESHOLD) ? 1 : 0;
            if (gd->mineData[i][j]) {
                gd->totalMines++;
            }
        }
    }
    for (int i = 0; i < GRIDHEIGHT; i++) {
        for (int j = 0; j < GRIDWIDTH; j++) {
            int mines = 0;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) {
                        continue;
                    }
                    if (i + dy < 0 || i + dy >= GRIDHEIGHT || j + dx < 0 || j + dx >= GRIDWIDTH) {
                        continue;
                    }
                    if (gd->mineData[i+dy][j+dx]) {
                        mines++;
                    }
                }
            }
            gd->adjMineData[i][j] = mines;
        }
    }
}

void GameData_initUpdate(GameData* gd) {
    for (int i = 0; i < GRIDHEIGHT; i++) {
        for (int j = 0; j < GRIDWIDTH; j++) {
            gd->update[i][j] = 1;
        }
    }
}

void GameData_init(GameData* gd) {
    gd->lastTime = 0;
    gd->repaint = 1;
    GameData_initDisplayData(gd);
    GameData_initGameData(gd);
    GameData_initUpdate(gd);
}

LLNode* LLNode_new(int gr, int gc) {
    LLNode* node = mmalloc(sizeof(LLNode));
    node->gr = gr;
    node->gc = gc;
    return node;
}

void Deque_push(Deque* deque, LLNode* value, int front) {
    if (deque->back == NULL && deque->front == NULL) {
        deque->back = value;
        deque->front = value;
    } else if (front) {
        value->next = deque->front;
        deque->front = value;
    } else {
        deque->back->next = value;
        deque->back = value;
    }
}

LLNode* Deque_popFront(Deque* deque) {
    LLNode* node = deque->front;
    if (deque->front != NULL) {
        if (deque->front == deque->back) {
            deque->front = NULL;
            deque->back = NULL;
        } else {
            deque->front = deque->front->next;
        }            
    } 
    return node;
}

// CURSOR UTILS

void moveCursorUp() {
    printf("\e[1A");
}

void moveCursorDown() {
    printf("\e[1B");
}

void moveCursorLeft() {
    printf("\e[1D");
}

void moveCursorRight() {
    printf("\e[1C");
}

void moveCursorStart() {
    printf("\r");
}

// DISPLAY UTILS

void bg_white() {
    printf("\e[47m");
}

void bg_black() {
    printf("\e[40m");
}

void text_red() {
    printf("\e[31m");
}

void text_white() {
    printf("\e[97m");
}

void text_green() {
    printf("\e[92m");
}

void resetText() {
    bg_black();
    text_white();
}

void clearLine() {
    printf("\r");
    for (int j = 0; j < GRIDWIDTH * 2 + longestDigits + 1; j++) {
        printf(" ");
    }
    printf("\r");
}

void clearInputAction() {
    moveCursorUp();
    clearLine();
}

void clearScreen() {
    for (int i = 0; i < GRIDHEIGHT + longestDigits; i++) {
        moveCursorUp();
        clearLine();
    }
}

void displayBottomAxis() {
    for (int i = 1; i < longestDigits; i++) {
        printf("\n");
    }
    for (int i = 0; i < longestDigits + 2; i++) {
        moveCursorRight();
    }
    for (int i = 0; i < GRIDWIDTH - 2; i++) {
        if (i == 0) {
            printf("0 ");
            continue;
        }

        int iCopy = i;
        for (int digitIdx = 0; digitIdx < longestDigits; digitIdx++) {
            int digit = iCopy % 10;
            iCopy = (iCopy - digit) / 10;
            if (digit == 0) {
                if (iCopy > 0) {
                    printf("0");
                } else {
                    printf(" ");
                }
            } else {
                printf("%d", digit);
            }
            moveCursorLeft();
            moveCursorUp();
        }
        for (int digitIdx = 0; digitIdx < longestDigits; digitIdx++) {
            moveCursorDown();
        }
        moveCursorRight();
        moveCursorRight();
    }
}

void display(GameData* gd) {
    for (int i = 0; i < GRIDHEIGHT; i++) {
        if (i != 0 && i != GRIDHEIGHT - 1) {
            printf("%2d ", GRIDHEIGHT - 2 - i);
        } else {
            printf("   ");
        }
        for (int j = 0; j < GRIDWIDTH; j++) {
            text_red();
            if (gd->displayData[i][j] != ' ') {
                bg_white();
            } else {
                bg_black();
            }
            printf("%c ", gd->displayData[i][j]);
        }
        resetText();
        printf("\n");
    }
    displayBottomAxis();
    printf("\n");
}

void makeBox(GameData* gd) {
    for (int i = 0; i < GRIDHEIGHT; i++) {
        if (i == 0 || i + 1 == GRIDHEIGHT) {
            for (int j = 0; j < GRIDWIDTH; j++) {
                gd->displayData[i][j] = '#';
            }
        } else {
            gd->displayData[i][0] = '#';
            gd->displayData[i][GRIDWIDTH - 1] = '#';
        }
    }
}


// GAME LOGIC

char getDisplayedChar(GameData* gd, int gr, int gc) {
    if (gd->mineData[gr][gc]) {
        return '@';
    } else if (gd->adjMineData[gr][gc] == 0) {
        return '.';
    } else {
        return (char) gd->adjMineData[gr][gc] + '0';
    }
}

void revealAll(GameData* gd) {
    for (int gr = 0; gr < GRIDHEIGHT; gr++) {
        for (int gc = 0; gc < GRIDWIDTH; gc++) {
            gd->displayData[gr][gc] = getDisplayedChar(gd, gr, gc);
            gd->update[gr][gc] = 1;
        }
    }
}

void revealBFS(GameData* gd, int gr, int gc) {
    int visited[GRIDHEIGHT][GRIDWIDTH];
    for (int i = 0; i < GRIDHEIGHT; i++) {
        for (int j = 0; j < GRIDWIDTH; j++) {
            visited[i][j] = 0;
        }
    }
    
    Deque q = {NULL, NULL};
    Deque_push(&q, LLNode_new(gr, gc), 0);
    visited[gr][gc] = 1;

    while (q.front) {
        LLNode* currentNode = Deque_popFront(&q);
        
        if (gd->displayData[currentNode->gr][currentNode->gc] == ' ') {
            gd->tilesRevealed++;
        }

        gd->displayData[currentNode->gr][currentNode->gc] = getDisplayedChar(gd, currentNode->gr, currentNode->gc);
        gd->update[currentNode->gr][currentNode->gc] = 1;

        if (gd->adjMineData[currentNode->gr][currentNode->gc] == 0) {
            for (int deltr = -1; deltr <= 1; deltr++) {
                for (int deltc = -1; deltc <= 1; deltc++) {
                    if (deltr == 0 && deltc == 0) {
                        continue;
                    }

                    if (currentNode->gr + deltr < 0 || currentNode->gr + deltr >= GRIDHEIGHT || currentNode->gc + deltc < 0 || currentNode->gc + deltc >= GRIDWIDTH) {
                        continue;
                    }

                    if (visited[currentNode->gr + deltr][currentNode->gc + deltc]) {
                        continue;
                    }

                    Deque_push(&q, LLNode_new(currentNode->gr + deltr, currentNode->gc + deltc), 0);
                    visited[currentNode->gr + deltr][currentNode->gc + deltc] = 1;
                }
            }
        }

        mfree(currentNode);
    }
}

static int handle(GameData* gd, int x, int y) {
    if (x >= 0 && x < GRIDWIDTH && y >= 0 && y < GRIDHEIGHT) {
        // convert to game perspective r,c
        int gr = y;
        int gc = x;

        // if mine is picked, reveal all end program
        if (gd->mineData[gr][gc]) {
            gd->minesClicked++;
            revealAll(gd);
            return 0;
        }

        revealBFS(gd, gr, gc);

        printf("reveal state %d %d\n", gd->tilesRevealed, gd->totalMines);

        if (gd->tilesRevealed + gd->totalMines == (GRIDHEIGHT * GRIDWIDTH)) {
            return 0;
        }
    }
    return 1;
}


// GRAPHICS UTILS

int xPixel(float screenX) {
    return (int) (SCREENWIDTH * screenX);
}
int yPixel(float screenX) {
    return (int) (SCREENHEIGHT * screenX);
}
float xScreen(float pixelX) {
    return ((pixelX / SCREENWIDTH) * 2) - 1.0;
}
float yScreen(float pixelY) {
    return  ((-pixelY / SCREENHEIGHT) * 2) + 1.0;
}

// GRAPHICS FUNCS

static void handleMouseClick(int button, int state, int x, int y, void* gd_ptr) {
    if (state != GLUT_DOWN) return;
    GameData* gd = (GameData*) gd_ptr;
    printf("clicked %d %d\n", gd->cursorGridX, gd->cursorGridY);
    gd->running = handle(gd, gd->cursorGridX, gd->cursorGridY);
    gd->repaint = 1;
}

static void handleMouseMove(int mouseX, int mouseY, void* gd_ptr) {
    GameData* gd = (GameData*) gd_ptr;
    int gridX = mouseX / CELLWIDTH;
    int gridY = mouseY / CELLHEIGHT;
    gd->update[gd->cursorGridY][gd->cursorGridX] = 1;
    gd->cursorGridX = gridX;
    gd->cursorGridY = gridY;
    gd->repaint = 1;
}

void quadDraw(int pixelX, int pixelY, int pixelW, int pixelH) {
    glBegin(GL_QUADS);
    float x = xScreen(pixelX);
    float y = yScreen(pixelY);
    const float w = (2.0 * pixelW) / SCREENWIDTH ;
    const float h = (2.0 * pixelH) / SCREENHEIGHT;
    glVertex3f(x,y,0);
    glVertex3f(x+w,y,0);
    glVertex3f(x+w,y-h,0);
    glVertex3f(x,y-h,0);
    glEnd();
}

void cellDraw(int cellX, int cellY, float r, float g, float b) {
    glColor3f(r,g,b);
    quadDraw(cellX * CELLWIDTH, cellY * CELLHEIGHT, CELLWIDTH, CELLHEIGHT);
}

void cellHighlightDraw(int cellX, int cellY, float r, float g, float b) {
    glColor3f(r,g,b);
    quadDraw(cellX * CELLWIDTH + 5, cellY * CELLHEIGHT + 5, CELLWIDTH - 10, CELLHEIGHT - 10);
}

void charDraw(int pixelX, int pixelY, char c) {
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(xScreen(pixelX), yScreen(pixelY));
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
}

static void graphicsDisplay(void* gd_ptr) {
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    
    GameData* gd = (GameData*) gd_ptr;

    const double delta = t - gd->lastTime;
    gd->lastTime = t;

    if (gd->repaint) {
        gd->repaint = 0;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
        // glClear(GL_COLOR_BUFFER_BIT);

        int pixelX = 0;
        int pixelY = 13;
        int dx = 8;
        int dy = 13;

        int quadsDrawn = 0;

        double drawStart = glutGet(GLUT_ELAPSED_TIME);
        for (int i = 0; i < GRIDHEIGHT; i++) {
            for (int j = 0; j < GRIDWIDTH; j++) {
                if (gd->update[i][j] == 0) continue;
                gd->update[i][j] = 0; 

                if (gd->displayData[i][j] == ' ') {
                    cellDraw(j, i, COLOR_GREY);
                } else if (gd->displayData[i][j] == '.') {
                    cellDraw(j, i, COLOR_BLACK);
                } else if (gd->displayData[i][j] == '@') {
                    cellDraw(j, i, COLOR_RED);
                } else {
                    cellDraw(j, i, 0.125 * gd->adjMineData[i][j], 0.0, 0.0);
                }

                quadsDrawn++;
            }
        }

        double drawEnd = glutGet(GLUT_ELAPSED_TIME);

        cellHighlightDraw(gd->cursorGridX, gd->cursorGridY, COLOR_BLUE);

        glutSwapBuffers();
    } else {
        // printf("frametime: %6.3lf, drawTime: %6.3lf, quads: %5d\n", delta, 0, 0);
    }
}

void initGraphics(GameData* gd) {
    glutInitWindowSize(SCREENWIDTH,SCREENHEIGHT);
    glutInitWindowPosition(40,40);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    int pargc = 0;
    glutInit(&pargc, NULL);
    int mainWindow = glutCreateWindow("gl");
    glutDisplayFuncUcall(graphicsDisplay, (void*) gd);
    glutMouseFuncUcall(handleMouseClick, (void*) gd);
    glutPassiveMotionFuncUcall(handleMouseMove, (void*) gd);
}

void loop(GameData* gd) {
    gd->running = 1;
    while (gd->running) {
#if MEMDEBUG == 1
        printf("malloc/frees: %d %d\n", mallocsCount, freesCount);
#endif
        glutPostRedisplay();
        glutMainLoopEvent();
    }
#if MEMDEBUG == 1
    printf("malloc/frees: %d %d\n", mallocsCount, freesCount);
#endif
    glutPostRedisplay();
    glutMainLoopEvent();
    if (gd->minesClicked) {
        printf("You Lose\n");
    } else {
        printf("You win\n");
    }
    printf("Press enter to close...\n");
    getchar();
}

int main() {
    srand(time(NULL));

    initDigits();

    GameData gd;
    GameData_init(&gd);

    initGraphics(&gd);
    
    loop(&gd);

    return 0;
}