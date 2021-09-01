  
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <stack>
#include <locale>
#include <cstring>
#include <sstream>

using namespace std;

// global variables
int nofTempVariables = 1;
int nofConditions = 1;
int nofLines = -1;
string line;
string outputFileName = "";
set<string> variables;
ifstream infile;            
ofstream outfile;
fstream afterAllocation;

// methods
int preced(char ch);
string expression(bool equal, string expr);
string inToPost(string infix);
bool isOperator(string str);
string spaceCheck(string str);
void error();
vector<string> chooseParser(string a);
string choose(string line);
vector<string> expressionParser(bool equal, string line);
void condition(string expres);
bool exists(string str);
bool isNumber(string str);
void storeVariable(string target, string value);
void allocateVariable(string s);
string loadVariable(string s);
void assignment(string leftName, string value);
string handleVariable(string var);
string expression(bool equal, string expr);
void printStatement(string s);
string fixLine(string line);

// takes a string as an infix and returns its postfix form
string inToPost(string infix) {
  
   stack<char> stk;
   stk.push('#');               
   string postfix = "";         
   string::iterator it;

   for(it = infix.begin(); it!=infix.end(); it++) {
      if(*it == ' '){
          postfix += " ";
          continue;
      }
      if(isalnum(char(*it)) || *it == '%'){
         postfix += *it;      
      }else if(*it == '(')
         stk.push('(');
      else if(*it == '^')
         stk.push('^');
      else if(*it == ')') {
         while(stk.top() != '#' && stk.top() != '(') {
            postfix += " ";
            postfix += stk.top();
            postfix += " ";
            stk.pop();
         }
         stk.pop();          
      }else {
         if(preced(*it) > preced(stk.top()))
            stk.push(*it); 
         else {
            while(stk.top() != '#' && preced(*it) <= preced(stk.top())) {
              postfix += " ";
               postfix += stk.top();       
               stk.pop();
            }
            stk.push(*it);
         }
      }
   }

   while(stk.top() != '#') {
      postfix += " ";
      postfix += stk.top();
      postfix += " ";    
      stk.pop();
   }
   return postfix;
}

// precedence method for turning infix to postfix
int preced(char ch) {
   if(ch == '+' || ch == '-') {
      return 1;              
   }else if(ch == '*' || ch == '/') {
      return 2;            
   }else if(ch == '^') {
      return 3;            
   }else {
      return 0;
   }
}

// returns true if the given string is an operator, else returns false
bool isOperator(string str) {
  if(str == "+" || str == "-" || str == "/" || str == "*") {
    return true;
  }
  return false;
}

// removes the unnecessary spaces and puts space before and after any operator
string spaceCheck(string str){

    string noSpace = "";
    for (int i=0; i<str.length(); i++) {
        string s (1, str[i]);
      
        if(isOperator(s) || str[i] == '='){
          noSpace += " ";
          noSpace += str[i];
          noSpace += " ";
          
        } else if (!isspace(str[i])){
          noSpace += str[i];
        } 
    }
    return noSpace;
}

// prints an error statement to the output file
void error(){
  
  remove(outputFileName.c_str());
  outfile.close();
  outfile.open(outputFileName.c_str());
  outfile << "; ModuleID = 'mylang2ir'" << endl;
  outfile << "declare i32 @printf(i8*, ...)" << endl;
  outfile << "@print.error = constant [24 x i8] c\"Line %d: syntax error\\0A\\00 \"" << endl;
  outfile << "define i32 @main() {" << endl;
  outfile << "call i32 (i8*, ...)* @printf(i8* getelementptr ([24 x i8]* @print.error, i32 0, i32 0), i32 "<< nofLines << " )"<< endl;
  outfile << "ret i32 0" << endl;
  outfile << "}" << endl;
  exit(0);

}

// takes the choose line as an input and divides its parameters and returns the vector that contaions the parameters of choose
// if there are more or less parameters than 4, then it gives an error
vector<string> chooseParser(string a){

    int len = 0;
    int realFirstIndex = 0;
    int realLastIndex = 0;
    int nofParenthes = 0;
    for(int i = 0; i < a.length(); i++){
       if(string(1, a[i]) == "c" && string(1, a[i+1]) == "h" && string(1, a[i+2]) == "o" && string(1, a[i+3]) == "o" && string(1, a[i+4]) == "s" && string(1, a[i+5]) == "e"){
            realFirstIndex = i;
            for(int j = i+6; j < a.length(); j++){
              
                if(string(1, a[j]) == "("){
                    nofParenthes++;
                }else if(string(1, a[j]) == ")"){
                    nofParenthes--;
                    if(nofParenthes == 0){
                      realLastIndex = j;
                      break;
                    }
                }
            }
            break;
       }
    }
    len = realLastIndex - realFirstIndex + 1;
    a = a.substr(realFirstIndex, len);    

    int insertPoint = a.find_last_of(")");
    a.insert(insertPoint, ",");
    vector<string> parList;
    
    bool firstTime = true;
    nofParenthes = 0;
    int virgulIndex = 0;
    int lastIndex = 0;
    int firstIndex = 0;
    len = 0;
    for(int i = 0; i < a.length(); i++){
      if(string(1, a[i]) == "(" && firstTime == true){
           firstTime = false;
           firstIndex = i + 1;
           continue;
       }
       if(string(1, a[i]) == "(" && firstTime == false){
           nofParenthes++;
       } 
      if(string(1, a[i]) == ")" && firstTime == false){
           nofParenthes--;
       }
      if(string(1, a[i]) == "," && nofParenthes == 0 && firstTime == false){
        virgulIndex = i;
        lastIndex = virgulIndex - 1;
        len = lastIndex - firstIndex + 1;
        parList.push_back(a.substr(firstIndex, len));
        firstIndex = virgulIndex + 1;
      }
    }
   
    if(parList.size() != 4){
      error();
    }
    return parList;
}

// writes the conditions and selects statements for the choose function
string choose(string line){

      vector<string> parameters = chooseParser(line);
      
      string par1Name = expression(false, parameters[0]);
      string firstConditionName = "%t" + to_string(nofTempVariables);
      afterAllocation << firstConditionName << " = icmp eq i32 " << par1Name << ", 0" << endl;
      nofTempVariables++;
      
      string par2Name = expression(false, parameters[1]);
      string secondConditionName = "%t" + to_string(nofTempVariables);
      afterAllocation << secondConditionName << " = icmp sgt i32 " << par1Name << ", 0" << endl;
      nofTempVariables++;
    
      string par3Name = expression(false, parameters[2]);
      string par4Name = expression(false, parameters[3]);
      string elseName = "%t" + to_string(nofTempVariables);
      afterAllocation << elseName << " = select i1 " << secondConditionName << ", i32 " << par3Name << ", i32 " << par4Name << endl;
      nofTempVariables++;
      string chooseName = "%t" + to_string(nofTempVariables);
      afterAllocation << chooseName << " = select i1 " << firstConditionName << ", i32 " << par2Name << ", i32 " << elseName << endl;
      nofTempVariables++;

      return chooseName;
  }

// one of the main functions of this program
// @param equal: true if line contains =, else false
// @param line: the line that needs to be parsed
// @return: a vector that contains the tokens in the line
vector<string> expressionParser(bool equal, string line){  
  
    while(line.find("choose") != string::npos){
    
    int chooseFirstIndex = line.find("choose");
    int chooseLastIndex = 0;
    bool inChoose = false;
    int nofParantheses = 0;
    for(int i=0; i<line.length(); i++){
      if(i == chooseFirstIndex){
        inChoose = true;
        i+=5;
        continue;
      }
      if(inChoose){
        if(line[i] == '('){
          nofParantheses++;
        } else if(line[i] == ')'){
          nofParantheses--;
          if(nofParantheses == 0){
            chooseLastIndex = i;
            break;
          }
        } else if(nofParantheses == 0 && !isspace(line[i])){
          error();
        }
      }
    }
    string chooseName = choose(line);
    string result = line.substr(0, chooseFirstIndex) + " " + chooseName + " " + line.substr(chooseLastIndex+1);
    line = result;
  }

    line = spaceCheck(line);

    int l = line.length();
    char str[l+1];
    strcpy(str, line.c_str());

    char delim[] = " ";
    char *token = strtok(str, delim);

    string leftSide = "";
    string rightSide = "";
    int leftSideCounter = 0;
    while (token) {
      
        if(strcmp(token, "=") == 0){
            equal = true;
        }else if(equal == false){
            if(leftSideCounter != 0){
              error();              
            }
            leftSide = token;
            leftSideCounter++; 
        }else if(equal == true && token != " "){
            rightSide += token;
            rightSide += " ";
        }
        token = strtok(NULL,delim);
    }
   
    string postfix = inToPost(rightSide);

    stringstream test(postfix);    
    string segment;
    vector<string> seglist;   
  
    seglist.push_back(leftSide);    
                  
    while(getline(test, segment, ' '))  
    {
        if(segment != "" && segment != " ") {
          
            seglist.push_back(segment);
        }        
    }

    return seglist;
}

// writes the condition's first part
void condition(string expres){
  afterAllocation << "br label %" << "cond" << nofConditions << endl;
  afterAllocation << "cond" << nofConditions << ":" << endl;
  string conditionTemp = expression(false, expres);
  afterAllocation << "%t" << nofTempVariables << " = icmp ne i32 " << conditionTemp  << ", 0" << endl;        // %t2 = icmp ne i32 %t1, 0
  afterAllocation << "br i1 %t" << nofTempVariables << ", label %body" << nofConditions << ", label %end" << nofConditions << endl;  
  nofTempVariables++; 
  afterAllocation << "body" << nofConditions << ":" << endl;   //whbody:
}

// returns true if the given variable has been allocated before, else returns false
bool exists(string str){
  if(variables.find(str) == variables.end()){
    return false;
  }
  return true;
}

// returns true if the given string is a number, else returns false
bool isNumber(string str){
  
  for(int i=0; i<str.length(); i++){
    if(str[i] > '9' || str[i] < '0'){
      return false;
    }
  }
  
  return true;
}

// returns true if the given string is a valid variable name, else returns false
bool isVariable(string str){
  
  if(!isalpha(str[0])){
    return false;
  }
 
  for(int i=1; i<str.length(); i++){
    if(!isalnum(str[i])){
      return false;
    }
  }
  
  return true;
}

// stores a value in the given variable
void storeVariable(string target, string value){
  afterAllocation << "store i32 " << value << ", i32* " << target << endl;
}

// allocates a variable, puts the variable inside the variable set so that checking if a variable does exist becomes easy
void allocateVariable(string s) {
  variables.insert(s);
  outfile << s << " = alloca i32" << endl;
  outfile << "store i32 " << 0 << ", i32* " << s << endl;
}

// loads an existing variable and returns its temp name
string loadVariable(string s){
  string tempVariableName = "%t" + to_string(nofTempVariables);
  afterAllocation << tempVariableName << " = load i32* " << s << endl;
  nofTempVariables++;
  return tempVariableName;
}

// does the assignment line
void assignment(string leftName, string value){
  
  if(!isVariable(leftName)) {
    error();
  }
  
  string leftVariableName = "%" + leftName;
  
  if(!exists(leftVariableName)) {
    allocateVariable(leftVariableName);
  } 
  
  storeVariable(leftVariableName, value); 
}

// takes the variable and if it exists, loads it; if not, allocates and if neither is required returns the variable as it is
// if the parameter is none of them, then it is an error
string handleVariable(string var){

  string vartemp = "";
  
  if(var[0] == '%' || isNumber(var)){
      vartemp = var;
  } else if(isVariable(var)){
    
    if(exists("%"+var)){
      vartemp = loadVariable("%"+var);
    } else {
      allocateVariable("%"+var);
      vartemp = loadVariable("%"+var);
    }
  
  } else {
    error();
  }
    return vartemp;
}

// one of the main functions of the program, takes the line and evaluates it
// @param expr: the whole line
// @param equal: if the line includes = true, else false
// @return: the name of the temp variable that holds the whole expression
string expression(bool equal, string expr) {

  vector<string> postfixVector = expressionParser(!equal, expr);
  stack<string> postfixVersion;
  stack<string> waitList;
  
  for(int i=postfixVector.size()-1; i>0; i--) {
      postfixVersion.push(postfixVector[i]);
  }
    
    if(postfixVersion.size() == 1) {     
      
      string token = postfixVersion.top();
      postfixVersion.pop();
      string result = handleVariable(token);
      
      // assignment
      if(equal){
        assignment(postfixVector[0], result);
      }
      return result;
    }
   
   // does the computations here
    while(!postfixVersion.empty()){
      
      string token = postfixVersion.top();
      postfixVersion.pop();
      
      if(isVariable(token) || isNumber(token) || token[0] == '%'){
        
        waitList.push(token);

      } else if(isOperator(token)) {
        
        string var1 = waitList.top();
        string var1temp = handleVariable(var1);
        waitList.pop();
        string var2 = waitList.top();
        string var2temp = handleVariable(var2);
        waitList.pop();
        
        if(token == "+"){
          
          string resultName = "%t" + to_string(nofTempVariables);
          afterAllocation << resultName << " = add i32 " << var2temp << ", " << var1temp << endl;
          waitList.push(resultName);

        } else if(token == "-"){

          string resultName = "%t" + to_string(nofTempVariables);
          afterAllocation << resultName << " = sub i32 " << var2temp << ", " << var1temp << endl;
          waitList.push(resultName);
        
        } else if(token == "/"){
          
          string resultName = "%t" + to_string(nofTempVariables);
          afterAllocation << resultName << " = sdiv i32 " << var2temp << ", " << var1temp << endl;
          waitList.push(resultName);

        } else if(token == "*"){

          string resultName = "%t" + to_string(nofTempVariables);
          afterAllocation << resultName << " = mul i32 " << var1temp << ", " << var2temp << endl;
          waitList.push(resultName);
        }
        
        nofTempVariables++;
      
      } else {
        error();
      }
    }
    
    if (equal) {
      string result = handleVariable(waitList.top());
      waitList.pop();
      assignment(postfixVector[0], result);
      return "";
    }
    
    string result = waitList.top();
    waitList.pop();
    
    if(!waitList.empty()) {
      error();
    }

    return result;
}

// takes the line as a whole and evaluates it in the expression function and prints the print line 
void printStatement(string s){
  string variableName = expression(false, s);
  afterAllocation << "call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 " << variableName << " )" << endl;
}

// removes the comments, counts the number of parantheses and if there is a syntax error, goes to error function
// removes the spaces at the end and beginning of the line
// returns the modified line
string fixLine(string line){
   
  if(line.find("#") != string::npos){
    int commentIndex = line.find("#");
    line = line.substr(0, commentIndex);
  }

  int nofParanthesis = 0;
  for(int i = 0 ; i < line.length(); i++){
    if(nofParanthesis < 0){
      error();
    }
    if(string(1, line[i]) == "("){
      nofParanthesis++;
    }else if(string(1, line[i]) == ")"){
      nofParanthesis--;
    }
  }
  
  if(nofParanthesis != 0){
    error();
  }

  int startOfLine = 0;
  int endOfLine = 0;
  for (int i=0; i<line.length(); i++) {
        if (!isspace(line[i])){
          break;
        } 
        startOfLine++;
  }
   for (int i=line.length()-1; i>=0; i--) {
        if (!isspace(line[i])){
          endOfLine = i;
          break;
        }
  }
  return line.substr(startOfLine, endOfLine - startOfLine + 1);
}

int main(int argc, char const *argv[]) {

  bool isCondition = false;
  bool isIf = false;  
  
  // input file
  infile.open(argv[1]);
  
  // output file
  string inputFileName = argv[1];
  int positionOfDot = inputFileName.length();
  for(int i=inputFileName.length()-1; i >= 0; i--){
    if(inputFileName[i] == '.'){
      positionOfDot = i;
      break;
    }
  }
  outputFileName = inputFileName.substr(0, positionOfDot) + ".ll";
  outfile.open(outputFileName);
  
  // temp file
  afterAllocation.open("a.txt", ios::in | ios::out| ios::trunc);

  outfile << "; ModuleID = 'mylang2ir'" << endl;
  outfile << "declare i32 @printf(i8*, ...)" << endl;
  outfile << "@print.str = constant [4 x i8] c\"%d\\0A\\00\"" << endl;
  outfile << "define i32 @main() {" << endl;

  while(getline(infile, line)) {
    
    nofLines++;
    line = fixLine(line);
  
    if(line.find('=')!= string::npos) {   
      
      expression(true, line);  

    } else if(line.find("print")!= string::npos) { 
      // printten once, parantezle arada, ) dan sonra bisey varsa error
      int startIndex = line.find_first_of("(")+1;
      int endIndex = line.find_last_of(")");
      string exprToPrint = line.substr(startIndex, endIndex - startIndex);
      
      printStatement(exprToPrint);

    } else if(line.find("while")!= string::npos && isCondition == false) { 
      // whileden once, parantezle arada, parantezle } arasında, } dan sonra bisey varsa, } yoksa error
      int startIndex = line.find("(") + 1;
      int endIndex = line.find_last_of(")");
      string exprToCheck = line.substr(startIndex, endIndex - startIndex);
      condition(exprToCheck);  
      isCondition = true;
    
    } else if(line.find("if")!= string::npos && isCondition == false) { 
      // ifden once, parantezle arada, parantezle } arasında, } dan sonra bisey varsa, } yoksa error
      int startIndex = line.find("(") + 1;
      int endIndex = line.find_last_of(")");
      string exprToCheck = line.substr(startIndex, endIndex - startIndex);
      condition(exprToCheck); 
      isIf = true;
      isCondition = true;

    } else if(line.find("}") != string::npos && isCondition == true){
      // }dan once ve sonra bisey varsa error
      if(!isIf){
        afterAllocation << "br label %cond" << nofConditions << endl; //br label %whcond
      } else {
        afterAllocation << "br label %end" << nofConditions << endl;
      }
      afterAllocation << "end" << nofConditions << ":" << endl;     //whend:
      isCondition = false;  
      isIf = false;
      nofConditions++;
    
    } else if(line.find("}") != string::npos && isCondition == false){          // when there is no condiiton }
      error();
    } else if(line.find("{") != string::npos && isCondition == true){           // nested if-while
      error();
    } else if (!line.empty()){
      error(); 
      
    }                        
  }
 
  if(isCondition){
    error();
  }

  string x;
  afterAllocation << "ret i32 0" << endl;
  afterAllocation << "}" << endl;
  afterAllocation.seekg(0, ios::beg);
  
  while(getline(afterAllocation, x)){
    outfile << x << endl;
  }
  
  afterAllocation.clear();
  afterAllocation.close();
  remove("a.txt");
  return 0;
}
