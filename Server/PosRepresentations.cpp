#include "PosRepresentations.h"
#include <bits/stdc++.h>
PosRepresentations::PosRepresentations(){
    //implement
}

int* PosRepresentations::fenToBoard(std::string fenNotation){
    int len = fenNotation.length();
    char arr[len + 1];
    strcpy(arr, fenNotation.c_str());
    int* board = new int[64];
    int i = 0;
    int j = 0;

    std::map<char, int> charToInt = {
        { 'p', 1 },
        { 'n', 2 },
        { 'b', 3 },
        { 'r', 4 },
        { 'q', 5 },
        { 'k', 6 },
        { 'P', 8 },
        { 'N', 9 },
        { 'B', 10 },
        { 'R', 11 },
        { 'Q', 12 },
        { 'K', 13 },
    };

    for(char c : arr){
        if(c == ' '){
            break;
        }
        else if(c == '/'){
            i++;
            j = 0;
        }else if(c >= '1' && c <= '9'){
            int loop = c - '0';
            for(int k = 0; k < loop; k++){
                board[(i * 8) + (j + k)] = 0;
            }
            j = j + loop;
        }else{
            board[(i * 8) + j] = charToInt[c];
            j++;
        }
    }

    return board;
    
}


// def fen_to_board(self):
//         # get's the board pieces, each / means new row.
//         rows = self.fen.split()[0].split("/")
//         board = [list(self.expand_row(row)) for row in rows]
//         return board

// def expand_row(self, row):
//         # if it is number it means how many squares are empty and we put '.' there on all those squares
//         # otherwise it is a piece.
//         return "".join(char if not char.isdigit() else "." * int(char) for char in row)