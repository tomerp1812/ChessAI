
#include "Communicator.h"
#include "PosRepresentations.h"
#include "ChessAi.h"
#include "evaluator.h"

using namespace std;

int main(){
    PosRepresentations* posRep = new PosRepresentations();
    Evaluator* evaluator = new Evaluator();
    ChessAi* ai = new ChessAi(*posRep, *evaluator);
    Communicator* communicator = new Communicator(8080, *ai);
    communicator->communicate();
    delete posRep;
    delete evaluator;
    delete ai;
    delete communicator;
}