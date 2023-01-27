#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define W 21 //width of terminal or rows
#define H 80 // height of terminal or columns
#define Red "\33[0:31m\\]"
#define Reset "\33[0m\\]" 

// Your map should have a Pokemon Center and a Pokéemart, Represented by one or more ’C’ and ’M’, ´
// respectively. I make my Pokemon Centers and Pokémarts 2 × 2.
// 1
// • Your Pokemon Center and Pokémart, should be reachable from the path without having to go through ´
// tall grass.
// • Your map should contain at least two regions of tall grass (represented with colons)
// • Your map should contain at least one region of water (represented with tildes)
// • The outermost cells of the map are immovable boulders (represented using percent signs), except that
// there is one exit on each border. Your N-S path goes between the top and bottom exit, while the E-W
// path goes between the left and right exits.2
// • Your map should contain at least two clearings (regions of short grass). Clearings are represented
// using periods.
// • Other option terrain includes rocks and boulders (’%’), trees (’ˆ’), and whatever else you think would
// be interesting.




int main(){
    char chars[10] = {'%','#','C','M'};
    int ranX, ranY, ranP; 
    srand(time(0));
    ranX = rand() % 19 + 1;
    ranY = rand() % 40 + 1;
    ranP = rand() % 70 + 1; //for place
  
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
            
                    
            
            
            //if row is 2 below ranX, and if the counter is 1 below (to the left of) ranP, place C in next two row spots.
            //if row is 1 below ranY, and if the counter is 1 below (to the left of) ranP, place C in next two row spots.

            
        else{
            printf(" ");
        }
             
        }
        printf("\n");
    }
    return 0;
}