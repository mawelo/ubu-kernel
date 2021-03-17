#ifndef CURL_H
#define CURL_H

/*
    Easy Curl C++ header only include. Use -lcurl to link libcurl

    Usage/Example:
#include <iostream>
#include <vector>

#include <curl.h>

#ifndef __STDINF__
    #define __STDINF__ __LINE__ << " : " << __FILE__ << " : " << __func__
#endif

const char *mainline_kernel="https://kernel.ubuntu.com/~kernel-ppa/mainline";

int main(int argc, char **argv){
	int rc=0;
	std::vector<std::string> allArgs(argv,argv+argc);
	try{
		Curl curl;
		if( curl.getErrNo() != 0 ) throw SError() << __STDINF__ << " : ERROR: caught from: " << curl.getErrStr();

		curl.get("buffer1",mainline_kernel);
		if( curl.getErrNo() != 0 ) throw SError() << __STDINF__ << " : ERROR: caught from: " << curl.getErrStr();
		std::vector<char> v=curl.takeBuffer("buffer1");
		std::cout << "A:" << v.size() << "\n" << std::endl;

		// this should fail cause we took already 'buffer1' from internal read space
		v=curl.takeBuffer("buffer1");
		if( curl.getErrNo() != 0 ) throw SError() << __STDINF__ << " : ERROR: caught from: " << curl.getErrStr();
		std::cout << "B: " << v.size() << "\n" << std::endl;
	}catch(const std::exception& e){
		std::cerr << e.what() << std::endl;
	}
	return(rc);
}

*/

#include <functional>
#include <vector>
#include <unordered_map>
#include <curl/curl.h>
#include <SError.h>
#include <Errors.h>

#ifndef __STDINF__
    #define __STDINF__ __LINE__ << " : " << __FILE__ << " : " << __func__
#endif

class Curl : public Errors {
    public:
        Curl();
        ~Curl();
        bool get(const char *aliasname,const char *url);
        // TODO: bool get_with_header(const char *aliasname,const char *url); --> wird get(...) aufrufen aber zuvor die option CURLOPT_HEADERFUNCTION eincodiert haben
        //       ein extra ReadBuffers fuer Header Daten zu dem jeweiligen Alias wird dann notwendig sein
        std::vector<char> takeBuffer(const char* aliasname);
        bool close(const char *aliasname);
        void close_all();
    private:
        std::unordered_map<std::string,CURL *> curls;
        std::unordered_map<std::string, std::vector<char> > ReadBuffers;
        bool curlInit();
        bool curlFreeUp();
};

Curl::Curl(){
    curlInit();
}

Curl::~Curl(){
    curlFreeUp();
}

bool Curl::curlInit(){
    bool okay=true;
    ClearError();

    try{
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }catch(const std::exception& e){
        ErrNo=1;
        okay=false;
        ErrStr << e.what();
    }
    return(okay);
}

bool Curl::close(const char *aliasname){
    if( aliasname == nullptr ) return(false);
    if( curls.find(aliasname) == curls.end() ) return(false);
    curl_easy_cleanup(curls[aliasname]);
    curls.erase(aliasname);
    return(true);
} 

void Curl::close_all(){
    for(auto& m : curls){
        curl_easy_cleanup(m.second);
    }
}

bool Curl::curlFreeUp(){
    bool okay=true;
    ClearError();

    try{
        close_all();
        curl_global_cleanup();
    }catch(const std::exception& e){
        ErrNo=1;
        okay=false;
        ErrStr << e.what();
    }
    return(okay);
}

bool Curl::get(const char* aliasname,const char *url){
    bool okay=true;
    ClearError();
    try{
        if( aliasname == nullptr ) throw SError() << __STDINF__ << " : ERROR: invalid alias name -> nullptr";
        if( url == nullptr ) throw SError() << __STDINF__ << " : ERROR: invalid url -> nullptr";

        // make sure to re-use an already established connection. only call curl_easy_init() for the first time !
        if(  curls.find(aliasname) == curls.end() ){
            CURL *curl=curl_easy_init();
            if( ! curl ) throw SError() << __STDINF__ << " : ERROR: curl_easy_init() failed";
            curls[aliasname]=curl;
        }

        curl_easy_setopt(curls[aliasname], CURLOPT_URL,url);
        curl_easy_setopt(curls[aliasname], CURLOPT_FOLLOWLOCATION, 1L); // follow redirection HTTP 301
        /* enable TCP keep-alive for this transfer */
        curl_easy_setopt(curls[aliasname], CURLOPT_TCP_KEEPALIVE, 1L);
        /* keep-alive idle time to 120 seconds */
        curl_easy_setopt(curls[aliasname], CURLOPT_TCP_KEEPIDLE, 120L);
        /* interval time between keep-alive probes: 60 seconds */
        curl_easy_setopt(curls[aliasname], CURLOPT_TCP_KEEPINTVL, 60L);

        // see: https://stackoverflow.com/questions/6624667/can-i-use-libcurls-curlopt-writefunction-with-a-c11-lambda-expression -- Can I use libcurls CURLOPT_WRITEFUNCTION with a C++11 lambda expression?
        // see: https://stackoverflow.com/questions/25513380/c-style-callbacks-in-c11 -- C-style Callbacks in C++11
        curl_easy_setopt(curls[aliasname], CURLOPT_WRITEFUNCTION,+[](char *ptr, size_t size, size_t nmemb,std::vector<char> *userdata) -> size_t {
            size_t retcode=size*nmemb;
            try{
                userdata->insert(userdata->end(),ptr,ptr+retcode);
            }catch(std::exception& e){
                std::cerr << __STDINF__ << " : ERROR: " << e.what() << std::endl;
            }
            return(retcode);
        });
        std::vector<char> ReadBuffer;
        curl_easy_setopt(curls[aliasname], CURLOPT_WRITEDATA,&ReadBuffer);

        // skip peer verfication ? curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        // skip hostname verification ?  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        CURLcode res = curl_easy_perform(curls[aliasname]);
        if(res != CURLE_OK) throw SError() << __STDINF__ << " : ERROR: curl_easy_perform() failed: " << curl_easy_strerror(res);
        ReadBuffers[aliasname]=std::move(ReadBuffer);
    }catch(const std::exception& e){
        ErrNo=1;
        okay=false;
        ErrStr << e.what();
    }
    return(okay);
}

std::vector<char> Curl::takeBuffer(const char* aliasname){
    std::vector<char> result;
    ClearError();
    try{
        if( aliasname == nullptr ) throw SError() << __STDINF__ << " : ERROR: invalid alias name -> nullptr";
        if( ReadBuffers.find(aliasname) == ReadBuffers.end() ) throw SError() << __STDINF__ << " : ERROR: no data for alias name [" << aliasname << "] found";
        result=std::move(ReadBuffers[aliasname]);
        ReadBuffers.erase(aliasname);
    }catch(const std::exception& e){
        ErrNo=1;
        ErrStr << e.what();
    }
    return(result);
}

#endif