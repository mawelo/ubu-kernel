#ifndef GLOBAL_ERRORS_H
#define GLOBAL_ERRORS_H

#ifndef GLOBAL_ERRORS
#define GLOBAL_ERRORS

#include<iostream>

class GlobalError {
	public:
		GlobalError(){};
		~GlobalError(){};
        int ErrNo;
        std::stringstream ErrStr;
        int getErrNo(){ return(ErrNo); }
        std::string getErrStr(){ return(ErrStr.str()); }
        void ClearError(){
            ErrNo=0;
            ErrStr.str("");
            ErrStr.clear();
        }
};
#endif

GlobalError gError;

#endif