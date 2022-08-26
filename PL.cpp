#include <stdio.h>
#include <string.h>

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

class Done {
};

class OK {
};

class Lexical_Error : public exception {
};

class Syntactical_Error : public exception {
};

class Semantic_Error : public exception {
};

class Token {
   public:
    string mType;    // Literal: reserved, identifier, constant, delimiter, 404(error)
    string mString;  // token string
    int mLine;       // where the token is located

    Token(const string& str, int currentLine) {
        mString = str;
        mLine = currentLine;
        mType = Type(str);
    }

   private:
    string Type(const string& str) {
        if (str == "int" || str == "float" || str == "char" || str == "bool" || str == "string" ||
            str == "void" || str == "if" || str == "else" || str == "while" || str == "do" ||
            str == "return") {
            return "reserved";
        }  // if
        else if (isDelimiter(str)) {
            return "delimiter";
        } else if (str == "true" || str == "false") {
            return "constant";
        }  // else if
        else if (IsDigit(str)) {
            return "constant";
        }  // else if
        else if (str.length() >= 2 && (str[0] == '\'' && str[str.length() - 1] == '\'' || str[0] == '\"' && str[str.length() - 1] == '\"')) {
            return "constant";
        } else if (IsLegalIdentifier(str)) {
            return "identifier";
        }

        return "404";
    }  // Type()

    bool
    IsLegalIdentifier(const string& str) {
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
    }  // IsLegalIdentifier()

    static bool isDelimiter(const string& str) {
        if (str == "(" || str == ")" || str == "[" || str == "]" || str == "{" || str == "}" || str == "+" || str == "-" || str == "*" || str == "/" ||
            str == "%" || str == "^" || str == ">" || str == "<" || str == ">=" || str == "<=" || str == "==" || str == "!=" || str == "&" || str == "|" ||
            str == "!" || str == "&&" || str == "||" || str == "+=" || str == "-=" || str == "*=" || str == "/=" || str == "%=" || str == "++" ||
            str == "--" || str == ">>" || str == "<<" || str == ";" || str == "," || str == "?" || str == ":" || str == "=") {
            return true;
        }

        return false;
    }

    bool IsDigit(const string& str) {
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

    friend class Reader;
};

class Reader {
   private:
    vector<Token> mBuffer;
    map<string, string> mDelimiterTable;
    int currentLine;

    /*separate line to tokens*/
    void Transform_to_tokens(const string& line) {
        string current = "", next = "", temp = "";
        bool out = false;
        for (int i = 0; i < line.length(); i++) {
            current = line[i];
            if (i + 1 < line.length())
                next = line[i + 1];
            else
                out = true;

            if ((current == "\n" || current == "\t" || current == " ")) {
                if (temp.length() > 0) {
                    Token token(temp, currentLine);
                    mBuffer.push_back(token);
                    temp.clear();
                }
            } else if (Token::isDelimiter(current)) {
                if (temp.length() != 0) {
                    Token token(temp, currentLine);
                    mBuffer.push_back(token);
                    temp.clear();
                }  // if

                if (i < line.length() - 1) {
                    if ((current + next) == "//")
                        i = line.length();
                    else if (Token::isDelimiter((current + next))) {
                        Token token((current + next), currentLine);
                        mBuffer.push_back(token);
                        i++;
                    }  // else if
                    else {
                        Token token(current, currentLine);
                        mBuffer.push_back(token);
                    }  // else
                    temp.clear();
                }  // if
                else {
                    Token token(current, currentLine);
                    mBuffer.push_back(token);
                    temp.clear();
                }  // else
            } else if (current == "\"") {
                bool slash = false;
                i = i + 1;
                while ((i < line.length() && line[i] != '\"') || slash == true) {
                    slash = false;
                    if (i >= line.length()) break;
                    current = current + line[i];
                    if (line[i] == '\\') slash = true;
                    i = i + 1;
                }  // while

                if (i >= line.length()) {
                    Token token(current, currentLine);
                    mBuffer.push_back(token);
                    temp.clear();
                }  // if
                else {
                    current = current + line[i];
                    Token token(current, currentLine);
                    mBuffer.push_back(token);
                    temp.clear();
                }  // else
            }      // else if
            else if (current == "'") {
                if (line[i + 2] == '\'') {
                    current = current + line[i + 1];
                    current = current + line[i + 2];
                    Token token(current, currentLine);
                    mBuffer.push_back(token);
                    temp.clear();
                }  // if
                else {
                    current = current + line[i + 1];
                    current = current + line[i + 2];
                    Token token(temp, currentLine);
                    mBuffer.push_back(token);
                    temp.clear();
                }  // else

                i = i + 2;
            }  // else if
            else {
                Token token((temp + current), currentLine);
                int k = 2;
                if (token.mType.find("404") == k && temp.length() > 0) {
                    mBuffer.push_back(token);
                    temp.clear();
                }
                temp = temp + current;
            }  // else
        }
        if (temp.length() > 0) {
            Token token(temp, currentLine);
            mBuffer.push_back(token);
            temp.clear();
        }  // if
    }

   public:
    Reader() {
        ResetAll();
    }

    static string GetLine() {
        string line;
        char ch = getchar();
        while (ch != '\n' && ch != EOF) {
            line.push_back(ch);
            ch = getchar();
        }

        return line;
    }

    Token GetNextToken() {
        while (mBuffer.size() == 0) {
            Transform_to_tokens(GetLine());
            currentLine++;
        }

        Token token = mBuffer[0];
        mBuffer.erase(mBuffer.begin());
        if (token.mType == "404") {
            cout << "Line " << currentLine << " : unrecognized token with first char : '" << token.mString[0] << "'";
            throw Lexical_Error();
        }
        return token;
    }

    void ResetAll() {
        currentLine = 1;
        mBuffer.clear();
    }

    void ResetLine() {
        currentLine = 1;
    }
};

class Recursive_descent_parser {
   private:
    map<string, string> mGrammarTable;
    map<char, char> mBrackets;
    int mCurrentIndex;

    bool Exchange(bool value) {
        if (value)
            return false;
        else
            return true;
    }

    int LevelChange(char temp, map<char, char>::iterator mBrackets_iterator, int level) {
        if (mBrackets_iterator->first == temp) {
            return level + 1;
        }  // if
        else if (mBrackets_iterator->second == temp) {
            return level - 1;
        }  // else if

        return level;
    }

    void addString(vector<string>& mQueue, string& temp) {
        mQueue.push_back(temp);
        temp.clear();
    }

    void Dismantling_grammar(const string& complicated_grammar, vector<string>& mQueue) {
        string buffer;
        map<char, char>::iterator mBrackets_iterator;
        int currentLevel = 0;
        bool pass = false;
        bool organize = false;
        for (int i = 0; i < complicated_grammar.length(); i++) {
            if (organize == false) {
                if (complicated_grammar[i] == '\'') {
                    pass = Exchange(pass);
                }  // if

                if (pass == false) {
                    mBrackets_iterator = mBrackets.find(complicated_grammar[i]);
                    if (mBrackets_iterator != mBrackets.end()) {
                        organize = true;
                        currentLevel++;
                    }  // if
                }      // if
            }          // if
            else {
                currentLevel = LevelChange(complicated_grammar[i], mBrackets_iterator, currentLevel);
                if (currentLevel == 0) {
                    organize = false;
                }  // if
            }      // else

            if (currentLevel == 0 && complicated_grammar[i] == ' ') {
                if (buffer.length() > 0)
                    addString(mQueue, buffer);
            }  // if
            else {
                buffer.push_back(complicated_grammar[i]);
            }  // else
        }      // for

        if (buffer.length() > 0)
            addString(mQueue, buffer);
    }

   public:
    Recursive_descent_parser() {
        mBrackets['{'] = '}';
        mBrackets['['] = ']';
        mBrackets['('] = ')';

        mGrammarTable["user_input"] = "definition ";
        mGrammarTable["user_input"] += ", statement";
        mGrammarTable["definition"] = "( 'void' identifier function_definition_without_ID) , ";
        mGrammarTable["definition"] += "( type_specifier identifier function_definition_or_declarators )";
        mGrammarTable["type_specifier"] = " 'int' , 'char' ";
        mGrammarTable["type_specifier"] += ", 'float' , 'string' , 'bool' ";
        mGrammarTable["function_definition_or_declarators"] = "function_definition_without_ID";
        mGrammarTable["function_definition_or_declarators"] += " , rest_of_declarators";
        mGrammarTable["rest_of_declarators"] = "[ '[' constant ']' ] { ',' identifier [ '[' constant ']' ] } ';'";
        mGrammarTable["function_definition_without_ID"] = " '(' [ 'void' , formal_parameter_list ] ')'";
        mGrammarTable["function_definition_without_ID"] += " compound_statement";
        mGrammarTable["formal_parameter_list"] = "type_specifier [ '&' ] identifier [ '[' constant ']' ]";
        mGrammarTable["formal_parameter_list"] += " { ',' type_specifier [ '&' ]";
        mGrammarTable["formal_parameter_list"] += " identifier [ '[' constant ']' ] }";
        mGrammarTable["compound_statement"] = "'{' { declaration , statement } '}'";
        mGrammarTable["declaration"] = "type_specifier identifier rest_of_declarators";
        mGrammarTable["statement"] = " ';' , ( expression ';' ) , ( 'return' [ expression ] ';' ) ,";
        mGrammarTable["statement"] += " compound_statement , ( 'if' '(' expression ')' statement";
        mGrammarTable["statement"] += " [ 'else' statement ] ) , ( 'while' '(' expression ')' ";
        mGrammarTable["statement"] += "statement ) , ( 'do' statement 'while' '(' expression ')' ';' )";
        mGrammarTable["expression"] = "basic_expression { ',' basic_expression }";
        mGrammarTable["basic_expression"] = "( identifier rest_of_Identifier_started_basic_exp )";
        mGrammarTable["basic_expression"] += " , ( ( '++' , '--' ) identifier ";
        mGrammarTable["basic_expression"] += "rest_of_PPMM_Identifier_started_basic_exp )";
        mGrammarTable["basic_expression"] += " , ( sign { sign } signed_unary_exp romce_and_romloe ) , ";
        mGrammarTable["basic_expression"] += "( ( constant , ( '(' expression ')' ) ) romce_and_romloe )";
        mGrammarTable["rest_of_Identifier_started_basic_exp"] = "( '(' [ actual_parameter_list ] ')' ";
        mGrammarTable["rest_of_Identifier_started_basic_exp"] += "romce_and_romloe ) , ( [ '[' expression ']' ] (";
        mGrammarTable["rest_of_Identifier_started_basic_exp"] += " ( assignment_operator basic_expression ) , ( ";
        mGrammarTable["rest_of_Identifier_started_basic_exp"] += "[ '++' , '--' ] romce_and_romloe ) ) )";
        mGrammarTable["rest_of_PPMM_Identifier_started_basic_exp"] = "[ '[' expression ']' ] romce_and_romloe";
        mGrammarTable["sign"] = "'+' , '-' , '!'";
        mGrammarTable["actual_parameter_list"] = "basic_expression { ',' basic_expression }";
        mGrammarTable["assignment_operator"] = "'=' , '*=' , '/=' , '%=' , '+=' , '-='";
        mGrammarTable["romce_and_romloe"] = "rest_of_maybe_logical_OR_exp";
        mGrammarTable["romce_and_romloe"] += " [ '?' basic_expression";
        mGrammarTable["romce_and_romloe"] += " ':' basic_expression ]";
        mGrammarTable["rest_of_maybe_logical_OR_exp"] = "rest_of_maybe_logical_AND_exp ";
        mGrammarTable["rest_of_maybe_logical_OR_exp"] += "{ '||' maybe_logical_AND_exp }";
        mGrammarTable["maybe_logical_AND_exp"] = "maybe_bit_OR_exp { '&&' maybe_bit_OR_exp }";
        mGrammarTable["rest_of_maybe_logical_AND_exp"] = "rest_of_maybe_bit_OR_exp { '&&' maybe_bit_OR_exp }";
        mGrammarTable["maybe_bit_OR_exp"] = "maybe_bit_ex_OR_exp { ',' maybe_bit_ex_OR_exp }";
        mGrammarTable["rest_of_maybe_bit_OR_exp"] = "rest_of_maybe_bit_ex_OR_exp { ',' maybe_bit_ex_OR_exp }";
        mGrammarTable["maybe_bit_ex_OR_exp"] = "maybe_bit_AND_exp { '^' maybe_bit_AND_exp }";
        mGrammarTable["rest_of_maybe_bit_ex_OR_exp"] = "rest_of_maybe_bit_AND_exp { '^' maybe_bit_AND_exp }";
        mGrammarTable["maybe_bit_AND_exp"] = "maybe_equality_exp { '&' maybe_equality_exp }";
        mGrammarTable["rest_of_maybe_bit_AND_exp"] = "rest_of_maybe_equality_exp { '&' maybe_equality_exp }";
        mGrammarTable["maybe_equality_exp"] = "maybe_relational_exp { ( '==' , '!=' ) maybe_relational_exp}";
        mGrammarTable["rest_of_maybe_equality_exp"] = "rest_of_maybe_relational_exp";
        mGrammarTable["rest_of_maybe_equality_exp"] += " { ( '==' , '!=' ) maybe_relational_exp }";
        mGrammarTable["maybe_relational_exp"] = "maybe_shift_exp";
        mGrammarTable["maybe_relational_exp"] += " { ( '<' , '>' , '<=' , '>=' ) maybe_shift_exp }";
        mGrammarTable["rest_of_maybe_relational_exp"] = "rest_of_maybe_shift_exp";
        mGrammarTable["rest_of_maybe_relational_exp"] += " { ( '<' , '>' , '<=' , '>=' ) maybe_shift_exp }";
        mGrammarTable["maybe_shift_exp"] = "maybe_additive_exp { ( '<<' , '>>' ) maybe_additive_exp }";
        mGrammarTable["rest_of_maybe_shift_exp"] = "rest_of_maybe_additive_exp";
        mGrammarTable["rest_of_maybe_shift_exp"] += " { ( '<<' , '>>' ) maybe_additive_exp }";
        mGrammarTable["maybe_additive_exp"] = "maybe_mult_exp { ( '+' , '-' ) maybe_mult_exp }";
        mGrammarTable["rest_of_maybe_additive_exp"] = "rest_of_maybe_mult_exp";
        mGrammarTable["rest_of_maybe_additive_exp"] += " { ( '+' , '-' ) maybe_mult_exp }";
        mGrammarTable["maybe_mult_exp"] = "unary_exp rest_of_maybe_mult_exp";
        mGrammarTable["rest_of_maybe_mult_exp"] = "{ ( '*' , '/' , '%' ) unary_exp }";
        mGrammarTable["unary_exp"] = "( sign { sign } signed_unary_exp ";
        mGrammarTable["unary_exp"] += ") , unsigned_unary_exp , ( ( '++' , '--' ) identifier ";
        mGrammarTable["unary_exp"] += "[ '[' expression ']' ] )";
        mGrammarTable["signed_unary_exp"] = "( identifier [ ( '(' [ actual_parameter_list ] ')' ) ,";
        mGrammarTable["signed_unary_exp"] += " ( '[' expression ']' ) ] ) , ";
        mGrammarTable["signed_unary_exp"] += "constant , ( '(' expression ')' )";
        mGrammarTable["unsigned_unary_exp"] = "( identifier [ ( '(' [ actual_parameter_list ] ')' )";
        mGrammarTable["unsigned_unary_exp"] += " , ( [ '[' expression ']' ] [ ( '++' , '--' ";
        mGrammarTable["unsigned_unary_exp"] += ") ] ) ] ) , constant , ( '(' expression ')' )";
    }

    bool Is_completed_expression(const vector<Token>& tokenList) {
        mCurrentIndex = 0;
        try {
            if (!Is_mach_rule(tokenList, "user_input")) {
                cout << "Line " << tokenList[mCurrentIndex].mLine << " : unexpected token : '" << tokenList[mCurrentIndex].mString << "'" << endl;
                throw Syntactical_Error();
            }
        } catch (const OK& ok) {
            return false;
        }

        return true;
    }

   private:
    bool Is_mach_rule(const vector<Token>& tokenList, const string& rule) {
        vector<string> dismantled_grammar_list;
        map<string, string>::iterator iterator = mGrammarTable.find(rule);
        if (iterator == mGrammarTable.end()) {
            Dismantling_grammar(rule, dismantled_grammar_list);
        } else {
            Dismantling_grammar(iterator->second, dismantled_grammar_list);
        }

        string rule_attribute = "sequence";
        for (int i = 0; i < dismantled_grammar_list.size(); i++) {
            if (dismantled_grammar_list[i] == ",") {
                rule_attribute = "or";
                dismantled_grammar_list.erase(dismantled_grammar_list.begin() + i);
                i--;
            }
        }

        if (rule_attribute == "sequence") {
            for (int i = 0; i < dismantled_grammar_list.size(); i++) {
                if (!Is_mach_single_rule(tokenList, dismantled_grammar_list[i])) {
                    return false;
                }
            }

            return true;
        } else {
            for (int i = 0; i < dismantled_grammar_list.size(); i++) {
                int preIndex = mCurrentIndex;
                if (Is_mach_single_rule(tokenList, dismantled_grammar_list[i])) {
                    return true;
                } else {
                    if (preIndex != mCurrentIndex) {
                        return false;
                    }
                }
            }

            return false;
        }
    }

    bool Is_mach_single_rule(const vector<Token>& tokenList, const string& single_rule) {
        int situation = 0;
        if (mBrackets.find(single_rule[0]) != mBrackets.end()) {
            situation = 1;
        } else if (mGrammarTable.find(single_rule) != mGrammarTable.end()) {
            situation = 2;
        } else {
            situation = 3;
        }

        if (situation == 1 || situation == 2) {
            int preIndex = mCurrentIndex;
            string preprocessed_rule = (situation == 1) ? single_rule.substr(1, single_rule.length() - 2) : single_rule;
            bool isMachRule = Is_mach_rule(tokenList, preprocessed_rule);
            char first_char = single_rule[0];
            if (first_char == '{') {
                while (isMachRule) {
                    preIndex = mCurrentIndex;
                    isMachRule = Is_mach_rule(tokenList, preprocessed_rule);
                }

                if (preIndex == mCurrentIndex) {
                    return true;
                } else {
                    return false;
                }
            } else if (first_char == '[') {
                if (isMachRule) {
                    return true;
                } else {
                    return preIndex == mCurrentIndex;
                }
            } else {
                return isMachRule;
            }
        } else {
            if (mCurrentIndex == tokenList.size()) {
                throw OK();
            } else {
                if (Is_fit_type(tokenList[mCurrentIndex], single_rule)) {
                    mCurrentIndex++;
                    return true;
                }

                return false;
            }
        }
    }

    bool Is_fit_type(const Token& token, const string& type) {
        if (type == "identifier" || type == "constant") {
            return token.mType == type;
        }

        string literal_string = "'" + token.mString + "'";
        return literal_string == type;
    }
};

class Variable_definition {
   public:
    string mName;
    string mType;
    int mArray_size;
    int mLifetime;

    Variable_definition(const Token& variable, int array_size, int currentLevel) {
        mName = variable.mString;
        mType = variable.mType;
        mArray_size = array_size;
        mLifetime = currentLevel;
    }

    Variable_definition(const string& global_variable) {
        mName = global_variable;
        mType = "identifier";
        mArray_size = 0;
        mLifetime = 0;
    }
};

class Function_definition {
   public:
    string mName;
    vector<Token> mBody;

    Function_definition(const vector<Token>& function_definition) {
        if (function_definition.size() >= 2) {
            mName = function_definition[1].mString;
            for (int i = 0; i < function_definition.size(); i++) {
                mBody.push_back(function_definition[i]);
            }
        }
    }

    Function_definition(const string& global_function) {
        mName = global_function;
    }
};

class Definiton {
   private:
    vector<Variable_definition> mVariable_definitions;
    vector<Variable_definition> mPseudo_definitions;

    vector<Function_definition> mFunction_definitions;

    bool Is_defined(const vector<Variable_definition>& definitions, const Token& variable, int currentLevel) {
        for (int i = 0; i < definitions.size(); i++) {
            if (definitions[i].mName == variable.mString && definitions[i].mLifetime <= currentLevel) {
                return true;
            }
        }

        return false;
    }

   public:
    Definiton() {
        Variable_definition id_cin("cin");
        Variable_definition id_cout("cout");
        mVariable_definitions.push_back(id_cin);
        mVariable_definitions.push_back(id_cout);

        Function_definition global_function_1("ListAllVariables");
        Function_definition global_function_2("ListAllFunctions");
        Function_definition global_function_3("ListVariable");
        Function_definition global_function_4("ListFunction");
        Function_definition global_function_5("Done");
        mFunction_definitions.push_back(global_function_1);
        mFunction_definitions.push_back(global_function_2);
        mFunction_definitions.push_back(global_function_3);
        mFunction_definitions.push_back(global_function_4);
        mFunction_definitions.push_back(global_function_5);
    }

    void Define_variable(const Token& variable, int array_size, int currentLevel) {
        if (!Is_variable_defined(variable, currentLevel)) {
            cout << "Definition of " << variable.mString << " entered ..." << endl;
        } else {
            cout << "New definition of " << variable.mString << " entered ..." << endl;
            for (int i = 0; i < mVariable_definitions.size(); i++) {
                if (mVariable_definitions[i].mName == variable.mString && mVariable_definitions[i].mLifetime == currentLevel) {
                    mVariable_definitions.erase(mVariable_definitions.begin() + i);
                }
            }
        }

        Variable_definition definition(variable, array_size, currentLevel);
        mVariable_definitions.push_back(definition);
    }

    void Pseudo_define_variable(const Token& variable, int array_size, int currentLevel) {
        Variable_definition definition(variable, array_size, currentLevel);
        mPseudo_definitions.push_back(definition);
    }

    bool Is_variable_defined(const Token& variable, int currentLevel) {
        if (Is_defined(mVariable_definitions, variable, currentLevel)) {
            return true;
        } else if (Is_defined(mPseudo_definitions, variable, currentLevel)) {
            return true;
        }

        return false;
    }

    void Define_function(const vector<Token>& expression) {
        Function_definition definition(expression);
        if (!Is_function_defined(definition.mName)) {
            cout << "Definition of " << definition.mName << "() entered ..." << endl;
        } else {
            cout << "New definition of " << definition.mName << "() entered ..." << endl;
            for (int i = 0; i < mFunction_definitions.size(); i++) {
                if (mFunction_definitions[i].mName == definition.mName ) {
                    mFunction_definitions.erase(mFunction_definitions.begin() + i);
                }
            }
        }

        mFunction_definitions.push_back(definition);
    }

    bool Is_function_defined(const string& function_name) {
        for (int i = 0; i < mFunction_definitions.size(); i++) {
            if (mFunction_definitions[i].mName == function_name) {
                return true;
            }
        }

        return false;
    }

    friend class Identifier_definition_checker;
};

class Identifier_definition_checker {
    Definiton mDefinition_table;

    string Get_expression_type(const vector<Token>& expression) {
        if (expression[0].mString == "int" || expression[0].mString == "char" || expression[0].mString == "float" || expression[0].mString == "string" || expression[0].mString == "bool" || expression[0].mString == "void") {
            if (expression[0].mString == "void") {
                return "function_definition";
            }

            if (expression.size() >= 3) {
                if (expression[2].mString == "(") {
                    return "function_definition";
                }

                return "definition";
            }

            return "definition";
        } else {
            return "statement";
        }
    }

    int Count_brackets(const vector<Token>& expression, int& start, int& end) {
        int balanced = 0;
        bool first = true;
        start = 0;
        end = expression.size();
        for (int i = 0; i < expression.size(); i++) {
            if (expression[i].mString == "{") {
                balanced++;
                if (first) {
                    start = i + 1;
                    first = false;
                }
            } else if (expression[i].mString == "}") {
                balanced--;
                end = i;
            }
        }

        return balanced;
    }

    void Crop_function_definition(const vector<Token>& expression) {
        vector<Token> sub_expression;
        vector<Token> param_list;
        bool start = false, param = false;
        for (int i = 0; i < expression.size(); i++) {
            if (expression[i].mString == "{") {
                start = true;
            } else if (expression[i].mString == "(") {
                param = true;
            } else if (expression[i].mString == ")") {
                param = false;
            }

            if (start) {
                sub_expression.push_back(expression[i]);
            }

            if (param) {
                param_list.push_back(expression[i]);
            }
        }

        Pseudo_define_variable(param_list, 1);
        Separate_expression(sub_expression, 0);
    }

    void Pseudo_execute(const vector<Token>& expression, int currentLevel) {
        string expression_type = Get_expression_type(expression);
        if (Is_simplest(expression, expression_type)) {
            if (expression_type == "statement") {
                Check_variable_definitions(expression, currentLevel);
            } else {
                Pseudo_define_variable(expression, currentLevel);
            }
        } else {
            if (expression_type == "statement") {
                vector<Token> sub_expression;
                int start, end;
                int balanced = Count_brackets(expression, start, end);
                if (balanced > 0) {
                    for (int i = start; i < end; i++) {
                        sub_expression.push_back(expression[i]);
                    }

                    Separate_expression(sub_expression, currentLevel);
                }
            }
        }
    }

    bool Is_simplest(const vector<Token>& expression, const string& type) {
        if (type == "definition") {
            return true;
        }

        if (type == "function_definition") {
            return false;
        }

        if (expression[0].mString == "{") {
            return false;
        } else {
            return true;
        }
    }

    void Separate_expression(const vector<Token>& expression, int currentLevel) {
        vector<Token> buffer;
        int levelCounter = 0;
        for (int i = 0; i < expression.size(); i++) {
            buffer.push_back(expression[i]);
            if (expression[i].mString == "{") {
                levelCounter++;
            } else if (expression[i].mString == "}") {
                levelCounter--;
            } else if (expression[i].mString == ";" && levelCounter == 0) {
                if (buffer.size() > 0) {
                    Pseudo_execute(buffer, currentLevel + levelCounter);
                    buffer.clear();
                }
            }
        }

        if (buffer.size() > 0) {
            Pseudo_execute(buffer, currentLevel + levelCounter);
        }
    }

    void Define_variable(const vector<Token>& expression) {
        for (int i = 0; i < expression.size(); i++) {
            if (expression[i].mType == "identifier") {
                mDefinition_table.Define_variable(expression[i], 0, 0);
            }
        }
    }

    void Pseudo_define_variable(const vector<Token>& expression, int currentLevel) {
        for (int i = 0; i < expression.size(); i++) {
            if (expression[i].mType == "identifier") {
                mDefinition_table.Pseudo_define_variable(expression[i], 0, currentLevel);
            }
        }
    }

    void Check_variable_definitions(const vector<Token>& expression, int currentLevel) {
        string next_token_string = "";
        for (int i = 0; i < expression.size(); i++) {
            if (i + 1 < expression.size()) {
                next_token_string = expression[i + 1].mString;
            } else {
                next_token_string = "";
            }

            if (expression[i].mType == "identifier") {
                bool is_variable = mDefinition_table.Is_variable_defined(expression[i], currentLevel);
                bool is_function = mDefinition_table.Is_function_defined(expression[i].mString);
                bool is_id_defined = false;
                if (next_token_string == "(") {
                    is_id_defined = is_function;
                } else if (next_token_string == "") {
                    is_id_defined = is_function | is_variable;
                } else {
                    is_id_defined = is_variable;
                }

                if (!is_id_defined) {
                    cout << "Line " << expression[i].mLine << " : undefined identifier : '" << expression[i].mString << "'" << endl;
                    throw Semantic_Error();
                }
            }
        }
    }

    void Define_function(const vector<Token>& expression) {
        mDefinition_table.Define_function(expression);
    }

   public:
    void Define_and_confirm(const vector<Token>& expression) {
        try {
            string expression_type = Get_expression_type(expression);
            if (expression_type == "function_definition") {
                Crop_function_definition(expression);
            } else {
                Separate_expression(expression, 0);
            }

            mDefinition_table.mPseudo_definitions.clear();
        } catch (const exception& e) {
            mDefinition_table.mPseudo_definitions.clear();
            throw;
        }
    }

    void Execute_function(const vector<Token>& expression) {
        if (expression[0].mString == "ListAllVariables") {
        } else if (expression[0].mString == "ListAllFunctions") {
        } else if (expression[0].mString == "ListVariable") {
        } else if (expression[0].mString == "ListFunction") {
        } else if (expression[0].mString == "Done") {
            throw Done();
        }
    }

    void Execute(const vector<Token>& expression) {
        string type = Get_expression_type(expression);
        if (type == "statement") {
            Execute_function(expression);
            cout << "Statement executed ..." << endl;
        } else if (type == "definition") {
            Define_variable(expression);
        } else {
            Define_function(expression);
        }
    }
};

class Expression_executor {
   private:
    Reader reader;
    Recursive_descent_parser recursive_descent_parser;
    Identifier_definition_checker identifier_definition_checker;

   public:
    void ExecuteNextExpression() {
        vector<Token> expression;
        bool complete = false;
        while (!complete) {
            try {
                expression.push_back(reader.GetNextToken());
                complete = recursive_descent_parser.Is_completed_expression(expression);
                identifier_definition_checker.Define_and_confirm(expression);
            } catch (const exception& e) {
                reader.ResetAll();
                expression.clear();
                cout << "> ";
                complete = false;
            }
        }

        identifier_definition_checker.Execute(expression);
        reader.ResetLine();
    }
};

int main() {
    // fstream file("output.txt", ios::out);
    // cout.rdbuf(file.rdbuf());
    Expression_executor expression_collector;
    cout << "Our-C running ..." << endl;
    try {
        while (true) {
            cout << "> ";
            expression_collector.ExecuteNextExpression();
        }  // while
    }      // try
    catch (const Done& e) {
        cout << "Our-C exited ..." << endl;
    }  // catch
}