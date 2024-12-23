#include <stdio.h>
static int RBits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12};

static int BBits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6};

int main(){
    int bb[64];
    int rb[64];
    for(int i = 0; i < 64; i++){
        bb[i] = 64 - BBits[i];
        rb[i] = 64 - RBits[i];
    }
    printf("static int BBits[64] = {");
    for(int i = 0; i < 64; i++){
        if(i % 8 == 0){
            printf("\n");
        }
        printf("%d, ", bb[i]);
    }
    printf("};\n");

    printf("static int RBits[64] = {");
    for(int i = 0; i < 64; i++){
        if(i % 8 == 0){
            printf("\n");
        }
        printf("%d, ", rb[i]);
    }
    printf("};\n");

};