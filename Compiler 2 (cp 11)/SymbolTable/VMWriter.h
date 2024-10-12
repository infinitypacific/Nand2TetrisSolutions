#ifndef PACIFIKY_VMWRITER_H
#define PACIFIKY_VMWRITER_H
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
enum class segmentEnum {CONST,ARG,LOCAL,STATIC,THIS,THAT,POINTER,TEMP};
enum class commandEnum {ADD,SUB,NEG,EQ,GT,LT,AND,OR,NOT};
class VMWriter {
	public:
		VMWriter(ofstream &ofilein):ofile(ofilein){
			
		};
		void writePush(segmentEnum Segment, int Index){
			
			return;
		};
		void writePop(segmentEnum Segment, int Index){
			
			return;
		};
		void WriteArithmetic(commandEnum command){
			
			return;
		};
		void WriteLabel(string label){
			
			return;
		};
		void WriteGoto(string label){
			
			return;
		};
		void WriteIf(string label){
			
			return;
		};
		void writeCall(string name, int nArgs){
			
			return;
		};
		void writeFunction(string name, int nLocals){
			
			return;
		};
		void writeReturn(){
			
			return;
		};
		void close(){
			ofile.close();
			return;
		};
	private:
		ofstream &ofile;
};
#endif
