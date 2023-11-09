#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "graphics.h"


// Stack data structure for robert's memory (ChatGPT, 2023)
typedef struct StackNode {
    int data;
    struct StackNode* next;
} StackNode;
typedef struct Stack {
    StackNode* top;
} Stack;
Stack* createStack() {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    if (!stack) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }
    stack->top = NULL;
    return stack;
}
void push(Stack* stack, int movement) {
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
    if (!newNode) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }
    newNode->data = movement;
    newNode->next = stack->top;
    stack->top = newNode;
}
int pop(Stack* stack) {
    if (stack->top == NULL) {
        return 0;
    }
    StackNode* poppedNode = stack->top;
    int movement = poppedNode->data;
    stack->top = poppedNode->next;
    free(poppedNode);
    return movement;
}



// Robot Code

typedef struct robot
{
    int x;
    int y;
    int direction;
    int isCarryingMarker;
} Robot;

typedef struct block
{
    int x;
    int y;
} Block; 

// Globally used variables, required numerous times
int rectSize, buffer, gridSize, waitTime, numberOfWalls; 
Robot *robertPtr;
Block *blocksPtr;
Stack* movementStack;


double radian(double degrees){
    return (degrees * (M_PI / 180));
}

void drawBackground(){
    background();
    // Nested for loop draws SIZExSIZE grid of rectangles  
    for(int i = 0; i < gridSize; i++){
        for(int j = 0; j < gridSize; j++){
            drawRect((buffer + j*rectSize), (buffer + i*rectSize), rectSize, rectSize);
        }
    } 
    // Draw Home Square
    setColour(blue);
    fillRect(buffer+rectSize*blocksPtr[0].x+1, buffer+rectSize*blocksPtr[0].y+1, rectSize-1, rectSize-1);

    // Draw Walls
    setColour(black);
    for(int i = 2; i < numberOfWalls + 2; i++){
        fillRect(buffer+rectSize*blocksPtr[i].x+1, buffer+rectSize*blocksPtr[i].y+1, rectSize-1, rectSize-1);
    }
} 

void drawMarker(int offset){
    setColour(lightgray);
    // If being carried, circle, if not, square
    if(robertPtr->isCarryingMarker){

        int direction = robertPtr->direction; 
        int x = buffer + rectSize*blocksPtr[1].x + rectSize/20;
        int y = buffer + rectSize*blocksPtr[1].y + rectSize/20;

        // Offsetting the marker to move with the robot
        if(direction == 0){
            y = y - offset;
        }else if(direction == 90){
            x = x + offset;
        }else if(direction == 180){
            y = y + offset;
        }else if(direction == 270){
            x = x - offset;
        }
        
        fillOval(x, y, 18*rectSize/20, 18*rectSize/20);
    }else{
        fillRect(buffer+rectSize*blocksPtr[1].x+1, buffer+rectSize*blocksPtr[1].y+1, rectSize-1, rectSize-1);
    }
}

void drawRobot(int offset){
    foreground();
    clear();
    int centreX, centreY, xCoords[3], yCoords[3];
    double directionRad;

    // Grabbing robert's values
    int direction = robertPtr->direction;
    robertPtr->direction = direction % 360;
    int x = robertPtr->x;
    int y = robertPtr->y; 

    // Calculating required variables 
    centreX = (buffer + rectSize * x) + (rectSize / 2);
    centreY = (buffer + rectSize * y) + (rectSize / 2);
    directionRad = radian(direction);
    offset = offset * rectSize / 10;

    // Drawing the robot
    int trigHeight = rectSize * 0.8; 
    xCoords[0] = (centreX);
    yCoords[0] = (centreY - trigHeight / 2);
    xCoords[1] = (centreX + trigHeight / 2);
    yCoords[1] = (centreY + trigHeight * cos(radian(30)) / 2);
    xCoords[2] = (centreX - trigHeight / 2);
    yCoords[2] = (centreY + trigHeight * cos(radian(30)) / 2) ;

    for(int i = 0; i < 3; i++){
        int tempX = xCoords[i];
        int tempY = yCoords[i];

        // Rotating robot if it is a turning animation
        xCoords[i] = centreX + cos(directionRad) * (tempX - centreX) - sin(directionRad) * (tempY - centreY);
        yCoords[i] = centreY + sin(directionRad) * (tempX - centreX) + cos(directionRad) * (tempY - centreY);

        // Offsetting robot if it is a moving animation
        if(direction == 0){
            yCoords[i] = yCoords[i] - offset;
        }else if(direction == 90){
            xCoords[i] = xCoords[i] + offset;
        }else if(direction == 180){
            yCoords[i] = yCoords[i] + offset;
        }else if(direction == 270){
            xCoords[i] = xCoords[i] - offset;
        }
    }

    drawMarker(offset);
    setColour(green);
    fillPolygon(3, xCoords, yCoords);

    sleep(waitTime);
}


Block* initBlocks(char wallLocations[]){
    Block *Blocks;
    Blocks = (Block*)malloc(sizeof(Block)*(numberOfWalls+2));

    // Populating the walls by separating coordinates from Str
    char *token = strtok(wallLocations, "."); 
    for(int i = 0; i < numberOfWalls + 2; i ++){
        Block temp = {0, 0};
        temp.x = atoi(token = strtok(NULL, "."));
        temp.y= atoi(token = strtok(NULL, "."));
        Blocks[i] = temp; 
    }

    return Blocks;
}

Robot nextPosition(){
    Robot temp = {robertPtr->x, robertPtr->y, robertPtr->direction, robertPtr->isCarryingMarker};
    if(robertPtr->direction == 0){
        temp.y -= 1;
    }else if(robertPtr->direction == 90){
        temp.x += 1;
    }else if(robertPtr->direction == 180){
        temp.y += 1;
    }else if(robertPtr->direction == 270){
        temp.x -= 1;
    }
    return temp;
}

void forward(){
    for(int i = 1; i < 11; i++){
        drawRobot(i);
    }
    *robertPtr = nextPosition();
    if(robertPtr->isCarryingMarker){
        blocksPtr[1].x = robertPtr->x;
        blocksPtr[1].y = robertPtr->y;
    }
}

void left(){
    for(int i = 0; i < 18; i++){
        robertPtr->direction -= 5;
        drawRobot(0);
    }
}

void right(){
    for(int i = 0; i < 18; i++){
        robertPtr->direction += 5;
        drawRobot(0);
    }
}


int atHome(){
    if(blocksPtr[0].x == robertPtr->x && blocksPtr[0].y == robertPtr->y){
        return 1;
    }else{
        return 0;
    }
}

int atMarker(){
    if(blocksPtr[1].x == robertPtr->x && blocksPtr[1].y == robertPtr->y){
        return 1;
    }else{
        return 0;
    }
}

void pickUpMarker(){
    if(atMarker()){
        robertPtr->isCarryingMarker = 1;
        drawRobot(0);
    }
}

void dropMarker(){
    robertPtr->isCarryingMarker = 0;
    drawRobot(0);
}

int canMoveForward(){
    Robot nextPos = nextPosition();
    if(nextPos.x < 0 || nextPos.x >= gridSize || nextPos.y < 0 || nextPos.y >= gridSize){
        return 0;
    }
    for(int i = 2; i < numberOfWalls + 2; i++){
        if(blocksPtr[i].x == nextPos.x && blocksPtr[i].y == nextPos.y){
            return 0;
        }
    }
    return 1;
}

int isCarryingAMarker(){
    return robertPtr->isCarryingMarker;
}

void goHome(){
    // Turn around and pop the previous movements to go back
    right();
    right();
    while(movementStack->top != NULL){
        int movement = pop(movementStack);
        switch (movement){
            case 1:
                forward();
                break;
            case 2:
                right();
                break;
            case 3:
                left();
                break;
            default:
                break;
        }
    }
    dropMarker();
}

void solve(){
    if(atMarker()){
        pickUpMarker();
        goHome();
        return;
    }else if (canMoveForward()){
        forward();
        push(movementStack, 1);
        solve();
        return;
    }else{
        right();
        push(movementStack, 3);
        solve();
    }
    return;
} 


int main(void){
    int screenResolutionY, drawableSize; 
    int randomPlacement = 0; //Allows for random placement of objects 

    waitTime = 20; 
    numberOfWalls = 1;
    
    // Input Parameters
    screenResolutionY = 982; 
    gridSize = 8; 

    char wallLocations[] = "-.0.1.1.0.0.2";

    // Calculated Parameters
    drawableSize = screenResolutionY - 210; // 210 pixels of the screen is drawApp unusable space
    rectSize = drawableSize / (gridSize+1);
    buffer = ((drawableSize % rectSize) + rectSize) / 2; 

    // Environment Set-Up
    Block *Blocks = initBlocks(wallLocations);
    blocksPtr = Blocks;

    Robot robert = {Blocks[0].x, Blocks[0].y, 0, 0};
    robertPtr = &robert;

    movementStack = createStack();

    // Drawing Methods
    setWindowSize(drawableSize, drawableSize); 
    drawBackground();
    drawRobot(0);
    
    // right();
    // forward();
    // left();
    // forward();
    // pickUpMarker();
    // right();
    // forward();
    // forward();
    // forward();
    // forward();
    // forward();
    // forward();
    // dropMarker();

    solve();

    free(movementStack);
    return 0; 
}