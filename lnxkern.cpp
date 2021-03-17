#include <iostream>
#include <vector>
#include <map>
#include <filesystem>
#include <regex>
#include <cstdio>
#include <global_errors.h>
#include <SError.h>
#include <json.hpp>
#include <ctre-unicode.hpp>
#include <get_opts.h>
#include <final.h>
#include <download.h>

using json = nlohmann::json;

// global program options:
json gOpts={
			{ "kernel",		{ "", 	    			false 	} },
			{ "action",		{ "", 					true 	} },
			{ "filter",		{ "",					false 	} },
			{ "url",        { mainline_kernel,      false   } }
};
// TODO: natuerlich koennte man noch immer "blind" die gOpts veraendern welche HIDENITEMS sind ! das muss in der get_opts.h noch angepasst werden !
const std::multimap<const std::string,const std::string> description={ 
	{"action", "=list        --> could add '--filter='  ### list all available/installed linux kernel in the system"},
	{"action", "=remove      --> could add '--filter='  ### create shell code to remove a kernel and modules"},
	{"action", "=list-online --> could add '--filter='  ### list online available linux kernels"},
	{"action", "=download    --> could add '--filter='  ### download available linux kernels"},
	{"action", "=changes    --> could add '--filter='  ### download CHANGES file"},
	{"filter", "=<regex>     --> use with --action=...  ### optional use a regex filter for an action"},
	{"url"	 , "HIDETHISITEM"},
	{"kernel", "HIDETHISITEM" }
};

int ExecCmd(std::vector<std::string>& result, const char *cmdstr)
{
	int command_returncode=-1;
    FILE *p=popen(cmdstr,"r");
    if( p == nullptr ) throw SError() << __STDINF__ << " : ERROR: unable to execute given command : " << strerror(errno);
	finally close_pfile([&p]
  	{
	 if( p != nullptr ) fclose(p);
  	});

    std::vector<char> Buffer(8*1024);
    while( ! feof(p) )
    {
     char *r=fgets(Buffer.data(),Buffer.size(),p);
     if( r == nullptr ) continue;
     std::string s=r;
	 if( s.rfind("\n") != std::string::npos )s.resize(s.rfind("\n"));
     result.push_back(s);
    }

	command_returncode=pclose(p);
	p=nullptr;
	return(command_returncode);
}

std::vector<std::string> List_all_available_kernels(const char *filter){
	const char *kerneldir="/boot";
	std::vector<std::string> vmln;
	for(auto& m: std::filesystem::directory_iterator(kerneldir)){
		if( ! (ctre::match<"^vmlinuz-.*">(m.path().filename().c_str()) && std::filesystem::is_regular_file(m)) ) continue;
		std::string fname=m.path().filename().c_str();
		if( fname.find(filter) == std::string::npos ) continue;
		vmln.push_back(fname);
	} // end of for-loop

	if( vmln.size() < 1 ) throw SError() << __STDINF__ << " : ERROR: no kernels found in " << kerneldir;

	std::smatch sm;
	std::regex e("[1-9]*\\.[0-9]*\\.[0-9].*\\-"); // match someting like -5.10.9-051009-
	std::vector<std::string> result;

	for(auto& m : vmln){
		std::regex_search(m.cbegin()+8,m.cend(),sm,e);
		if( sm.size() < 1 ){
			std::cerr << __STDINF__ << " : WARNING: bogus line found: [" << m << "]" << std::endl;
			continue;
		}

		result.push_back(sm[0].str().substr(0,sm[0].str().rfind("-")));
	} // end of for-loop

	return(result);
}

std::map<std::string,std::vector<std::string> > get_linux_kernel_packets(std::vector<std::string>& versions){
	std::map<std::string,std::vector<std::string> > result;
	std::vector<std::string> dpkgout;
	ExecCmd(dpkgout,"dpkg-query -W -f='${binary:Package}\\n'");
	for(auto& m : versions){
		for(auto& d : dpkgout){
			if( d.find("linux-") == std::string::npos ) continue;
			if( d.rfind(m) == std::string::npos ) continue;
			result[m].push_back(d);
		} // end of for-loop
	} // end of for-loop
	return(result);
}

bool is_hidden_option(std::string s){
	bool okay=false;
	for(auto& d : description){
		if( s == d.first && d.second == "HIDETHISITEM" ){
			okay=true;
			break;
		}
	}
	return(okay);
}

void Usage(const char *prgname,std::string what){
	std::cout << "git-id: " << GIT_VERS << " : martin@lonkwitz.eu : " << __DATE__ << " : " << __TIME__ << " : " << prgname << std::endl;
	std::stringstream ss;
	if( what == "" ){
		std::cout << "Usage: " << prgname << " options: " << std::endl;

		for(auto& m : gOpts.items()){
			if( is_hidden_option(m.key()) ) continue;
			ss << m.key().c_str() << std::endl;
			for(auto& d : description){
				if( d.first != m.key() ) continue;
				ss << "	|-> --" << d.first << d.second << std::endl;
			}
		}
		ss << std::endl << "default download site: " << mainline_kernel << std::endl << "filter example for v5 Kernel: --filter='v5\\.[1-9][0-9]\\..*'" << std::endl;
	}
	std::cout << ss.str();
}

int main(int argc, char **argv){
	int rc=0;
	std::vector<std::string> allArgs(argv,argv+argc);
	try{
		if( argc < 2 ){
			gOpts["action"][0]="list";  // default is "list" current installed kernel
		}

		GetOpts cmdopts(argc-1, &argv[1], gOpts);
		if( cmdopts.getErrNo() != 0 ){
			std::cerr << __STDINF__ << " : got error from: " << cmdopts.getErrStr() << "\n";
		}

		if( gOpts["action"][0].get<std::string>() == "help" ){
			Usage(allArgs.at(0).c_str(),"");
		}else if( gOpts["action"][0].get<std::string>() == "list" ){
			auto kernels=List_all_available_kernels(gOpts["filter"][0].get<std::string>().c_str());
			auto packets=get_linux_kernel_packets(kernels);
			for(auto& m : packets){
				std::cout << m.first << " : " << std::endl;
				for(auto& p :  m.second){
					std::cout << "\t|- " << p << std::endl;
				} // end of for-loop
			} // end of for-loop
		}else if( gOpts["action"][0].get<std::string>() == "remove" ){
			auto kernels=List_all_available_kernels(gOpts["filter"][0].get<std::string>().c_str());
			auto packets=get_linux_kernel_packets(kernels);
			for(auto& m : packets){
				std::string allpkgs="";
				for(auto& p :  m.second){
					allpkgs+=" "+p;
				}
				std::cout << "sudo apt remove -y " << allpkgs << std::endl;
				std::cout << "sudo rm -rf /lib/modules/" << m.first << "*" << std::endl; // THIS IS DANGER !!!
			}
		}else if( gOpts["action"][0].get<std::string>() == "list-online" ){
			auto kernels=list_all_online_kernels(gOpts["url"][0].get<std::string>().c_str(),gOpts["filter"][0].get<std::string>());
			for(auto& m : kernels.items()){
            	for(auto& k : kernels[m.key()].items()){
                	std::cout << " |- " << m.key() << " : " << gOpts["url"][0].get<std::string>().c_str() << "/" << k.key() << " : " << k.key() << std::endl;
            	} // end of for-loop
			} // end of for-loop
        }else if( gOpts["action"][0].get<std::string>() == "changes" ){
			auto kernels=list_all_online_kernels(gOpts["url"][0].get<std::string>().c_str(),gOpts["filter"][0].get<std::string>());
			std::string changes_file_uri;
			Curl curl;
        	if( curl.getErrNo() != 0 ) throw SError() << __STDINF__ << " : ERROR: caught from: " << curl.getErrStr();
			for(auto& m : kernels.items()){
				for(auto& k : kernels[m.key()].items()){
					changes_file_uri=gOpts["url"][0].get<std::string>().c_str()+std::string("/")+k.key()+std::string("/CHANGES");
					auto&& [changes,errnum,errstr]=download_changes_file(changes_file_uri.c_str(),curl);
					if( errnum != 0 ){
						std::cerr << __LINE__ << " : " << __FILE__ << " : " << __func__ << " : WARNING: got error from: " << errstr << std::endl;
					}else{
						std::cout << m.key() << " : " << k.key() << " : " << changes_file_uri << " : " << std::endl << changes << std::endl;
					}
				}
			}
		}else{
			Usage(allArgs.at(0).c_str(),"");
		}
	}catch(const std::exception& e){
		std::cerr << e.what() << std::endl;
	}
	return(rc);
}
