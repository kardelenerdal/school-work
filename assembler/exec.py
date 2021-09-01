import sys
# given the operand and addressing mode, stores the value in register A, to the expected address.
def store(operand, add_mode):
    global memory
    global registers
    decimal_address = int(operand, 16)
    value_to_be_stored = registers[1]  
    if add_mode == '01':
        reg_name = get_register_name(operand)       
        registers[reg_name] = value_to_be_stored
    elif add_mode == '10':
        reg_name = get_register_name(operand)      
        store_dec_add = registers[reg_name]   
        o = format(value_to_be_stored, '016b')
        first_part = o[0:8]
        second_part = o[8:]
        memoryCheck(store_dec_add)
        memory[store_dec_add] = first_part   
        memoryCheck(store_dec_add+1) 
        memory[store_dec_add+1] = second_part
    elif add_mode == '11':
        o = format(value_to_be_stored, '016b')
        first_part = o[0:8]
        second_part = o[8:]
        memoryCheck(decimal_address) 
        memory[decimal_address] = first_part   
        memoryCheck(decimal_address+1)  
        memory[decimal_address+1] = second_part

# calculates a positive version of a negative number
def twos_comp(a):         
    binary = format(a & 0xffff, '016b')
    return int(binary, 2)

# given the addressing mode and operand, returns the actual value of operand as decimal.
def get_value(addmode, operand):  
    global memory
    global registers
    if addmode == "00":
        return int(operand,16)
    elif addmode == "01":
        reg = get_register_name(operand)
        return registers[reg]
    elif addmode == "11":
        memoryCheck(int(operand, 16))
        memoryCheck(int(operand, 16)+1)
        f = memory[int(operand, 16)] 
        s = memory[int(operand, 16) + 1] 
        combine = f + s
        return int(combine, 2)
    else: # 10 memory address is in the register
        reg = get_register_name(operand)
        memoryLoc = registers[reg]
        memoryCheck(memoryLoc)
        memoryCheck(memoryLoc+1)
        f = memory[memoryLoc]
        s = memory[memoryLoc + 1]
        combine = f + s
        return int(combine, 2)

# returns the instruction number
def getTheInstructionNumber(address):
    decimal = int(address, 16)
    return decimal

# removes spaces.
def removeSpaces(s):
    while s[0].isspace():
        s = s[1:]
    while s[-1].isspace():
        leng = len(s)
        s = s[0:leng-1]
    return s

# returns the instruction and addressing mode.
def get_inst_add_type(s):
    inst_type = s[0:6]
    add_mode = s[6:8]
    return_list = []
    # INSTRUCTION TYPE
    if inst_type == '000001':
        return_list.append("HALT")
    elif inst_type == '000010':
        return_list.append("LOAD")
    elif inst_type == '000011':
        return_list.append("STORE")
    elif inst_type == '000100':
        return_list.append("ADD")
    elif inst_type == '000101':
        return_list.append("SUB")
    elif inst_type == '000110':
        return_list.append("INC")
    elif inst_type == '000111':
        return_list.append("DEC")
    elif inst_type == '001000':
        return_list.append("XOR")
    elif inst_type == '001001':
        return_list.append("AND")
    elif inst_type == '001010':
        return_list.append("OR")
    elif inst_type == '001011':
        return_list.append("NOT")
    elif inst_type == '001100':
        return_list.append("SHL")
    elif inst_type == '001101':
        return_list.append("SHR")
    elif inst_type == '001110':
        return_list.append("NOP")
    elif inst_type == '001111':
        return_list.append("PUSH")
    elif inst_type == '010000':
        return_list.append("POP")
    elif inst_type == '010001':
        return_list.append("CMP")
    elif inst_type == '010010':
        return_list.append("JMP")
    elif inst_type == '010011':
        return_list.append("JZ")  # also can be JE 
    elif inst_type == '010100':
        return_list.append("JNZ")  # also can be JNE
    elif inst_type == '010101':
        return_list.append("JC")
    elif inst_type == '010110':
        return_list.append("JNC")
    elif inst_type == '010111':
        return_list.append("JA")
    elif inst_type == '011000':
        return_list.append("JAE")
    elif inst_type == '011001':
        return_list.append("JB")
    elif inst_type == '011010':
        return_list.append("JBE")
    elif inst_type == '011011':
        return_list.append("READ")
    elif inst_type == '011100':
        return_list.append("PRINT")
    else:
        print("illegal instruction")
        exit()    
    # ADDRESSING MODE
    return_list.append(add_mode)
    return return_list

# checks if the given address is a valid address for our memory.
def memoryCheck(s):
    if s > 65535 or s < 0:
        print("memory limit exceeded")
        exit()

# returns register index according to the operand.
def get_register_name(s):
    if s == '0001':
        return 1
    elif s == '0002':
        return 2
    elif s == '0003':
        return 3
    elif s == '0004':
        return 4
    elif s == '0005':
        return 5
    elif s == '0006':
        return 6
    elif s == '0000':
        return 0    

# main
inputFile = open(sys.argv[1], "r")
outputFile = open(sys.argv[1][0:sys.argv[1].index('.')]+".txt", "w")
# there are binary string in memory array. Default value is 0.
memory = ['00000000'] * 65536
i = 0
reg_max_value = 65535

# from input file, writes the instructions to memory.
for line in inputFile:
    first_part = line[0:2]
    second_part = line[2:]
    second_part = removeSpaces(second_part)
    dec_first = int(first_part, 16)
    bin_first = format(dec_first, '08b')
    bin_second = format(int(second_part, 16), '016b')
    memoryCheck(i)
    memory[i] = bin_first
    memoryCheck(i+1)
    memory[i+1] = bin_second[0:8]
    memoryCheck(i+2)
    memory[i+2] = bin_second[8:]
    i+=3


isJump = False
# stores decimal values
registers = [0, 0, 0, 0, 0, 0, 65534]  # PC A B C D E S 
# flags
cf = False
zf = False
sf = False

# actual loop. It ends when sees halt intruction.
while True:
    isJump = False
    # getting the next instruction.
    memoryCheck(registers[0])
    ia = memory[registers[0]] 
    memoryCheck(registers[0]+1)
    operand_f = memory[registers[0]+1]
    memoryCheck(registers[0]+2)
    operand_s = memory[registers[0]+2]
    
    iaa = get_inst_add_type(ia)
    inst_type = iaa[0]
    addressing_mode = iaa[1]
    combine = int((operand_f + operand_s), 2)
    last = format(combine, '04x')
    value = get_value(addressing_mode, last) 
    
    if inst_type == "HALT":
        break
    elif inst_type == 'LOAD':
        registers[1] = value
    elif inst_type == 'STORE':
        store(last, addressing_mode)
    elif inst_type == 'ADD':
        # cf, sf, zf
        registers[1] += value 
        if registers[1] > reg_max_value:
            cf = True
            registers[1] -= (reg_max_value + 1)
        else: 
            cf = False     
        
        b = format(registers[1], '016b')    
        
        if b[0] == '1':
            sf = True
        else:
            sf = False
        if registers[1] == 0:
            zf = True
        else:
            zf = False    
    elif inst_type == 'SUB':
        # cf, sf, zf
        registers[1] = registers[1] + ((~value) & 65535) + 1 
        
        if registers[1] > reg_max_value:
            cf = True
            registers[1] -= (reg_max_value + 1)
        else: 
            cf = False     
        
        b = format(registers[1], '016b')   

        if b[0] == '1':
            sf = True
        else:
            sf = False
        if registers[1] == 0:
            zf = True
        else:
            zf = False    
    elif inst_type == 'INC':       
        # cf, sf, zf
        result = 0
        if addressing_mode == '01':
            reg_name = get_register_name(last)  
            registers[reg_name] += 1
            # cf
            if registers[reg_name] > reg_max_value:
                cf = True
                registers[reg_name] -= (reg_max_value + 1)
            else:
                cf =  False    
            result = registers[reg_name]
            # sf
            b = format(result, '016b')
            if b[0] == '1':
                sf = True
            else:
                sf = False        
        elif addressing_mode == '00':
            dec_value = int(last, 16)
            result = dec_value + 1
            # cf
            if result > reg_max_value:
                result -= (reg_max_value + 1)
                cf = True 
            else:
                cf = False 
            
            b = format(result, '016b')
            # sf
            if b[0] == '1':
                sf = True
            else:
                sf = False  
        elif addressing_mode == '10':
            reg = get_register_name(last)
            memoryLoc = registers[reg]
            memoryCheck(memoryLoc)
            f = memory[memoryLoc]
            memoryCheck(memoryLoc + 1)
            s = memory[memoryLoc + 1]
            v = int(f+s, 2) + 1
            # cf
            if v > 65535:
                cf = True
                v = v - 65536
            else:
                cf = False    
            bv = format(v, '016b')   
            memory[memoryLoc] = bv[0:8]
            memory[memoryLoc+1] = bv[8:]
            result = v
            # sf
            if bv[0] == '1':
                sf = True
            else:
                sf = False    
        elif addressing_mode == '11':
            dec_value = int(last, 16)
            memoryCheck(dec_value)
            f = memory[dec_value]
            memoryCheck(dec_value + 1)
            s = memory[dec_value + 1]
            v = int(f+s, 2) + 1
            # cf
            if v > 65535:
                cf = True
                v = v - 65536
            else:
                cf = False    
            bv = format(v, '016b') 
            # sf
            if bv[0] == '1':
                sf = True
            else:
                sf = False     
            memory[dec_value] = bv[0:8]
            memory[dec_value+1] = bv[8:]
            result = v
        # zf
        if result == 0:
            zf = True
        else:
            zf = False    

    elif inst_type == 'DEC':
        # cf, sf, zf
        result = 0
        if addressing_mode == '01':
            reg_name = get_register_name(last)  
            registers[reg_name] = registers[reg_name] + ((~1)&65535) + 1
            # cf
            if registers[reg_name] > reg_max_value:
                cf = True
                registers[reg_name] -= (reg_max_value + 1)
            else:
                cf =  False    
            result = registers[reg_name]
            # sf
            b = format(result, '016b')
            if b[0] == '1':
                sf = True
            else:
                sf = False        
        elif addressing_mode == '00': 
            dec_value = int(last, 16)
            result = dec_value + ((~1)&65535) + 1
            # cf
            if result > reg_max_value:
                cf = True
                result -= (reg_max_value + 1)
            else :
                cf = False
            b = format(result, '016b')
            # sf
            if b[0] == '1':
                sf = True
            else:
                sf = False  
        elif addressing_mode == '10':
            reg = get_register_name(last)
            memoryLoc = registers[reg]
            memoryCheck(memoryLoc)
            f = memory[memoryLoc]
            memoryCheck(memoryLoc+1)
            s = memory[memoryLoc + 1]
            v = int(f+s, 2) + ((~1)&65535) + 1 
            # cf
            if v > 65535:
                cf = True
                v = v - 65536
            else:
                cf = False    
            bv = format(v, '016b')    
            memory[memoryLoc] = bv[0:8]
            memory[memoryLoc + 1] = bv[8:]  
            result = v
            # sf
            if bv[0] == '1':
                sf = True
            else:
                sf = False    
        elif addressing_mode == '11':
            dec_value = int(last, 16)
            memoryCheck(dec_value)
            f = memory[dec_value]
            memoryCheck(dec_value+1)
            s = memory[dec_value + 1]
            v = int(f+s, 2) + ((~1)&65535) + 1 
            # cf
            if v > 65535:
                cf = True
                v = v - 65536
            else:
                cf = False    
            bv = format(v, '016b') 
            # sf
            if bv[0] == '1':
                sf = True
            else:
                sf = False      
            memory[dec_value] = bv[0:8]
            memory[dec_value + 1] = bv[8:]
            result = v
        # zf
        if result == 0:
            zf = True
        else:
            zf = False  
    elif inst_type == 'XOR':
        # sf, zf
        a_dec_value = registers[1]
        result = value ^ a_dec_value
        registers[1] = result
        if registers[1] < 0:
            registers[1] = twos_comp(registers[1])
            result = registers[1]
        b = format(registers[1], '016b')
        if b[0] == '1':
            sf = True
        else:
            sf = False
        if result == 0:
            zf = True
        else:
            zf = False 
    elif inst_type == 'AND':
        # sf, zf
        a_dec_value = registers[1]
        result = value & a_dec_value
        registers[1] = result
        if registers[1] < 0:
            registers[1] = twos_comp(registers[1])
            result = registers[1]
        b = format(registers[1], '016b')
        if b[0] == '1':
            sf = True
        else:
            sf = False
        if result == 0:
            zf = True
        else:
            zf = False
    elif inst_type == 'OR':
        # sf, zf
        a_dec_value = registers[1]
        result = value | a_dec_value
        registers[1] = result
        if registers[1] < 0:
            registers[1] = twos_comp(registers[1])
            result = registers[1]
        b = format(registers[1], '016b')
        if b[0] == '1':
            sf = True
        else:
            sf = False
        if result == 0:
            zf = True
        else:
            zf = False
    elif inst_type == 'NOT':
        result = ~value & 65535
        registers[1] = result
        if registers[1] < 0:
            registers[1] = twos_comp(registers[1])
            result = registers[1]
        b = format(registers[1], '016b')
        if b[0] == '1':
            sf = True
        else:
            sf = False
        if result == 0:
            zf = True
        else:
            zf = False
    elif inst_type == 'SHL':
        # cf, sf, zf
        result = value << 1
        regName = get_register_name(last)
        registers[regName] = result
        if registers[regName] < 0:
            registers[regName] = twos_comp(registers[regName])
            result = registers[1]
        if registers[regName] > reg_max_value:
            cf = True
            registers[regName] -= (reg_max_value + 1)
        else:
            cf =  False  

        b = format(registers[regName], '016b')

        if b[0] == '1':
            sf = True
        else:
            sf = False
        if result == 0:
            zf = True
        else:
            zf = False
    
    elif inst_type == 'SHR':
        # sf, zf
        result = value >> 1
        regName = get_register_name(last)
        registers[regName] = result
        
        if registers[regName] < 0:
            registers[regName] = twos_comp(registers[regName])
            result = registers[1]

        b = format(registers[regName], '016b')

        if b[0] == '1':
            sf = True
        else:
            sf = False
        if result == 0:
            zf = True
        else:
            zf = False

    elif inst_type == 'NOP':
        registers[0] += 3
        continue
    elif inst_type == 'PUSH':
        b = format(value, '016b')
        f = b[0:8]
        s = b[8:]
        memoryCheck(registers[6]) 
        memory[registers[6]] = f 
        memoryCheck(registers[6]+1) 
        memory[registers[6]+1] = s
        registers[6] -= 2
    elif inst_type == 'POP':
        registers[6] += 2
        if registers[6] >= 65535:
            print("stack is empty")
            exit()
        reg = get_register_name(last)
        f = memory[registers[6]]
        s = memory[registers[6] + 1]
        b = f + s
        registers[reg] = int(b, 2)
    elif inst_type == 'CMP':
        # cf, sf, zf
        result = registers[1] + ((~value) & 65535) + 1
        if result > reg_max_value:
            result = result - 65536
            cf = True
        else:
            cf = False
        
        b = format(result, '016b')      
        if b[0] == '1':
            sf = True
        else:
            sf = False
        if result == 0:
            zf = True
        else:
            zf = False   
    elif inst_type == 'JMP': 
        nextInst = getTheInstructionNumber(last)
        registers[0] = nextInst
        isJump = True
    elif inst_type == 'JZ':
        # Conditional jump. Jump to address (given as immediate operand) if zero flag is true.
        if(zf):
            nextInst = getTheInstructionNumber(last)
            registers[0] = nextInst
            isJump = True
    elif inst_type == 'JNZ':
        # Conditional jump. Jump to address (given as immediate operand) if zero flag is false.
        if not zf:
            nextInst = getTheInstructionNumber(last)
            registers[0] = nextInst
            isJump = True
    elif inst_type == 'JC':
        # Conditional jump. Jump if carry flag is true.
        if cf:
            nextInst = getTheInstructionNumber(last)
            registers[0] = nextInst
            isJump = True
    elif inst_type == 'JNC':
        # Conditional jump. Jump if carry flag is false.
        if not cf:
            nextInst = getTheInstructionNumber(last)
            registers[0] = nextInst
            isJump = True
    elif inst_type == 'JA':
        # Conditional jump. Jump if above.
        if (not zf) and (not sf):
            nextInst = getTheInstructionNumber(last)
            registers[0] = nextInst
            isJump = True
    elif inst_type == 'JAE':
        # Conditional jump. Jump if above or equal.
        if not sf:
            nextInst = getTheInstructionNumber(last)
            registers[0] = nextInst
            isJump = True
    elif inst_type == 'JB':
        # Conditional jump. Jump if below
        if sf:
            nextInst = getTheInstructionNumber(last)
            registers[0] = nextInst
            isJump = True
    elif inst_type == 'JBE':
        # Conditional jump. Jump if below or equal
        if sf or zf:
            nextInst = getTheInstructionNumber(last)
            registers[0] = nextInst
            isJump = True
    elif inst_type == 'READ': 
        # Reads a character into the operand.
        read_char = input()
        result = ord(read_char)
        if addressing_mode == '01':
            reg_name = get_register_name(last)  
            registers[reg_name] = result
        elif addressing_mode == '10':
            reg = get_register_name(last)
            b = format(result, '016b')
            memoryLoc = registers[reg]
            f = b[0:8]
            s = b[8:]
            memoryCheck(memoryLoc)
            memory[memoryLoc] = f
            memoryCheck(memoryLoc+1)
            memory[memoryLoc+1] = s
        elif addressing_mode == '11':
            dec_value = int(last, 16)
            b = format(result, '016b')
            f = b[0:8]
            s = b[8:]
            memoryCheck(dec_value)
            memory[dec_value] = f
            memoryCheck(dec_value+1)
            memory[dec_value+1] = s
    elif inst_type == 'PRINT':
        # Prints the operand as a character.
        outputFile.write(chr(value))
        outputFile.write("\n")

   
    if not isJump:
        registers[0] += 3
        
