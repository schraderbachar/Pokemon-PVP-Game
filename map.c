#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define W 21 //width of terminal or rows
#define H 80 // height of terminal or columns

//  At least two paths pass through your map, one oriented N-S, the other E-W; these will intersect somewhere in your map. Additional paths are acceptable according to your taste. Paths are represented
// using hashes (’#’).

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
    char chars[10] = {'%','#'};
    int ranX;
    int ranY;
    srand(time(0));
    ranX = rand() % 21 + 1;
    ranY = rand() % 80 + 1;
    printf("%d\n", ranX);
    printf("%d\n", ranY);
    for (int i = 0; i <= W; i++){
        for (int j = 0; j <= H; j++){
           if (i==0 || i==W || j==0 || j==H){
            printf("%c", chars[0]);     
           }           
            else if (i==ranX){
                printf("%c", chars[1]);
            }
            else if (j==ranY){
            printf("%c", chars[1]);
            }         
        else{
            printf(" ");
        }
             
        }
        printf("\n");
    }
    return 0;
}