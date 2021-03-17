#ifndef ERRORS_H
#define ERRORS_H

#include <sstream>

class Errors{
    public:
        Errors(){};
        ~Errors(){};
        int getErrNo();
        std::string getErrStr();
    protected:
        int ErrNo=0;
        std::stringstream ErrStr;
        void ClearError();
};

#endif