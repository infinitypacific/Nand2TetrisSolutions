#ifndef PACIFIKY_JACKTOKENIZER_H
#define PACIFIKY_JACKTOKENIZER_H
#include <iostream>
#include <string>
#include <fstream>
class CompilationEngine {
	public:
		CompilationEngine(std::ifstream &ifilein,std::ofstream &ofilein):ifile(ifilein),ofile(ofilein){
			std::cout << "Init CEngine" << std::endl;
		};
		void CompileClass(){};
		void CompileClassVarDec(){};
		void CompileSubroutine(){};
		void compileParameterList(){};
	private:
		std::ifstream &ifile;
		std::ofstream &ofile;
};
#endif