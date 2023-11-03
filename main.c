#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "graphics.h"

// Globally required variables, set in main
int rectSize, buffer, waitTime; 

double radian(double degrees){
    return (degrees * (M_PI / 180));
}

typedef struct robot
{
    int x;
    int y;
    int direction;
} Robot;


void drawBackground(int gridSize){
    background();
    for(int i = 0; i < gridSize; i++){
        for(int j = 0; j < gridSize; j++){
            drawRect((buffer + j*rectSize), (buffer + i*rectSize), rectSize, rectSize);
        }
    } //Nested for loop draws SIZExSIZE grid of rectangles   
} 

void drawRobot(struct robot *robertPtr, int offset){
    foreground();
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

}

void forward(struct robot *robertPtr){
    for(int i = 1; i < 11; i++){
        clear();
        drawRobot(robertPtr, i);
        sleep(waitTime);
    }
    if(robertPtr->direction == 0){
        robertPtr->y -= 1;
    }else if(robertPtr->direction == 90){
        robertPtr->x += 1;
    }else if(robertPtr->direction == 180){
        robertPtr->y += 1;
    }else if(robertPtr->direction == 270){
        robertPtr->x -= 1;
    }
}

void left(struct robot *robertPtr){
    for(int i = 0; i < 18; i++){
        robertPtr->direction -= 5;
        clear();
        drawRobot(robertPtr, 0);
        sleep(waitTime);
    }
}

void right(struct robot *robertPtr){
    for(int i = 0; i < 18; i++){
        robertPtr->direction += 5;
        clear();
        drawRobot(robertPtr, 0);
        sleep(waitTime);
    }
}

int main(void){
    int screenResolutionY, drawableSize, gridSize; 
    int randomPlacement = 0; //Allows for random placement of objects 

    waitTime = 25; 
    Robot robert = {0, 0, 0};
    Robot *robertPtr = &robert;

    // Input Parameters
    screenResolutionY = 982; 
    gridSize = 8; 

    // Calculated Parameters
    drawableSize = screenResolutionY - 210; // 210 pixels of the screen is drawApp unusable space
    rectSize = drawableSize / (gridSize+1);
    buffer = ((drawableSize % rectSize) + rectSize) / 2; 

    // Drawing Methods
    setWindowSize(drawableSize, drawableSize); 
    drawBackground(gridSize);

    drawRobot(robertPtr, 0);

    
    // right(robertPtr);
    // forward(robertPtr);
    // forward(robertPtr);
    // forward(robertPtr);
    // right(robertPtr);
    // forward(robertPtr);
    // left(robertPtr);
    


    return 0; 
}
