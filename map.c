#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define W 21 //width of terminal or rows
#define H 80 // height of terminal or columns
#define Red "\33[0:31m\\]"
#define Reset "\33[0m\\]" 

// 1
// • Other option terrain includes rocks and boulders (’%’), trees (’ˆ’), and whatever else you think would
// be interesting.


int main(){
    char chars[10] = {'%','#','C','M',':','~','^'};
    int ranX, ranY, ranP, ranTX, ranTY, ranFX, ranFY; 
    srand(time(0));
    ranX = rand() % 17 + 4;
    ranY = rand() % 40 + 4;
    ranP = rand() % 70 + 4; //for place
    ranTX = rand() % 7 + 1; // for tall grass
    ranTY = rand() % 18 + 4;// for tall grass
    ranFX = rand() % 16 + 10;// for forests
    ranFY = rand() % 10 + 5;// for forests
    // make sure ranPs aren't close to the tall grass
    if (ranP - ranTX <= 5){
        ranTX += 7;
    } 
    if (ranP - ranTY <= 5){
        ranTY += 7;
    }
    if (ranX > 17){ //too close to bottom border for my comfort
        ranX = 17;
    }
  
    for (int i = 0; i <= W; i++){
        for (int j = 0; j <= H; j++){
            if (j == H && i == ranX || j == 0 && i == ranX){
            printf("%c", chars[1]);
            } 
            else if (i == W && j == ranY || i == 0 && j == ranY){
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
            else if (i-2 == ranX && (ranP - j == 1 || ranP == j) || i-1 == ranX && (ranP - j == 1 || ranP == j)){
                printf("%c", chars[2]);
                
            }    
             else if (i+2 == ranX && (ranP - j == 1 || ranP == j) || i+1 == ranX && (ranP - j == 1 || ranP == j)){
                printf("%c", chars[3]);
            } 
            else if (j >= ranTX && j <= ranTX + 5 && i >= ranTX && i <= ranTX + 5 || j >= (H - ranTY - 5) && j <= (H - ranTY) && i >= (W - ranTY) && i <= (W - ranTY + 5)){
                printf("%c",chars[4]);
            }
            else if (j >= ranFX && j <= ranFX + 4 && i >= ranFX && i <= ranFX + 4 || j >= (H - ranFY - 4) && j <= (H - ranFY) && i >= (W - ranFY) && i <= (W - ranFY + 4)){
                if (abs(j - ranFX) >= 2 && abs(j - ranFX) < 4 || abs(j - (H - ranFY - 4) >= 2 && abs(j - (H - ranFY) < 4))){
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
    return 0;
}