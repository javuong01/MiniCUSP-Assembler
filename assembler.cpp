#include <string>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>


//idea: use unordered map to save address of each instruction in pass 1. Pass two utilizes the unordered map to make the machine code
// idea: for get line, use the fact that there is lowercase for label, upper for operand, and space for other
//problem: repeat instructions can only hold 1 address. solution? vectors of pairs?
std::string toMechCode(std::string line, std::unordered_map<std::string, std::string> *memmapPtr, std::unordered_map<std::string, std::string> *instMapPtr, std::unordered_map<std::string, int> *labelCounterPtr, int strLength);
std::string baseTentoSixteen(int numToTen);
std::string endingNumParse(std::string ending);

int main(int argc, char *argv[]){

    if (argc != 3) {  //Error if there is not enough file arguments
		std::cerr << "Usage: ./assembler <input.csp> <output.csp>" << "\n";
		exit(1);
	}

    std::ifstream infile(argv[1]);     //a file stream is open under the name of file
    if (!infile.is_open()) {         //This if statement checks if the file opened actually opened
        std::cerr << "Error: cannot open file " << argv[1] << "\n";
            exit(2);
    } 

    std::ofstream outfile(argv[2]);                //a file stream is open under the name of file2
    if (!outfile.is_open()) {             //This if statement checks if the file opened actually opened
        std::cerr << "Error: cannot open file " << argv[2] << "\n";
            exit(3);
    }

    std::unordered_map<std::string, std::string> memmap, instMap;
    std::unordered_map<std::string, int> labelCounter;
    std::unordered_map<std::string, std::string> *memmapPtr = &memmap;
    std::unordered_map<std::string, std::string> *instMapPtr = &instMap;
    std::unordered_map<std::string, int> *labelCounterPtr = &labelCounter;

    // Placing inst and their representations in instMap
    instMap["PSHA"] = "FFF010";
    instMap["PSHX"] = "FFF011";
    instMap["PSHF"] = "FFF012";
    instMap["POPA"] = "FFF013";
    instMap["POPX"] = "FFF014";
    instMap["POPF"] = "FFF015";
    instMap["SHRA"] = "FFF022";
    instMap["SHLA"] = "FFF023";
    instMap["NOP"] = "FFF038";
    instMap["RTN"] = "FFF040";
    instMap["HLT"] = "FFFFFF";
    instMap["LDA"] = "00";
    instMap["LDX"] = "01";
    instMap["LDS"] = "02";
    instMap["LDF"] = "03";
    instMap["STA"] = "04";
    instMap["STX"] = "05";
    instMap["STS"] = "06";
    instMap["STF"] = "07";
    instMap["PSH"] = "08";
    instMap["POP"] = "09";
    instMap["CLR"] = "0A";
    instMap["SET"] = "0B";
    instMap["ADA"] = "10";
    instMap["ADX"] = "11";
    instMap["ADS"] = "12";
    instMap["ADF"] = "13";
    instMap["SBA"] = "14";
    instMap["SBX"] = "15";
    instMap["SBS"] = "16";
    instMap["SBF"] = "17";
    instMap["MUL"] = "18";
    instMap["DIV"] = "19";
    instMap["MOD"] = "1A";
    instMap["CMA"] = "20";
    instMap["CMX"] = "21";
    instMap["CMS"] = "22";
    instMap["CMF"] = "23";
    instMap["JSR"] = "41";
    instMap["JEQ"] = "48";
    instMap["JLT"] = "4A";
    instMap["JGE"] = "4B";

    std::string command, line, converted;
    int addCount = 0;
    // int lineCount = 0;

    //This while loop is pass 1. It reads through the csv file once and assigns each line an address in memmap
    while (std::getline(infile, line)){  
        std::istringstream iss(line);
        // lineCount += 1;
        if(islower(line[0])){
            std::getline(iss, command, ':');
            converted = baseTentoSixteen(addCount);
            if(labelCounter[command] != 1){
                labelCounter[command] = 1;
            }
            else {
                std::cerr << "ASSEMBLER ERROR: label defined multiple times." << "\n";
                exit(5);
            }
            memmap[command] = converted;
            continue;
        } 
        addCount += 1;
    }

    // std::unordered_map<std::string, std::string>:: iterator itr;
    // for (auto itr = memmap.begin(); itr != memmap.end(); itr++)
    // {
    //     std::cout << itr->first << "  " << itr->second << "\n";
    // }

    //These two lines reset the cursor to the beginning of the file
    infile.clear();
    infile.seekg(0, std::ios::beg);

    // Second pass of the file
    int strLength;
    std::string machine;

    while (std::getline(infile, line)){  
        std::istringstream iss(line);
        if(islower(line[0])){
            continue;
        }
        strLength = line.length();
        if(line[0] == ' '){
            command = line.substr(4, (strLength - 4));
            strLength = command.length();
        }
        else{
            command = line;
        }
        //std::cout << command << "\n";
        //might need to have last line not have "\n"
        machine = toMechCode(command, memmapPtr, instMapPtr, labelCounterPtr, strLength);
        if (machine == "ERROR3"){
            std::cerr << "ASSEMBLER ERROR: instruction using unsupported addressing mode." << "\n";
            exit(4);
        }
        if (machine == "ERROR2"){
            std::cerr << "ASSEMBLER ERROR: undefined label." << "\n";
            exit(6);
        }
        //std::cout << machine << "\n";
        // lineCount -= 1;
        // if (lineCount < -1){
        //     outfile << machine;
        //     continue;
        // }
        outfile << machine << "\n";
    }

    infile.close();
    outfile.close();
}

// g++ -Wall -Werror  -std=c++14 -g assembler.cpp -o assembler

// ./assembler example1.csp  output.txt

// ./assembler  example2.csp  output.txt

// ./assembler  example3.csp  output.txt

// ./assembler  example4.csp  blah.txt

// ./assembler  invalid_labels.csp  output.txt

// ./assembler  invalid_undefined_label.csp  output.txt

// ./assembler  invalid_addr_mode.csp  output.txt

std::string toMechCode(std::string line, std::unordered_map<std::string, std::string> *memmapPtr, std::unordered_map<std::string, std::string> *instMapPtr, std::unordered_map<std::string, int> *labelCounterPtr, int strLength){
    std::string machineLine = "None";
    //This list of if statements takes care of the 3-4 letter operate instructions
    if (line.substr(0, 4) == "POPA" && strLength > 4){
        machineLine = "ERROR3";
        return machineLine;
    }
    if (strLength == 4 || strLength == 3){
        machineLine = (*instMapPtr)[line];
        return machineLine;
    }
    
    //Takes care of .words
    else if (line[0] == '.'){
        machineLine = line.substr(6, strLength - 6);
        while (machineLine.length() < 6){
            machineLine = "0" + machineLine;
        }
        return machineLine;
    }
    //Takes care of normal operand lines
    else if (isupper(line[0])){
        std::string inst = line.substr(0, 3); //POPA bypasses these checks
        machineLine = (*instMapPtr)[inst];
        int addMode = -1; // addMode == 0 means 0-9 and addMode == 2 means 2-9
        if (inst[0] == 'J' || inst[1] == 'T'|| inst == "POP" || inst == "CLR" || inst == "SET"){
            addMode = 2;
        }
        else{
            addMode = 0;
        }
        std::string ending = "";
        size_t found = line.find('!');
        if (found == std::string::npos){
            if (line[4] == '#'){
                if (addMode == 2){
                    machineLine = "ERROR3";
                    return machineLine;
                }
                machineLine = machineLine + "0"; 
            }
	        else if (line[4] == '+'){
                machineLine = machineLine + "4";
            }
            else if (line[4] == '*'){
                machineLine = machineLine + "6";
            }
            else if (line[4] == '&'){
                machineLine = machineLine + "8";
            }
            else{
                machineLine = machineLine + "2";
                ending = line.substr(4, strLength - 4);
            }
            if (ending == ""){
                ending = line.substr(6, strLength - 6);
            }
            if (islower(ending[0])){
                if((*labelCounterPtr)[ending] != 1){
                    machineLine = "ERROR2";
                    return machineLine;
                }
                machineLine = machineLine + (*memmapPtr)[ending];
                return machineLine;
            }
            machineLine = machineLine + endingNumParse(ending);
            return machineLine;
        }
        else if (found != std::string::npos){
            if (line[4] == '!'){
                machineLine = machineLine + "3";
                ending = line.substr(6, strLength - 6);
            }
            else if (line[4] == '#'){
                if (addMode == 2){
                    machineLine = "ERROR3";
                    return machineLine;
                }
                machineLine = machineLine + "1";
            }
            else if (line[4] == '+'){
                machineLine = machineLine + "5";
            }
            else if (line[4] == '*'){
                machineLine = machineLine + "7";
            }
            else if (line[4] == '&'){
                machineLine = machineLine + "9";
            }
            if (ending == ""){
                ending = line.substr(8, strLength - 8);
            }
            if (islower(ending[0])){
                if((*labelCounterPtr)[ending] != 1){
                    machineLine = "ERROR 2";
                    return machineLine;
                }
                machineLine = machineLine + (*memmapPtr)[ending];
                return machineLine;
            }
            machineLine = machineLine + endingNumParse(ending);
            return machineLine;
        }
    }
    return machineLine;
}

std::string baseTentoSixteen(int numToTen){
    int value, deBase = 16;
    std::string valString = "";
    while (numToTen != 0) {     //This while loop converts from base 10 to base 16 or less
        value = numToTen % deBase;
        numToTen = numToTen / deBase;
        switch (value) {
            case 10: valString = ("A") + valString;
                    break;
            case 11: valString = ("B") + valString;
                    break;
            case 12: valString = ("C") + valString;
                    break;
            case 13: valString = ("D") + valString;
                    break;
            case 14: valString = ("E") + valString;
                    break;
            case 15: valString = ("F") + valString;
                    break;                    
            default: valString = std::to_string(value) + valString;
                    break;                                                              
        }
    }
    while (valString.length() < 3){
        valString = "0" + valString;
    }
    return valString;
}

std::string endingNumParse(std::string ending){
    std::string output;
    if (ending[0] == '$'){
        output = ending.substr(1, (ending.length()-1));
        while (output.length() < 3){
            output = "0" + output;
        }
    }
    else {
        output = baseTentoSixteen(std::stoi(ending));
    }
    return output;
}