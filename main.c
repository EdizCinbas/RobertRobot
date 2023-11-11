#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "graphics.h"

// Custom Structs
typedef struct robot
{
    int x;
    int y;
    int direction;
    int markerID; // 0 means no marker
} Robot;

typedef struct block
{
    int x;
    int y;
} Block; 


// Stack data structure for robert's movement memory (ChatGPT, 2023)
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

// Linked list data structure for robert's positional memory (ChatGPT, 2023)
struct Node {
    Block data;
    struct Node* next;
};
void append(struct Node** head, Block data) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->data = data;
    newNode->next = NULL;

    if (*head == NULL) {
        *head = newNode;
    } else {
        struct Node* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}
int isInside(struct Node* head, int x, int y) {
    struct Node* temp = head;
    while (temp != NULL) {
        if(temp->data.x == x && temp->data.y == y){
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}
void resetList(struct Node** head) {
    // Free the memory of all nodes
    struct Node* current = *head;
    struct Node* next;
    
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    // Set the head to NULL
    *head = NULL;
}


// Robot Code

// Some variables are deliberately declared globally to keep the code neater and more efficient. 

// Drawing function variables, solution algorithm does not have access, as a real robot would not know these.
int rectSize, buffer, gridSize, waitTime, numberOfWalls, numberOfMarkers, markersRetrieved; 
Robot *robertPtr;
Block *blocksPtr;
Block *markerPtr;


// These are the variables the robot has access to. No information is given to the robot about surroundings or itself. 
Robot guessRobert = {0, 0, 0, 0}; // Keeps track of its 'guess' position which is the relative position taking home to be (0,0)
struct Node* visitedList = NULL; // Keeps track of the coordinates its 'guess' position has been to
Stack* movementStack; // Keeps track of its last moves



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
    int marker = robertPtr->markerID;
    for(int i = 0; i < numberOfMarkers; i++){
        if(!(markerPtr[i].x == blocksPtr[0].x && markerPtr[i].y == blocksPtr[0].y)){ // Draw if marker is not home
            if(marker-1 == i){ // If being carried, circle
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
            }else{ // If not, square
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


Block* initBlocks(char *wallLocations){
    Block *Blocks;
    Blocks = (Block*)malloc(sizeof(Block)*(numberOfWalls+1)); // home square is also in this array since it is a background object

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

Block* initMarkers(char *markerLocations){
    Block *Markers;
    Markers = (Block*)malloc(sizeof(Block)*(numberOfMarkers));

    /* Note: This array indexes at 0, but the markerID 0 is used as 'No Markers'.
    Thus when acessing this array 'MarkerID - 1' is used, and when returning, 'index + 1' */
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
        drawRobot(i); // Draw the robot with increasing offsets to animate moving forward
    }
    *robertPtr = nextPosition();
    if(robertPtr->markerID){
        markerPtr[robertPtr->markerID-1].x = robertPtr->x;
        markerPtr[robertPtr->markerID-1].y = robertPtr->y;
    }
}

void left(){
    for(int i = 0; i < 18; i++){
        robertPtr->direction -= 5; // Draw the robot with increasing direction to animate rotation
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
            return i+1; // the markerID is 'index + 1' as noted in init
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
    // Note: this method does not differentiate between wall or out of bounds, as a real robot would not know

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


// Solution methods

void goHome(){
    // Retraces the moves back to the home square
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

void simpleSolve(){
    // This was the first solution for up to stage 3, without using dynamic memory
    if(atMarker()){
        pickUpMarker();
        goHome();
        return;
    }else if (canMoveForward()){
        forward();
        push(movementStack, 1);
        simpleSolve();
        return;
    }else{
        right();
        push(movementStack, 3);
        simpleSolve();
        return;
    }
    return;
} 

void goBack(int step){
    // Goes back step amount of squares (but only used to go back 1 at a time)
    while(movementStack->top != NULL && step){
        int movement = pop(movementStack);
        if(movement == 1){
            step -= 1;
        }
    }
    forward();
    left();
    left();
}


// guessRobert is where robert 'guesses' itself to be, relative to where it was initially placed
void guessRobertReset(){
    guessRobert.x = 0;
    guessRobert.y = 0;
    guessRobert.direction = 0;
    resetList(&visitedList);
}

void guessRobertMove(){
    // Moves the robot's relative position according to its relative direction.
    guessRobert.direction = (guessRobert.direction + 360) % 360;
    if(guessRobert.direction == 0){
        guessRobert.y += 1;
    }else if(guessRobert.direction == 90){
        guessRobert.x += 1;
    }else if(guessRobert.direction == 180){
        guessRobert.y -= 1;
    }else if(guessRobert.direction == 270){
        guessRobert.x -= 1;
    }
}

Robot guessRobertNext(){
    // Calculates the robot's relative next position.
    guessRobert.direction = (guessRobert.direction + 360) % 360;
    Robot temp = {guessRobert.x, guessRobert.y, guessRobert.direction, 0};
    if(guessRobert.direction == 0){
        temp.y += 1;
    }else if(guessRobert.direction == 90){
        temp.x += 1;
    }else if(guessRobert.direction == 180){
        temp.y -= 1;
    }else if(guessRobert.direction == 270){
        temp.x -= 1;
    }
    return temp;
}

void complexSolve(int step){
    if(atMarker()){
        pickUpMarker();
        return;
    }else{
        if(step == 0){ // Step 0 means we have not checked any paths from the current square
            left();
            guessRobert.direction -= 90;
            push(movementStack, 2); 
        }
        while(step < 3 && !isCarryingAMarker()){ // Step 3 means we have checked every path from the current square
            if(canMoveForward() && !(nextPosition().x == blocksPtr[0].x && nextPosition().y == blocksPtr[0].y) && !isInside(visitedList, guessRobertNext().x, guessRobertNext().y)){
                forward();
                push(movementStack, 1);

                guessRobertMove();
                Block temp = {guessRobert.x, guessRobert.y};
                append(&visitedList, temp);

                complexSolve(0); // Call the search again in this new block
            }else{
                right(); // Turn right to check new path
                guessRobert.direction += 90;
                push(movementStack, 3);
                step += 1;
            }
        }
        if(!isCarryingAMarker()){ // If every path has been checked, robot retraces its steps to find a new path
            guessRobertMove();
            guessRobert.direction -= 180;
            goBack(1);
            return;
        }    
    }
}


int main(int argc, char **argv){
    int screenResolutionY, drawableSize;
    long tempLength; // Used for grabbing string lenght

    markersRetrieved = 0;
    
    // Input Parameters
    if (argc != 6){
        return 0;
    }

    waitTime = atoi(argv[1]); 
    gridSize = atoi(argv[2]); 
    screenResolutionY = atoi(argv[3]); 

    // Calculated Parameters
    drawableSize = screenResolutionY - 210; // 210 pixels of the screen is drawApp unusable space
    rectSize = drawableSize / (gridSize+1);
    buffer = ((drawableSize % rectSize) + rectSize) / 2; 


    tempLength = strlen(argv[4]);
    numberOfWalls = ((tempLength - 1) / 4) - 1;

    tempLength = strlen(argv[5]);
    numberOfMarkers = (tempLength - 1) / 4;

    // Environment Set-Up
    Block *Blocks = initBlocks(argv[4]);
    blocksPtr = Blocks;

    Block *Markers = initMarkers(argv[5]);
    markerPtr = Markers;

    Robot robert = {Blocks[0].x, Blocks[0].y, 0, 0};
    robertPtr = &robert;

    // Drawing Methods
    setWindowSize(drawableSize, drawableSize); 
    drawBackground();
    drawRobot(0);


    // Algorithm
    movementStack = createStack();
    while(markersRetrieved != numberOfMarkers){
        guessRobertReset(); // Reset its own relativistic position and memory
        complexSolve(0);
        goHome();
    } 

    // Exiting program
    free(movementStack);
    free(visitedList);
    free(blocksPtr);
    free(markerPtr);
    return 0; 
}


/*
    The logic of the complexSolve() algorithm is as follows:
    1. Check if we are on a marker, if so pick up and go home
    2. If not, turn left 
    3. Check if you can move forward
    4. If yes, move forward and call yourself again (resulting in another left turn)
    5. If no, turn right and try the same
    6. If you still cannot move forward after 2 right turns, go back to the block you were last on
    7. Since the function had called itself before the previous function turned right twice, returning the function 
       will cause the previous instance to turn right, and try this path instead.
    8. If it can move forward after this right turn, the robot goes to another path
       if it fails to move forward after 2 turns again, it moves back another block
    
    Essentially I treat every block on the grid as a junction in a maze, and search through every possible option in a grid,
    while avoiding blocks I have already visited, or blocks like walls and home. 

    This algorithm therefore only uses the 1 sensor that the robot has access to (check forward), and information it can
    store like which moves it has made. Using these moves it can store its direction and the positions it visited relative to where 
    it was initially placed. Hence this robot stays true to real life and does not use any information from within the code to solve 
    the puzzle, which was my goal.
*/

/*
    My trial input values:
    waitTime = 20; 
    gridSize = 8; 
    screenResolutionY = 982; 
    char wallLocations[] = "-.1.0.1.1.1.3.3.0.3.1.3.2.3.3";
    char markerLocations[] = "-.5.5.0.6";

    ./main 20 8 982 -.1.0.1.1.1.3.3.0.3.1.3.2.3.3 -.5.5.0.6 | java -jar drawapp.jar
*/