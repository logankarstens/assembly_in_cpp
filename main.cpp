#include <iostream>
#include <algorithm>
#include <bits/stdc++.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <bitset>
#include <stdlib.h>

// virtual machine class
// holds vector of 16-bit registers
// execute_program and execute_instruction functions are used to execute compiled code in the registers

class VM {
    public:

        VM(int n) {
            registers.resize(std::min(n, 256), 0);
        }

        void execute_program(std::vector<unsigned int> code);
        void execute_instruction(unsigned int, int&);

    private:
        bool v = false;
        bool c = false;
        bool n = false;
        bool z = false;
        
        std::vector<int16_t> registers;
};

const std::vector<std::string> instructions = {"imm",  "ind",  "indi", "idx",  "prt",  "call", "ret",  "rra",  "rrc",  "rla",  "rlc",  "inc",  "incd", "dec", "decd",                                // single-operand commands
                                               "jmp",  "jz",   "jnz",  "jc",   "jnc",  "js",   "jns",  "jl",   "jle",  "jg",   "jge",                           // jump commands
                                               "mov",  "add",  "sub",  "cmp",  "and",  "or",   "xor",  "bic",  "bis",  "bit"};  // double-operand commands


// execute_program takes in a program of compiled code and executes it line by line
// counter is used to keep track of the current line of code, can be manipulated by execute_instruction in case of jumps

void VM :: execute_program(std::vector<unsigned int> compiledcode) {
    std::cout << std::endl << "OUTPUT" << std::endl << std::endl;
    //std::cout << "here?";
    compiledcode.push_back(UINT_MAX);
    int counter = 0;
    while (compiledcode[counter] != UINT_MAX) {
        execute_instruction(compiledcode[counter], counter);

    }
}


// execute_instruction takes in a given line of compiled code and executes it in the registers
// extended switch statement stores cases for each instruction type
// arguments are parsed from compiled line and modified in case of indirect addressing mode

void VM :: execute_instruction(unsigned int line, int& counter) {
    int instruction = line / (int)pow(2, 24);
    int arg1, arg2, result = INT_MAX;
    if (instruction < 26) {
        arg1 = line % (int)pow(2, 16);
    } else {
        arg1 = (line % (int)pow(2, 24)) / (int)pow(2, 12);
        arg2 = line % (int)pow(2, 12);
    }
    if (arg1 == 1 && instruction > 3) {
        arg1 = registers[1];
        
    }
    if (arg2 == 1 && instruction > 3) {
        arg2 = registers[1];
    }

    //std::cout << "line " << counter << ", arg1 " << arg1 << ", arg2 " << arg2 << std::endl; // debug print


    // note: result variable is used in costly instructions in order to affect status flags
    switch (instruction) {
        
        // unary operations involving one argument, arg1

        case 0 : // imm
           
            // arguments are in twos compliment form, must be derived before conversion
            if (arg1 > (int)pow(2, 15)) {
                arg1 = (int)pow(2, 16) - arg1;
                arg1 *= -1;
            }

            // used for immediate addressing mode, numerical argument placed into register 0
            // allows code to differentiate between immediate numbers and register values
            registers[0] = arg1;

            break; 
        case 1 : // ind
            // used for indirect addressing mode, puts value of register into register1
            // this must be done because the compiler does not know the value of these registers until previous code is executed
            registers[1] = registers[arg1];

            break;
        case 2 : // indi
            // used for indirect autoincrement addressing mode
            registers[1] = registers[arg1];
            result = registers[arg1]+1;
            registers[arg1] = result;
            //std::cout << "r[1]: " << registers[1] << std::endl;
            break;
           
        case 4 : // prt
            // simply prints value of given register
            std::cout << "r" << arg1 << ": " << registers[arg1] << std::endl;
            break;
        case 11 : // inc
            result = registers[arg1]+1;
            registers[arg1] = result;
            break;
        case 12 : // incd
            result = registers[arg1]+2;
            registers[arg1] = result;
            break;
        case 13 : // dec
            result = registers[arg1]-1;
            registers[arg1] = result;
            break;
        case 14 : // decd
            result = registers[arg1]-2;
            registers[arg1] = result;
            break;

        // jump instructions
        // note: instructions decrement jump line because they are incremented later in the function

        case 15 : // jmp
            counter = arg1-1;
            break;
        case 16 : // jz
            if (z)
                counter = arg1-1;
            break;
        case 17 : // jnz
            if (!z)
                counter = arg1-1;
            break;
        case 18 : // jc
            if (c)
                counter = arg1-1;
            break;
        case 19 : // jnc
            if (!c)
                counter = arg1-1;
            break;
        case 20 : // js
            if (n)
                counter = arg1-1;
            break;
        case 21 : //jns
            if (!n)
                counter = arg1-1;
            break;
        case 22 : // jl
            if (n^v)
                counter = arg1-1;
            break;
        case 23 : // jle
            if ((n^v) || z)
                counter = arg1-1;
            break;
        case 24 : // jg
            if (n==v && !z)
                counter = arg1-1;
            break;
        case 25 : // jge
            if (n==v)
                counter = arg1-1;
            break;

        // binary operations involving two arguments, arg1 and arg2 (incomplete)

        case 26: // mov
            result = registers[arg1];
            registers[arg2] = result;
            break;
        case 27: // add
            result = registers[arg2] + registers[arg1];
            registers[arg2] = result;
            break;
        case 28: // sub
            result = registers[arg2] - registers[arg1];
            registers[arg2] = result;
            break;
        case 29: // cmp
            result = registers[arg2] - registers[arg1];
            break;
    }

    // handling status flags

    c = false; v = false; n = false; z = false;
    if (result != INT_MAX) {
        if (abs(result) > (int)pow(2,15)) {
            v = true;
        } else if (result < 0) {
            n = true;
        } else if (!result) {
            z = true;
        }
    }
    
    //std::cout << "v: " << v << " n: " << n << " z: " << z << std::endl; // debug print for status flags
    counter++;

}


// fix_base allows for various bases to be used in immediate addressing mode arguments, i.e. #45h or #00001100b. also removes # from argument for compilation

std::string fix_base(std::string str) {
    str = str.substr(1);
    int temp;
    if (str[str.length()-1] == 'b') {
        temp = std::stoi(str, nullptr, 2);
    } else if (str[str.length()-1] == 'h') {
        temp = std::stoi(str, nullptr, 16);
    } else if (str[str.length()-1] == 'o') {
        temp = std::stoi(str, nullptr, 8);
    } else {
        temp = std::stoi(str);
    }
    str = std::to_string(temp);
    return str;
}

// get_code handles code input with two modes
// manual mode: enter code line-by-line into terminal
// file mode: enter code from txt file

std::vector<std::string> get_code() {
    std::vector<std::string> code;
    std::ifstream file;
    std::string input;
    int mode = 0;
    std::cout << "Welcome to the assembly interpreter." 
    << std::endl << "Press 1 to read input from text file." 
    << std::endl << "Press 2 to manually enter assembly code."  << std::endl;
    std::cin >> mode;
    
    mode--;
    if (mode) {
        std::cout << "Enter assembly code. Use 'end' when finished." << std::endl;
        
    } else {
        std::cout << "Enter name of .txt input file relative to the program folder." << std::endl;
        std::cin >> input;
        if (input.find(".txt") == input.length()-4) {
            file.open(input, std::ifstream::in);
        } else {
            file.open(input + ".txt", std::ifstream::in);
        }
        if (!file.is_open()) {
            std::cout << "Error opening file. Check file name.";
            exit(0);
        }
    }

    std::cin.ignore();
    if (mode) {
        std::getline(std::cin, input);
    } else {
        std::getline(file, input);
        std::cout << "Reading file input.txt..." << std::endl;
    }
    while (input.find("end") != 0) {
            
        code.push_back(input);

        if (mode) {
            std::getline(std::cin, input);
        } else {
            std::getline(file, input);
        }
    }
    return code;
}

// lex is the code lexer
// performs basic string manipulation i.e. unnecessary whitespace/comment removal
std::vector<std::string> lex(std::vector<std::string> code) {
    for (int i=0; i<code.size(); i++) {
        if (code[i] == "" || code[i][0] == ';') {
            code.erase(code.begin()+i);
            i--;
        } else {

            std::string line = code[i];

            // make all lowercase
            for (int j=0; j<line.length(); j++) {
                if (isalpha(line[i])) {
                    line[i] = tolower(line[i]);
                }
            }

            // remove comments
            if (line.find(";") != std::string::npos) {
                line.erase(line.begin() + line.find(";"), line.end());
            }

            // remove commas, unnecessary for compilation as spaces are used to separate instructions and arguments
            while (line.find(",") != std::string::npos) {
                    int index = line.find(",");
                    line[index++] = ' ';
                    
                    while (line[index] == ' ') {
                        line.erase(index, 1);
                    }
            }

            //remove spaces from end
            while (line[line.length()-1] == ' ') {
                line.erase(line.end()-1);
            }

            code[i] = line;
        }
    }

    return code;
}

// compiler compiles code into bytecode with one byte reserved for instructions and 3 bytes for optional parameters
// function consists of preprocessor, which prepares code for compilation, and compiler

std::vector<unsigned int> compiler(std::vector<std::string> code) {
 
    std::map<std::string, int> jumpmap;

    // PRE PROCESSOR

    // first pass: find all jump tags and move them to the end (simplifies instruction detection)

    for (int i=0; i<code.size(); i++) {
        std::string firstword = code[i].substr(0, code[i].find(" "));
        if (std::find(instructions.begin(), instructions.end(), firstword) == instructions.end()) {
            code[i].erase(0, firstword.length()+1);
            code[i].append(" ;" + firstword);
        }
    }

    // second pass: code generation and modification
    // replace all immediate addresses with 'imm' equivalent and indirect addresses with 'ind' equivalent for compilation

    for (int i=0; i<code.size(); i++) {
        std::string firstword = code[i].substr(0, code[i].find(" "));
        std::string lastword = code[i].substr(code[i].find_last_of(" ")+1);
        int addressCount = 0;
        while (code[i].find("@") != std::string::npos || code[i].find("#") != std::string::npos) {
            addressCount++;
            std::string number, temp;
            if (code[i].find("@") != std::string::npos) {
                if (code[i].find("@") != code[i].find_last_of("@")) {
                    std::cout << "Preprocess error: cannot address both arguments in binary operation indirectly" << std::endl;
                }
                number = code[i].substr(code[i].find("@"), code[i].substr(code[i].find("@")).find(" "));
                temp =  "ind " + number.substr(1 + (number[1]=='r'));
                if (temp[temp.size()-1] == '+') {
                    temp.insert(3, "i");
                    temp.erase(temp.end()-1);
                }
            } else if (code[i].find("#") != std::string::npos) {
                if (code[i].find("#") != code[i].find_last_of("#")) {
                    std::cout << "Preprocess error: cannot use immediate addressing for both arguments in binary operation" << std::endl;
                }
                number = code[i].substr(code[i].find("#"), code[i].substr(code[i].find("#")).find(" "));
                temp =  "imm " + fix_base(number);
            }
            if (number.find(" ") != std::string::npos) {
                number.erase(number.find(" "));
            }
            if (i == 0) {
                code.insert(code.begin(), temp);
            } else {
                code.insert(code.begin()+ i, temp);
            }
            i++;
            if (code[i].find("@") != std::string::npos) {
                code[i].replace(code[i].find(number), number.length(), "r1");
            } else if (code[i].find("#") != std::string::npos) {
                code[i].replace(code[i].find(number), number.length(), "r0");
            }
        }
    
        if (lastword[0] == ';' && addressCount) {
            code[i-addressCount].append(" " + lastword);
            code[i] = code[i].substr(0, code[i].find_last_of(" "));
        }
    }

    // third pass: add jumptags at end of lines to jumpmap

    for (int i=0; i<code.size(); i++) {
        std::string lastword = code[i].substr(code[i].find_last_of(" ")+1);
        if (lastword[0] == ';') {
            jumpmap.insert(std::pair<std::string, int>(lastword.substr(1), i));
            code[i] = code[i].substr(0, code[i].find_last_of(" "));
        }
    }

    // fourth pass: replace parameters of jump instructions with line number to jump to
    // this must be performed after all code generation is completed or else jump parameters will be inaccurate

    for (int i=0; i<code.size(); i++) {
        std::string firstword = code[i].substr(0, code[i].find(" "));
        if (firstword[0] == 'j') {
            std::string test = std::to_string(jumpmap.find(code[i].substr(firstword.size()+1))->second);
            code[i].replace(4, code[i].length()-4, "" + test);
        }
    }

    // debug print of generated jumpmap
    /*
    std::cout << "jumpmap:" << std::endl;
    for (auto itr = jumpmap.begin(); itr != jumpmap.end(); ++itr) {
        std::cout  << itr->first << '\t' << itr->second << '\n';
    }
    std::cout << std::endl;
    */
    
    // debug print of preprocessed code
    /*
    std::cout << "code:" << std::endl;
    for (int i=0; i<code.size(); i++) {
        std::cout << "line " << i << ": " << code[i] << std::endl;
    }
    */

    // COMPILER

    // compiler turns preprocessed instructions into 32-bit compressed instructions to be executed by virtual machine
    // compiler includes limited error handler to help diagnose basic syntax problems, will be developed further

    std::cout << std::endl << "compiled bytecode" << std::endl << std::endl;
    std::vector<unsigned int> compiledcode;
    bool error = 0;
    for (int i=0; i<code.size(); i++) {
        std::string line = code[i];   

        // initialize instruction to 0, selects first word from line to find instruction type
        compiledcode.push_back(0);
        std::string word = line.substr(0, code[i].find(" "));
    
        if (std::find(instructions.begin(), instructions.end(), word) == instructions.end()) {
            std::cout << "Error: instruction '" << word << "' does not exist." << std::endl;
            error = 1;
            break;
        } else {
            // adds instruction to first 8 bits of compiled instruction line
            compiledcode[i] += pow(2, 24) * (int)(std::find(instructions.begin(), instructions.end(), word)-instructions.begin());
        }
        line.erase(0, word.length()+1);

            // handles the 1-2 arguments
            // if only 1 argument, full 24-bit space is used to store argument
            // if 2 arguments, 24-bit space split into two 12-bit spaces (maximum of 2^12 registers)

            if (line.find(" ") != std::string::npos) {
                compiledcode[i] += pow(2, 12) * stoi(line.substr(1));
                line.erase(0, line.find(" ")+1);
                compiledcode[i] += stoi(line.substr(1));
            } else {
                if (line[0] == 'r') {
                    line.erase(line.begin());
                } else {
                    if (abs(stoi(line)) >= pow(2, 15)) {
                        std::cout << "Error: immediate value of " << pow(2, 15) << " is too large (16-bit signed registers)" << std::endl;
                        error = 1;
                        break;
                    }
                    if (stoi(line) < 0) {
                        compiledcode[i] += pow(2, 16);
                    }
                }
               compiledcode[i] += stoi(line);
            }

        // prints out compiled bytecode
        // will include feature to output compressed code into txt file and enter it into program
        // this will allow for faster file transfer and greater time & space efficiency for all steps except program execution

        std::bitset<32> x(compiledcode[i]);
        std::cout << x << std::endl;
    }
    return compiledcode;
}

// main function creates virtual machine, gets code from user, lexes code, compiles it, and executes using VM functions
int main() {
    VM machine = VM(256);
    std::vector<std::string> code = get_code();
    code = lex(code);
    std::vector<unsigned int> compiledcode = compiler(code); 
    machine.execute_program(compiledcode);
}
