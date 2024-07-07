import os
class Parser:
	def __init__(self, name):
		#open file and remove white space and comments
		f = open(name, "r")
		cont = f.read()
		f.close()
		csp = cont.split("\t")
		cont = "".join(csp)
		csp = cont.split("\ufeff")
		self.file = removenull("".join(csp).split("\n"))
		self.hasMoreCommands = True
		self.api = False
		self.commandType = ""
		self.arg1 = ""
		self.arg2 = 0
		self.line = 0
		self.arithmetic = ["add", "sub", "neg", "eq", "gt", "lt", "and", "or", "not"]
		
	def advance(self):
		line = removenull(self.file[self.line].split(" "))
		"""
		Static: 16
		Stack: 256
		Heap: 2048
		"""
		if self.line+1 == len(self.file):
			self.hasMoreCommands = False
		
		# needs to handle api comments and line comments
		
		if "//" in line:
			del line[line.index("//"):]
			if(len(line) <= 0 and self.hasMoreCommands):
				self.line += 1
				self.advance()
				return
			elif(len(line) <= 0 and self.hasMoreCommands == False):
				self.line += 1
				return
		
		if(line[0] == "/*"):
			self.api = True
			self.line += 1
			self.advance()
			return
		
		if(self.api):
			if(line[-1] == "*/"):
				self.api = False
			self.line += 1
			self.advance()
			return
		
		if(len(line) > 3):
			raise Exception("Too many arguments")
			
		if(len(line) <= 0):
			raise Exception("Insufficient commands")
		
		if(line[0] == "push"):
			self.commandType = "C_PUSH"
			self.arg1 = line[1]
			self.arg2 = int(line[2])
		elif(line[0] == "pop"):
			self.commandType = "C_POP"
			self.arg1 = line[1]
			self.arg2 = int(line[2])	
		elif(line[0] in self.arithmetic):
			self.commandType = "C_ARITHMETIC"
			self.arg1 = ""
			self.arg2 = 0
		elif(line[0] == "label"):
			self.commandType = "C_LABEL"
			self.arg1 = line[1]
			self.arg2 = 0
		elif(line[0] == "function"):
			self.commandType = "C_FUNCTION"
			self.arg1 = line[1]
			self.arg2 = int(line[2])
		elif(line[0] == "goto"):
			self.commandType = "C_GOTO"
			self.arg1 = line[1]
			self.arg2 = 0
		elif(line[0] == "if-goto"):
			self.commandType = "C_IF"
			self.arg1 = line[1]
			self.arg2 = 0
		elif(line[0] == "call"):
			self.commandType = "C_CALL"
			self.arg1 = line[1]
			self.arg2 = int(line[2])
		elif(line[0] == "return"):
			self.commandType = "C_RETURN"
			self.arg1 = ""
			self.arg2 = 0
		else:
			raise Exception("Invalid Command")
		
		if(self.arg2 < 0):
			raise Exception("Negative integers are not allowed")
		
		self.line += 1
		
class CodeWriter:
	def __init__(self, name):
		self.file = open(name, "w")
		self.comp = 0 #Count comparisons
		self.init = False #Check if sys.init exists
		self.current = "" #current vm file
		self.func = "" #current function
		"""
		SP,LCL,ARG,THIS,THAT
		Static: 16
		Stack: 256
		Heap: 2048
		"""
	
	def writeInit(self):
		self.file.write("""@256
D=A
@SP
M=D
@ARG
M=D
@LCL
M=D
@Sys.init
0;JMP
""")
	
	def setFileName(self,fileName):
		self.file = open(fileName, "w")
		
	def writeArithmetic(self,command):
		if command == "add":
			self.file.write(f"""@SP
M=M-1
A=M
D=M
@SP
A=M-1
M=D+M
""")
		elif command == "sub":
			self.file.write(f"""@SP
M=M-1
A=M
D=M
@SP
A=M-1
D=M-D
@SP
A=M-1
M=D
""")
		elif command == "neg":
			self.file.write(f"""@SP
A=M-1
M=-M
""")
		elif command == "eq":
			self.file.write(f"""@SP
M=M-1
A=M
D=M
@SP
A=M-1
D=D-M
@TRU{self.comp}
D;JEQ
D=0
@END{self.comp}
0;JMP
(TRU{self.comp})
D=-1
(END{self.comp})
@SP
A=M-1
M=D
""")
			self.comp += 1
		elif command == "lt":
			self.file.write(f"""@SP
M=M-1
A=M
D=M
@SP
A=M-1
D=D-M
@TRU{self.comp}
D;JGT
D=0
@END{self.comp}
0;JMP
(TRU{self.comp})
D=-1
(END{self.comp})
@SP
A=M-1
M=D
""")
			self.comp += 1
		elif command == "gt":
			self.file.write(f"""@SP
M=M-1
A=M
D=M
@SP
A=M-1
D=D-M
@TRU{self.comp}
D;JLT
D=0
@END{self.comp}
0;JMP
(TRU{self.comp})
D=-1
(END{self.comp})
@SP
A=M-1
M=D
""")
			self.comp += 1
		elif command == "and":
			self.file.write(f"""@SP
M=M-1
A=M
D=M
@SP
A=M-1
M=D&M
""")
		elif command == "or":
			self.file.write(f"""@SP
M=M-1
A=M
D=M
@SP
A=M-1
M=D|M
""")
		elif command == "not":
			self.file.write(f"""@SP
A=M-1
M=!M
""")
		
	def WritePushPop(self,command,segment,index):
		# Assign value to a first with if statements then commit using concluding code
		# is temp in heap?
		
		if command == "C_PUSH":
			at = f"""D=M
@{index}
A=D+A"""
			if(index == 0):
				at = "A=M"
			if(index == 1):
				at = "A=M+1"
				
			if segment == "constant":
				if(index > 1):
					self.file.write(f"""@{str(index)}
D=A
@SP
M=M+1
A=M-1
M=D
""")
				else:
					self.file.write(f"""@SP
M=M+1
A=M-1
M={str(index)}
""")
			elif segment == "argument":
				self.file.write(f"""@ARG
{at}
D=M
@SP
M=M+1
A=M-1
M=D
""")
			elif segment == "local":
				self.file.write(f"""@LCL
{at}
D=M
@SP
M=M+1
A=M-1
M=D
""")
			elif segment == "static":
				#Auto allocate static vars?
				self.file.write(f"""@{self.current}.{str(index)}
D=M
@SP
M=M+1
A=M-1
M=D
""")
			elif segment == "this":
				self.file.write(f"""@THIS
{at}
D=M
@SP
M=M+1
A=M-1
M=D
""")
			elif segment == "that":
				self.file.write(f"""@THAT
{at}
D=M
@SP
M=M+1
A=M-1
M=D
""")
			elif segment == "pointer":
				if(pointer > 1):
					raise Exception("Invalid pointer index: {index}")
				self.file.write(f"""@{str(3+index)}
D=M
@SP
M=M+1
A=M-1
M=D
""")
			elif segment == "temp":
				if(index > 7):
					raise Exception(f"Index {index} is out of range for temp")
				self.file.write(f"""@{str(5+index)}
D=M
@SP
M=M+1
A=M-1
M=D
""")
			else:
				raise Exception(f"Segment {segment} is invalid")

		elif command == "C_POP":
			at = f"""@?
D=M
@{index}
D=D+A
@13
M=D
"""
			bt = f"""@13
A=M"""
			
			if(index == 0):
				at = ""
				bt = "@?"
			elif(index == 1):
				at = ""
				bt = """@?
A=A+1"""
			
			if segment == "constant":
				raise Exception(f"Segment constant is incompatible with C_POP")
			elif segment == "argument":
				at = at.replace("?","ARG")
				bt = bt.replace("?","ARG")
				self.file.write(f"""{at}@SP
M=M-1
A=M
D=M
{bt}
M=D
""")
			elif segment == "local":
				at = at.replace("?","LCL")
				bt = bt.replace("?","LCL")
				self.file.write(f"""{at}@SP
M=M-1
A=M
D=M
{bt}
M=D
""")
			elif segment == "static":
				self.file.write(f"""@SP
M=M-1
A=M
D=M
@{self.current}.{str(index)}
M=D
""")
			elif segment == "this":
				at = at.replace("?","THIS")
				bt = bt.replace("?","THIS")
				self.file.write(f"""{at}@SP
M=M-1
A=M
D=M
{bt}
M=D
""")
			elif segment == "that":
				at = at.replace("?","THAT")
				bt = bt.replace("?","THAT")
				self.file.write(f"""{at}@SP
M=M-1
A=M
D=M
{bt}
M=D
""")
			elif segment == "pointer":
				if(pointer > 1):
					raise Exception("Invalid pointer index: {index}")
				self.file.write(f"""@SP
M=M-1
A=M
D=M
@{str(3+index)}
M=D
""")
			elif segment == "temp":
				if(index > 7):
					raise Exception(f"Index {index} is out of range for temp")
				self.file.write(f"""@SP
M=M-1
A=M
D=M
@{str(5+index)}
M=D
""")
			else:
				raise Exception(f"Segment {segment} is invalid")
		
	def writeLabel(self,label):
		self.file.write(f"({self.func}${label})\n")
		
	def writeGoto(self,label):
		self.file.write(f"@{self.func}${label}\n0;JMP\n")
		
	def writeIf(self,label):
		self.file.write(f"""@SP
M=M-1
A=M
D=M
@{self.func}${label}
D;JNZ
""")
	
	def writeCall(self,functionName,numArgs):
		# use comp
		self.file.write(f"""@RET{self.comp}
D=A
@SP
M=M+1
A=M-1
M=D
@LCL
D=M
@SP
M=M+1
A=M-1
M=D
@ARG
D=M
@SP
M=M+1
A=M-1
M=D
@THIS
D=M
@SP
M=M+1
A=M-1
M=D
@THAT
D=M
@SP
M=M+1
A=M-1
M=D
@{5+numArgs}
D=A
@SP
D=M-D
@ARG
M=D
@SP
D=M
@LCL
M=D
@{functionName}
0;JMP
(RET{self.comp})
""")
		self.comp += 1
		
	def writeFunction(self,functionName,numLocals):
		self.func = functionName

		if(functionName == "Sys.init"):
			self.init = True
		
		if(numLocals == 0):
			self.file.write(f"({functionName})\n")
		elif(numLocals == 1):
			self.file.write(f"""({functionName})
@SP
M=M+1
A=M-1
M=0
""")
		else:
			self.file.write(f"""({functionName})
@{numLocals}
D=A
(LOP{self.comp})
@END{self.comp}
D;JEQ
@SP
M=M+1
A=M-1
M=0
D=D-1
@LOP{self.comp}
0;JMP
(END{self.comp})
""")
			self.comp += 1
			
	def writeReturn(self):
		self.func = ""
		self.file.write(f"""@LCL
D=M
@13
M=D
@SP
A=M-1
D=M
@ARG
A=M
M=D
D=A+1
@SP
M=D
@13
M=M-1
A=M
D=M
@THAT
M=D
@13
M=M-1
A=M
D=M
@THIS
M=D
@13
M=M-1
A=M
D=M
@ARG
M=D
@13
M=M-1
A=M
D=M
@LCL
M=D
@13
M=M-1
A=M
A=M
0;JMP
""")
		
	def Close(self):
		self.file.close()
		
		
def removenull(list):
	for i in range(0,list.count("")):
		ind = list.index("")
		del list[ind]
	
	return list
		
while True:
	dir = False
	error = 0
	#^ for edge cases
	filestoparse = []
	name = input("VMTranslator> ")
	#Construct logic for distinguishing between directory and file and then use seprate parsers for each file in a directory.
	if name[-3:] == ".vm":
		filestoparse.append(name)
		cw = CodeWriter(f"{name[:-3]}.asm")
	elif name.find(".") == -1:
		dir = True
		try:
			files = os.listdir(name)
			
			for file in files:
				if file[-3:] == ".vm":
					filestoparse.append(file)
					
			if len(filestoparse) == 0:
				raise Exception("No VM files in directory")
			
			cw = CodeWriter(f"{name}/{name}.asm")
				
		except Exception as e:
			print("Error: " + str(e))
			continue
			
	else:
		print(f"Error: {name} is not a relative directory or vm file")
		continue
	
	
	cw.writeInit()
	# Implement parser
	# Parser finish?
	# Work on CodeWriter
	# Handle errors
	for i in range(0,len(filestoparse)):
		if(dir):
			p = Parser(name + "/" + filestoparse[i])
		else:
			p = Parser(filestoparse[i])
			
		cw.current = filestoparse[i][:-3]
		while p.hasMoreCommands:
			try:
				p.advance()
				"""
				print(p.commandType)
				print(p.arg1)
				print(p.arg2)
				"""
				if(p.commandType == "C_PUSH" or p.commandType == "C_POP"):
					cw.WritePushPop(p.commandType,p.arg1,p.arg2)
				elif(p.commandType == "C_ARITHMETIC"):
					cw.writeArithmetic(p.file[p.line-1])
				elif(p.commandType == "C_LABEL"):
					cw.writeLabel(p.arg1)
				elif(p.commandType == "C_FUNCTION"):
					cw.writeFunction(p.arg1,p.arg2)
				elif(p.commandType == "C_GOTO"):
					cw.writeGoto(p.arg1)
				elif(p.commandType == "C_IF"):
					cw.writeIf(p.arg1)
				elif(p.commandType == "C_CALL"):
					cw.writeCall(p.arg1,p.arg2)
				elif(p.commandType == "C_RETURN"):
					cw.writeReturn()
				else:
					raise Exception(f"Command not supported")
			except Exception as e:
				a = input("Error: " + str(e) + " on line " + str(p.line-1) + " on file " + filestoparse[i] + "\nContinue (y/n)>")
				if a == "y":
					error = 1
				else:
					error = 2
					break
			
	
	
	if(cw.init == False):
		print("Error: Sys.init function not found")
		error = 2
		
	cw.Close()
	
	if(error == 0):
		print("Compilation successful!")
	elif(error == 1):
		print("Compilation complete with err")
	else:
		print("Compilation failure")