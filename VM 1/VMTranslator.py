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
			raise Exception(f"Too many arguments on line {self.line}!")
			
		if(len(line) <= 0):
			raise Exception(f"Insufficient commands on line {self.line}!")
		
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
			raise Exception(f"Invalid Command on line {self.line}!")
		
		if(self.arg2 < 0):
			raise Exception(f"Negative integers are not allowed on line {self.line}!")
		
		self.line += 1
		
class CodeWriter:
	def __init__(self, name):
		self.file = open(name, "w")
		self.comp = 0
		# v to initialize pointers
		"""
		SP,LCL,ARG,THIS,THAT
		Static: 16
		Stack: 256
		Heap: 2048
		"""
		self.file.write("""@256
D=A
@SP
M=D
""")
		# Initialization temp
		self.file.write("""@300
D=A
@LCL
M=D
@400
D=A
@ARG
M=D
@3000
D=A
@THIS
M=D
@3001
D=A
@THAT
M=D
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
A=M
D=A-D
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
A=M
M=D
@SP
M=M+1
""")
				else:
					self.file.write(f"""@SP
A=M
M={str(index)}
@SP
M=M+1
""")
			elif segment == "argument":
				self.file.write(f"""@ARG
{at}
D=M
@SP
A=M
M=D
@SP
M=M+1
""")
			elif segment == "local":
				self.file.write(f"""@LCL
{at}
D=M
@SP
A=M
M=D
@SP
M=M+1
""")
			elif segment == "static":
				self.file.write(f"""@{str(16+index)}
D=M
@SP
A=M
M=D
@SP
M=M+1
""")
			elif segment == "this":
				self.file.write(f"""@THIS
{at}
D=M
@SP
A=M
M=D
@SP
M=M+1
""")
			elif segment == "that":
				self.file.write(f"""@THAT
{at}
D=M
@SP
A=M
M=D
@SP
M=M+1
""")
			elif segment == "pointer":
				if(pointer > 1):
					raise Exception("Invalid pointer index: {index}")
				self.file.write(f"""@{str(3+index)}
D=M
@SP
A=M
M=D
@SP
M=M+1
""")
			elif segment == "temp":
				if(index > 7):
					raise Exception(f"Index {index} is out of range for temp")
				self.file.write(f"""@{str(5+index)}
D=M
@SP
A=M
M=D
@SP
M=M+1
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
@{str(16+index)}
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
					filestoparse.append(name + "/" + file)
					
			if len(filestoparse) == 0:
				raise Exception("No VM files in directory")
			
			cw = CodeWriter(f"{name}/{name}.asm")
				
		except Exception as e:
			print("Error: " + str(e))
			continue
			
	else:
		print(f"Error: {name} is not a relative directory or vm file")
		continue
	
	# Implement parser
	# Parser finish?
	# Work on CodeWriter
	# Handle errors
	for i in range(0,len(filestoparse)):
		p = Parser(filestoparse[i])
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
				else:
					raise Exception("Command not supported")
			except Exception as e:
				a = input("Error: " + str(e) + "\nContinue (y/n)>")
				if a == "y":
					error = 1
				else:
					error = 2
					break
			
	cw.Close()
	if(error == 0):
		print("Compilation successful!")
	elif(error == 1):
		print("Compilation complete with err")
	else:
		print("Compilation failure")