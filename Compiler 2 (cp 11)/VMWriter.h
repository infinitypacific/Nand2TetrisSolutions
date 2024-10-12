#ifndef PACIFIKY_VMWRITER_H
#define PACIFIKY_VMWRITER_H
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
enum class segmentEnum : short unsigned int {CONST,ARG,LOCAL,STATIC,THIS,THAT,POINTER,TEMP};
enum class commandEnum : short unsigned int {ADD,SUB,NEG,EQ,GT,LT,AND,OR,NOT};
class VMWriter {
	public:
		VMWriter(std::ofstream &ofilein):ofile(ofilein){
			
		};
		void writePush(segmentEnum Segment, int Index){
			ofile << "push " << segmentStr[static_cast<short unsigned int>(Segment)] << ' ' << Index << std::endl;
			return;
		};
		void writePop(segmentEnum Segment, int Index){
			ofile << "pop " << segmentStr[static_cast<short unsigned int>(Segment)] << ' ' << Index << std::endl;
			return;
		};
		void WriteArithmetic(commandEnum command){
			ofile << commandStr[static_cast<short unsigned int>(command)] << std::endl;
			return;
		};
		void WriteLabel(std::string label){
			ofile << "label " << label << std::endl;
			return;
		};
		void WriteGoto(std::string label){
			ofile << "goto " << label << std::endl;
			return;
		};
		void WriteIf(std::string label){
			ofile << "if-goto " << label << std::endl;
			return;
		};
		void writeCall(std::string name, short unsigned int nArgs){
			ofile << "call " << name << ' ' << nArgs << std::endl;
			return;
		};
		void writeFunction(std::string name, short unsigned int nLocals){
			ofile << "function " << name << ' ' << nLocals << std::endl;
			return;
		};
		void writeReturn(){
			ofile << "return" << std::endl;
			return;
		};
		void close(){
			ofile.close();
			return;
		};
	private:
		std::ofstream &ofile;
		std::string segmentStr[8] = {"constant","argument","local","static","this","that","pointer","temp"};
		std::string commandStr[9] = {"add","sub","neg","eq","gt","lt","and","or","not"};
};
#endif
