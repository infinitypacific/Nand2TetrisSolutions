#ifndef PACIFIKY_JACKTOKENIZER_H
#define PACIFIKY_JACKTOKENIZER_H
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <stdexcept>
enum class keyWordEnum {CLASS,METHOD,FUNCTION,CONSTRUCTOR,INT,BOOLEAN,CHAR,VOID,VAR,STATIC,FIELD,LET,DO,IF,ELSE,WHILE,RETURN,TRUE,FALSE,J_NULL,THIS};
enum class tokenTypeEnum {KEYWORD,SYMBOL,IDENTIFIER,INT_CONST,STRING_CONST};
class JackTokenizer {
	public:
		JackTokenizer(ifstream &filein):file(filein){
			cout << "Init JToken" << endl;
			if(!(file.is_open())){
				throw invalid_argument("File not found!");
			}
			ostringstream temp;
			temp << file.rdbuf();
			content = temp.str();
			int tempind,temploc;
			while((temploc = tempind = content.find("//"))!=-1){
				for(;tempind<content.length() && content[tempind] != '\n';tempind++);
				content = content.erase(temploc,tempind-temploc);
				//cout << to_string((temploc = tempind = content.find("//"))!=-1) << endl;
			}
			//cout << content << endl;
			//Remove block and api comments
			
			while((temploc = tempind = content.find("/*"))!=-1){
				for(;(tempind+1)<content.length() && !(content[tempind] == '*' && content[tempind+1] == '/');tempind++);
				content = content.erase(temploc,(tempind+2)-temploc);
			}
			
			//Maybe use white space as a splitter?
			/*
			for(unsigned int i;i<content.length();i++){
				switch(content[i]){
					case '\t':
					case '\n':
						content = content.erase(i,1);
						break;
				};
			}
			*/
			//cout << content << endl;
			cout << "Comments removed!" << endl;
			//Set hasMoreTokens
			this->hasMoreTokens = this->checkToken();
		};
		void advance() {
			if(!this->hasMoreTokens){throw invalid_argument("No more tokens!");}
			char currentSelect = content[cursor];
			/*
			switch(content[cursor]){
				case '\t':
				case ' ':
				case '\n':
				    cursor++;
			};
			*/
			bool vldTok = false;
			if(this->isNum(currentSelect)){
				for(size_t i=cursor+1;i<content.length();i++){
					if(!(this->isNum(content[i]))){
						this->tokenType=tokenTypeEnum::INT_CONST;
						this->intVal = stoi(content.substr(cursor,i-cursor));
						vldTok=true;
						cursor=i;
						break;
					};
				};
			} else if(this->isQuote(currentSelect)){
				for(size_t i=cursor+1;i<content.length();i++){
					if(content[i]==currentSelect){
						this->tokenType = tokenTypeEnum::STRING_CONST;
						this->stringVal = content.substr(cursor+1,i-(cursor+1));
						cursor = i+1;
						vldTok=true;
						break;
					}
				};
			} else if(this->isSymb(currentSelect)){
				vldTok=true;
				this->tokenType = tokenTypeEnum::SYMBOL;
				this->symbol = currentSelect;
				cursor++;
			} else{
				for(size_t i=0;i<keywordStringLen;i++){
					string currKeySel = keywordString[i];
					if(currentSelect==currKeySel.at(0) && cursor+currKeySel.length()<content.length() && (this->isWhite(content[cursor+currKeySel.length()]) || this->isSymb(content[cursor+currKeySel.length()]))){
						//cout << currKeySel << endl;
						vldTok = true;
						for(unsigned int j=1;j<currKeySel.length();j++){
							if(content[cursor+j] != currKeySel.at(j)){vldTok=false;break;};
						}
						if(vldTok){
							this->tokenType=tokenTypeEnum::KEYWORD;
							this->keyWord=static_cast<keyWordEnum>(i);
							cursor += currKeySel.length();
							break;
						}
					}
				}
				if(!vldTok){
					char currCharSel;
					for(size_t i=cursor;i<content.length();i++){
						currCharSel = content[i];
						//cout << currCharSel << endl;
						if(this->isWhite(currCharSel) || this->isQuote(currCharSel) || this->isSymb(currCharSel)){
							this->tokenType = tokenTypeEnum::IDENTIFIER;
							this->identifier = content.substr(cursor,i-cursor);
							vldTok=true;
							cursor = i;
							break;
						}
					}
				}
			}
			//Have a routine look ahead to check for tokens (place cursor after white space so no need)
			this->hasMoreTokens = this->checkToken();
		};
		bool hasMoreTokens;
		tokenTypeEnum tokenType;
		keyWordEnum keyWord;
		string stringVal,identifier;
		int intVal;
		char symbol;
	private:
		ifstream &file;
		string content;
		const char symbolChar[19] = {'}','{','(',')','.','[',']',',',';','+','-','*','/','&','|','<','>','=','~'};
		const unsigned int symbolCharLen = 19;
		const string keywordString[21] = {"class","method","function","constructor","int","boolean","char","void","var","static","field","let","do","if","else","while","return","true","false","null","this"};
		const unsigned int keywordStringLen = 21;
		size_t cursor = 0;
		bool isNum(char check){
			switch(check){
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					return true;
					break;
				default:
					return false;
					break;
			}
		}
		bool isWhite(char check){
			switch(check){
				case ' ':
				case '\n':
				case '\t':
				case '\r':
					//Whoever invented CRLF should die in a fire
					return true;
					break;
				default:
					return false;
					break;
			}
		}
		bool isQuote(char check){
			switch(check){
				case '\'':
				case '\"':
					return true;
					break;
				default:
					return false;
					break;
			}
		}
		bool isSymb(char check){
			for(unsigned int i=0;i<symbolCharLen;i++){
				if(symbolChar[i]==check){
					return true;
					break;
				}
			}
			return false;
		}
		bool checkToken(){
			for(size_t i=cursor;i<content.length();i++){
				if(!(isWhite(content[i]))){
					cursor = i;
					//cout << "cursor: " << cursor << endl;
					return true;
				}
			}
			return false;
		}
};
#endif
