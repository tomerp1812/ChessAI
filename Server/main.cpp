
#include "Communicator.h"
using namespace std;

int main(){
    PosRepresentations* posRep = new PosRepresentations();
    ChessAi* ai = new ChessAi(*posRep);
    Communicator* communicator = new Communicator(8080, *ai);
    communicator->communicate();
    delete communicator;
    delete ai;
}