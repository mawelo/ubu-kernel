#ifndef LNXKERN_DOWNLOAD_H
#define LNXKERN_DOWNLOAD_H

#include <iostream>
#include <regex>
#include <tuple>
#include <curl/curl.h>
#include <json.hpp>
#include <SError.h>
#include <stdio.h>
#include <curl.h>
#include <global_errors.h>

using json = nlohmann::json;

#ifndef __STDINF__
    #define __STDINF__ __LINE__ << " : " << __FILE__ << " : " << __func__
    #endif
const char *mainline_kernel="https://kernel.ubuntu.com/~kernel-ppa/mainline";

size_t curlread_callback(char *buffer,size_t size,size_t nitems,std::vector<char> *userdata){
    // userdata wird ein zeiger auf ein std::vector<char> sein !
    size_t retcode=size*nitems;
    
    try{
        userdata->insert(userdata->end(),buffer,buffer+retcode);
    }catch(std::exception& e){
        std::cerr << __STDINF__ << " : ERROR: " << e.what() << std::endl;
    }
    
    return retcode;
}

json parse_online_kernellist(std::vector<char>& ReadBuffer,std::string& filter){
    gError.ClearError(); // from global_errors.h
    json result;
    try{
        const char *regex_str="\\salt=\"\\[DIR\\]\"></td><td><a href=\"v[1-9].*</a>";
        std::regex rgx(regex_str);
        std::smatch sm;
        std::string s=ReadBuffer.data();
        bool skipthis=false;
        while( std::regex_search(s,sm,rgx) ){
            skipthis=false;
            for(auto x : sm){
                size_t pos=x.str().find("<a href=\"");
                pos+=9;
                size_t pos2=x.length()-x.str().find("/\">v");
                size_t len=x.length()-pos2-pos;
                std::string kernelv=x.str().substr(pos,len);
                std::string kernv_major=kernelv.substr(0,2);
                if( filter != "" ){
                    if( ! std::regex_match (kernelv.c_str(),std::regex(filter))) skipthis=true;
                }

                if( skipthis ) continue;
                result[kernv_major][kernelv]="";
            } // end of for-loop
            s=sm.suffix().str();
        } // end of while-loop
    }catch(std::exception& e){
        gError.ErrNo=1;
        gError.ErrStr << e.what();
    }
    return(result);
}

json list_all_online_kernels(const char *url,std::string filter){
    json result;
    try{
        Curl curl;
        if( curl.getErrNo() != 0 ) throw SError() << __STDINF__ << " : ERROR: caught from: " << url << " : " << curl.getErrStr();
        
        curl.get("mainline_kernel_page",mainline_kernel);
		if( curl.getErrNo() != 0 ) throw SError() << __STDINF__ << " : ERROR: caught from: " << url << " : " << curl.getErrStr();
		std::vector<char> ReadBuffer=curl.takeBuffer("mainline_kernel_page");

        result=parse_online_kernellist(ReadBuffer,filter);
    }catch(const std::exception& e){
        result["ErrNo"]=1;
        result["ErrStr"]=e.what();
    }

    return(result);
}

void download_kernel_packages(const char *filter){
            /*
            TODO: 2021-02-26: 
            Hier muss in der jeweiligen Page https://kernel.ubuntu.com/~kernel-ppa/mainline/v<N...> nach :
                den links zu den .deb packeten
            gesucht werden. an erster stelle steht dann die Architektur. wobei die aktuelle architektur bei x86_64 nach amd64 gemapped werden muss
            das geht aber auch aus den packet-namen hervor... _<arch>.deb
            wonbei CHECKSUMS -und CHANGES Dateien mitgenommen werden sollten !

            Brauchen wir auch wieder:
                curl.get("mainline_kernel_page",mainline_kernel);
		        if( curl.getErrNo() != 0 ) throw SError() << __STDINF__ << " : ERROR: caught from: " << curl.getErrStr();
		        std::vector<char> ReadBuffer=curl.takeBuffer("mainline_kernel_page");

                result=parse_online_kernellist(ReadBuffer,filter);

        und dann....
            std::string mainline_kernel_versionX=mainline_kernel;
                mainline_kernel_versionX+="/"+k.key();
                curl.get("mainline_kernel_page",mainline_kernel_versionX.c_str());
		        if( curl.getErrNo() != 0 ) throw SError() << __STDINF__ << " : ERROR: failed to fetch: " << mainline_kernel_versionX << "\n" << curl.getErrStr();
		        std::vector<char> ReadBuffer=curl.takeBuffer("mainline_kernel_page");
                for(size_t i=0; i < ReadBuffer.size();){
                    ++i; // TODO ! suche per find nach deb"> und von da auch per rfind nach href=" // verschiebe i entsprechend ...
                }
        */
}

// return: CHANGES-File as String, error-number,error-string
std::tuple<std::string,int,std::string> download_changes_file(const char *uri,Curl& curl){
    
    try{
        curl.get("CHANGES_kernel_page",uri);
		if( curl.getErrNo() != 0 ) throw SError() << __STDINF__ << " : ERROR: caught from: " << curl.getErrStr();
        std::vector<char> ReadBuffer=curl.takeBuffer("CHANGES_kernel_page");
        return std::make_tuple(std::string(ReadBuffer.data(),ReadBuffer.size()-1),0,"");
    }catch(const std::exception& e){
        return std::make_tuple("",1,e.what());
    }
}
#endif
