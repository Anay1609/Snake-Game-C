#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

#define WIDTH 30
#define HEIGHT 15

// Colors
#define GREEN 10
#define RED 12
#define YELLOW 14
#define BLUE 9
#define WHITE 15

typedef struct Node {
    int x, y;
    struct Node* next;
} Node;

typedef struct LL {
    Node* start;
} LL;

LL snake;           
char direction = 'd';       
int foodX, foodY;
int specialFoodX = -1, specialFoodY = -1;
int score = 0;
int gameSpeed = 150; // milliseconds between moves
int isPaused = 0;
int hasObstacles = 0;
int obstacleX[5], obstacleY[5];
int specialFoodTimer = 0;
int specialFoodActive = 0;

Node* createNode(int x, int y) {
    Node* newrec = (Node*)malloc(sizeof(Node));
    newrec->x = x;
    newrec->y = y;
    newrec->next = NULL;
    return newrec;
}

void addStart(int x, int y) {
    Node* newStart = createNode(x, y);
    newStart->next = snake.start;
    snake.start = newStart;
}

void removeTail() {
    Node* p = snake.start;
    if (!p || !p->next) return;

    while (p->next->next) {
        p = p->next;
    }
    free(p->next);
    p->next = NULL;
}

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void clearScreen() {
    system("cls");
}

int isCollision(int x, int y) {
    if (x <= 0 || x >= WIDTH - 1 || y <= 0 || y >= HEIGHT - 1)
        return 1;

    // Check collision with obstacles
    if (hasObstacles) {
        for (int i = 0; i < 5; i++) {
            if (x == obstacleX[i] && y == obstacleY[i])
                return 1;
        }
    }

    Node* p = snake.start;
    while (p) {
        if (p->x == x && p->y == y)
            return 1;
        p = p->next;
    }
    return 0;
}

void placeObstacles() {
    for (int i = 0; i < 5; i++) {
        while (1) {
            obstacleX[i] = rand() % (WIDTH - 2) + 1;
            obstacleY[i] = rand() % (HEIGHT - 2) + 1;
            
            // Make sure obstacle is not on snake
            Node* p = snake.start;
            int collision = 0;
            while (p) {
                if (p->x == obstacleX[i] && p->y == obstacleY[i]) {
                    collision = 1;
                    break;
                }
                p = p->next;
            }
            
            // Make sure obstacle is not on food
            if (obstacleX[i] == foodX && obstacleY[i] == foodY)
                collision = 1;
                
            // Make sure obstacle is not on another obstacle
            for (int j = 0; j < i; j++) {
                if (obstacleX[i] == obstacleX[j] && obstacleY[i] == obstacleY[j]) {
                    collision = 1;
                    break;
                }
            }
            
            if (!collision) break;
        }
    }
}

void placeSpecialFood() {
    if (rand() % 5 != 0) return; // 20% chance to spawn special food
    
    specialFoodActive = 1;
    specialFoodTimer = 10; // Special food disappears after 10 moves
    
    while (1) {
        specialFoodX = rand() % (WIDTH - 2) + 1;
        specialFoodY = rand() % (HEIGHT - 2) + 1;
        
        // Check not on snake
        Node* p = snake.start;
        int collision = 0;
        while (p) {
            if (p->x == specialFoodX && p->y == specialFoodY) {
                collision = 1;
                break;
            }
            p = p->next;
        }
        
        // Check not on regular food
        if (specialFoodX == foodX && specialFoodY == foodY)
            collision = 1;
            
        // Check not on obstacles
        if (hasObstacles) {
            for (int i = 0; i < 5; i++) {
                if (specialFoodX == obstacleX[i] && specialFoodY == obstacleY[i]) {
                    collision = 1;
                    break;
                }
            }
        }
        
        if (!collision) break;
    }
}

void placeFood() {
    while (1) {
        foodX = rand() % (WIDTH - 2) + 1;
        foodY = rand() % (HEIGHT - 2) + 1;

        // Check not on snake
        Node* p = snake.start;
        int collision = 0;
        while (p) {
            if (p->x == foodX && p->y == foodY) {
                collision = 1;
                break;
            }
            p = p->next;
        }
        
        // Check not on obstacles
        if (hasObstacles) {
            for (int i = 0; i < 5; i++) {
                if (foodX == obstacleX[i] && foodY == obstacleY[i]) {
                    collision = 1;
                    break;
                }
            }
        }
        
        // Check not on special food
        if (specialFoodActive && foodX == specialFoodX && foodY == specialFoodY)
            collision = 1;
            
        if (!collision) break;
    }
}

void drawBoard() {
    clearScreen();
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == 0 || x == WIDTH - 1 || y == 0 || y == HEIGHT - 1) {
                setColor(BLUE);
                if ((x == 0 || x == WIDTH - 1) && (y == 0 || y == HEIGHT - 1)) {
                    printf("+"); // Corner pieces
                } else if (y == 0 || y == HEIGHT - 1) {
                    printf("-"); // Horizontal border
                } else if (x == 0 || x == WIDTH - 1) {
                    printf("|"); // Vertical border
                }
            } else if (hasObstacles) {
                int isObstacle = 0;
                for (int i = 0; i < 5; i++) {
                    if (x == obstacleX[i] && y == obstacleY[i]) {
                        setColor(RED);
                        printf("X");
                        isObstacle = 1;
                        break;
                    }
                }
                if (isObstacle) continue;
            }
            
            if (x == foodX && y == foodY) {
                setColor(RED);
                printf("*"); 
            } else if (specialFoodActive && x == specialFoodX && y == specialFoodY) {
                setColor(YELLOW);
                printf("$"); 
            } else {
                Node* p = snake.start;
                int printed = 0;
                while (p) {
                    if (p->x == x && p->y == y) {
                        if (p == snake.start) {
                            setColor(GREEN);
                            printf("O");
                        } else {
                            setColor(GREEN);
                            printf("o");
                        }
                        printed = 1;
                        break;
                    }
                    p = p->next;
                }
                if (!printed) {
                    setColor(WHITE);
                    printf(" "); 
                }
            }
        }
        setColor(WHITE);
        printf("\n");
    }
    
    setColor(WHITE);
    printf("Score: %d\n", score);
    printf("Speed: %d ms\n", gameSpeed);
    if (isPaused)
        printf("GAME PAUSED - Press P to resume\n");
    else
        printf("Controls: W/A/S/D to move, P to pause, Q to quit\n");
}

void moveSnake() {
    if (isPaused) return;
    
    int newX = snake.start->x;
    int newY = snake.start->y;

    if (direction == 'w') newY--;
    else if (direction == 's') newY++;
    else if (direction == 'a') newX--;
    else if (direction == 'd') newX++;

    if (isCollision(newX, newY)) {
        clearScreen();
        setColor(RED);
        printf("\nGame Over!\n");
        setColor(YELLOW);
        printf("Final Score: %d\n", score);
        setColor(WHITE);
        exit(0);
    }

    addStart(newX, newY);

    if (newX == foodX && newY == foodY) {
        score++;
        placeFood();
        
        // Chance to place special food when eating regular food
        if (!specialFoodActive)
            placeSpecialFood();
    } else if (specialFoodActive && newX == specialFoodX && newY == specialFoodY) {
        score += 3;
        specialFoodActive = 0;
        specialFoodX = -1;
        specialFoodY = -1;
        
        // Speed up the game slightly
        if (gameSpeed > 50)
            gameSpeed -= 5;
    } else {
        removeTail();
    }
    
    // Update special food timer
    if (specialFoodActive) {
        specialFoodTimer--;
        if (specialFoodTimer <= 0) {
            specialFoodActive = 0;
            specialFoodX = -1;
            specialFoodY = -1;
        }
    }
}

void showMenu() {
    clearScreen();
    setColor(GREEN);
    printf("===================================\n");
    setColor(BLUE);
    printf("           SNAKE GAME          \n");
    setColor(GREEN);
    printf("===================================\n\n");
    setColor(WHITE);
    printf("INSTUCTIONS::\n");
    setColor(GREEN);
    printf("O is the snake head\no is the snake body\n");
    setColor(RED);
    printf("* ");
    setColor(GREEN);
    printf("is the normal food\n");
    setColor(YELLOW);
    printf("$ ");
    setColor(GREEN);
    printf("is special food\n");
    setColor(RED);
    printf("X");
    setColor(GREEN);
    printf(" are obstacles\n");
    setColor(BLUE);
    printf("- ");
    setColor(GREEN);
    printf("and ");
    setColor(BLUE);
    printf("|");
    setColor(GREEN);
    printf(" are the borders\n\n");
    
    setColor(WHITE);
    printf("Select Difficulty:\n");
    
    setColor(GREEN);
    printf("1. Easy (Slow speed, no obstacles)\n");
    
    setColor(YELLOW);
    printf("2. Medium (Medium speed, no obstacles)\n");
    
    setColor(RED);
    printf("3. Hard (Fast speed with obstacles)\n");
    
    setColor(WHITE);
    printf("\nPress 1, 2, or 3 to select: ");
    
    char choice;
    do {
        choice = getch();
    } while (choice != '1' && choice != '2' && choice != '3');
    
    switch (choice) {
        case '1':
            gameSpeed = 450;
            hasObstacles = 0;
            break;
        case '2':
            gameSpeed = 350;
            hasObstacles = 0;
            break;
        case '3':
            gameSpeed = 250;
            hasObstacles = 1;
            placeObstacles();
            break;
    }
}

int main() {
    srand(time(NULL));

    showMenu();
    
    snake.start = createNode(WIDTH / 2, HEIGHT / 2);
    placeFood();

    char input = 0;
    
    while (1) {
        drawBoard();
        
        // Non-blocking input check
        if (_kbhit()) {
            input = getch();
            if (input == 'p' || input == 'P')
                isPaused = !isPaused;
            else if (input == 'q' || input == 'Q') {
                clearScreen();
                printf("Game ended. Final score: %d\n", score);
                exit(0);
            }
            else if (!isPaused && (input == 'w' || input == 'a' || input == 's' || input == 'd'))
                direction = input;
        }

        moveSnake();
        Sleep(gameSpeed);
    }

    return 0;
}

