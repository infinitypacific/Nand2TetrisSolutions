#ifndef PACIFIKY_COMPILATIONENGINE_H
#define PACIFIKY_COMPILATIONENGINE_H
#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"
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
							if(Tokenizer.symbol == '}'){this->commitToken();inClass=false;break;}else{this->syntaxErr("Invalid ClassBody!","Floating symbol: "+Tokenizer.symbol);};
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
			if(Tokenizer.tokenType != tokenTypeEnum::KEYWORD){this->syntaxErr("Invalid subroutineDec!","Called without keyword!");}};
			keyWordEnum subType = Tokenizer.keyWord;
			switch(subType){
				case keyWordEnum::METHOD:
					
					break;
				case keyWordEnum::FUNCTION:
					
					break;
				case keyWordEnum::CONSTRUCTOR:
					
					break;
				default:
					this->syntaxErr("Invalid subroutineDec!","Called without def keyword!");
					break;
			}
			Tokenizer.advance();
			std::string subRetType;
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
			}
			Tokenizer.advance();
			if(!(Tokenizer.tokenType == tokenTypeEnum::IDENTIFIER)){this->syntaxErr("Invalid subroutineDec!","subroutineDec needs an identifier!");};
			std::string subIdn = Tokenizer.identifier;
			Tokenizer.advance();
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '(')){this->syntaxErr("Invalid subroutineDec!","subroutineDec missing parameterList open paren!");};
			Tokenizer.advance();
			this->compileParameterList();
			//Parameter list is csv. if nexttok != ',' || identifier || type, stop
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ')')){this->syntaxErr("Invalid subroutineDec!","subroutineDec missing parameterList close paren!");};
			this->commitToken();
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
			Writer.writeFunction(this->className+'.'+subIdn,lclAmt);
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
			switch(Tokenizer.symbol){
				case '.':
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
			Tokenizer.advance();
		};
		void compileLet(){
			if(!(Tokenizer.tokenType == tokenTypeEnum::KEYWORD && Tokenizer.keyWord == keyWordEnum::LET)){this->syntaxErr("Invalid Let!","Called without LET!");};
			short unsigned outInd;
			varType outVarType;
			Tokenizer.advance();
			if(!(Tokenizer.tokenType == tokenTypeEnum::IDENTIFIER)){this->syntaxErr("Invalid Let!","Missing identifier!");};
			outInd = Tabler.IndexOf(Tokenizer.identifier);
			outVarType = Tabler.KindOf(Tokenizer.identifier);
			Tokenizer.advance();
			switch(Tokenizer.tokenType){
				case tokenTypeEnum::SYMBOL:
					switch(Tokenizer.symbol){
						case '[':
							Tokenizer.advance();
							this->CompileExpression();
							if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ']')){this->syntaxErr("Invalid Let!","Missing close bracket; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
							this->commitToken();
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
			this->CompileExpression();
			//this will leave res on stack
			
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ';')){this->syntaxErr("Invalid Let!","Missing semicolon; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
			Tokenizer.advance();
		};
		void compileWhile(){
			ofile << "<whileStatement>" << std::endl;
			if(!(Tokenizer.tokenType == tokenTypeEnum::KEYWORD && Tokenizer.keyWord == keyWordEnum::WHILE)){this->syntaxErr("Invalid While!","Called without WHILE!");};
			this->commitToken();
			Tokenizer.advance();
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '(')){this->syntaxErr("Invalid While!","Missing open paren; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
			this->commitToken();
			Tokenizer.advance();
			this->CompileExpression();
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ')')){this->syntaxErr("Invalid While!","Missing close paren; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
			this->commitToken();
			Tokenizer.advance();
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '{')){this->syntaxErr("Invalid While!","Missing open curly bracket; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
			this->commitToken();
			Tokenizer.advance();
			this->compileStatements();
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '}')){this->syntaxErr("Invalid While!","Missing close curly bracket; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
			this->commitToken();
			Tokenizer.advance();
			ofile << "</whileStatement>" << std::endl;
		};
		void compileReturn(){
			ofile << "<returnStatement>" << std::endl;
			if(!(Tokenizer.tokenType == tokenTypeEnum::KEYWORD && Tokenizer.keyWord == keyWordEnum::RETURN)){this->syntaxErr("Invalid Return!","Called without RETURN!");};
			this->commitToken();
			Tokenizer.advance();
			/*
			switch(Tokenizer.tokenType){
				case tokenTypeEnum::SYMBOL:
					switch(Tokenizer.symbol){
						case '':
							
							
						default:
							
							
					}
					break;
				default:
					this->CompileExpression();
					break;
			}
			*/
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ';')){
				this->CompileExpression();
				if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ';')){this->syntaxErr("Invalid Return!","Missing semicolon; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");}
			}
			this->commitToken();
			Tokenizer.advance();
			ofile << "</returnStatement>" << std::endl;
		};
		void compileIf(){
			ofile << "<ifStatement>" << std::endl;
			if(!(Tokenizer.tokenType == tokenTypeEnum::KEYWORD && Tokenizer.keyWord == keyWordEnum::IF)){this->syntaxErr("Invalid IF!","Called without IF!");};
			this->commitToken();
			Tokenizer.advance();
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '(')){this->syntaxErr("Invalid If!","Missing open paren; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
			this->commitToken();
			Tokenizer.advance();
			this->CompileExpression();
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ')')){this->syntaxErr("Invalid If!","Missing close paren; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
			this->commitToken();
			Tokenizer.advance();
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '{')){this->syntaxErr("Invalid If!","Missing open curly bracket; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
			this->commitToken();
			Tokenizer.advance();
			this->compileStatements();
			if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '}')){this->syntaxErr("Invalid If!","Missing close curly bracket; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
			this->commitToken();
			//else
			Tokenizer.advance();
			if(Tokenizer.tokenType == tokenTypeEnum::KEYWORD && Tokenizer.keyWord == keyWordEnum::ELSE){
				this->commitToken();
				Tokenizer.advance();
				if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '{')){this->syntaxErr("Invalid ELSE!","Missing open curly bracket; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
				this->commitToken();
				Tokenizer.advance();
				this->compileStatements();
				if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '}')){this->syntaxErr("Invalid ELSE!","Missing close curly bracket; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
				this->commitToken();
				Tokenizer.advance();
			};
			ofile << "</ifStatement>" << std::endl;
		};
		void CompileExpression(){
			ofile << "<expression>" << std::endl;
			bool needOp = false;
			while(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && this->isExpressionTerminator(Tokenizer.symbol))){
				if(needOp){
					if(Tokenizer.tokenType==tokenTypeEnum::SYMBOL && this->isOpChar(Tokenizer.symbol)){
						this->commitToken();
						Tokenizer.advance();
					}else{this->syntaxErr("Invalid EXPRESSION!","Repeated term; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");}
				}else{
					this->CompileTerm();
				}
				needOp = !needOp;
			};
			//Terminators: ) ] ; ,
			ofile << "</expression>" << std::endl;
		};
		void CompileTerm(){
			ofile << "<term>" << std::endl;
			switch(Tokenizer.tokenType){
				case tokenTypeEnum::INT_CONST:
				case tokenTypeEnum::STRING_CONST:
					this->commitToken();
					Tokenizer.advance();
					break;
				case tokenTypeEnum::IDENTIFIER:
					this->commitToken();
					ofile << "<info>Name: " << Tokenizer.identifier << ",Type: " << Tabler.TypeOf(Tokenizer.identifier) << ",Kind: " << static_cast<short unsigned int>(Tabler.KindOf(Tokenizer.identifier)) << ",Ind: " << Tabler.IndexOf(Tokenizer.identifier) << "</info>" << std::endl;
					Tokenizer.advance();
					switch(Tokenizer.tokenType){
						case tokenTypeEnum::SYMBOL:
							switch(Tokenizer.symbol){
								case '[':
									//Make meth?
									this->commitToken();
									Tokenizer.advance();
									this->CompileExpression();
									if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ']')){this->syntaxErr("Invalid Term!","Arrayvar missing close bracket; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
									this->commitToken();
									Tokenizer.advance();
									break;
								case '.':
									this->commitToken();
									Tokenizer.advance();
									if(!(Tokenizer.tokenType == tokenTypeEnum::IDENTIFIER)){this->syntaxErr("Invalid Term!","subroutineCall Missing meth identifier!");};
									this->commitToken();
									Tokenizer.advance();
									if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == '(')){this->syntaxErr("Invalid Term!","subroutineCall Missing open paren; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
								case '(':
									this->commitToken();
									//Tokenizer.advance();
									this->CompileExpressionList();
									//uncommitted )
									if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ')')){this->syntaxErr("Invalid Term!","subroutineCall Missing close paren; Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+"!");};
									this->commitToken();
									Tokenizer.advance();
									break;
								default:
									break;
							}
						default:
							break;
					}
					break;
				case tokenTypeEnum::SYMBOL:
					switch(Tokenizer.symbol){
						case '(':
							this->commitToken();
							Tokenizer.advance();
							this->CompileExpression();
							if(!(Tokenizer.tokenType == tokenTypeEnum::SYMBOL && Tokenizer.symbol == ')')){this->syntaxErr("Invalid TERM!","Missing expression close paren!");};
							this->commitToken();
							Tokenizer.advance();
							break;
						case '-':
						case '~':
							this->commitToken();
							Tokenizer.advance();
							this->CompileTerm();
							break;
						default:
							this->syntaxErr("Invalid TERM!","Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+": "+Tokenizer.symbol);
							break;
					};
					break;
				case tokenTypeEnum::KEYWORD:
					if(this->isKeywordConst(Tokenizer.keyWord)){
						this->commitToken();
						Tokenizer.advance();
					}else{
						this->syntaxErr("Invalid TERM!","Floating "+tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)]+": "+keywordString[static_cast<short unsigned int>(Tokenizer.keyWord)]);
					}
					break;
			};
			ofile << "</term>" << std::endl;
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
		void commitToken(){
			ofile << "<" << tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)] << ">" << std::endl;
			//ofile << "<" << tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)] << "> ";
			switch(Tokenizer.tokenType){
				case tokenTypeEnum::KEYWORD:
					ofile << keywordString[static_cast<short unsigned int>(Tokenizer.keyWord)];
					break;
				case tokenTypeEnum::IDENTIFIER:
					ofile << Tokenizer.identifier;
					break;
				case tokenTypeEnum::SYMBOL:
					switch(Tokenizer.symbol){
						case '&':
							ofile << "&amp;";
							break;
						case '<':
							ofile << "&lt;";
							break;
						case '>':
							ofile << "&gt;";
							break;
						default:
							ofile << Tokenizer.symbol;
							break;
					}
					break;
				case tokenTypeEnum::STRING_CONST:
					//ofile << Tokenizer.stringVal << std::endl;
					ofile << Tokenizer.stringVal;
					break;
				case tokenTypeEnum::INT_CONST:
					//ofile << Tokenizer.intVal << std::endl;
					ofile << Tokenizer.intVal;
					break;
			}
			ofile << std::endl << "</" << tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)] << ">" << std::endl;
			//ofile << " </" << tokenTypeTags[static_cast<short unsigned int>(Tokenizer.tokenType)] << ">" << std::endl;
		}
		void syntaxErr(std::string etype,std::string emsg) {ofile.close();ifile.close();throw std::invalid_argument("SYNTAXERR:\n" + etype + "\n" + emsg);}
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
		/*
		bool isUnaryOpChar(char check){
			switch(check){
				case '~':
				case '-':
					return true;
				default:
					return false;
			}
		}
		*/
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
