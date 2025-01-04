
#include "Communicator.h"

using namespace std;

int main(){
    Communicator* communicator = new Communicator(8080);
    communicator->communicate();
    delete communicator;
}