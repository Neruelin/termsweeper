#include <stdio.h>
#include <stdlib.h>
#define SCREENWIDTH 50
#define SCREENHEIGHT 20
#define THRESHOLD 90
#define MEMDEBUG 0

int longestDigits = 0;

int initDigits() {
    for (int temp = 1; temp < SCREENWIDTH; temp *= 10) {
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
    char displayData[SCREENHEIGHT][SCREENWIDTH];
    int mineData[SCREENHEIGHT - 2][SCREENWIDTH - 2];
    int adjMineData[SCREENHEIGHT - 2][SCREENWIDTH - 2];
    int minesClicked;
    int tilesRevealed;
    int totalMines;
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
    for (int i = 0; i < SCREENHEIGHT; i++) {
        for (int j = 0; j < SCREENWIDTH; j++) {
            gd->displayData[i][j] = ' ';
        }
    }
}

void GameData_initGameData(GameData* gd) {
    for (int i = 0; i < SCREENHEIGHT - 2; i++) {
        for (int j = 0; j < SCREENWIDTH - 2; j++) {
            gd->mineData[i][j] = (rand() % 100 > THRESHOLD) ? 1 : 0;
            if (gd->mineData[i][j]) {
                gd->totalMines++;
                // break;
            }
        }
        // break;
    }
    for (int i = 0; i < SCREENHEIGHT - 2; i++) {
        for (int j = 0; j < SCREENWIDTH - 2; j++) {
            int mines = 0;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) {
                        continue;
                    }
                    if (i + dy < 0 || i + dy >= SCREENHEIGHT - 2 || j + dx < 0 || j + dx >= SCREENWIDTH - 2) {
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
    for (int j = 0; j < SCREENWIDTH * 2 + longestDigits + 1; j++) {
        printf(" ");
    }
    printf("\r");
}

void clearInputAction() {
    moveCursorUp();
    clearLine();
}

void clearScreen() {
    for (int i = 0; i < SCREENHEIGHT + longestDigits; i++) {
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
    for (int i = 0; i < SCREENWIDTH - 2; i++) {
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
    for (int i = 0; i < SCREENHEIGHT; i++) {
        if (i != 0 && i != SCREENHEIGHT - 1) {
            printf("%2d ", SCREENHEIGHT - 2 - i);
        } else {
            printf("   ");
        }
        for (int j = 0; j < SCREENWIDTH; j++) {
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
    for (int i = 0; i < SCREENHEIGHT; i++) {
        if (i == 0 || i + 1 == SCREENHEIGHT) {
            for (int j = 0; j < SCREENWIDTH; j++) {
                gd->displayData[i][j] = '#';
            }
        } else {
            gd->displayData[i][0] = '#';
            gd->displayData[i][SCREENWIDTH - 1] = '#';
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
    for (int gr = 0; gr < SCREENHEIGHT - 2; gr++) {
        for (int gc = 0; gc < SCREENWIDTH - 2; gc++) {
            gd->displayData[gr + 1][gc + 1] = getDisplayedChar(gd, gr, gc);
        }
    }
}

void revealBFS(GameData* gd, int gr, int gc) {
    int visited[SCREENHEIGHT - 2][SCREENWIDTH - 2];
    for (int i = 0; i < SCREENHEIGHT - 2; i++) {
        for (int j = 0; j < SCREENWIDTH - 2; j++) {
            visited[i][j] = 0;
        }
    }
    
    Deque q = {NULL, NULL};
    Deque_push(&q, LLNode_new(gr, gc), 0);
    visited[gr][gc] = 1;

    while (q.front) {
        LLNode* currentNode = Deque_popFront(&q);
        
        if (gd->displayData[currentNode->gr + 1][currentNode->gc + 1] == ' ') {
            gd->tilesRevealed++;
        }

        gd->displayData[currentNode->gr + 1][currentNode->gc + 1] = getDisplayedChar(gd, currentNode->gr, currentNode->gc);

        if (gd->adjMineData[currentNode->gr][currentNode->gc] == 0) {
            for (int deltr = -1; deltr <= 1; deltr++) {
                for (int deltc = -1; deltc <= 1; deltc++) {
                    if (deltr == 0 && deltc == 0) {
                        continue;
                    }

                    if (currentNode->gr + deltr < 0 || currentNode->gr + deltr >= SCREENHEIGHT - 2 || currentNode->gc + deltc < 0 || currentNode->gc + deltc >= SCREENWIDTH - 2) {
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

int handle(GameData* gd, int x, int y) {
    if (x >= 0 && x < SCREENWIDTH - 2 && y >= 0 && y < SCREENHEIGHT - 2) {
        // convert to game perspective r,c
        int gr = SCREENHEIGHT - 2 - y - 1;
        int gc = x;

        // if mine is picked, reveal all end program
        if (gd->mineData[gr][gc]) {
            gd->minesClicked++;
            revealAll(gd);
            return 0;
        }

        revealBFS(gd, gr, gc);

        if (gd->tilesRevealed + gd->totalMines == (SCREENHEIGHT - 2) * (SCREENWIDTH - 2)) {
            return 0;
        }
    }
    return 1;
}

void loop(GameData* gd) {
    int running = 1;
    while (running) {
#if MEMDEBUG == 1
        printf("malloc/frees: %d %d\n", mallocsCount, freesCount);
#endif
        display(gd);

        int x;
        int y;
        scanf("%d %d", &x, &y); // user perspective (0, 0) in bottom left
        clearInputAction();

        running = handle(gd, x, y);

        clearScreen();
    }
#if MEMDEBUG == 1
    printf("malloc/frees: %d %d\n", mallocsCount, freesCount);
#endif
    display(gd);
    if (gd->minesClicked) {
        printf("You Lose\n");
    } else {
        printf("You win\n");
    }
}

int main() {

    initDigits();

    GameData gd;

    GameData_initDisplayData(&gd);
    GameData_initGameData(&gd);

    makeBox(&gd);
    
    loop(&gd);

    return 0;
}