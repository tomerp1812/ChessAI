#include "PosRepresentations.h"
#include <string>
#include <bits/stdc++.h>

PosRepresentations::PosRepresentations()
{
}

posRepresent *PosRepresentations::fenToBoard(std::string fenNotation)
{
    int len = fenNotation.length();
    char arr[len + 1];
    strcpy(arr, fenNotation.c_str());
    // int *board = new int[64];
    int myKing;
    int oppKing;
    unsigned long long int friends = 0;
    unsigned long long int enemies = 0;
    char turn; // w is white, b is black
    // char castle[4] = {'.','.','.','.'};
    int enPassant = -1;
    int halfMove;
    int fullMove;
    int i = 0;
    int j = 0;
    posRepresent* posRep = new posRepresent();
    posRep->castle[0] = '.';
    posRep->castle[1] = '.';
    posRep->castle[2] = '.';
    posRep->castle[3] = '.';

    std::map<char, int> charToInt = {
        {'p', -1},
        {'n', -2},
        {'b', -3},
        {'r', -4},
        {'q', -5},
        {'k', -6},
        {'P', 1},
        {'N', 2},
        {'B', 3},
        {'R', 4},
        {'Q', 5},
        {'K', 6},
    };

    char *token = strtok(arr, " ");
    int part = 0;
    while (token != NULL)
    {
        std::string tokenStr = std::string(token);
        switch (part)
        {
            // board
        case 0:
            for (char c : tokenStr)
            {
                if (c == '/')
                {
                    i++;
                    j = 0;
                }
                else if (c >= '1' && c <= '9')
                {
                    int loop = c - '0';
                    for (int k = 0; k < loop; k++)
                    {
                        int t = (((7 - i) * 8) + (j + k));
                        posRep->board[t] = 0;
                    }
                    j += loop;
                }
                else
                {
                    int t = (((7 - i) * 8) + j);
                    posRep->board[t] = charToInt[c];
                    if(charToInt[c] > 0){
                        if(c == 'K'){
                            myKing = t;
                        }
                        friends |= (1ULL << t);
                    }else{
                        if(c == 'k'){
                            oppKing = t;
                        }
                        enemies |= (1ULL << t);
                    }
                    j++;
                }
            }
            break;
        case 1:
        // turns
            turn = *token;
            if(turn == 'b'){
                unsigned long long int tmp = friends;
                friends = enemies;
                enemies = tmp;
                int tmpKing = myKing;
                myKing = oppKing;
                oppKing = tmpKing;
            }
            break;
        case 2:
        // castling
            for(int i = 0; i < strlen(token); i++){
                if(token[i] == 'K'){
                    posRep->castle[0] = 'K';
                }else if(token[i] == 'Q'){
                    posRep->castle[1] = 'Q';
                }else if(token[i] == 'k'){
                    posRep->castle[2] = 'k';
                }else if(token[i] == 'q'){
                    posRep->castle[3] = 'q';
                }
            }
            break;
        case 3:
        // en-passant
            if(strcmp(token, "-") != 0){
                char c1 = token[0];
                char c2 = token[1];
                enPassant = (8 * (c2 - '1') + (c1 - 'a'));
            }
            break;

        case 4:
        // half moves
        halfMove = atoi(token);
        break;

        case 5:
        // full move
        fullMove = atoi(token);
        break;
        default:
            break;
        }
        part++;
        token = strtok(NULL, " ");
    }

    // posRep->board = board;
    posRep->myKing = myKing;
    posRep->oppKing = oppKing;
    posRep->friends = friends;
    posRep->enemies = enemies;
    posRep->blockers = (friends | enemies);
    // posRep->castle = new char[4]{castle[0], castle[1], castle[2], castle[3]};
    posRep->enPassant = enPassant;
    posRep->fullMove = fullMove;
    posRep->halfMove = halfMove;
    posRep->turn = turn;


    return posRep;
}