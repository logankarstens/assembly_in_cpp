#include <iostream>
#include <algorithm>
#include <bits/stdc++.h>
#include <fstream>
#include <string>
#include <vector>

// register 0 is status register
// each register consists of 32 bits
// only necessary functions are for access and retrieval of register values
// other assembly functions are created seperately

class Register {
    public:
        
        Register(int n) {
            registers.resize(n, 0);  
        }

        int32_t get(int address) {
            return registers[address];
        }

        void set(int value, int address) {
            registers[address] = value;
        }

    private:
        std::vector<int32_t> registers;
};

// function prototypes

// reads code from .txt file or user input into a 2-dimensional vector of strings
std::vector<std::vector<std::string>> get_code();

// lexes 2-dimensional vector from get_code() into jump labels, instructions, adn instruction parameters
void perform_code(Register &r, std::vector<std::vector<std::string>> code);

// parses and executes instruction by modifying or accessing register data
int execute_instruction(Register &r, int index, int instruction_code, std::vector<std::string> line, std::vector<std::string> jumps);

// converts binary (#___b) or hexadecimal (#___h) inputs into decimal (primary base for this system)
std::string fix_base(std::string);

// converts instruction parameter into necessary data for execute_intruction() using available addresisng modes
int address(Register &r, std::string str, bool pos);

std::string fix_base(std::string str) {
    std::string first;
    first += str[0];
    str = str.substr(1);
    int temp;
    if (str[str.length()-1] == 'b') {
        temp = std::stoi(str, nullptr, 2);
    } else if (str[str.length()-1] == 'h') {
        temp = std::stoi(str, nullptr, 16);
    } else {
        temp = std::stoi(str);
    }
    str = std::to_string(temp);
    str.insert(0, first);
    return str;
}

std::vector<std::vector<std::string>> get_code() {
    std::vector<std::vector<std::string>> code;
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
        std::cout << "Enter name of .txt input file." << std::endl;
        std::cin >> input;
        file.open(input, std::ifstream::in);
        if (!file.is_open()) {
            std::cout << "Error opening file. Check file name.";
        }
    }

    
    
    std::cin.ignore();
    if (mode) {
        std::getline(std::cin, input);
    } else {
        std::getline(file, input);
        std::cout << "Reading file input.txt..." << std::endl << std::endl << input << std::endl;
    }
    while (input != "end") {
        for (int i=0; i<input.length(); i++) {
            if (isalpha(input[i])) {
                input[i] = tolower(input[i]);
            }
        }
        
        std::vector<std::string> temp;
        while (input.length() > 0) {
            //std::cout << "input: " << input << std::endl;
            while (input.find(",") != std::string::npos) {
                input[input.find(",")] = ' ';
            }
            temp.push_back(input.substr(0, input.find(" ")));
            input = input.substr(temp.back().length());
            while (input[0] == ' ') {
                input = input.substr(1);
            }
        }
        code.push_back(temp);
        if (mode) {
            std::getline(std::cin, input);
        } else {
            std::getline(file, input);
            std::cout << input << std::endl;
        }
        
    }
    return code;
}

void perform_code(Register &r, std::vector<std::vector<std::string>> code) {
    std::cout << "\nOUTPUT\n\n";
    const std::vector<std::string> instructions = {"mov", "prt", "jmp"}; // etc
    int size = instructions.size();

    // print all code
    /*
    for (int i=0; i<code.size(); i++) {
        for (int j=0; j<code[i].size(); j++) {
            if (code[i][j][0] == '#') {
                code[i][j] = fix_base(code[i][j]);
            }
            std::cout << code[i][j] << " ";
        }
        std::cout << std::endl;
    }
    */
    
    
    std::vector<std::string> jumps(code.size()); 
    for (int i=0; i<code.size(); i++) {
        jumps[i] = "";
       if (code[i].size() == 4) {
            jumps[i] = code[i][0];
        }
    }
    
    int i=0;
    while (i<code.size()) {
        //std::cout << "i: " << i << std::endl;
        for (int j=0; j<code[i].size(); j++) {
            if (code[i][j][0] == '#') {
                code[i][j] = fix_base(code[i][j]);
            }
        }
        bool isjump = 0;
        if (jumps[i].compare("") != 0) {
            isjump++;
        }
        for (int j=0; j<size; j++) {
            
            if (code[i][isjump].find(instructions[j]) == 0) {
                std::vector<std::string> temp = code[i];
                if (isjump) {
                    temp.erase(temp.begin());
                }
            
                i = execute_instruction(r, i, j, temp, jumps);
                
            }
        }
        i++;
    }
}

int execute_instruction(Register &r, int index, int instruction_code, std::vector<std::string> line, std::vector<std::string> jumps) {
    int src, dest = 0;
    if (line.size() == 2) {
        dest = address(r, line[line.size()-1], 0);
    } else {
        dest = address(r, line[line.size()-1], 1);
        src = address(r, line[line.size()-2], 0);
    }

    switch (instruction_code) {
       
        case 0 : 
            r.set(src, dest);
            return index;
        case 1 :

            std::cout << line[line.size()-1] << ": " << dest << std::endl;
            return index;
        case 2 :
        for (int i=0; i<jumps.size(); i++) {
            if (jumps[i] == line[1]) {
                return i-1;
            }
        }
    }
}

int address(Register &r, std::string str, bool pos) {
    char first = str[0];
    str = str.substr(1);
    switch (first) {
        case '#' :
            return std::stoi(str);
        case 'r' :
            if (pos) {
                return std::stoi(str);
            } else {
                /* 
                if (!isdigit(str[str.length()-1])) {
                    std::string temp = "#" + std::to_string(r.get(std::stoi(str))) + str[str.length()-1];
                    return std::stoi(fix_base(temp).substr(1));
                }
                */
                return r.get(std::stoi(str));
            }
    }
}


int main() {
    Register r(8);
    std::vector<std::vector<std::string>> code = get_code();
    perform_code(r, code);
}
