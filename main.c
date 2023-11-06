#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "graphics.h"


typedef struct robot
{
    int x;
    int y;
    int direction;
} Robot;

typedef struct block
{
    int x;
    int y;
} Block; 

// Globally required variables, set in main
int rectSize, buffer, waitTime, numberOfWalls;
Robot *robertPtr;
Block *blocksPtr;


double radian(double degrees){
    return (degrees * (M_PI / 180));
}

void drawBackground(int gridSize){
    background();
    for(int i = 0; i < gridSize; i++){
        for(int j = 0; j < gridSize; j++){
            drawRect((buffer + j*rectSize), (buffer + i*rectSize), rectSize, rectSize);
        }
    } //Nested for loop draws SIZExSIZE grid of rectangles   
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

    setColour(green);
    fillPolygon(3, xCoords, yCoords);

    sleep(waitTime);

}

Block* initBlocks(int numberOfWalls, Block home, Block marker, char wallLocations[]){
    Block *Blocks;
    Blocks = (Block*)calloc(numberOfWalls+2, sizeof(Block));
    Blocks[0] = home;
    Blocks[1] = marker; 

    // Populating the walls by separating coordinates from Str
    char *token = strtok(wallLocations, "."); 
    for(int i = 2; i < numberOfWalls + 2; i ++){
        Block temp = {0, 0};
        temp.x = atoi(token = strtok(NULL, "."));
        temp.y= atoi(token = strtok(NULL, "."));
        Blocks[i] = temp; 
    }

    return Blocks;
}

Robot nextPosition(){
    Robot temp = {robertPtr->x, robertPtr->y, robertPtr->direction};
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

int atMarker(){
    if(blocksPtr[1].x == robertPtr->x && blocksPtr[1].y == robertPtr->y){
        return 1;
    }else{
        return 0;
    }
}

int atHome(){
    if(blocksPtr[0].x == robertPtr->x && blocksPtr[0].y == robertPtr->y){
        return 1;
    }else{
        return 0;
    }
}

int canMoveForward(){
    Robot nextPos = nextPosition();
    for(int i = 2; i < numberOfWalls + 2; i++){
        if(blocksPtr[i].x == nextPos.x && blocksPtr[i].y == nextPos.y){
            return 0;
        }
    }
    return 1;
}

int main(void){
    int screenResolutionY, drawableSize, gridSize; 
    int randomPlacement = 0; //Allows for random placement of objects 

    waitTime = 20; 
    numberOfWalls = 1;
    
    // Input Parameters
    screenResolutionY = 982; 
    gridSize = 8; 
    Block home = {0, 0};
    Block marker = {1, 0};
    char wallLocations[] = "-.0.0";

    // Calculated Parameters
    drawableSize = screenResolutionY - 210; // 210 pixels of the screen is drawApp unusable space
    rectSize = drawableSize / (gridSize+1);
    buffer = ((drawableSize % rectSize) + rectSize) / 2; 

    // Environment Set-Up
    Robot robert = {0, 1, 0};
    robertPtr = &robert;

    Block *Blocks = initBlocks(numberOfWalls, home, marker, wallLocations);
    blocksPtr = Blocks;

    // Drawing Methods
    setWindowSize(drawableSize, drawableSize); 
    drawBackground(gridSize);
    drawRobot(0);
    
    // right();
    // forward();
    // forward();
    // forward();
    // right();
    // forward();
    // left();

    return 0; 
}