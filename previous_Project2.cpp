#include <stdio.h>
#include <string.h>

#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

string gErrorMsg;

class QUIT {};  // class QUIT

class Partially_Correct {};  // class Partially_Correct

enum TokenType { RESERVED, IDENTIFIER, CONSTANT, DELIMITER, ERROR };

enum VariableType { INT, FLOAT, CHAR, BOOL, STRING, FUNCTION, UNDEFINED };

VariableType ToVariableType(const string &type) {
    if (type == "int") {
        return INT;
    } else if (type == "float") {
        return FLOAT;
    } else if (type == "char") {
        return CHAR;
    } else if (type == "bool") {
        return BOOL;
    } else if (type == "string") {
        return STRING;
    }

    return UNDEFINED;
}

int ToInt(const string &str) {
    int num;
    stringstream ss;
    ss << str;
    ss >> num;
    return num;
}

class BasicInformation {
   public:
    string mString;
    VariableType mVariableType;
};

class BasicToken : public BasicInformation {
   public:
    TokenType mType;
    BasicToken(const string &str) {
        mString = str;
        mType = Type(str);
    }

    BasicToken(const string &str, const TokenType &type) {
        mString = str;
        mType = type;
    }

    bool Mach(const string &str) {
        if (str == "IDENTIFIER") {
            return mType == IDENTIFIER;
        } else if (str == "CONSTANT") {
            return mType == CONSTANT;
        } else {
            return ("'" + mString + "'") == str;
        }
    }

   private:
    bool IsLegalIdentifier(const string &str) {
        if ((str[0] < 'a' || str[0] > 'z') && (str[0] < 'A' || str[0] > 'Z')) {
            return false;
        }  // if

        for (int i = 0; i < str.length(); i++) {
            if ((str[i] < '0' || str[i] > '9') && (str[i] < 'a' || str[i] > 'z') &&
                (str[i] < 'A' || str[i] > 'Z') && str[i] != '_') {
                return false;
            }  // if
        }      // for

        return true;
    }

    bool IsDigit(const string &str) {
        int numOfDecimalPoint = 0;
        for (int i = 0; i < str.length(); i++) {
            if (str[i] == '.') {
                numOfDecimalPoint++;
            }  // if

            if (((str[i] < '0' || str[i] > '9') && str[i] != '.') || numOfDecimalPoint > 1) {
                return false;
            }  // if
        }      // for

        return true;
    }  // IsDigit()

    TokenType Type(const string &str) {
        if (str == "int" || str == "float" || str == "char" || str == "bool" || str == "string" ||
            str == "void" || str == "if" || str == "else" || str == "while" || str == "do" ||
            str == "return" || str == "ListAllVariables" || str == "ListAllFunctions" ||
            str == "ListVariable" || str == "ListFunction" || str == "Done" || str == "cin" ||
            str == "cout") {
            mVariableType = ToVariableType(str);

            return RESERVED;
        } else if (str == "true" || str == "false") {
            mVariableType = BOOL;
            return CONSTANT;
        } else if (IsDigit(str)) {
            if (str.find(".") == str.npos) {
                mVariableType = INT;
            } else {
                mVariableType = FLOAT;
            }

            return CONSTANT;
        } else if (str[0] == '\'' && str[str.length() - 1] == '\'' && str.length() >= 2) {
            mVariableType = CHAR;
            return CONSTANT;
        } else if (str[0] == '\"' && str[str.length() - 1] == '\"' && str.length() >= 2) {
            mVariableType = STRING;
            return CONSTANT;
        } else {
            return IsLegalIdentifier(str) ? IDENTIFIER : ERROR;
        }
    }
};

class TokenGetter {
   private:
    map<string, int> mDelimiters;
    vector<BasicToken> mTokenList;
    int mLineCounter;
    bool Contains(const string &str) { return mDelimiters.find(str) != mDelimiters.end(); }  // Contains()

    void GetTokenList() {
        string line;
        GetLine(line);
        mLineCounter++;
        string buffer;
        string currentChar = " ";
        string nextChar = " ";
        bool isReadingString = false, isReadingChar = false, escapeCharacter = false;
        for (int i = 0; i < line.length() && (currentChar != "/" || nextChar != "/"); i++) {
            currentChar = line.substr(i, 1);
            if (i < line.length() - 1) {
                nextChar = line.substr(i + 1, 1);
            }  // if
            else {
                nextChar = " ";
            }  // else

            if (isReadingString || isReadingChar) {
                buffer += currentChar;
                if (escapeCharacter) {
                    escapeCharacter = false;
                } else {
                    if (currentChar == "\"") {
                        if (isReadingString) {
                            mTokenList.push_back(BasicToken(buffer));
                            buffer.clear();
                        }

                        isReadingString = false;
                    } else if (currentChar == "\'") {
                        if (isReadingChar) {
                            mTokenList.push_back(BasicToken(buffer));
                            buffer.clear();
                        }

                        isReadingChar = false;
                    }

                    if ((isReadingString || isReadingChar) && currentChar == "\\") {
                        escapeCharacter = true;
                    }
                }
            } else {
                if (currentChar != "/" || nextChar != "/") {
                    if (Contains(currentChar) || Contains(currentChar + nextChar) || currentChar == " " ||
                        currentChar == "\t" || currentChar == "\'" || currentChar == "\"") {
                        if (buffer.length() > 0) {
                            mTokenList.push_back(BasicToken(buffer));
                            buffer.clear();
                        }  // if

                        if (currentChar == "\'") {
                            isReadingChar = true;
                            buffer += currentChar;
                        } else if (currentChar == "\"") {
                            isReadingString = true;
                            buffer += currentChar;
                        } else if (Contains(currentChar + nextChar)) {
                            mTokenList.push_back(BasicToken(currentChar + nextChar, DELIMITER));
                            i++;
                        }  // if
                        else if (Contains(currentChar)) {
                            mTokenList.push_back(BasicToken(currentChar, DELIMITER));
                        }  // else if
                    }      // if
                    else {
                        if (BasicToken(buffer + currentChar).mType == ERROR) {
                            if (buffer.length() > 0) {
                                mTokenList.push_back(BasicToken(buffer));
                                buffer.clear();
                            }  // if
                        }      // if

                        buffer += currentChar;
                    }  // else
                }      // if
            }

        }  // for

        if (buffer.length() > 0) {
            mTokenList.push_back(BasicToken(buffer));
        }  // if
    }      // GetTokenList()
   public:
    static void GetLine(string &str) {
        char ch = getchar();
        while (ch != '\n' && ch != EOF) {
            str.push_back(ch);
            ch = getchar();
        }  // while

        if (str.length() == 0 && ch == EOF) {
            throw QUIT();
        }
    }  // GetLine()

    TokenGetter() {
        string delimiters[] = {"(",  ")",  "[",  "]",  "{",  "}",  "+", "-", "*",  "/",  "%",  "^",  ">",
                               "<",  ">=", "<=", "==", "!=", "&",  "|", "!", "&&", "||", "+=", "-=", "*=",
                               "/=", "%=", "++", "--", ">>", "<<", ";", ",", "?",  ":",  "="};
        for (int i = 0; i < 37; i++) {
            mDelimiters[delimiters[i]] = 0;
        }  // for

        ResetLineCounter();
    }  // TokenGetter()

    void ResetLineCounter() { mLineCounter = 0; }

    int GetCurrentLineNumber() { return mLineCounter; }

    BasicToken GetNextToken() {
        while (mTokenList.size() == 0) {
            GetTokenList();
        }

        BasicToken token = mTokenList.front();
        mTokenList.erase(mTokenList.begin());
        return token;
    }

    void ClearUnusedTokens() { mTokenList.clear(); }

};  // class TokenGetter

class VariableDefinition : public BasicInformation {
   public:
    int mArraySize;
    bool mIsReference;
    VariableDefinition(const string &variableName, VariableType type, int arraySize) {
        mString = variableName;
        mVariableType = type;
        mArraySize = arraySize;
    }

    VariableDefinition(const string &variableName, VariableType type, bool isReference) {
        mString = variableName;
        mVariableType = type;
        mIsReference = isReference;
    }
};

class FunctionDefinition : public BasicInformation {};

class FunctionDefinitionManager {
   private:
    map<string, FunctionDefinition *> mFunctionDefinitionTable;

   private:

   public:
    void Define(const string &variableName, VariableType type,
                const vector<VariableDefinition> &parameterList) {}

    bool IsExist(const string &functionName) {
        return mFunctionDefinitionTable.find(functionName) != mFunctionDefinitionTable.end();
    }

    ~FunctionDefinitionManager() {
        map<string, FunctionDefinition *>::iterator iter = mFunctionDefinitionTable.begin();
        while (iter != mFunctionDefinitionTable.end()) {
            delete mFunctionDefinitionTable[iter->first];
            iter++;
        }
    }
};

class VariableValue {
   public:
    int *mINTptr;
    float *mFLOATptr;
    char *mCHARptrE;
    bool *mBOOLptr;
    string *mSTRINGptr;

    int arraySize;

    VariableValue() {
        mINTptr = NULL;
        mFLOATptr = NULL;
        mCHARptrE = NULL;
        mBOOLptr = NULL;
        mSTRINGptr = NULL;
        arraySize = 0;
    }
};

class VariableDefinitionManager {
   private:
    map<string, int *> mINTDefinitionTable;
    map<string, float *> mFLOATDefinitionTable;
    map<string, char *> mCHARDefinitionTable;
    map<string, bool *> mBOOLDefinitionTable;
    map<string, string *> mSTRINGDefinitionTable;

    map<string, VariableDefinition *> mVariableDefinitionTable;
    vector<vector<string> *> mVariableDefinitionStack;

    int mCurrentLevel;

   private:
    void DeleteVariable(const string &variableName) {
        VariableDefinition *target = mVariableDefinitionTable[variableName];
        VariableType type = target->mVariableType;
        int arraySize = target->mArraySize;
        if (type == INT) {
            if (arraySize > 0) {
                delete[] mINTDefinitionTable[variableName];
            } else {
                delete mINTDefinitionTable[variableName];
            }
            mINTDefinitionTable.erase(variableName);
        } else if (type == FLOAT) {
            if (arraySize > 0) {
                delete[] mFLOATDefinitionTable[variableName];
            } else {
                delete mFLOATDefinitionTable[variableName];
            }
            mFLOATDefinitionTable.erase(variableName);
        } else if (type == CHAR) {
            if (arraySize > 0) {
                delete[] mCHARDefinitionTable[variableName];
            } else {
                delete mCHARDefinitionTable[variableName];
            }
            mCHARDefinitionTable.erase(variableName);
        } else if (type == BOOL) {
            if (arraySize > 0) {
                delete[] mBOOLDefinitionTable[variableName];
            } else {
                delete mBOOLDefinitionTable[variableName];
            }
            mBOOLDefinitionTable.erase(variableName);
        } else if (type == STRING) {
            if (arraySize > 0) {
                delete[] mSTRINGDefinitionTable[variableName];
            } else {
                delete mSTRINGDefinitionTable[variableName];
            }
            mSTRINGDefinitionTable.erase(variableName);
        }

        delete mVariableDefinitionTable[variableName];
        mVariableDefinitionTable.erase(variableName);
    }

    string ToDistinctName(const string &variableName) {
        string distinctVariableName = "";
        stringstream ss;
        ss << variableName << mCurrentLevel;
        distinctVariableName = ss.str();
        return distinctVariableName;
    }

   public:
    bool Define(const string &variableName, VariableType type, int arraySize) {
        string distinctVariableName = ToDistinctName(variableName);
        bool hasDefined = false;
        if (mVariableDefinitionTable.find(distinctVariableName) != mVariableDefinitionTable.end()) {
            hasDefined = true;
            DeleteVariable(distinctVariableName);
        } else {
            mVariableDefinitionStack[mCurrentLevel]->push_back(distinctVariableName);
        }

        mVariableDefinitionTable[distinctVariableName] =
            new VariableDefinition(distinctVariableName, type, arraySize);
        if (type == INT) {
            if (arraySize > 0) {
                mINTDefinitionTable[distinctVariableName] = new int[arraySize];
            } else {
                mINTDefinitionTable[distinctVariableName] = new int;
            }
        } else if (type == FLOAT) {
            if (arraySize > 0) {
                mFLOATDefinitionTable[distinctVariableName] = new float[arraySize];
            } else {
                mFLOATDefinitionTable[distinctVariableName] = new float;
            }
        } else if (type == CHAR) {
            if (arraySize > 0) {
                mCHARDefinitionTable[distinctVariableName] = new char[arraySize];
                strcpy(mCHARDefinitionTable[distinctVariableName], "\0");
            } else {
                mCHARDefinitionTable[distinctVariableName] = new char;
            }
        } else if (type == BOOL) {
            if (arraySize > 0) {
                mBOOLDefinitionTable[distinctVariableName] = new bool[arraySize];
            } else {
                mBOOLDefinitionTable[distinctVariableName] = new bool;
            }
        } else if (type == STRING) {
            if (arraySize > 0) {
                mSTRINGDefinitionTable[distinctVariableName] = new string[arraySize];
            } else {
                mSTRINGDefinitionTable[distinctVariableName] = new string;
            }
        }

        return hasDefined;
    }

    bool IsExist(const string &variableName) {
        for (int i = mCurrentLevel; i >= 0; i--) {
            if (mVariableDefinitionTable.find(ToDistinctName(variableName)) != mVariableDefinitionTable.end())
                return true;
        }
        return false;
    }

    VariableDefinitionManager() {
        mCurrentLevel = 0;
        mVariableDefinitionStack.push_back(new vector<string>());
    }

    ~VariableDefinitionManager() {
        map<string, VariableDefinition *>::iterator iter = mVariableDefinitionTable.begin();
        vector<string> variables;
        while (iter != mVariableDefinitionTable.end()) {
            variables.push_back(iter->first);
            iter++;
        }

        for (int i = 0 ; i < variables.size() ; i++) {
            DeleteVariable(variables[i]);
        }
    }

    void LevelUp() {
        mCurrentLevel++;
        mVariableDefinitionStack.push_back(new vector<string>);
    }

    void LevelDown() {
        for (int i = 0; i < mVariableDefinitionStack[mCurrentLevel]->size(); i++) {
            DeleteVariable(mVariableDefinitionStack[mCurrentLevel]->at(i));
        }

        delete mVariableDefinitionStack[mCurrentLevel];
        mVariableDefinitionStack.erase(mVariableDefinitionStack.begin() + mCurrentLevel);

        mCurrentLevel--;
    }

    void BackToRoot() {
        for (int i = mCurrentLevel; i > 0; i--) {
            LevelDown();
        }
    }
};

class GrammarChecker {
   private:
    map<string, string> mRuleMap;
    map<char, char> mPair;
    string mErrorToken;
    vector<string> *GetToken(const string &grammar) {
        if (grammar.length() == 0) {
            return NULL;
        }  // if

        vector<string> *grammarQueue = new vector<string>();
        string buffer;
        int levelCounter = 0;
        bool isGroup = false;
        bool groupLock = false;
        map<char, char>::iterator iter;
        for (int i = 0; i < grammar.length(); i++) {
            if (!isGroup) {
                if (grammar[i] == '\'') {
                    groupLock = !groupLock;
                }

                if (!groupLock) {
                    iter = mPair.find(grammar[i]);
                    if (iter != mPair.end()) {
                        isGroup = true;
                        levelCounter++;
                    }  // if
                }      // if
            }          // if
            else {
                if (grammar[i] == iter->first)
                    levelCounter++;
                else if (grammar[i] == iter->second)
                    levelCounter--;
                if (levelCounter == 0) isGroup = false;
            }  // else

            if (grammar[i] == ' ' && levelCounter == 0) {
                if (buffer.length() > 0) {
                    grammarQueue->push_back(buffer);
                    buffer.clear();
                }  // if
            }      // if
            else {
                buffer.push_back(grammar[i]);
            }  // else
        }      // for

        if (buffer.length() > 0) {
            grammarQueue->push_back(buffer);
        }  // if

        return grammarQueue;
    }  // GetToken()

    bool Compare(vector<BasicToken> &instructionQueue, const string &grammar, string currentTokenType) {
        if (grammar[0] == '{' || grammar[0] == '[' || grammar[0] == '(' || grammar[0] == '<') {
            int previousSize = instructionQueue.size();
            bool ifSuccess =
                Check(instructionQueue, grammar.substr(1, grammar.length() - 2), currentTokenType);
            if (grammar[0] == '(' || grammar[0] == '<') {
                mErrorToken = instructionQueue[0].mString;
                return ifSuccess;
            }  // if
            else if (grammar[0] == '[') {
                mErrorToken = instructionQueue[0].mString;
                int currentSize = instructionQueue.size();
                return (ifSuccess || previousSize == currentSize);
            }  // else if
            else {
                while (ifSuccess) {
                    previousSize = instructionQueue.size();
                    ifSuccess =
                        Check(instructionQueue, grammar.substr(1, grammar.length() - 2), currentTokenType);
                }  // while

                mErrorToken = instructionQueue[0].mString;
                int currentSize = instructionQueue.size();
                return previousSize == currentSize;
            }  // else
        }      // if
        else {
            if (instructionQueue.size() == 0) {
                throw Partially_Correct();
            }  // if

            if (instructionQueue[0].Mach(grammar)) {
                instructionQueue.erase(instructionQueue.begin());
                mTokenTypeList.push_back(currentTokenType);
                return true;
            } else {
                return false;
            }
        }  // else
    }      // Compare()

    bool Check(vector<BasicToken> &instructionQueue, const string &grammar, string currentTokenType) {
        map<string, string>::iterator iter = mRuleMap.find(grammar);
        vector<string> *grammarQueue = NULL;
        if (iter != mRuleMap.end()) {
            currentTokenType = grammar;
            grammarQueue = GetToken(iter->second);
        }  // if
        else {
            grammarQueue = GetToken(grammar);
        }  // else

        bool isORRelation = false;
        for (int i = 0; i < grammarQueue->size(); i++) {
            if (grammarQueue->at(i) == "|") {
                grammarQueue->erase(grammarQueue->begin() + i);
                isORRelation = true;
                i--;
            }  // if
        }      // for

        if (isORRelation) {
            for (int i = 0; i < grammarQueue->size(); i++) {
                int previousSize = instructionQueue.size();
                bool isCorrect = Compare(instructionQueue, grammarQueue->at(i), currentTokenType);
                int currentSize = instructionQueue.size();
                if (isCorrect) {
                    return true;
                }  // if
                else {
                    if (previousSize != currentSize) {
                        return false;
                    }  // if
                }      // else
            }          // for

            return false;
        }  // if
        else {
            for (int i = 0; i < grammarQueue->size(); i++) {
                if (!Compare(instructionQueue, grammarQueue->at(i), currentTokenType)) {
                    return false;
                }  // if
            }      // for

            return true;
        }  // else

        delete grammarQueue;
    }  // Check()

   public:
    GrammarChecker() {
        mRuleMap["user_input"] = "<definition> | <statement>";

        mRuleMap["system_function"] = "( ( <parameter_function> '(' CONSTANT ')' ) ";
        mRuleMap["system_function"] += "| ( <none_parameter_function> '(' ')' ) ) ';'";

        mRuleMap["parameter_function"] = "'ListVariable' | 'ListFunction'";

        mRuleMap["none_parameter_function"] = " 'ListAllVariables' | 'ListAllFunctions' | 'Done' ";

        mRuleMap["iostream"] = "( ( 'cin' '>>' ( IDENTIFIER <rest_of_Identifier_started_basic_exp> ) ";
        mRuleMap["iostream"] += "{ '>>' ( IDENTIFIER <rest_of_Identifier_started_basic_exp> ) } ) | ";
        mRuleMap["iostream"] += "( 'cout' '<<' <basic_expression> { '<<' ";
        mRuleMap["iostream"] += "<basic_expression> } ) ) ';'";

        mRuleMap["definition"] = "( 'void' IDENTIFIER <function_definition_without_ID>) | ";
        mRuleMap["definition"] += "( <type_specifier> IDENTIFIER <function_definition_or_declarators> )";

        mRuleMap["type_specifier"] = " 'int' | 'char' | 'float' | 'string' | 'bool' ";

        mRuleMap["function_definition_or_declarators"] = "<function_definition_without_ID>";
        mRuleMap["function_definition_or_declarators"] += " | <rest_of_declarators>";

        mRuleMap["rest_of_declarators"] = "[ '[' CONSTANT ']' ] { ',' IDENTIFIER [ '[' CONSTANT ']' ] } ';'";

        mRuleMap["function_definition_without_ID"] = " '(' [ 'void' | <formal_parameter_list> ] ')'";
        mRuleMap["function_definition_without_ID"] += " <compound_statement>";

        mRuleMap["formal_parameter_list"] = "<type_specifier> [ '&' ] IDENTIFIER [ '[' CONSTANT ']' ]";
        mRuleMap["formal_parameter_list"] += " { ',' <type_specifier> [ '&' ] ";
        mRuleMap["formal_parameter_list"] += "IDENTIFIER [ '[' CONSTANT ']' ] }";

        mRuleMap["compound_statement"] = "'{' { <declaration> | <statement> } '}'";

        mRuleMap["declaration"] = "<type_specifier> IDENTIFIER <rest_of_declarators>";

        mRuleMap["statement"] = " ';' | ( <expression> ';' ) | ( 'return' [ <expression> ] ';' )";
        mRuleMap["statement"] += " | <compound_statement> | ( 'if' '(' <expression> ')' <statement>";
        mRuleMap["statement"] += " [ 'else' <statement> ] ) | ( 'while' '(' <expression> ')' ";
        mRuleMap["statement"] += "<statement> ) | ( 'do' <statement> 'while' '(' <expression> ')' ';' )";
        mRuleMap["statement"] += " | <system_function> | <iostream>";

        mRuleMap["expression"] = "<basic_expression> { ',' <basic_expression> }";

        mRuleMap["basic_expression"] = "( IDENTIFIER <rest_of_Identifier_started_basic_exp> )";
        mRuleMap["basic_expression"] += " | ( ( '++' | '--' ) IDENTIFIER ";
        mRuleMap["basic_expression"] += "<rest_of_PPMM_Identifier_started_basic_exp> )";
        mRuleMap["basic_expression"] += " | ( <sign> { <sign> } <signed_unary_exp> <romce_and_romloe> ) | ";
        mRuleMap["basic_expression"] += "( ( CONSTANT | ( '(' <expression> ')' ) ) <romce_and_romloe> )";

        mRuleMap["rest_of_Identifier_started_basic_exp"] = "( '(' [ <actual_parameter_list> ] ')' ";
        mRuleMap["rest_of_Identifier_started_basic_exp"] += "<romce_and_romloe> ) | ";
        mRuleMap["rest_of_Identifier_started_basic_exp"] += "( [ '[' <expression> ']' ] ( ( ";
        mRuleMap["rest_of_Identifier_started_basic_exp"] += "<assignment_operator> <basic_expression> ) |";
        mRuleMap["rest_of_Identifier_started_basic_exp"] += " ( [ '++' | '--' ] <romce_and_romloe> ) ) )";

        mRuleMap["rest_of_PPMM_Identifier_started_basic_exp"] = "[ '[' <expression> ']' ] <romce_and_romloe>";

        mRuleMap["sign"] = "'+' | '-' | '!'";

        mRuleMap["actual_parameter_list"] = "<basic_expression> { ',' <basic_expression> }";

        mRuleMap["assignment_operator"] = "'=' | '*=' | '/=' | '%=' | '+=' | '-='";

        mRuleMap["romce_and_romloe"] = "<rest_of_maybe_logical_OR_exp>";
        mRuleMap["romce_and_romloe"] += " [ '?' <basic_expression> ':' <basic_expression> ]";

        mRuleMap["rest_of_maybe_logical_OR_exp"] = "<rest_of_maybe_logical_AND_exp>";
        mRuleMap["rest_of_maybe_logical_OR_exp"] += " { '||' <maybe_logical_AND_exp> }";

        mRuleMap["maybe_logical_AND_exp"] = "<maybe_bit_OR_exp> { '&&' <maybe_bit_OR_exp> }";

        mRuleMap["rest_of_maybe_logical_AND_exp"] = "<rest_of_maybe_bit_OR_exp> { '&&' <maybe_bit_OR_exp> }";

        mRuleMap["maybe_bit_OR_exp"] = "<maybe_bit_ex_OR_exp> { '|' <maybe_bit_ex_OR_exp> }";

        mRuleMap["rest_of_maybe_bit_OR_exp"] = "<rest_of_maybe_bit_ex_OR_exp> { '|' <maybe_bit_ex_OR_exp> }";

        mRuleMap["maybe_bit_ex_OR_exp"] = "<maybe_bit_AND_exp> { '^' <maybe_bit_AND_exp> }";

        mRuleMap["rest_of_maybe_bit_ex_OR_exp"] = "<rest_of_maybe_bit_AND_exp> { '^' <maybe_bit_AND_exp> }";

        mRuleMap["maybe_bit_AND_exp"] = "<maybe_equality_exp> { '&' <maybe_equality_exp> }";

        mRuleMap["rest_of_maybe_bit_AND_exp"] = "<rest_of_maybe_equality_exp> { '&' <maybe_equality_exp> }";

        mRuleMap["maybe_equality_exp"] = "<maybe_relational_exp> { ( '==' | '!=' ) <maybe_relational_exp>}";

        mRuleMap["rest_of_maybe_equality_exp"] = "<rest_of_maybe_relational_exp>";
        mRuleMap["rest_of_maybe_equality_exp"] += " { ( '==' | '!=' ) <maybe_relational_exp> }";

        mRuleMap["maybe_relational_exp"] = "<maybe_shift_exp>";
        mRuleMap["maybe_relational_exp"] += " { ( '<' | '>' | '<=' | '>=' ) <maybe_shift_exp> }";

        mRuleMap["rest_of_maybe_relational_exp"] = "<rest_of_maybe_shift_exp>";
        mRuleMap["rest_of_maybe_relational_exp"] += " { ( '<' | '>' | '<=' | '>=' ) <maybe_shift_exp> }";

        mRuleMap["maybe_shift_exp"] = "<maybe_additive_exp> { ( '<<' | '>>' ) <maybe_additive_exp> }";

        mRuleMap["rest_of_maybe_shift_exp"] = "<rest_of_maybe_additive_exp>";
        mRuleMap["rest_of_maybe_shift_exp"] += " { ( '<<' | '>>' ) <maybe_additive_exp> }";

        mRuleMap["maybe_additive_exp"] = "<maybe_mult_exp> { ( '+' | '-' ) <maybe_mult_exp> }";

        mRuleMap["rest_of_maybe_additive_exp"] = "<rest_of_maybe_mult_exp>";
        mRuleMap["rest_of_maybe_additive_exp"] += " { ( '+' | '-' ) <maybe_mult_exp> }";

        mRuleMap["maybe_mult_exp"] = "<unary_exp> <rest_of_maybe_mult_exp>";

        mRuleMap["rest_of_maybe_mult_exp"] = "{ ( '*' | '/' | '%' ) <unary_exp> }";

        mRuleMap["unary_exp"] = "( <sign> { <sign> } <signed_unary_exp> ";
        mRuleMap["unary_exp"] += ") | <unsigned_unary_exp> | ( ( '++' | '--' ) IDENTIFIER ";
        mRuleMap["unary_exp"] += "[ '[' <expression> ']' ] )";

        mRuleMap["signed_unary_exp"] = "( IDENTIFIER [ ( '(' [ <actual_parameter_list> ] ')' )";
        mRuleMap["signed_unary_exp"] += " | ( '[' <expression> ']' ) ] ) | ";
        mRuleMap["signed_unary_exp"] += "CONSTANT | ( '(' <expression> ')' )";

        mRuleMap["unsigned_unary_exp"] = "( IDENTIFIER [ ( '(' [ <actual_parameter_list> ] ')' )";
        mRuleMap["unsigned_unary_exp"] += " | ( [ '[' <expression> ']' ] [ ( '++' | '--' ";
        mRuleMap["unsigned_unary_exp"] += ") ] ) ] ) | CONSTANT | ( '(' <expression> ')' )";

        mPair['{'] = '}';
        mPair['['] = ']';
        mPair['('] = ')';
        mPair['<'] = '>';
    }  // GrammarChecker()

    vector<string> mTokenTypeList;

    string GetCurrentTokenType() { return mTokenTypeList[mTokenTypeList.size() - 1]; }

    int Check(const vector<BasicToken> &statementBuffer, int currentLineNumber) {
        if (statementBuffer.size() == 0) {
            return -1;
        }

        vector<BasicToken> instructionQueue = statementBuffer;
        try {
            mTokenTypeList.clear();
            if (!Check(instructionQueue, "user_input", "None")) {
                stringstream ss;
                ss << "Line " << currentLineNumber << " : unexpected token : '" << mErrorToken << "'";
                throw logic_error(ss.str());
            }  // if
        }      // try
        catch (const Partially_Correct &e) {
            return -1;
        }  // catch

        return statementBuffer.size() - instructionQueue.size();
    }  // Check()
};     // class GrammarChecker

enum ProcessState { DEFINITION, EXPRESSION, IDLE };

class SimplestInstructionProcessor {
   private:
    vector<BasicToken> mSimplestInstruction;
    ProcessState mState;
    VariableDefinitionManager mVariableDefinitionManager;

    void CheckIdentifier(const BasicToken &token, int currentLine) {
        if (token.mType == IDENTIFIER) {
            if (!mVariableDefinitionManager.IsExist(token.mString)) {
                stringstream ss;
                ss << "Line " << currentLine << " : undefined identifier : '" << token.mString << "'";
                throw logic_error(ss.str());
            }
        }
    }

    void DefineIdentifier() {
        VariableType definitionType = ToVariableType(mSimplestInstruction[0].mString);
        for (int i = 0; i < mSimplestInstruction.size(); i++) {
            if (mSimplestInstruction[i].mType == IDENTIFIER) {
                if (mSimplestInstruction[i + 1].mString == "[") {
                    mVariableDefinitionManager.Define(mSimplestInstruction[i].mString, definitionType, ToInt(mSimplestInstruction[i].mString));
                } else {
                    mVariableDefinitionManager.Define(mSimplestInstruction[i].mString, definitionType, 0);
                }
            }
        }
    }

   public:
    SimplestInstructionProcessor() { mState = IDLE; }

    void RecoverDefinition() {
        mVariableDefinitionManager.BackToRoot();
    }

    void SetNextToken(const BasicToken &token, const string &currentTokenType, int currentLine) {
        if (token.mString == "{") {
            mVariableDefinitionManager.LevelUp();
        } else if (token.mString == "}") {
            mVariableDefinitionManager.LevelDown();
        }

        if (mState == IDLE) {
            if (currentTokenType == "basic_expression") {
                mSimplestInstruction.push_back(token);
                CheckIdentifier(token, currentLine);
                mState = EXPRESSION;
            } else if (currentTokenType == "type_specifier") {
                mSimplestInstruction.push_back(token);
                mState = DEFINITION;
            }
        } else if (mState == EXPRESSION) {
            mSimplestInstruction.push_back(token);
            CheckIdentifier(token, currentLine);
            if (currentTokenType == "statement" || currentTokenType == "expression") {
                mSimplestInstruction.clear();
                mState = IDLE;
            }
        } else {
            if (currentTokenType != "function_definition_without_ID") {
                mSimplestInstruction.push_back(token);
            }

            if (token.mString == ";" || currentTokenType == "function_definition_without_ID") {
                DefineIdentifier();
                mSimplestInstruction.clear();
                mState = IDLE;
            }
        }
    }
};

class InstructionAssembler {
   private:
    TokenGetter mTokenGetter;
    GrammarChecker mGrammarChecker;
    vector<BasicToken> mTokenBuffer;
    SimplestInstructionProcessor mSimplestInstructionProcessor;

    void CheckNextToken() {
        BasicToken nextToken = mTokenGetter.GetNextToken();
        if (nextToken.mType == ERROR) {
            stringstream ss;
            ss << "Line " << mTokenGetter.GetCurrentLineNumber()
               << " : unrecognized token with first char : '" << nextToken.mString[0] << "'";
            throw logic_error(ss.str());
        }
        mTokenBuffer.push_back(nextToken);
    }  // PopToken()

   public:
    vector<BasicToken> *GetSingleInstruction(vector<string> &tokenTypeList) {
        mTokenGetter.ResetLineCounter();
        bool completeInstruction = false;
        vector<BasicToken> *singleInstruction = new vector<BasicToken>();
        while (!completeInstruction) {
            try {
                int instructionLength =
                    mGrammarChecker.Check(mTokenBuffer, mTokenGetter.GetCurrentLineNumber());
                if (mTokenBuffer.size() > 0) {
                    mSimplestInstructionProcessor.SetNextToken(mTokenBuffer[mTokenBuffer.size() - 1],
                                                               mGrammarChecker.GetCurrentTokenType(),
                                                               mTokenGetter.GetCurrentLineNumber());
                }

                if (instructionLength >= 0) {  // 指令完整
                    singleInstruction->assign(mTokenBuffer.begin(), mTokenBuffer.begin() + instructionLength);
                    mTokenBuffer.erase(mTokenBuffer.begin(), mTokenBuffer.begin() + instructionLength);
                    completeInstruction = true;
                } else {  // 指令不完整
                    CheckNextToken();
                }
            }  // try
            catch (const logic_error &err) {
                mTokenGetter.ResetLineCounter();
                mTokenGetter.ClearUnusedTokens();
                mSimplestInstructionProcessor.RecoverDefinition();
                cout << err.what() << endl;
                cout << "> ";
                mTokenBuffer.clear();
                completeInstruction = false;
            }  // catch
        }      // while

        tokenTypeList = mGrammarChecker.mTokenTypeList;
        return singleInstruction;
    }
};  // class StatementProcessor

class Interpreter {
   private:
    VariableDefinitionManager mVariableDefinitionManager;
    FunctionDefinitionManager mFunctionDefinitionManager;

    void Definition(vector<BasicToken> *definition, const string &definitionType) {
        VariableType type = definition->at(0).mVariableType;
        if (definitionType == "<function_definition_without_ID>") {
            cout << "AAA" << endl;
        } else {
            for (int i = 1; i < definition->size(); i++) {
                if (definition->at(i - 1).mType == IDENTIFIER) {
                    if (definition->at(i).mString == "[") {
                        // mVariableDefinitionManager.Define(definition->at(i - 1).mString, type,
                        // ToInt(definition->at(i + 1).mString));
                    } else {
                        // mVariableDefinitionManager.Define(definition->at(i - 1).mString, type, 0);
                    }
                }
            }
        }
    }

    void Statement(vector<BasicToken> *statement) {
        if (statement->size() == 1) {
        }
        for (int i = 0; i < statement->size(); i++) {
        }
        cout << "Statement executed ..." << endl;
    }

    void Done() { throw QUIT(); }

    void Execute(vector<BasicToken> *singleInstruction, const vector<string> &tokenTypeList) {
        if (tokenTypeList[0] == "none_parameter_function") {
            if (singleInstruction->at(0).mString == "Done") {
                Done();
            }
        }
    }

   public:
    Interpreter() {
        InstructionAssembler instructionAssembler;
        vector<BasicToken> *singleInstruction = NULL;
        cout << "Our-C running ..." << endl;
        try {
            while (true) {
                cout << "> ";
                vector<string> tokenTypeList;
                singleInstruction = instructionAssembler.GetSingleInstruction(tokenTypeList);
                Execute(singleInstruction, tokenTypeList);
                delete singleInstruction;
                singleInstruction = NULL;
            }
        } catch (const QUIT &quit) {
            cout << "Our-C exited .." << endl;
            if (singleInstruction != NULL) {
                delete singleInstruction;
            }
        }
    }
};

int main() {
    string str;
    TokenGetter::GetLine(str);
    Interpreter();
}