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
        Reset();
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
        }

        Token token = mBuffer[0];
        mBuffer.erase(mBuffer.begin());
        if (token.mType == "404") {
            cout << "Line " << currentLine << " : unrecognized token with first char : '" << token.mString[0] << "'";
            throw Lexical_Error();
        }
        return token;
    }

    void Reset() {
        currentLine = 1;
        mBuffer.clear();
    }
};

class Recursive_descent_parser {
   private:
    map<string, string> mGrammarTable;
    map<char, char> mBrackets;
    int mCurrentIndex;

    /*將文法規則拆解一層*/
    void Dismantling_grammar(const string& complicated_grammar, vector<string>& mQueue) {
        string buffer;
        map<char, char>::iterator mBrackets_iterator;
        int currentLevel = 0;
        bool isGroup = false;
        bool groupLock = false;
        for (int i = 0; i < complicated_grammar.length(); i++) {
            if (!isGroup) {
                if (complicated_grammar[i] == '\'') {
                    groupLock = !groupLock;
                }  // if

                if (!groupLock) {
                    mBrackets_iterator = mBrackets.find(complicated_grammar[i]);
                    if (mBrackets_iterator != mBrackets.end()) {
                        isGroup = true;
                        currentLevel++;
                    }  // if
                }      // if
            }          // if
            else {
                if (complicated_grammar[i] == mBrackets_iterator->first) {
                    currentLevel++;
                }  // if
                else if (complicated_grammar[i] == mBrackets_iterator->second) {
                    currentLevel--;
                }  // else if

                if (currentLevel == 0) {
                    isGroup = false;
                }  // if
            }      // else

            if (complicated_grammar[i] == ' ' && currentLevel == 0) {
                if (buffer.length() > 0) {
                    mQueue.push_back(buffer);
                    buffer.clear();
                }  // if
            }      // if
            else {
                buffer.push_back(complicated_grammar[i]);
            }  // else
        }      // for

        if (buffer.length() > 0) {
            mQueue.push_back(buffer);
        }  // if
    }

   public:
    Recursive_descent_parser() {
        mBrackets['{'] = '}';
        mBrackets['['] = ']';
        mBrackets['('] = ')';

        mGrammarTable["user_input"] = "definition | statement";
        mGrammarTable["definition"] = "( 'void' identifier function_definition_without_ID) | ";
        mGrammarTable["definition"] += "( type_specifier identifier function_definition_or_declarators )";
        mGrammarTable["type_specifier"] = " 'int' | 'char' | 'float' | 'string' | 'bool' ";
        mGrammarTable["function_definition_or_declarators"] = "function_definition_without_ID";
        mGrammarTable["function_definition_or_declarators"] += " | rest_of_declarators";
        mGrammarTable["rest_of_declarators"] = "[ '[' constant ']' ] { ',' identifier [ '[' constant ']' ] } ';'";
        mGrammarTable["function_definition_without_ID"] = " '(' [ 'void' | formal_parameter_list ] ')'";
        mGrammarTable["function_definition_without_ID"] += " compound_statement";
        mGrammarTable["formal_parameter_list"] = "type_specifier [ '&' ] identifier [ '[' constant ']' ]";
        mGrammarTable["formal_parameter_list"] += " { ',' type_specifier [ '&' ] ";
        mGrammarTable["formal_parameter_list"] += "identifier [ '[' constant ']' ] }";
        mGrammarTable["compound_statement"] = "'{' { declaration | statement } '}'";
        mGrammarTable["declaration"] = "type_specifier identifier rest_of_declarators";
        mGrammarTable["statement"] = " ';' | ( expression ';' ) | ( 'return' [ expression ] ';' )";
        mGrammarTable["statement"] += " | compound_statement | ( 'if' '(' expression ')' statement";
        mGrammarTable["statement"] += " [ 'else' statement ] ) | ( 'while' '(' expression ')' ";
        mGrammarTable["statement"] += "statement ) | ( 'do' statement 'while' '(' expression ')' ';' )";
        mGrammarTable["expression"] = "basic_expression { ',' basic_expression }";
        mGrammarTable["basic_expression"] = "( identifier rest_of_Identifier_started_basic_exp )";
        mGrammarTable["basic_expression"] += " | ( ( '++' | '--' ) identifier ";
        mGrammarTable["basic_expression"] += "rest_of_PPMM_Identifier_started_basic_exp )";
        mGrammarTable["basic_expression"] += " | ( sign { sign } signed_unary_exp romce_and_romloe ) | ";
        mGrammarTable["basic_expression"] += "( ( constant | ( '(' expression ')' ) ) romce_and_romloe )";
        mGrammarTable["rest_of_Identifier_started_basic_exp"] = "( '(' [ actual_parameter_list ] ')' ";
        mGrammarTable["rest_of_Identifier_started_basic_exp"] += "romce_and_romloe ) | ";
        mGrammarTable["rest_of_Identifier_started_basic_exp"] += "( [ '[' expression ']' ] ( ( ";
        mGrammarTable["rest_of_Identifier_started_basic_exp"] += "assignment_operator basic_expression ) |";
        mGrammarTable["rest_of_Identifier_started_basic_exp"] += " ( [ '++' | '--' ] romce_and_romloe ) ) )";
        mGrammarTable["rest_of_PPMM_Identifier_started_basic_exp"] = "[ '[' expression ']' ] romce_and_romloe";
        mGrammarTable["sign"] = "'+' | '-' | '!'";
        mGrammarTable["actual_parameter_list"] = "basic_expression { ',' basic_expression }";
        mGrammarTable["assignment_operator"] = "'=' | '*=' | '/=' | '%=' | '+=' | '-='";
        mGrammarTable["romce_and_romloe"] = "rest_of_maybe_logical_OR_exp";
        mGrammarTable["romce_and_romloe"] += " [ '?' basic_expression ':' basic_expression ]";
        mGrammarTable["rest_of_maybe_logical_OR_exp"] = "rest_of_maybe_logical_AND_exp";
        mGrammarTable["rest_of_maybe_logical_OR_exp"] += " { '||' maybe_logical_AND_exp }";
        mGrammarTable["maybe_logical_AND_exp"] = "maybe_bit_OR_exp { '&&' maybe_bit_OR_exp }";
        mGrammarTable["rest_of_maybe_logical_AND_exp"] = "rest_of_maybe_bit_OR_exp { '&&' maybe_bit_OR_exp }";
        mGrammarTable["maybe_bit_OR_exp"] = "maybe_bit_ex_OR_exp { '|' maybe_bit_ex_OR_exp }";
        mGrammarTable["rest_of_maybe_bit_OR_exp"] = "rest_of_maybe_bit_ex_OR_exp { '|' maybe_bit_ex_OR_exp }";
        mGrammarTable["maybe_bit_ex_OR_exp"] = "maybe_bit_AND_exp { '^' maybe_bit_AND_exp }";
        mGrammarTable["rest_of_maybe_bit_ex_OR_exp"] = "rest_of_maybe_bit_AND_exp { '^' maybe_bit_AND_exp }";
        mGrammarTable["maybe_bit_AND_exp"] = "maybe_equality_exp { '&' maybe_equality_exp }";
        mGrammarTable["rest_of_maybe_bit_AND_exp"] = "rest_of_maybe_equality_exp { '&' maybe_equality_exp }";
        mGrammarTable["maybe_equality_exp"] = "maybe_relational_exp { ( '==' | '!=' ) maybe_relational_exp}";
        mGrammarTable["rest_of_maybe_equality_exp"] = "rest_of_maybe_relational_exp";
        mGrammarTable["rest_of_maybe_equality_exp"] += " { ( '==' | '!=' ) maybe_relational_exp }";
        mGrammarTable["maybe_relational_exp"] = "maybe_shift_exp";
        mGrammarTable["maybe_relational_exp"] += " { ( '<' | '>' | '<=' | '>=' ) maybe_shift_exp }";
        mGrammarTable["rest_of_maybe_relational_exp"] = "rest_of_maybe_shift_exp";
        mGrammarTable["rest_of_maybe_relational_exp"] += " { ( '<' | '>' | '<=' | '>=' ) maybe_shift_exp }";
        mGrammarTable["maybe_shift_exp"] = "maybe_additive_exp { ( '<<' | '>>' ) maybe_additive_exp }";
        mGrammarTable["rest_of_maybe_shift_exp"] = "rest_of_maybe_additive_exp";
        mGrammarTable["rest_of_maybe_shift_exp"] += " { ( '<<' | '>>' ) maybe_additive_exp }";
        mGrammarTable["maybe_additive_exp"] = "maybe_mult_exp { ( '+' | '-' ) maybe_mult_exp }";
        mGrammarTable["rest_of_maybe_additive_exp"] = "rest_of_maybe_mult_exp";
        mGrammarTable["rest_of_maybe_additive_exp"] += " { ( '+' | '-' ) maybe_mult_exp }";
        mGrammarTable["maybe_mult_exp"] = "unary_exp rest_of_maybe_mult_exp";
        mGrammarTable["rest_of_maybe_mult_exp"] = "{ ( '*' | '/' | '%' ) unary_exp }";
        mGrammarTable["unary_exp"] = "( sign { sign } signed_unary_exp ";
        mGrammarTable["unary_exp"] += ") | unsigned_unary_exp | ( ( '++' | '--' ) identifier ";
        mGrammarTable["unary_exp"] += "[ '[' expression ']' ] )";
        mGrammarTable["signed_unary_exp"] = "( identifier [ ( '(' [ actual_parameter_list ] ')' )";
        mGrammarTable["signed_unary_exp"] += " | ( '[' expression ']' ) ] ) | ";
        mGrammarTable["signed_unary_exp"] += "constant | ( '(' expression ')' )";
        mGrammarTable["unsigned_unary_exp"] = "( identifier [ ( '(' [ actual_parameter_list ] ')' )";
        mGrammarTable["unsigned_unary_exp"] += " | ( [ '[' expression ']' ] [ ( '++' | '--' ";
        mGrammarTable["unsigned_unary_exp"] += ") ] ) ] ) | constant | ( '(' expression ')' )";
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
            if (dismantled_grammar_list[i] == "|") {
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

class InstructionCollector {
   private:
    Reader reader;
    Recursive_descent_parser recursive_descent_parser;

   public:
    void GetNextInstruction(vector<Token>& expression) {
        bool complete = false;
        while (!complete) {
            try {
                expression.push_back(reader.GetNextToken());
                complete = recursive_descent_parser.Is_completed_expression(expression);
            } catch (const exception& e) {
                reader.Reset();
                expression.clear();
            }
        }
    }
};

class Interpreter {
   private:
    InstructionCollector instructionCollector;

   public:
    Interpreter() {
        cout << "Our-C running ..." << endl;
        try {
            while (true) {
                cout << "> ";
                vector<Token> expression;
                instructionCollector.GetNextInstruction(expression);
                for (int i = 0; i < expression.size(); i++) {
                    cout << expression[i].mString << " ";
                }

                cout << endl;
            }  // while
        }      // try
        catch (const Done& e) {
            cout << "Our-C exited ..." << endl;
        }  // catch
    }
};

int main() {
    Interpreter interpreter;
}