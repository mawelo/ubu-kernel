#include <Errors.h>

void Errors::ClearError(){
    ErrNo=0;
    ErrStr.str("");
    ErrStr.clear();
}

int Errors::getErrNo(){
    return(ErrNo);
}

std::string Errors::getErrStr(){
    return(ErrStr.str());
}
