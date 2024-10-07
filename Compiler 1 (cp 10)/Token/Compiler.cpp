#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include <sys/stat.h>
#include <dirent.h>
#include <vector>
using namespace std;
#include "JackTokenizer.h"
#include "CompilationEngine.h"

//string tokenTypeEnumDeb[] = {"KEYWORD","SYMBOL","IDENTIFIER","INT_CONST","STRING_CONST"};
//string keyWordEnumDeb[] = {"CLASS","METHOD","FUNCTION","CONSTRUCTOR","INT","BOOLEAN","CHAR","VOID","VAR","STATIC","FIELD","LET","DO","IF","ELSE","WHILE","RETURN","TRUE","FALSE","J_NULL","THIS"};

class JackAnalyzer {
	public:
		JackAnalyzer(string file){
			//Check if file is directory and iterate to create tokenizers
			cout << file.length() << endl;
			try{
				if(file.length()>5 && file.substr(file.length()-5,5) == ".jack"){
					//Initialize a stream for the file
					files.push_back(file);
				} else if(file.find(".") == -1){
					struct stat temp;
					char* dir = &file[0];
					if(stat(dir,&temp) != 0){
						throw invalid_argument("Folder not found");
					};
					struct dirent *item;
					DIR *folder = opendir(dir);
					
					if(folder==nullptr){
						//cerr << "Folder open error!" << endl;
						throw invalid_argument("Folder open error!");
					}
					
					while((item=readdir(folder))){
						string fname = string(item->d_name);
						string fullfile = file+'/'+fname;
						if(fname.length()>5 && fname.substr(fname.length()-5,5) == ".jack"){
							files.push_back(fullfile);
						};
					};
					
				} else {
					throw invalid_argument("Invalid format!");
				};
			}catch(const out_of_range& e){
				cout << "Err with if!" << endl;
			}
			//Initialize stream for filewriting xml
			cout << to_string(files.size()) << endl;
			for(const string foldfil : files){
				cout << "Init " << foldfil << endl;
				ifstream foldfilin(foldfil);
				if(isUTF(foldfilin)){cout<<"Processing format UTF-8"<<endl;}else{cout<<"Processing format ASCII"<<endl;};
				ofstream foldfilout(foldfil.substr(0,foldfil.length()-5) + ".xml");
				cout << foldfil.substr(0,foldfil.length()-5) + "T.xml" << endl;
				JackTokenizer Tokeny(foldfilin);
				CompilationEngine Compiley(foldfilin,foldfilout,Tokeny);
				/*
				while(Tokeny.hasMoreTokens){
					Tokeny.advance();
					cout << tokenTypeEnumDeb[static_cast<int>(Tokeny.tokenType)] << endl;
					switch(Tokeny.tokenType){
						case tokenTypeEnum::KEYWORD:
							cout << "Keyword: " << keyWordEnumDeb[static_cast<int>(Tokeny.keyWord)] << endl;
							break;
						case tokenTypeEnum::IDENTIFIER:
							cout << "ID: " << Tokeny.identifier << endl;
							break;
						case tokenTypeEnum::SYMBOL:
							cout << "Symbol: " << Tokeny.symbol << endl;
							break;
						case tokenTypeEnum::STRING_CONST:
							cout << "SCon: " << Tokeny.stringVal << endl;
							break;
						case tokenTypeEnum::INT_CONST:
							cout << "ICon: " << Tokeny.intVal << endl;
							break;
					}
				};
				*/
				Compiley.CompileClass();
			}
			
		}
	private:
		vector<string> files;
		bool isUTF(ifstream &check){
			char buff[3];
			check.read(buff,3);
			if(buff[0]==(char)0xEF && buff[1]==(char)0xBB && buff[2]==(char)0xBF){return true;}else{check.seekg(0);return false;};
		}
};

int main(int argc, char* argv[]) {
	if(argc<2){
		cerr << "Insufficient arguments!";
	} else if(argc>2){
	  	cerr << "Too many arguments!";
	} else {
		try{
			JackAnalyzer(string(argv[1]));
		}catch(const out_of_range& e){
			cout << "Err on Init" << endl;
		}
	};
	return 0;
};
