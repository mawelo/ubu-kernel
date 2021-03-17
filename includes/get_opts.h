#ifndef GET_OPTS_H
#define GET_OPTS_H

#include <iostream>
#include <json.hpp>
#include <Errors.h>
#include <SError.h>
using json = nlohmann::json;

/*
    will return parsed aruments back to jason object opts
    json object opts must include all required options without '--'

    Example Usage:
		// key, value , true or false if argument is mandatory or not
		json gOpts={
			{ "version", 	{ false, 	false 	} },
			{ "strace",		{ false, 	false 	} },
			{ "truss",		{ true, 	false 	} },
			{ "files", 		{ "",		true	} }
		};

		std::cout << "gOpts dump: " << gOpts.dump(4) << std::endl;
		
		GetOpts cmdopts(argc-1, &argv[1], gOpts);
		if( cmdopts.getErrNo() != 0 ){
			std::cerr << __STDINF__ << " : got error from: " << cmdopts.getErrStr() << "\n";
		}
        ...
*/
#ifndef __STDINF__
    #define __STDINF__ __LINE__ << " :" << __FILE__ << " : " << __func__
#endif

class GetOpts : public Errors {
    public:
        GetOpts(int argc, char **argv,json& opts){
            ClearError();
            try{
                if( opts.size() < 1 ) throw SError() << __STDINF__ << " : ERROR: json opts is empty";
                
                for(int i=0;i < argc; ++i){
                    std::string s=argv[i];
                    if( s.length() < 3){
                        ErrNo=99;
                        ErrStr << __STDINF__ << " : WARNING: argument " << i << " : [" << s << "] is invalid. (too short.must start with '--' and min. length should be 3)\n";
                        continue;
                    }else if( s.at(0) != '-' || s.at(1) != '-' ){
                        ErrNo=98;
                        ErrStr << __STDINF__ << " : WARNING: argument " << i << " : [" << s << "] is invalid. (argument must start with '--')\n";
                        continue;
                    }
                    bool is_with_value=false;
                    size_t apos=s.find("=");
                    if( apos != std::string::npos ){ is_with_value=true; }
                    else apos=s.length();

                    std::string akey=s.substr(2,apos-2);
                    if( opts.find(akey) == opts.end() ){
                        ErrNo=97;
                        ErrStr << __STDINF__ << " : WARNING: argument " << i << " : [" << s << "] is invalid. (unkown argument)\n";
                        continue;
                    }

                    std::string aval="";
                    if( is_with_value ){
                        aval=s.substr(1+apos,s.length());
                    }
                    
                    try{
                        if( (!is_with_value) && opts[akey][1].get<bool>() == true ){
                            ErrNo=96;
                            ErrStr << __STDINF__ << " : WARNING: argument " << i << " : [" << s << "] is invalid. (no value give but value is mandatory)\n";
                            continue;
                        }

                        if( opts[akey][0].is_boolean() ){
                            if( ! is_with_value ){
                                if( opts[akey][0] == true ) opts[s][0]=false;
                                else opts[akey][0]=true;
                            }else{
                                if( aval== "true" ) opts[akey][0]=true;
                                else if( aval== "false" ) opts[akey][0]=false;
                                else{
                                    ErrNo=95;
                                    ErrStr << __STDINF__ << " : WARNING: argument " << i << " : [" << s << "] is invalid. (should have 'true' or 'false' as value given)\n";
                                    continue;
                                }
                            }
                        }else if( opts[akey][0].is_number_integer() ){
                            if( ! is_with_value ){
                                ErrNo=94;
                                ErrStr << __STDINF__ << " : WARNING: argument " << i << " : [" << s << "] is invalid. (should have a integer value given)\n";
                                continue;
                            }else{
                                opts[akey][0]=atoll(aval.c_str());
                            }
                        }else if( opts[akey][0].is_number_float() ){
                                if( ! is_with_value ){
                                    ErrNo=94;
                                    ErrStr << __STDINF__ << " : WARNING: argument " << i << " : [" << s << "] is invalid. (should have a float number value given)\n";
                                    continue;
                                }else{
                                    opts[akey][0]=atof(aval.c_str());
                                }
                        }else if( opts[akey][0].is_string() ){
                                if( ! is_with_value ){
                                    ErrNo=94;
                                    ErrStr << __STDINF__ << " : WARNING: argument " << i << " : [" << s << "] is invalid. (should have a string value given)\n";
                                    continue;
                                }else{
                                    opts[akey][0]=aval;
                                }
                        }else{
                            ErrNo=90;
                            ErrStr << __STDINF__ << " : WARNING: argument " << i << " : [" << s << "] is invalid. (unkown type: " << opts[akey][0].type_name() << " !)\n";
                            continue;
                        }
                    }catch(const std::exception& je){
                        ErrNo=80;
                        ErrStr << __STDINF__ << " : WARNING: argument " << i << " : [" << s << "] : type: " << opts[akey][0].type_name() << " : got json error: " << je.what();
                        continue;
                    }
                } // end of for-loop thrue all args
            }catch(const std::exception& e){
                ErrNo=1;
                ErrStr << e.what();
            }
        }

        ~GetOpts(){};
    private:
    
};

#endif
