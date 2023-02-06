#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define W 21 //width of terminal or rows
#define H 80 // height of terminal or columns
#define CENTER 200
#define WIDTH 401
#define mapxy(x, y) (m->map[y][x])

typedef struct map {
  int x,y;
} map_t;

map_t *mapArray[WIDTH][WIDTH];

// prints given map at xLoc, yLoc
//TODO need to have directions passed to get gates between worlds.
void print_map(map_t *m,int xLoc, int yLoc){
     if (m[xLoc][yLoc] == NULL){
        // Map does not exist, create it
        m[xLoc][yLoc] = (map_t *)malloc(sizeof(map_t));
    }
    //print the address stored in the pointer m
    char chars[10] = {'%','#','C','M',':','~','^'};
    int ranX, ranY, ranP, ranTX, ranTY, ranFX, ranFY; 
    ranX = rand() % 17 + 4;
    ranY = rand() % 40 + 4;
    ranP = rand() % 70 + 4; //for place
    ranTX = rand() % 7 + 1; // for tall grass
    ranTY = rand() % 18 + 4;// for tall grass
    ranFX = rand() % 16 + 10;// for forests
    ranFY = rand() % 10 + 5;// for forests
    // make sure ranPs aren't close to the tall grass
    int prev_y = m->y;
    if (yLoc > 201){
        prev_y = m->y;
        printf("prev Y gate: %d \n", prev_y);
    }
    
    
    if (ranP - ranTX <= 5){
        ranTX += 7;
    } 
    if (ranP - ranTY <= 5){
        ranTY += 7;
    }
    if (ranX > 17){ //too close to bottom border for my comfort
        ranX = 17;
    }
    m->x = ranX;
    m->y = ranY;
    printf("current y gate: %d \n", m->y);
    
    
    printf("You are visiting map %d , %d \n", xLoc,yLoc);
    printf("North and south gate: %d\n", ranY);
    //TODO check north and south gate from this one, and pass that into the current map. Connecet the paths between border and new ranX / ranY
    for (int i = 0; i <= W; i++){
        for (int j = 0; j <= H; j++){
            if ((j == H && i == ranX) || (j == 0 && i == ranX)){
            printf("%c", chars[1]);
            } 
            else if ((i == W && j == ranY) || (i == 0 && j == ranY)){
            printf("%c", chars[1]);
            } 
           else if (i==0 || i==W || j==0 || j==H){
            // printf("\033[0;31m"); //prints red
            printf("%c", chars[0]);     
           }           
            else if (i==ranX){
                // printf("\033[0m"); //resets color
                printf("%c", chars[1]);
            }
            else if (j==ranY){
            printf("%c", chars[1]);
            }     
                    //if row is 2 or 1 above ranX, and if the counter is 1 below (to the left of) ranP, place C in next two row spots.
            else if ((i-2 == ranX && (ranP - j == 1 || ranP == j)) || (i-1 == ranX && (ranP - j == 1 || ranP == j))){
                printf("%c", chars[2]);
                
            }    
             else if ((i+2 == ranX && (ranP - j == 1 || ranP == j)) ||(i+1 == ranX && (ranP - j == 1 || ranP == j))){
                printf("%c", chars[3]);
            } 
            else if ((j >= ranTX && j <= ranTX + 5 && i >= ranTX && i <= ranTX + 5) || (j >= (H - ranTY - 5) && j <= (H - ranTY) && i >= (W - ranTY) && i <= (W - ranTY + 5))){
                printf("%c",chars[4]);
            }
            else if ((j >= ranFX && j <= ranFX + 4 && i >= ranFX && i <= ranFX + 4) || (j >= (H - ranFY - 4) && j <= (H - ranFY) && i >= (W - ranFY) && i <= (W - ranFY + 4))){
                if ((abs(j - ranFX) >= 2 && abs(j - ranFX) < 4) || (abs(j - (H - ranFY - 4) >= 2 && abs(j - (H - ranFY) < 4)))){
                    printf("%c",chars[5]);
                }
                else{
                    printf("%c",chars[6]); 
                }
               
            }
            else if (i >= 10 && i <= 15 && j >= 57 && j <= 63){
                printf("%c", chars[0]);
            }
              
            
            else{
                printf(".");
            }
             
        }
        printf("\n");
    }
}

int main(){
    map_t map;
    srand(time(0));
    mapArray[CENTER][CENTER] = (map_t *)malloc(sizeof(map_t));
    print_map(mapArray[CENTER][CENTER],200,200);
    // char move;
    // int xLoc, yLoc;
    // printf("enter n,w,s,e to go 1 map directly in that direction. Enter f num num where num is -200 to 200. Enter q to quit.");
    // scanf("%c %d %d", &move, &xLoc, &yLoc);
    // char move = 'x';
    int xLoc = 200;
    int yLoc = 201;
    // 
 
   
    print_map(mapArray[xLoc][yLoc],xLoc,yLoc);
    print_map(mapArray[CENTER][CENTER],200,200);
    // yLoc = 202;
    // // 
 
    // if (mapArray[xLoc][yLoc] == NULL){
    //     // Map does not exist, create it
    //     map = *(mapArray[xLoc][yLoc] = (map_t *)malloc(sizeof(map_t)));
    // }
    // print_map(&map,xLoc,yLoc);
    // printf("enter n,w,s,e to go 1 map directly in that direction. Enter f num num where num is -200 to 200. Enter q to quit.");
    // scanf("%c", &move);
    
    
    return 0;
}