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
    int markerID;
} Robot;

typedef struct block
{
    int x;
    int y;
} Block; 

/* 
Globally used variables declared here:
The globalisation of these variables is a deliberate choice. These could just as well be 
passed into each function. However they are so often used that having these be global makes
the code neater and also more efficient.
*/
int rectSize, buffer, gridSize, waitTime, numberOfWalls, numberOfMarkers, markersRetrieved; 
Robot *robertPtr;
Block *blocksPtr;
Block *markerPtr;
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
    for(int i = 1; i < numberOfWalls + 1; i++){
        fillRect(buffer+rectSize*blocksPtr[i].x+1, buffer+rectSize*blocksPtr[i].y+1, rectSize-1, rectSize-1);
    }
} 

void drawMarker(int offset){
    setColour(lightgray);
    // If being carried, circle, if not, square
    int marker = robertPtr->markerID;
    for(int i = 0; i < numberOfMarkers; i++){
        if(!(markerPtr[i].x == blocksPtr[0].x && markerPtr[i].y == blocksPtr[0].y)){
            if(marker-1 == i){
                int direction = robertPtr->direction; 
                int x = buffer + rectSize*markerPtr[i].x + rectSize/20;
                int y = buffer + rectSize*markerPtr[i].y + rectSize/20;

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
                fillRect(buffer+rectSize*markerPtr[i].x+1, buffer+rectSize*markerPtr[i].y+1, rectSize-1, rectSize-1);
            }
        }
    }
}

void drawRobot(int offset){
    foreground();
    clear();
    int centreX, centreY, xCoords[3], yCoords[3];
    double directionRad;

    // Grabbing robert's values
    int direction = robertPtr->direction;
    robertPtr->direction = (direction + 360) % 360;
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
    Blocks = (Block*)malloc(sizeof(Block)*(numberOfWalls+1));

    // Populating the walls by separating coordinates from Str
    char *token = strtok(wallLocations, "."); 
    for(int i = 0; i < numberOfWalls + 1; i ++){
        Block temp = {0, 0};
        temp.x = atoi(token = strtok(NULL, "."));
        temp.y= atoi(token = strtok(NULL, "."));
        Blocks[i] = temp; 
    }

    return Blocks;
}

Block* initMarkers(char markerLocations[]){
    Block *Markers;
    Markers = (Block*)malloc(sizeof(Block)*(numberOfMarkers));
    // Populating the walls by separating coordinates from Str
    char *token = strtok(markerLocations, "."); 
    for(int i = 0; i < numberOfMarkers; i ++){
        Block temp = {0, 0};
        temp.x = atoi(token = strtok(NULL, "."));
        temp.y= atoi(token = strtok(NULL, "."));
        Markers[i] = temp; 
    }

    return Markers;
}

Robot nextPosition(){
    Robot temp = {robertPtr->x, robertPtr->y, robertPtr->direction, robertPtr->markerID};
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


// The required robot functions 
void forward(){
    for(int i = 1; i < 11; i++){
        drawRobot(i);
    }
    *robertPtr = nextPosition();
    if(robertPtr->markerID){
        markerPtr[robertPtr->markerID-1].x = robertPtr->x;
        markerPtr[robertPtr->markerID-1].y = robertPtr->y;
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
    for(int i = 0; i < numberOfMarkers; i++){
        if((markerPtr[i].x == robertPtr->x && markerPtr[i].y == robertPtr->y) && !(markerPtr[i].x == blocksPtr[0].x && markerPtr[i].y == blocksPtr[0].y)){
            return i+1;
        }
    }
    return 0;
}

void pickUpMarker(){
    int marker = atMarker();
    if(marker){
        robertPtr->markerID = marker;
        drawRobot(0);
    }
}

void dropMarker(){
    robertPtr->markerID = 0;
    drawRobot(0);
}

int canMoveForward(){
    Robot nextPos = nextPosition();
    // Check it is within the grid
    if(nextPos.x < 0 || nextPos.x >= gridSize || nextPos.y < 0 || nextPos.y >= gridSize){
        return 0;
    }
    // Check if there is a wall
    for(int i = 1; i < numberOfWalls + 1; i++){
        if(blocksPtr[i].x == nextPos.x && blocksPtr[i].y == nextPos.y){
            return 0;
        }
    }
    return 1;
}

int isCarryingAMarker(){
    return robertPtr->markerID;
}
//-------------------------



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
    markersRetrieved += 1;
    right();
    right();
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
        return;
    }
    return;
} 


int main(void){
    int screenResolutionY, drawableSize;
    long tempLength; 

    waitTime = 20; 
    markersRetrieved = 0;
    
    // Input Parameters
    screenResolutionY = 982; 
    gridSize = 8; 
    char wallLocations[] = "-.1.0.1.2";
    char markerLocations[] = "-.0.1.5.0";


    // Calculated Parameters
    drawableSize = screenResolutionY - 210; // 210 pixels of the screen is drawApp unusable space
    rectSize = drawableSize / (gridSize+1);
    buffer = ((drawableSize % rectSize) + rectSize) / 2; 

    tempLength = strlen(wallLocations);
    numberOfWalls = ((tempLength - 1) / 4) - 1;

    tempLength = strlen(markerLocations);
    numberOfMarkers = (tempLength - 1) / 4;

    // Environment Set-Up
    Block *Blocks = initBlocks(wallLocations);
    blocksPtr = Blocks;

    Block *Markers = initMarkers(markerLocations);
    markerPtr = Markers;

    Robot robert = {Blocks[0].x, Blocks[0].y, 0, 0};
    robertPtr = &robert;

    movementStack = createStack();

    // Drawing Methods
    setWindowSize(drawableSize, drawableSize); 
    drawBackground();
    drawRobot(0);

    while(markersRetrieved != numberOfMarkers){
        solve();
    } 

    free(movementStack);
    free(blocksPtr);
    free(markerPtr);
    return 0; 
}