#Parser module, Code for mnemonics, Symboltable for storing var and labels
class Parser:
	def __init__(self, file):
		self.file = open(file, "r")
		self.lines = self.file.read().split("\n")
		if len(self.lines) == 0:
			raise Exception("Error: No data in file")
		self.hasmorecommands = True
		self.commandType = ""
		self.symbol = ""
		self.dest = ""
		self.comp = ""
		self.jump = ""
		self.line = 0
		self.advance()
	
	def advance(self):
		if self.hasmorecommands == False:
			return
		if self.line >= len(self.lines)-1:
			self.hasmorecommands = False
		line = self.lines[self.line]
		st = line.split(" ")
		line = "".join(st)
		st = line.split("\t")
		line = "".join(st)
		st = line.split("\n")
		line = "".join(st)
		st = line.split("\ufeff")
		line = "".join(st)
		if len(line) == 0:
			self.line += 1
			self.advance()
			return
		elif line[0] == "(" and line[-1] == ")":
			self.commandType = "L_COMMAND"
			self.symbol = line[1:-1]
			self.dest = ""
			self.comp = ""
			self.jump = ""
		elif line[0] == "@":
			self.commandType = "A_COMMAND"
			self.symbol = line[1:]
			self.dest = ""
			self.comp = ""
			self.jump = ""
		else:
			compute = line.split("=")
			
			if len(compute) > 2:
				raise Exception(f"command at {self.line} is not valid")
			elif len(compute) == 1:
				compute.append(compute[0])
				compute[0] = "null"
			
			jup = compute[1].split(";")
			
			if len(jup) == 1:
				jup.append("null")
			elif len(jup) > 2:
				raise Exception(f"Error: command at {self.line} is not valid")
			del compute[1]
			compute.extend(jup)
			self.commandType = "C_COMMAND"
			self.symbol = ""
			self.dest = compute[0]
			self.comp = compute[1]
			self.jump = compute[2]
			
		self.line += 1
		
class Code:
	def __init__(self):
		self.desttable = {"null":"000", "M":"001","D":"010","MD":"011","A":"100","AM":"101","AD":"110","AMD":"111"}
		self.comptable = {"0":"0101010","1":"0111111", "-1":"0111010","D":"0001100","A":"0110000","M":"1110000","!D":"0001101","!A":"0110001","!M":"1110001","-D":"0001111","-A":"0110011","-M":"1110011","D+1":"0011111","A+1":"0110111","M+1":"0110111","D-1":"0001110","A-1":"0110010","M-1":"1110010","D+A":"0000010","D+M":"1000010","D-A":"0010011","D-M":"1010011","A-D":"0000111","M-D":"1000111","D&A":"0000000","D&M":"1000000","D|A":"0010101","D|M":"1010101"}
		self.jumptable = {"null":"000","JGT":"001","JEQ":"010","JGE":"011","JLT":"100","JNE":"101","JLE":"110","JMP":"111"}
	
	def dest(self, mm):
		return self.desttable[mm]
		
	def comp(self, mm):
		return self.comptable[mm]
		
	def jump(self, mm):
		return self.jumptable[mm]
	
class SymbolTable:
	def __init__(self):
		#include predef here on initialization
		self.table = {"SP":0,"LCL":1,"ARG":2,"THIS":3,"THAT":4,"SCREEN":16384,"KBD":24576}
		self.varval = 16
		
	def addEntry(self, symbol, address):
		try:
			self.table[symbol] = int(address)
		except:
			pass
	
	def contains(self, symbol):
		#implement logic for R0-R15
		if(symbol[0] == "R"):
			try:
				addr = int(symbol[1:])
				if addr <= 15:
					return True
			except:
				pass
		
		if symbol in self.table:
			return True
		
		return False
	
	def GetAddress(self, symbol):
		#implement logic for R0-R15
		if(symbol[0] == "R"):
			try:
				addr = int(symbol[1:])
				if addr <= 15:
					return addr
			except:
				pass
		
		try:
			return self.table[symbol]
		except:
			val = self.varval
			self.table[symbol] = val
			self.varval += 1
			return val

#Create a function for decimal to binary if needed using division
#Start here
while True:
	name = input("Assembly file>")
	
	if len(name) < 5:
		print(f"Error: {name} is not a file")
		continue
	
	if name[-4:] != ".asm":
		print("Error: Input is not assembly file")
		continue
	 
	#First pass here
	#Include the rom line counter
	try:
		parse = Parser(name)
	except Exception as e:
		print("Error: " + str(e))
		continue
	
	rline = 0
	symtab = SymbolTable()
	
	while True:
		if parse.commandType == "L_COMMAND":
			symtab.addEntry(parse.symbol,rline)
		else:
			rline += 1
			
		parse.advance()
		break
		
	#Second pass
	try:
		parse = Parser(name)
	except Exception as e:
		print("Error: " + str(e))
		continue
	
	coder = Code()
	
	output = open(name[:-4] + ".hack", "w")
	
	fail = False
	
	while True:
		bincmd = ""
		
		if parse.commandType == "A_COMMAND":
			bincmd += "0"
			
			try:
				bin = '{0:015b}'.format(int(parse.symbol))
			except:
				bin = '{0:015b}'.format(symtab.GetAddress(parse.symbol))
			
			if len(bin) > 15:
				print(f"Error: {parse.commandType} at line {parse.line} is too large")
				fail = True
				break
			
			bincmd += bin
			
		elif parse.commandType == "C_COMMAND":
			bincmd += "111"
			
			try:
				bincmd += coder.comp(parse.comp)
			except:
				print(f"Error: {parse.commandType} at {parse.line} has an invalid computation")
				fail = True
				break
			
			try:
				bincmd += coder.dest(parse.dest)
			except:
				print(f"Error: {parse.commandType} at {parse.line} has an invalid destination")
				fail = True
				break
				
			try:
				bincmd += coder.jump(parse.jump)
			except:
				print(f"Error: {parse.commandType} at {parse.line} has an invalid jump")
				fail = True
				break
			
		else:
			if parse.hasmorecommands == False:
				break
			parse.advance()
			continue
			
		if parse.hasmorecommands == False:
			output.write(bincmd)
			break
		
		bincmd += "\n"
		output.write(bincmd)
		
		try:
			parse.advance()
		except Exception as e:
			print("Error: "+str(e))
			fail=True
			break
		
	if fail == True:
		print(f"Assembly of {name} failed")
	else:
	    print(f"Assembly of {name} success")
	#Close file
	parse.file.close()
	output.close()
