#ifndef PACIFIKY_COMPILATIONENGINE_H
#define PACIFIKY_COMPILATIONENGINE_H
#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"
//where are the segmentenum::this???
class CompilationEngine {
	public:
		CompilationEngine(std::ifstream &ifilein,std::ofstream &ofilein,JackTokenizer &tokenin,VMWriter &writein):ifile(ifilein),ofile(ofilein),Tokenizer(tokenin),Writer(writein){
			std::cout << "Init CEngine" << std::endl;
			if(!ofilein.is_open()){throw std::out_of_range("Output file inaccessable!");};
		};
		void CompileClass(){
			std::cout << "Parsing class" << std::endl;
			Tokenizer.advance();
			if(Tokenizer.tokenType == tokenTypeEnum::KEYWORD && Tokenizer.keyWord == keyWordEnum::CLASS){
				Tokenizer.advance();
				if(!(Tokenizer.tokenType == tokenTypeEnum::IDENTIFIER)){this->syntaxErr("Invalid class definition!","Classes need an identifier!");};
				className = Tokenizer.identifier;
				Tokenizer.advance();
				if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '{')){this->syntaxErr("Invalid class definition!","Class missing curly bracket!");};
				bool inClass = true;
				while(Tokenizer.hasMoreTokens && inClass){
					Tokenizer.advance();
					switch(Tokenizer.tokenType){
						case tokenTypeEnum::SYMBOL:
							if(Tokenizer.symbol == '}'){inClass=false;break;}else{this->syntaxErr("Invalid ClassBody!","Floating symbol: "+Tokenizer.symbol);};
						case tokenTypeEnum::IDENTIFIER:
							this->syntaxErr("Invalid ClassBody!","Floating identifier: " + Tokenizer.identifier);
						case tokenTypeEnum::INT_CONST:
							this->syntaxErr("Invalid ClassBody!","Floating integerConstant: " + to_string(Tokenizer.intVal));
						case tokenTypeEnum::STRING_CONST:
							this->syntaxErr("Invalid ClassBody!","Floating stringConstant: " + Tokenizer.stringVal);
						case tokenTypeEnum::KEYWORD:
							switch(Tokenizer.keyWord){
								case keyWordEnum::METHOD:
								case keyWordEnum::CONSTRUCTOR:
								case keyWordEnum::FUNCTION:
									this->CompileSubroutine();
									break;
								case keyWordEnum::FIELD:
								case keyWordEnum::STATIC:
									this->CompileClassVarDec();
									break;
								default:
									this->syntaxErr("Invalid ClassBody!","Floating KEYWORD: "+keywordString[static_cast<short unsigned int>(Tokenizer.keyWord)]);
							}
							break;
					}
				}
				cout << "Compilation Success!" << endl;
			}else{
				this->syntaxErr("Invalid CLASS!","Called without class;Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");
			}
		};
		void CompileClassVarDec(){
			std::string type;
			varType vt;
			if(!(Tokenizer.tokenType==tokenTypeEnum::KEYWORD)){this->syntaxErr("Invalid classVarDec!","Called without STATIC or FIELD!");};
			switch(Tokenizer.keyWord){
				case keyWordEnum::STATIC:
					vt = varType::STATIC;
					break;
				case keyWordEnum::FIELD:
					vt = varType::FIELD;
					break;
				default:
					this->syntaxErr("Invalid classVarDec!","Called without STATIC or FIELD!");
					break;
			};
			Tokenizer.advance();
			switch(Tokenizer.tokenType){
				case tokenTypeEnum::KEYWORD:
					if(this->isTypeKeyword(Tokenizer.keyWord)){
						type = keywordString[static_cast<short unsigned int>(Tokenizer.keyWord)];
					}else{
						this->syntaxErr("Invalid classVarDec!","classVarDec needs a type!");
					};
					break;
				case tokenTypeEnum::IDENTIFIER:
					type = Tokenizer.identifier;
					break;
				default:
					this->syntaxErr("Invalid classVarDec!","classVarDec needs a type!");
					break;
			}
			Tokenizer.advance();
			bool nedcomm = false;
			while(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ';') && Tokenizer.hasMoreTokens){
				switch(Tokenizer.tokenType){
					case tokenTypeEnum::SYMBOL:
						switch(Tokenizer.symbol){
							case ',':
								if(!nedcomm){
									this->syntaxErr("Invalid classVarDec!","classVarDec has a repeating comma!");
								};
								break;
							default:
								this->syntaxErr("Invalid classVarDec!","Floating symbol: "+Tokenizer.symbol);
						}
						break;
					case tokenTypeEnum::IDENTIFIER:
						if(!nedcomm){
							Tabler.Define(Tokenizer.identifier,type,vt);
						}else{
							this->syntaxErr("Invalid classVarDec!","classVarDec has a repeating identifier! (Did you accidentally add a space?)");
						};
						break;
					case tokenTypeEnum::INT_CONST:
						this->syntaxErr("Invalid classVarDec!","Floating integerConstant: "+to_string(Tokenizer.intVal));
					case tokenTypeEnum::STRING_CONST:
						this->syntaxErr("Invalid classVarDec!","Floating stringConstant: "+Tokenizer.stringVal);
					case tokenTypeEnum::KEYWORD:
						this->syntaxErr("Invalid classVarDec!","Floating keyword: "+keywordString[static_cast<short unsigned int>(Tokenizer.keyWord)]);
				}
				nedcomm = !nedcomm;
				Tokenizer.advance();
			}
		};
		void CompileSubroutine(){
			Tabler.startSubroutine();
			if(Tokenizer.tokenType != tokenTypeEnum::KEYWORD){this->debugErr("Invalid subroutineDec!","Called without keyword!");};
			keyWordEnum subType = this->Tokenizer.keyWord;
			switch(subType){
				case keyWordEnum::METHOD:
				case keyWordEnum::FUNCTION:
				case keyWordEnum::CONSTRUCTOR:
					break;
				default:
					this->syntaxErr("Invalid subroutineDec!","Called without def keyword!");
					break;
			};
			Tokenizer.advance();
			switch(Tokenizer.tokenType){
				case tokenTypeEnum::KEYWORD:
					if(this->isTypeKeyword(Tokenizer.keyWord) || Tokenizer.keyWord == keyWordEnum::VOID){
						subRetType = keywordString[static_cast<short unsigned int>(Tokenizer.keyWord)];
					}else{
						this->syntaxErr("Invalid subroutineDec!","subroutineDec needs an type!");
					};
					break;
				case tokenTypeEnum::IDENTIFIER:
					subRetType = Tokenizer.identifier;
					break;
				default:
					this->syntaxErr("Invalid subroutineDec!","subroutineDec needs an type!");
					break;
			};
			Tokenizer.advance();
			if(!(Tokenizer.tokenType == tokenTypeEnum::IDENTIFIER)){this->syntaxErr("Invalid subroutineDec!","subroutineDec needs an identifier!");};
			std::string subIdn = Tokenizer.identifier;
			Tokenizer.advance();
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '(')){this->syntaxErr("Invalid subroutineDec!","subroutineDec missing parameterList open paren!");};
			Tokenizer.advance();
			this->compileParameterList();
			//Parameter list is csv. if nexttok != ',' || identifier || type, stop
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ')')){this->syntaxErr("Invalid subroutineDec!","subroutineDec missing parameterList close paren!");};
			Tokenizer.advance();
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '{')){this->syntaxErr("Invalid subroutineBody!","subroutineBody missing open curly bracket!");};
			//Compile vardecs
			unsigned short int lclAmt = 0;
			Tokenizer.advance();
			while(Tokenizer.hasMoreTokens && Tokenizer.tokenType == tokenTypeEnum::KEYWORD && Tokenizer.keyWord == keyWordEnum::VAR){
				this->compileVarDec();
				Tokenizer.advance();
				lclAmt++;
			}
			//Most services handeled by jcakstd. Sys.init is one
			//subName = this->className+'.'+subIdn;
			Writer.writeFunction(this->className+'.'+subIdn,lclAmt);
			switch(subType){
				case keyWordEnum::METHOD:
					Writer.writePush(segmentEnum::ARG,0);
					Writer.writePop(segmentEnum::POINTER,0);
					break;
				case keyWordEnum::CONSTRUCTOR:
					Writer.writePush(segmentEnum::CONST,Tabler.VarCount(varType::FIELD));
					Writer.writeCall("Memory.alloc",1);
					Writer.writePop(segmentEnum::POINTER,0);
					break;
			};
			//labels are handled auto (see 8.6)
			this->compileStatements();
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '}')){this->syntaxErr("Invalid subroutineBody!","subroutineBody missing close curly bracket!");};
		};
		void compileParameterList(){
			int mode = 0;
			std::string type;
			while(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ')') && Tokenizer.hasMoreTokens){
				switch(Tokenizer.tokenType){
					case tokenTypeEnum::SYMBOL:
						switch(Tokenizer.symbol){
							case ',':
								if(mode != 2){
									this->syntaxErr("Invalid parameterList!","parameterList has a repeating comma!");
								};
								break;
							default:
								this->syntaxErr("Invalid parameterList!","Floating symbol: "+Tokenizer.symbol);
						}
						break;
					case tokenTypeEnum::IDENTIFIER:
						switch(mode){
							case 0:
								type = Tokenizer.identifier;
								break;
							case 1:
								Tabler.Define(Tokenizer.identifier,type,varType::ARG);
								//std::cout << Tabler.TypeOf(Tokenizer.identifier) << std::endl;
								break;
							default:
								this->syntaxErr("Invalid parameterList!","parameterList has a repeating identifier! (Did you accidentally add a space?)");
						}
						break;
					case tokenTypeEnum::INT_CONST:
						this->syntaxErr("Invalid parameterList!","Floating integerConstant: "+to_string(Tokenizer.intVal));
						break;
					case tokenTypeEnum::STRING_CONST:
						this->syntaxErr("Invalid parameterList!","Floating stringConstant: "+Tokenizer.stringVal);
						break;
					case tokenTypeEnum::KEYWORD:
						if(mode==0 && this->isTypeKeyword(Tokenizer.keyWord)){
							type = keywordString[static_cast<short unsigned int>(Tokenizer.keyWord)];
						}else{
							this->syntaxErr("Invalid parameterList!","Floating keyword: "+keywordString[static_cast<short unsigned int>(Tokenizer.keyWord)]);
						};
						break;
				}
				mode = mode >= 2 ? 0 : mode+1;
				Tokenizer.advance();
			}
		};
		void compileVarDec(){
			std::string type;
			if(!(Tokenizer.tokenType==tokenTypeEnum::KEYWORD && Tokenizer.keyWord == keyWordEnum::VAR)){this->syntaxErr("Invalid varDec!","Called without VAR!");};
			Tokenizer.advance();
			switch(Tokenizer.tokenType){
				case tokenTypeEnum::KEYWORD:
					if(this->isTypeKeyword(Tokenizer.keyWord)){
						type = keywordString[static_cast<short unsigned int>(Tokenizer.keyWord)];
					}else{
						this->syntaxErr("Invalid varDec!","varDec needs a type!");
					};
					break;
				case tokenTypeEnum::IDENTIFIER:
					type = Tokenizer.identifier;
					break;
				default:
					this->syntaxErr("Invalid varDec!","varDec needs a type!");
					break;
			}
			Tokenizer.advance();
			bool nedcomm = false;
			while(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ';') && Tokenizer.hasMoreTokens){
				switch(Tokenizer.tokenType){
					case tokenTypeEnum::SYMBOL:
						switch(Tokenizer.symbol){
							case ',':
								if(!nedcomm){
									this->syntaxErr("Invalid varDec!","varDec has a repeating comma!");
								};
								break;
							default:
								this->syntaxErr("Invalid varDec!","Floating symbol: "+Tokenizer.symbol);
						}
						break;
					case tokenTypeEnum::IDENTIFIER:
						if(!nedcomm){
							Tabler.Define(Tokenizer.identifier,type,varType::VAR);
						}else{
							this->syntaxErr("Invalid varDec!","varDec has a repeating identifier! (Did you accidentally add a space?)");
						};
						break;
					case tokenTypeEnum::INT_CONST:
						this->syntaxErr("Invalid varDec!","Floating integerConstant: "+to_string(Tokenizer.intVal));
					case tokenTypeEnum::STRING_CONST:
						this->syntaxErr("Invalid varDec!","Floating stringConstant: "+Tokenizer.stringVal);
					case tokenTypeEnum::KEYWORD:
						this->syntaxErr("Invalid varDec!","Floating keyword: "+keywordString[static_cast<short unsigned int>(Tokenizer.keyWord)]);
				}
				nedcomm = !nedcomm;
				Tokenizer.advance();
			}
		};
		void compileStatements(){
			bool vldloop = true;
			while(Tokenizer.hasMoreTokens && vldloop){
				switch(Tokenizer.tokenType){
					case tokenTypeEnum::KEYWORD:
						switch(Tokenizer.keyWord){
							case keyWordEnum::LET:
								this->compileLet();
								break;
							case keyWordEnum::DO:
								this->compileDo();
								break;
							case keyWordEnum::IF:
								this->compileIf();
								break;
							case keyWordEnum::WHILE:
								this->compileWhile();
								break;
							case keyWordEnum::RETURN:
								this->compileReturn();
								break;
							default:
								this->syntaxErr("Invalid statements!","Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+": "+keywordString[static_cast<short unsigned int>(Tokenizer.keyWord)]);
								break;
						}
						break;
					case tokenTypeEnum::SYMBOL:
						switch(Tokenizer.symbol){
							case '}':
								vldloop = false;
								continue;
								break;
							default:
								this->syntaxErr("Invalid statements!","Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+": "+Tokenizer.symbol);
								break;
						}
						break;
					default:
						this->syntaxErr("Invalid statements!","Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");
						break;
				};
				//if(vldloop){Tokenizer.advance();};
			};
		};
		void compileDo(){
			if(!(Tokenizer.tokenType == tokenTypeEnum::KEYWORD && Tokenizer.keyWord == keyWordEnum::DO)){this->syntaxErr("Invalid Do!","Called without DO!");};
			std::string funcName;
			Tokenizer.advance();
			if(!(Tokenizer.tokenType == tokenTypeEnum::IDENTIFIER)){this->syntaxErr("Invalid Do!","Missing identifier!");};
			funcName = Tokenizer.identifier;
			Tokenizer.advance();
			if(Tokenizer.tokenType != tokenTypeEnum::SYMBOL){this->syntaxErr("Invalid Do!","Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
			std::string callerType = "";
			switch(Tokenizer.symbol){
				case '.':
					callerType = Tabler.TypeOf(funcName);
					if(this->isPrimitive(callerType)){this->symbolErr("Invalid classFunc call!",funcName+" is a Jack Primitive!");}else if(callerType != ""){Writer.writePush(this->varToSeg(Tabler.KindOf(funcName)),Tabler.IndexOf(funcName));funcName = callerType;}
					//to be corrected
					funcName += '.';
					Tokenizer.advance();
					if(!(Tokenizer.tokenType == tokenTypeEnum::IDENTIFIER)){this->syntaxErr("Invalid Do!","Missing identifier!");};
					funcName += Tokenizer.identifier;
					Tokenizer.advance();
					if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '(')){this->syntaxErr("Invalid Do!","Missing open paren; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
					break;
				case '(':
					break;
				default:
					this->syntaxErr("Invalid Do!","Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+": "+Tokenizer.symbol);
					break;
			}
			short unsigned numArg;
			numArg = this->CompileExpressionList();
			//uncommitted )
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ')')){this->syntaxErr("Invalid Do!","Missing close paren; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
			Tokenizer.advance();
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ';')){this->syntaxErr("Invalid Do!","Missing semicolon; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
			Writer.writeCall(funcName,numArg);
			Writer.writePop(segmentEnum::TEMP,0);
			Tokenizer.advance();
		};
		void compileLet(){
			if(!(Tokenizer.tokenType == tokenTypeEnum::KEYWORD && Tokenizer.keyWord == keyWordEnum::LET)){this->debugErr("Invalid Let!","Called without LET!");};
			short unsigned outInd;
			varType outVarType;
			std::string outType;
			bool isClassName = true;
			bool hasIndex = false;
			//bool isField = false;
			segmentEnum outSeg;
			Tokenizer.advance();
			if(!(Tokenizer.tokenType == tokenTypeEnum::IDENTIFIER)){this->syntaxErr("Invalid Let!","Missing identifier!");};
			outType = Tabler.TypeOf(Tokenizer.identifier);
			//:mar:cur
			if(outType == ""){this->symbolErr("Invalid Let!","Variable "+Tokenizer.identifier+" is undefined!");}else if(this->isPrimitive(outType)){isClassName = false;}
			outInd = Tabler.IndexOf(Tokenizer.identifier);
			outVarType = Tabler.KindOf(Tokenizer.identifier);
			outSeg = this->varToSeg(outVarType);
			Tokenizer.advance();
			switch(Tokenizer.tokenType){
				case tokenTypeEnum::SYMBOL:
					switch(Tokenizer.symbol){
						case '[':
							hasIndex = true;
							Tokenizer.advance();
							this->CompileExpression();
							//Wouldn't work because of func calls
							//Writer.writePop(segmentEnum::TEMP,1);
							if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ']')){this->syntaxErr("Invalid Let!","Missing close bracket; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
							Tokenizer.advance();
							if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '=')){this->syntaxErr("Invalid Let!","Missing equal; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
						case '=':
							break;
						default:
							this->syntaxErr("Invalid Let!","Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+": "+Tokenizer.symbol);
							break;
					}
					break;
				default:
					this->syntaxErr("Invalid Let!","Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");
					break;
				
			}
			Tokenizer.advance();
			
			/*
			if(isClassName){
				Writer.writePush(outSeg,outInd);
				if(hasIndex){Writer.WriteArithmetic(commandEnum::ADD);}
				//this?
				Writer.writePop(segmentEnum::POINTER,1);
			}
			*/
			
			this->CompileExpression();
			//this will leave res on stack
			if(isClassName){
				//Place it in this instead of that?
				//solution I made is compute output after computing value
				if(hasIndex){
					Writer.writePop(segmentEnum::TEMP,0);
					Writer.writePush(outSeg,outInd);
					Writer.WriteArithmetic(commandEnum::ADD);
					Writer.writePop(segmentEnum::POINTER,1);
					Writer.writePush(segmentEnum::TEMP,0);
					Writer.writePop(segmentEnum::THAT,0);
				}else{
					Writer.writePush(outSeg,outInd);
					Writer.writePop(segmentEnum::POINTER,1);
					Writer.writePop(segmentEnum::THAT,0);
				}
				
				/*
				if(hasIndex){Writer.writePush(segmentEnum::TEMP,1);Writer.WriteArithmetic(commandEnum::ADD);}
				Writer.writePop(segmentEnum::POINTER,1);
				Writer.writePop(segmentEnum::THAT,0);
				*/
			}else{
				Writer.writePop(outSeg,outInd);
			}
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ';')){this->syntaxErr("Invalid Let!","Missing semicolon; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
			Tokenizer.advance();
		};
		void compileWhile(){
			if(!(Tokenizer.tokenType == tokenTypeEnum::KEYWORD && Tokenizer.keyWord == keyWordEnum::WHILE)){this->debugErr("Invalid While!","Called without WHILE!");};
			Writer.WriteLabel("startwhile"+to_string(this->whilecount));
			Tokenizer.advance();
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '(')){this->syntaxErr("Invalid While!","Missing open paren; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
			Tokenizer.advance();
			this->CompileExpression();
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ')')){this->syntaxErr("Invalid While!","Missing close paren; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
			Writer.WriteArithmetic(commandEnum::NOT);
			Writer.WriteIf("endwhile"+to_string(this->whilecount));
			Tokenizer.advance();
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '{')){this->syntaxErr("Invalid While!","Missing open curly bracket; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
			Tokenizer.advance();
			this->compileStatements();
			Writer.WriteGoto("startwhile"+to_string(this->whilecount));
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '}')){this->syntaxErr("Invalid While!","Missing close curly bracket; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
			Writer.WriteLabel("endwhile"+to_string(this->whilecount));
			this->whilecount++;
			Tokenizer.advance();
		};
		void compileReturn(){
			//mar:cur
			if(!(Tokenizer.tokenType == tokenTypeEnum::KEYWORD && Tokenizer.keyWord == keyWordEnum::RETURN)){this->debugErr("Invalid Return!","Called without RETURN!");};
			Tokenizer.advance();
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ';')){
				this->CompileExpression();
				if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ';')){this->syntaxErr("Invalid Return!","Missing semicolon; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");}
			}else{
				if(subRetType == "void"){Writer.writePush(segmentEnum::CONST,0);}else{this->symbolErr("Invalid Return!","Void return from function type "+subRetType);}
			}
			Writer.writeReturn();
			Tokenizer.advance();
		};
		void compileIf(){
			if(!(Tokenizer.tokenType == tokenTypeEnum::KEYWORD && Tokenizer.keyWord == keyWordEnum::IF)){this->debugErr("Invalid IF!","Called without IF!");};
			Tokenizer.advance();
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '(')){this->syntaxErr("Invalid If!","Missing open paren; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
			Tokenizer.advance();
			this->CompileExpression();
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ')')){this->syntaxErr("Invalid If!","Missing close paren; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
			Writer.WriteArithmetic(commandEnum::NOT);
			Writer.WriteIf("elseif"+to_string(this->ifcount));
			Tokenizer.advance();
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '{')){this->syntaxErr("Invalid If!","Missing open curly bracket; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
			Tokenizer.advance();
			this->compileStatements();
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '}')){this->syntaxErr("Invalid If!","Missing close curly bracket; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
			Writer.WriteGoto("endif"+to_string(this->ifcount));
			//else
			Tokenizer.advance();
			if(Tokenizer.tokenType == tokenTypeEnum::KEYWORD && Tokenizer.keyWord == keyWordEnum::ELSE){
				Tokenizer.advance();
				Writer.WriteLabel("elseif"+to_string(this->ifcount));
				if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '{')){this->syntaxErr("Invalid ELSE!","Missing open curly bracket; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
				Tokenizer.advance();
				this->compileStatements();
				if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '}')){this->syntaxErr("Invalid ELSE!","Missing close curly bracket; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
				Tokenizer.advance();
			}else{Writer.WriteLabel("elseif"+to_string(this->ifcount));};
			Writer.WriteLabel("endif"+to_string(this->ifcount));
			this->ifcount++;
		};
		void CompileExpression(){
			bool needOp = false;
			char prevOp = '\0';
			while(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && this->isExpressionTerminator(Tokenizer.symbol))){
				if(needOp){
					if(Tokenizer.tokenType==tokenTypeEnum::SYMBOL && this->isOpChar(Tokenizer.symbol)){
						prevOp = Tokenizer.symbol;
						Tokenizer.advance();
					}else{this->syntaxErr("Invalid EXPRESSION!","Repeated term; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");}
				}else{
					this->CompileTerm();
					if(prevOp != '\0'){
						switch(prevOp){
							case '+':
								Writer.WriteArithmetic(commandEnum::ADD);
								break;
							case '-':
								Writer.WriteArithmetic(commandEnum::SUB);
								break;
							case '*':
								Writer.writeCall("Math.multiply",2);
								break;
							case '/':
								Writer.writeCall("Math.divide",2);
								break;
							case '&':
								Writer.WriteArithmetic(commandEnum::AND);
								break;
							case '|':
								Writer.WriteArithmetic(commandEnum::OR);
								break;
							case '<':
								Writer.WriteArithmetic(commandEnum::LT);
								break;
							case '>':
								Writer.WriteArithmetic(commandEnum::GT);
								break;
							case '=':
								Writer.WriteArithmetic(commandEnum::EQ);
								break;
							default:
								this->debugErr("Invalid EXPRESSION!","Repeated term; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");
								break;
						}
					}
				}
				needOp = !needOp;
			};
			//Terminators: ) ] ; ,
		};
		void CompileTerm(){
			char uOp = '\0';
			std::string termID = "";
			std::string termType = "";
			short unsigned callType = 0;
			short unsigned numArg = 0;
			switch(Tokenizer.tokenType){
				case tokenTypeEnum::INT_CONST:
					Writer.writePush(segmentEnum::CONST,Tokenizer.intVal);
					Tokenizer.advance();
					break;
				case tokenTypeEnum::STRING_CONST:
					//Unfinished
					Writer.writePush(segmentEnum::CONST,Tokenizer.stringVal.length());
					Writer.writeCall("String.new",1);
					for(short unsigned i;i<Tokenizer.stringVal.length();i++){
						Writer.writePush(segmentEnum::CONST,static_cast<short unsigned>(Tokenizer.stringVal[i]));
						Writer.writeCall("String.appendChar",2);
						Writer.writePop(segmentEnum::TEMP,0);
					}
					Tokenizer.advance();
					break;
				case tokenTypeEnum::IDENTIFIER:
					termID = Tokenizer.identifier;
					termType = Tabler.TypeOf(Tokenizer.identifier);
					callType = 0;
					numArg = 0;
					Tokenizer.advance();
					switch(Tokenizer.tokenType){
						case tokenTypeEnum::SYMBOL:
							switch(Tokenizer.symbol){
								case '[':
									//Make meth?
									if(termType == ""){this->symbolErr("Invalid var indexing!","Undefined identifier!");}else if(this->isPrimitive(termType)){this->symbolErr("Invalid var indexing!",termID+" is a Jack Primitive!");};
									Writer.writePush(this->varToSeg(Tabler.KindOf(termID)),Tabler.IndexOf(termID));
									Tokenizer.advance();
									this->CompileExpression();
									if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ']')){this->syntaxErr("Invalid Term!","Arrayvar missing close bracket; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
									Writer.WriteArithmetic(commandEnum::ADD);
									Writer.writePop(segmentEnum::POINTER,1);
									Writer.writePush(segmentEnum::THAT,0);
									Tokenizer.advance();
									break;
								case '.':
									if(termType == ""){callType=2;}else if(this->isPrimitive(termType)){this->symbolErr("Invalid method call!",termID+" is a Jack Primitive!");}else{callType=1;};
									Tokenizer.advance();
									if(!(Tokenizer.tokenType == tokenTypeEnum::IDENTIFIER)){this->syntaxErr("Invalid Term!","subroutineCall Missing meth identifier!");};
									switch(callType){
										case 2:
											termID += '.'+Tokenizer.identifier;
											break;
										case 1:
											Writer.writePush(this->varToSeg(Tabler.KindOf(termID)),Tabler.IndexOf(termID));
											termID = termType+'.'+Tokenizer.identifier;
										default:
											this->debugErr("Invalid callType","Called not using method or static on .");
									}
									Tokenizer.advance();
									if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '(')){this->syntaxErr("Invalid Term!","subroutineCall Missing open paren; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
								case '(':
									numArg = this->CompileExpressionList();
									//uncommitted )
									if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ')')){this->syntaxErr("Invalid Term!","subroutineCall Missing close paren; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
									switch(callType){
										case 0:
											Writer.writeCall(this->className+'.'+termID,numArg);
											break;
										case 1:
											Writer.writeCall(termID,numArg+1);
											
											break;
										case 2:
											Writer.writeCall(termID,numArg);
											break;
									}
									Tokenizer.advance();
									break;
								default:
									Writer.writePush(this->varToSeg(Tabler.KindOf(termID)),Tabler.IndexOf(termID));
									break;
							}
						default:
							break;
					}
					break;
				case tokenTypeEnum::SYMBOL:
					switch(Tokenizer.symbol){
						case '(':
							Tokenizer.advance();
							this->CompileExpression();
							if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ')')){this->syntaxErr("Invalid TERM!","Missing expression close paren!");};
							Tokenizer.advance();
							break;
						case '-':
						case '~':
							uOp = Tokenizer.symbol;
							Tokenizer.advance();
							this->CompileTerm();
							switch(uOp){
								case '-':
									Writer.WriteArithmetic(commandEnum::NEG);
									break;
								case '~':
									Writer.WriteArithmetic(commandEnum::NOT);
									break;
								default:
									this->debugErr("Invalid UOP","Called without UOP Symbol.");
							}
							break;
						default:
							this->syntaxErr("Invalid TERM!","Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+": "+Tokenizer.symbol);
							break;
					};
					break;
				case tokenTypeEnum::KEYWORD:
					switch(Tokenizer.keyWord){
						case keyWordEnum::THIS:
							Writer.writePush(segmentEnum::POINTER,0);
							break;
						case keyWordEnum::TRUE:
							Writer.writePush(segmentEnum::CONST,1);
							Writer.WriteArithmetic(commandEnum::NEG);
							break;
						case keyWordEnum::FALSE:
						case keyWordEnum::J_NULL:
							Writer.writePush(segmentEnum::CONST,0);
							break;
						default:
							this->syntaxErr("Invalid TERM!","Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+": "+keywordString[static_cast<short unsigned int>(Tokenizer.keyWord)]);
					};
					Tokenizer.advance();
					break;
			};
		};
		short unsigned CompileExpressionList(){
			bool nedComma = false;
			short unsigned expAmt = 0;
			Tokenizer.advance();
			while(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ')') && Tokenizer.hasMoreTokens){
				if(nedComma){
					if(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ','){Tokenizer.advance();}else{this->syntaxErr("Invalid ExpressionList!","Repeated expression with no sep; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
				}else{
					this->CompileExpression();
					expAmt++;
				}
				nedComma = !nedComma;
			}
			return expAmt;
		};
	private:
		std::ifstream &ifile;
		std::ofstream &ofile;
		JackTokenizer &Tokenizer;
		VMWriter &Writer;
		SymbolTable Tabler;
		const std::string tokenTypeTags[5] = {"keyword","symbol","identifier","integerConstant","stringConstant"};
		const std::string keywordString[21] = {"class","method","function","constructor","int","boolean","char","void","var","static","field","let","do","if","else","while","return","true","false","null","this"};
		std::string className;
		keyWordEnum subType;
		std::string subRetType;
		short unsigned ifcount = 0;
		short unsigned whilecount = 0;
		void syntaxErr(std::string etype,std::string emsg) {ofile.close();ifile.close();throw std::invalid_argument("SYNTAXERR:\n" + etype + "\n" + emsg);}
		void symbolErr(std::string etype,std::string emsg) {ofile.close();ifile.close();throw std::out_of_range("SYMBOLERR:\n" + etype + "\n" + emsg);}
		void debugErr(std::string etype,std::string emsg) {ofile.close();ifile.close();throw std::invalid_argument("COSMICERR:\nA cosmic ray may have hit your computer.\n" + etype + "\n" + emsg);}
		bool isTypeKeyword(keyWordEnum check){
			switch(check){
				case keyWordEnum::INT:
				case keyWordEnum::CHAR:
				case keyWordEnum::BOOLEAN:
					return true;
				default:
					return false;
			}
		}
		bool isOpChar(char check){
			switch(check){
				case '+':
				case '-':
				case '*':
				case '/':
				case '&':
				case '|':
				case '<':
				case '>':
				case '=':
					return true;
				default:
					return false;
			}
		}
		segmentEnum varToSeg(varType chk){
			switch(chk){
				case varType::STATIC:
					return segmentEnum::STATIC;
					break;
				case varType::FIELD:
					return segmentEnum::THIS;
					break;
				case varType::ARG:
					return segmentEnum::ARG;
					break;
				case varType::VAR:
					return segmentEnum::LOCAL;
					break;
				default:
					this->debugErr("Invalid varType!","varToSeg call invalid.");
					break;
			}
		}
		bool isKeywordConst(keyWordEnum check){
			switch(check){
				case keyWordEnum::TRUE:
				case keyWordEnum::FALSE:
				case keyWordEnum::THIS:
				case keyWordEnum::J_NULL:
					return true;
				default:
					return false;
			}
		}
		bool isPrimitive(std::string check){
			if(check == keywordString[4] || check == keywordString[5] || check == keywordString[6]){
				return true;
			}
			return false;
		}
		bool isExpressionTerminator(char check){
			switch(check){
				case ')':
				case ']':
				case ';':
				case ',':
					return true;
				default:
					return false;
			}
		}
};
#endif
