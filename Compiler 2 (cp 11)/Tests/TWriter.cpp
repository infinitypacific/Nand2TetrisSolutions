#include <iostream>
#include <string>
#include <fstream>
#include "../VMWriter.h"

int main(){
	std::ofstream ofile("Test.vm");
	VMWriter writey(ofile);
	writey.writePush(segmentEnum::CONST,1);
	writey.writePop(segmentEnum::LOCAL,0);
	writey.WriteArithmetic(commandEnum::ADD);
	writey.WriteLabel("Test");
	writey.WriteGoto("Test");
	writey.WriteIf("Test");
	writey.writeCall("TFunc",2);
	writey.writeFunction("TFunc",4);
	writey.writeReturn();
	writey.close();
}
