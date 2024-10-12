#ifndef PACIFIKY_SYMBOLTABLE_H
#define PACIFIKY_SYMBOLTABLE_H
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
//#include <tuple>
//#include <array>
enum class varType : unsigned short {STATIC, FIELD, ARG, VAR, NONE};
class SymbolTable {
	public:
		SymbolTable(){
			
		};
		void startSubroutine(){
			subHashName.clear();
			subHashType.clear();
			subHashVarType.clear();
			subHashIndicies.clear();
			segamt[2]=segamt[3]=0;
			return;
		};
		void Define(std::string name, std::string type, varType kind){
			std::cout << "Name: " << name << ", Type: " << type << ", Kind: " << static_cast<short unsigned int>(kind) << std::endl;
			switch(kind){
				case varType::STATIC:
				case varType::FIELD:
					for(short unsigned int i=0;i<classHashName.size();i++){
						if(classHashName[i]==name){throw std::invalid_argument("SYMBOLERR: Variable already defined");};
					};
					classHashName.push_back(name);
					classHashType.push_back(type);
					classHashVarType.push_back(kind);
					classHashIndicies.push_back(segamt[static_cast<short unsigned int>(kind)]);
					break;
				case varType::ARG:
				case varType::VAR:
					for(short unsigned int i=0;i<subHashName.size();i++){
						if(subHashName[i]==name){throw std::invalid_argument("SYMBOLERR: Variable already defined");};
					};
					subHashName.push_back(name);
					subHashType.push_back(type);
					subHashVarType.push_back(kind);
					subHashIndicies.push_back(segamt[static_cast<short unsigned int>(kind)]);
					break;
			}
			segamt[static_cast<short unsigned int>(kind)]++;
			return;
		};
		short unsigned int VarCount(varType kind){
			/*
			short unsigned int amount;
			switch(kind){
				case varType::STATIC:
				case varType::FIELD:
					for(short unsigned int i=0;i<classHashVarType.size();i++){
						if(classHashVarType[i]==kind){amount++};
					};
					break;
				case varType::ARG:
				case varType::VAR:
					for(short unsigned int i=0;i<subHashVarType.size();i++){
						if(subHashVarType[i]==kind){amount++};
					};
					break;
			}
			return amount;
			*/
			return segamt[static_cast<short unsigned int>(kind)];
		};
		varType KindOf(std::string name){
			for(short unsigned int i=0;i<subHashName.size();i++){
				if(subHashName[i]==name){return subHashVarType[i];};
			};
			for(short unsigned int i=0;i<classHashName.size();i++){
				if(classHashName[i]==name){return classHashVarType[i];};
			};
			return varType::NONE;
		};
		std::string TypeOf(std::string name){
			for(short unsigned int i=0;i<subHashName.size();i++){
				if(subHashName[i]==name){return subHashType[i];};
			};
			//std::cout << "not in subhash" << std::endl;
			//std::cout << classHashName.size() << std::endl;
			//std::cout << classHashType.size() << std::endl;
			for(short unsigned int i=0;i<classHashName.size();i++){
				//std::cout << i << std::endl;
				if(classHashName[i]==name){return classHashType[i];};
			};
			//std::cout << "not in classHash" << std::endl;
			return "";
		};
		short unsigned int IndexOf(std::string name){
			for(short unsigned int i=0;i<subHashName.size();i++){
				if(subHashName[i]==name){return subHashIndicies[i];};
			};
			for(short unsigned int i=0;i<classHashName.size();i++){
				if(classHashName[i]==name){return classHashIndicies[i];};
			};
			return 0;
		};
	private:
		//std::vector<std::tuple<std::string,std::string,varType>> classHash;
		//std::vector<std::tuple<std::string,std::string,varType>> subHash;
		std::vector<std::string> classHashName;
		std::vector<std::string> classHashType;
		std::vector<varType> classHashVarType;
		std::vector<unsigned short int> classHashIndicies;
		std::vector<std::string> subHashName;
		std::vector<std::string> subHashType;
		std::vector<varType> subHashVarType;
		std::vector<unsigned short int> subHashIndicies;
		//short unsigned int samt,famt,vamt,aamt;
		short unsigned int segamt[4] = {0,0,0,0};
};
#endif
