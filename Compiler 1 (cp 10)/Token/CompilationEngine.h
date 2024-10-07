#ifndef PACIFIKY_COMPILATIONENGINE_H
#define PACIFIKY_COMPILATIONENGINE_H
#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include "JackTokenizer.h"
class CompilationEngine {
	public:
		CompilationEngine(std::ifstream &ifilein,std::ofstream &ofilein,JackTokenizer &tokenin):ifile(ifilein),ofile(ofilein),Tokenizer(tokenin){
			std::cout << "Init CEngine" << std::endl;
			if(!ofilein.is_open()){throw out_of_range("Output file inaccessable!");};
		};
		void CompileClass(){
			ofile << "<tokens>" << std::endl;
			while(Tokenizer.hasMoreTokens){
				Tokenizer.advance();
				ofile << "<" << tokenTypeTags[static_cast<int>(Tokenizer.tokenType)] << ">" << std::endl;
				switch(Tokenizer.tokenType){
					case tokenTypeEnum::KEYWORD:
						ofile << keywordString[static_cast<int>(Tokenizer.keyWord)] << std::endl;
						break;
					case tokenTypeEnum::IDENTIFIER:
						ofile << Tokenizer.identifier << std::endl;
						break;
					case tokenTypeEnum::SYMBOL:
						ofile << Tokenizer.symbol << std::endl;
						break;
					case tokenTypeEnum::STRING_CONST:
						ofile << Tokenizer.stringVal << std::endl;
						break;
					case tokenTypeEnum::INT_CONST:
						ofile << Tokenizer.intVal << std::endl;
						break;
				}
				ofile << "</" << tokenTypeTags[static_cast<int>(Tokenizer.tokenType)] << ">" << std::endl;
			}
			ofile << "</tokens>" << std::endl;
			ifile.close();
			ofile.close();
			cout << "Compilation Success!" << endl;
		};
		void CompileClassVarDec(){};
		void CompileSubroutine(){};
		void compileParameterList(){};
		void compileVarDec(){};
		void compileStatements(){};
		void compileDo(){};
		void compileLet(){};
		void compileWhile(){};
		void compileReturn(){};
		void compileIf(){};
		void CompileExpression(){};
		void CompileTerm(){};
		void CompileExpressionList(){};
	private:
		std::ifstream &ifile;
		std::ofstream &ofile;
		JackTokenizer &Tokenizer;
		const std::string tokenTypeTags[5] = {"keyword","symbol","identifier","integerConstant","stringConstant"};
		const std::string keywordString[21] = {"class","method","function","constructor","int","boolean","char","void","var","static","field","let","do","if","else","while","return","true","false","null","this"};
};
#endif
