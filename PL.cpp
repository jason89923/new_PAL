# include <stdio.h>
# include <string.h>
# include <iostream>
# include <map>
# include <sstream>
# include <stdexcept>
# include <string>
# include <vector>

using namespace std ;

class Done {
}; // class Done


class OK {
}; // class OK


class Lexical_Error :

public exception {
}; // class Lexical_Error


class Syntactical_Error :

public exception {
}; // class Syntactical_Error


class Semantic_Error :

public exception {
}; // class Semantic_Error


class Tools {

  public :
  static int String_to_int( const string & str ) {
    stringstream ss ;
    ss << str ;
    int returnValue ;
    ss >> returnValue ;
    return returnValue ;
  } // String_to_int()
}; // class Tools


class Token {

  public :
  string mType ;
  string mString ;
  int mLine ;
  Token( const string & str, int currentLine ) {
    mString = str ;
    mLine = currentLine ;
    mType = Type( str ) ;
  } // Token()

  Token() { Clear() ; }

  void Clear() {
    mType = "" ;
    mString = "" ;
    mLine = 0 ;
  } // Clear()

  bool IsLegalConstant( const string & str ) {
    if ( str == "true" || str == "false" ) {
      return true ;
    } // if

    if ( IsDigit( str ) ) {
      return true ;
    } // if

    if ( str.length() >= 2 ) {
      if ( str[ 0 ] == '\'' && str[ str.length() - 1 ] == '\'' ) {
        return true ;
      } // if
      else if ( str[ 0 ] == '\"' && str[ str.length() - 1 ] == '\"' ) {
        return true ;
      } // else if
    } // if

    return false ;
  } // IsLegalConstant()


  private :
  string Type( const string & str ) {
    if ( str == "int" || str == "float" || str == "char" || str == "bool" || str == "string" ||
         str == "void" || str == "if" || str == "else" || str == "while" || str == "do" ||
         str == "return" ) {
      return "reserved" ;
    } // if
    else if ( IsDelimiter( str ) ) {
      return "delimiter" ;
    } // else if
    else if ( IsLegalConstant( str ) ) {
      return "constant" ;
    } // else if
    else if ( IsLegalIdentifier( str ) ) {
      return "identifier" ;
    } // else if

    return "404" ;
  } // Type()

  bool IsLegalIdentifier( const string & str ) {
    if ( ( str[ 0 ] < 'a' || str[ 0 ] > 'z' ) && ( str[ 0 ] < 'A' || str[ 0 ] > 'Z' ) ) {
      return false ;
    } // if

    for ( int i = 0 ; i < str.length() ; i++ ) {
      if ( ( str[ i ] < '0' || str[ i ] > '9' ) && ( str[ i ] < 'a' || str[ i ] > 'z' ) &&
           ( str[ i ] < 'A' || str[ i ] > 'Z' ) && str[ i ] != '_' ) {
        return false ;
      } // if
    } // for

    return true ;
  } // IsLegalIdentifier()

  static bool IsDelimiter( const string & str ) {
    if ( str == "(" || str == ")" || str == "[" || str == "]" || str == "{" || str == "}" || str == "+" ||
         str == "-" || str == "*" || str == "/" || str == "%" || str == "^" || str == ">" || str == "<" ||
         str == ">=" || str == "<=" || str == "==" || str == "!=" || str == "&" || str == "|" ||
         str == "!" || str == "&&" || str == "||" || str == "+=" || str == "-=" || str == "*=" ||
         str == "/=" || str == "%=" || str == "++" || str == "--" || str == ">>" || str == "<<" ||
         str == ";" || str == "," || str == "?" || str == ":" || str == "=" ) {
      return true ;
    } // if

    return false ;
  } // IsDelimiter()

  bool IsDigit( const string & str ) {
    int point = 0 ;
    for ( int i = 0 ; i < str.length() ; i++ ) {
      if ( str[ i ] == '.' ) {
        point++ ;
      } // if

      if ( ( ( str[ i ] < '0' || str[ i ] > '9' ) && str[ i ] != '.' ) || point > 1 ) {
        return false ;
      } // if
    } // for

    return true ;
  } // IsDigit()

  friend

  class Reader ;
}; // class Reader


class Reader {

  private :
  vector < Token > mBuffer ;
  map < string, string > mDelimiterTable ;
  int mcurrentLine ;
  void Transform_to_tokens( const string & line ) {
    string current = "", next = "", temp = "" ;
    bool out = false ;
    for ( int i = 0 ; i < line.length() ; i++ ) {
      current = line[ i ] ;
      if ( i + 1 < line.length() )
        next = line[ i + 1 ] ;
      else
        out = true ;
      if ( ( current == "\n" || current == "\t" || current == " " ) ) {
        if ( temp.length() > 0 ) {
          Token token( temp, mcurrentLine ) ;
          mBuffer.push_back( token ) ;
          temp.clear() ;
        } // if
      } // if
      else if ( Token::IsDelimiter( current ) ) {
        if ( temp.length() != 0 ) {
          Token token( temp, mcurrentLine ) ;
          mBuffer.push_back( token ) ;
          temp.clear() ;
        } // if

        if ( i < line.length() - 1 ) {
          string tmp = current + next ;
          if ( tmp == "//" )
            i = line.length() ;
          else if ( Token::IsDelimiter( tmp ) ) {
            Token token( tmp, mcurrentLine ) ;
            mBuffer.push_back( token ) ;
            i++ ;
          } // else if
          else {
            Token token( current, mcurrentLine ) ;
            mBuffer.push_back( token ) ;
          } // else

          temp.clear() ;
        } // if
        else {
          Token token( current, mcurrentLine ) ;
          mBuffer.push_back( token ) ;
          temp.clear() ;
        } // else
      } // else if
      else if ( current == "\"" ) {
        bool slash = false ;
        i = i + 1 ;
        try {
          while ( ( i < line.length() && line[ i ] != '\"' ) || slash == true ) {
            slash = false ;
            if ( i >= line.length() ) {
              throw OK() ;
            } // if

            current = current + line[ i ] ;
            if ( line[ i ] == '\\' ) {
              slash = true ;
            } // if

            i = i + 1 ;
          } // while
        } // try
        catch( OK ok ) {
        } // catch

        if ( i >= line.length() ) {
          Token token( current, mcurrentLine ) ;
          mBuffer.push_back( token ) ;
          temp.clear() ;
        } // if
        else {
          current = current + line[ i ] ;
          Token token( current, mcurrentLine ) ;
          mBuffer.push_back( token ) ;
          temp.clear() ;
        } // else
      } // else if
      else if ( current == "'" ) {
        if ( line[ i + 2 ] == '\'' ) {
          current = current + line[ i + 1 ] ;
          current = current + line[ i + 2 ] ;
          Token token( current, mcurrentLine ) ;
          mBuffer.push_back( token ) ;
          temp.clear() ;
        } // if
        else {
          current = current + line[ i + 1 ] ;
          current = current + line[ i + 2 ] ;
          Token token( temp, mcurrentLine ) ;
          mBuffer.push_back( token ) ;
          temp.clear() ;
        } // else

        i = i + 2 ;
      } // else if
      else {
        string ctmp = temp + current ;
        Token token( ctmp, mcurrentLine ) ;
        if ( token.mType == "404" && temp.length() > 0 ) {
          Token token1( temp, mcurrentLine ) ;
          mBuffer.push_back( token1 ) ;
          temp.clear() ;
        } // if

        temp = temp + current ;
      } // else
    } // for

    if ( temp.length() > 0 ) {
      Token token( temp, mcurrentLine ) ;
      mBuffer.push_back( token ) ;
      temp.clear() ;
    } // if
  } // Transform_to_tokens()


  public :
  Reader() { ResetAll() ; }

  static string GetLine() {
    string line ;
    char ch = getchar() ;
    while ( ch != '\n' && ch != EOF ) {
      line.push_back( ch ) ;
      ch = getchar() ;
    } // while

    return line ;
  } // GetLine()

  Token GetNextToken() {
    while ( mBuffer.size() == 0 ) {
      Transform_to_tokens( GetLine() ) ;
      mcurrentLine++ ;
    } // while

    Token token = mBuffer[ 0 ] ;
    mBuffer.erase( mBuffer.begin() ) ;
    if ( token.mType == "404" ) {
      cout << "Line " << mcurrentLine << " : unrecognized token with first char : '" ;
      cout << token.mString[ 0 ] << "'" << endl ;
      throw Lexical_Error() ;
    } // if

    return token ;
  } // GetNextToken()

  void ResetAll() {
    mcurrentLine = 1 ;
    mBuffer.clear() ;
  } // ResetAll()

  void ResetLine() { mcurrentLine = 1 ; }
}; // ResetLine()


class Recursive_descent_parser {

  private :
  map < string, string > mGrammarTable ;
  map < char, char > mBrackets ;
  int mCurrentIndex ;
  bool Exchange( bool value ) {
    if ( value )
      return false ;
    else
      return true ;
  } // Exchange()

  int LevelChange( char temp, map < char, char >::iterator mBrackets_iterator, int level ) {
    if ( mBrackets_iterator -> first == temp ) {
      return level + 1 ;
    } // if
    else if ( mBrackets_iterator -> second == temp ) {
      return level - 1 ;
    } // else if

    return level ;
  } // LevelChange()

  void AddString( vector < string > & mQueue, string & temp ) {
    mQueue.push_back( temp ) ;
    temp.clear() ;
  } // AddString()

  void Dismantling_grammar( const string & complicated_grammar, vector < string > & mQueue ) {
    string buffer ;
    map < char, char >::iterator mBrackets_iterator ;
    int currentLevel = 0 ;
    bool pass = false ;
    bool organize = false ;
    for ( int i = 0 ; i < complicated_grammar.length() ; i++ ) {
      if ( organize == false ) {
        if ( complicated_grammar[ i ] == '\'' ) {
          pass = Exchange( pass ) ;
        } // if

        if ( pass == false ) {
          mBrackets_iterator = mBrackets.find( complicated_grammar[ i ] ) ;
          if ( mBrackets_iterator != mBrackets.end() ) {
            organize = true ;
            currentLevel++ ;
          } // if
        } // if
      } // if
      else {
        currentLevel = LevelChange( complicated_grammar[ i ], mBrackets_iterator, currentLevel ) ;
        if ( currentLevel == 0 ) {
          organize = false ;
        } // if
      } // else

      if ( currentLevel == 0 && complicated_grammar[ i ] == ' ' ) {
        if ( buffer.length() > 0 ) {
          AddString( mQueue, buffer ) ;
        } // if
      } // if
      else {
        buffer.push_back( complicated_grammar[ i ] ) ;
      } // else
    } // for

    if ( buffer.length() > 0 ) {
      AddString( mQueue, buffer ) ;
    } // if
  } // Dismantling_grammar()


  public :
  Recursive_descent_parser() {
    mBrackets[ '{' ] = '}' ;
    mBrackets[ '[' ] = ']' ;
    mBrackets[ '(' ] = ')' ;
    mGrammarTable[ "user_input" ] = "definition " ;
    mGrammarTable[ "user_input" ] += ", statement" ;
    mGrammarTable[ "definition" ] = "( 'void' identifier function_definition_without_ID ) , " ;
    mGrammarTable[ "definition" ] += "( type_specifier identifier function_definition_or_declarators )" ;
    mGrammarTable[ "type_specifier" ] = " 'int' , 'char' " ;
    mGrammarTable[ "type_specifier" ] += ", 'float' , 'string' , 'bool' " ;
    mGrammarTable[ "function_definition_or_declarators" ] = "function_definition_without_ID" ;
    mGrammarTable[ "function_definition_or_declarators" ] += " , rest_of_declarators" ;
    mGrammarTable[ "rest_of_declarators" ] = "[ '[' constant ']' ] " ;
    mGrammarTable[ "rest_of_declarators" ] += "{ ',' identifier [ '[' constant ']' ] } ';'" ;
    mGrammarTable[ "function_definition_without_ID" ] = " '(' [ 'void' , formal_parameter_list ] ')'" ;
    mGrammarTable[ "function_definition_without_ID" ] += " compound_statement" ;
    mGrammarTable[ "formal_parameter_list" ] = "type_specifier [ '&' ] identifier [ '[' constant ']' ]" ;
    mGrammarTable[ "formal_parameter_list" ] += " { ',' type_specifier [ '&' ]" ;
    mGrammarTable[ "formal_parameter_list" ] += " identifier [ '[' constant ']' ] }" ;
    mGrammarTable[ "compound_statement" ] = "'{' { declaration , statement } '}'" ;
    mGrammarTable[ "declaration" ] = "type_specifier identifier rest_of_declarators" ;
    mGrammarTable[ "statement" ] = " ';' , ( expression ';' ) , ( 'return' [ expression ] ';' ) ," ;
    mGrammarTable[ "statement" ] += " compound_statement , ( 'if' '(' expression ')' statement" ;
    mGrammarTable[ "statement" ] += " [ 'else' statement ] ) , ( 'while' '(' expression ')' " ;
    mGrammarTable[ "statement" ] += "statement ) , ( 'do' statement 'while' '(' expression ')' ';' )" ;
    mGrammarTable[ "expression" ] = "basic_expression { ',' basic_expression }" ;
    mGrammarTable[ "basic_expression" ] = "( identifier rest_of_Identifier_started_basic_exp )" ;
    mGrammarTable[ "basic_expression" ] += " , ( ( '++' , '--' ) identifier " ;
    mGrammarTable[ "basic_expression" ] += "rest_of_PPMM_Identifier_started_basic_exp )" ;
    mGrammarTable[ "basic_expression" ] += " , ( sign { sign } signed_unary_exp romce_and_romloe ) , " ;
    mGrammarTable[ "basic_expression" ] += "( ( constant , ( '(' expression ')' ) ) romce_and_romloe )" ;
    mGrammarTable[ "rest_of_Identifier_started_basic_exp" ] = "( '(' [ actual_parameter_list ] ')' " ;
    mGrammarTable[ "rest_of_Identifier_started_basic_exp" ] += "romce_and_romloe ) , " ;
    mGrammarTable[ "rest_of_Identifier_started_basic_exp" ] += "( [ '[' expression ']' ] (" ;
    mGrammarTable[ "rest_of_Identifier_started_basic_exp" ] += " ( assignment_operator basic_expression )" ;
    mGrammarTable[ "rest_of_Identifier_started_basic_exp" ] += " , ( " ;
    mGrammarTable[ "rest_of_Identifier_started_basic_exp" ] += "[ '++' , '--' ] romce_and_romloe ) ) )" ;
    mGrammarTable[ "rest_of_PPMM_Identifier_started_basic_exp" ] = "[ '[' expression ']' " ;
    mGrammarTable[ "rest_of_PPMM_Identifier_started_basic_exp" ] += "] romce_and_romloe" ;
    mGrammarTable[ "sign" ] = "'+' , '-' , '!'" ;
    mGrammarTable[ "actual_parameter_list" ] = "basic_expression { ',' basic_expression }" ;
    mGrammarTable[ "assignment_operator" ] = "'=' , '*=' , '/=' , '%=' , '+=' , '-='" ;
    mGrammarTable[ "romce_and_romloe" ] = "rest_of_maybe_logical_OR_exp" ;
    mGrammarTable[ "romce_and_romloe" ] += " [ '?' basic_expression" ;
    mGrammarTable[ "romce_and_romloe" ] += " ':' basic_expression ]" ;
    mGrammarTable[ "rest_of_maybe_logical_OR_exp" ] = "rest_of_maybe_logical_AND_exp " ;
    mGrammarTable[ "rest_of_maybe_logical_OR_exp" ] += "{ '||' maybe_logical_AND_exp }" ;
    mGrammarTable[ "maybe_logical_AND_exp" ] = "maybe_bit_OR_exp { '&&' maybe_bit_OR_exp }" ;
    mGrammarTable[ "rest_of_maybe_logical_AND_exp" ] = "rest_of_maybe_bit_OR_exp { '&&' maybe_bit_OR_exp }" ;
    mGrammarTable[ "maybe_bit_OR_exp" ] = "maybe_bit_ex_OR_exp { ',' maybe_bit_ex_OR_exp }" ;
    mGrammarTable[ "rest_of_maybe_bit_OR_exp" ] = "rest_of_maybe_bit_ex_OR_exp { ',' maybe_bit_ex_OR_exp }" ;
    mGrammarTable[ "maybe_bit_ex_OR_exp" ] = "maybe_bit_AND_exp { '^' maybe_bit_AND_exp }" ;
    mGrammarTable[ "rest_of_maybe_bit_ex_OR_exp" ] = "rest_of_maybe_bit_AND_exp { '^' maybe_bit_AND_exp }" ;
    mGrammarTable[ "maybe_bit_AND_exp" ] = "maybe_equality_exp { '&' maybe_equality_exp }" ;
    mGrammarTable[ "rest_of_maybe_bit_AND_exp" ] = "rest_of_maybe_equality_exp { '&' maybe_equality_exp }" ;
    mGrammarTable[ "maybe_equality_exp" ] = "maybe_relational_exp { ( '==' , '!=' ) maybe_relational_exp}" ;
    mGrammarTable[ "rest_of_maybe_equality_exp" ] = "rest_of_maybe_relational_exp" ;
    mGrammarTable[ "rest_of_maybe_equality_exp" ] += " { ( '==' , '!=' ) maybe_relational_exp }" ;
    mGrammarTable[ "maybe_relational_exp" ] = "maybe_shift_exp" ;
    mGrammarTable[ "maybe_relational_exp" ] += " { ( '<' , '>' , '<=' , '>=' ) maybe_shift_exp }" ;
    mGrammarTable[ "rest_of_maybe_relational_exp" ] = "rest_of_maybe_shift_exp" ;
    mGrammarTable[ "rest_of_maybe_relational_exp" ] += " { ( '<' , '>' , '<=' , '>=' ) maybe_shift_exp }" ;
    mGrammarTable[ "maybe_shift_exp" ] = "maybe_additive_exp { ( '<<' , '>>' ) maybe_additive_exp }" ;
    mGrammarTable[ "rest_of_maybe_shift_exp" ] = "rest_of_maybe_additive_exp" ;
    mGrammarTable[ "rest_of_maybe_shift_exp" ] += " { ( '<<' , '>>' ) maybe_additive_exp }" ;
    mGrammarTable[ "maybe_additive_exp" ] = "maybe_mult_exp { ( '+' , '-' ) maybe_mult_exp }" ;
    mGrammarTable[ "rest_of_maybe_additive_exp" ] = "rest_of_maybe_mult_exp" ;
    mGrammarTable[ "rest_of_maybe_additive_exp" ] += " { ( '+' , '-' ) maybe_mult_exp }" ;
    mGrammarTable[ "maybe_mult_exp" ] = "unary_exp rest_of_maybe_mult_exp" ;
    mGrammarTable[ "rest_of_maybe_mult_exp" ] = "{ ( '*' , '/' , '%' ) unary_exp }" ;
    mGrammarTable[ "unary_exp" ] = "( sign { sign } signed_unary_exp " ;
    mGrammarTable[ "unary_exp" ] += ") , unsigned_unary_exp , ( ( '++' , '--' ) identifier " ;
    mGrammarTable[ "unary_exp" ] += "[ '[' expression ']' ] )" ;
    mGrammarTable[ "signed_unary_exp" ] = "( identifier [ ( '(' [ actual_parameter_list ] ')' ) ," ;
    mGrammarTable[ "signed_unary_exp" ] += " ( '[' expression ']' ) ] ) , " ;
    mGrammarTable[ "signed_unary_exp" ] += "constant , ( '(' expression ')' )" ;
    mGrammarTable[ "unsigned_unary_exp" ] = "( identifier [ ( '(' [ actual_parameter_list ] ')' )" ;
    mGrammarTable[ "unsigned_unary_exp" ] += " , ( [ '[' expression ']' ] [ ( '++' , '--' " ;
    mGrammarTable[ "unsigned_unary_exp" ] += ") ] ) ] ) , constant , ( '(' expression ')' )" ;
  } // Recursive_descent_parser()

  bool Is_completed_expression( vector < Token > & tokenList, bool & use_newest_token ) {
    mCurrentIndex = 0 ;
    try {
      if ( !Is_mach_rule( tokenList, "user_input" ) ) {
        cout << "Line " << tokenList[ mCurrentIndex ].mLine << " : unexpected token : '" ;
        cout << tokenList[ mCurrentIndex ].mString << "'" << endl ;
        throw Syntactical_Error() ;
      } // if
    } // try
    catch( const OK & ok ) {
      return false ;
    } // catch

    if ( mCurrentIndex < tokenList.size() ) {
      tokenList.erase( tokenList.begin() + tokenList.size() - 1 ) ;
      use_newest_token = false ;
    } // if

    return true ;
  } // Is_completed_expression()


  private :
  bool Is_mach_rule( const vector < Token > & tokenList, const string & rule ) {
    vector < string > dismantled_grammar_list ;
    map < string, string >::iterator iterator = mGrammarTable.find( rule ) ;
    if ( iterator == mGrammarTable.end() ) {
      Dismantling_grammar( rule, dismantled_grammar_list ) ;
    } // if
    else {
      Dismantling_grammar( iterator -> second, dismantled_grammar_list ) ;
    } // else

    string rule_attribute = "sequence" ;
    for ( int i = 0 ; i < dismantled_grammar_list.size() ; i++ ) {
      if ( dismantled_grammar_list[ i ] == "," ) {
        rule_attribute = "or" ;
        dismantled_grammar_list.erase( dismantled_grammar_list.begin() + i ) ;
        i-- ;
      } // if
    } // for

    if ( rule_attribute == "sequence" ) {
      for ( int i = 0 ; i < dismantled_grammar_list.size() ; i++ ) {
        if ( !Is_mach_single_rule( tokenList, dismantled_grammar_list[ i ] ) ) {
          return false ;
        } // if
      } // for

      return true ;
    } // if
    else {
      for ( int i = 0 ; i < dismantled_grammar_list.size() ; i++ ) {
        int preIndex = mCurrentIndex ;
        if ( Is_mach_single_rule( tokenList, dismantled_grammar_list[ i ] ) ) {
          return true ;
        } // if
        else {
          if ( preIndex != mCurrentIndex ) {
            return false ;
          } // if
        } // else
      } // for

      return false ;
    } // else
  } // Is_mach_rule()

  bool Is_mach_single_rule( const vector < Token > & tokenList, const string & single_rule ) {
    int situation = 0 ;
    if ( mBrackets.find( single_rule[ 0 ] ) != mBrackets.end() ) {
      situation = 1 ;
    } // if
    else if ( mGrammarTable.find( single_rule ) != mGrammarTable.end() ) {
      situation = 2 ;
    } // else if
    else {
      situation = 3 ;
    } // else

    if ( situation == 1 || situation == 2 ) {
      int preIndex = mCurrentIndex ;
      string preprocessed_rule = single_rule ;
      if ( situation == 1 ) {
        preprocessed_rule = single_rule.substr( 1, single_rule.length() - 2 ) ;
      } // if

      bool isMachRule = Is_mach_rule( tokenList, preprocessed_rule ) ;
      char first_char = single_rule[ 0 ] ;
      if ( first_char == '{' ) {
        while ( isMachRule ) {
          preIndex = mCurrentIndex ;
          isMachRule = Is_mach_rule( tokenList, preprocessed_rule ) ;
        } // while

        if ( preIndex == mCurrentIndex ) {
          return true ;
        } // if
        else {
          return false ;
        } // else
      } // if
      else if ( first_char == '[' ) {
        if ( isMachRule ) {
          return true ;
        } // if
        else {
          return preIndex == mCurrentIndex ;
        } // else
      } // else if
      else {
        return isMachRule ;
      } // else
    } // if
    else {
      if ( mCurrentIndex == tokenList.size() ) {
        throw OK() ;
      } // if
      else {
        if ( Is_fit_type( tokenList[ mCurrentIndex ], single_rule ) ) {
          mCurrentIndex++ ;
          return true ;
        } // if

        return false ;
      } // else
    } // else
  } // Is_mach_single_rule()

  bool Is_fit_type( const Token & token, const string & type ) {
    if ( type == "identifier" || type == "constant" ) {
      return token.mType == type ;
    } // if

    string literal_string = "'" + token.mString + "'" ;
    return literal_string == type ;
  } // Is_fit_type()
}; // class Recursive_descent_parser


class Variable_definition {

  public :
  string mName ;
  string mType ;
  int mArray_size ;
  int mLifetime ;
  bool mSystem_variable ;
  Variable_definition( const Token & variable, const string & type, int array_size, int currentLevel ) {
    mName = variable.mString ;
    mType = type ;
    mArray_size = array_size ;
    mLifetime = currentLevel ;
    mSystem_variable = false ;
  } // Variable_definition()

  Variable_definition( const Token & variable, int array_size, int currentLevel ) {
    mName = variable.mString ;
    mType = variable.mType ;
    mArray_size = array_size ;
    mLifetime = currentLevel ;
    mSystem_variable = false ;
  } // Variable_definition()

  Variable_definition( const string & global_variable ) {
    mName = global_variable ;
    mType = "identifier" ;
    mArray_size = 0 ;
    mLifetime = 0 ;
    mSystem_variable = true ;
  } // Variable_definition()

  void Print() {
    cout << mType << " " << mName ;
    if ( mArray_size != 0 ) {
      cout << "[ " << mArray_size << " ]" ;
    } // if

    cout << " ;" << endl ;
  } // Print()
}; // class Variable_definition


class Function_definition {

  public :
  string mName ;
  vector < Token > mBody ;
  bool mSystem_function ;
  Function_definition( const vector < Token > & function_definition ) {
    mSystem_function = false ;
    if ( function_definition.size() >= 2 ) {
      mName = function_definition[ 1 ].mString ;
      for ( int i = 0 ; i < function_definition.size() ; i++ ) {
        mBody.push_back( function_definition[ i ] ) ;
      } // for
    } // if
  } // Function_definition()

  Function_definition( const string & global_function ) {
    mSystem_function = true ;
    mName = global_function ;
  } // Function_definition()

  string Create_tab( int num_of_tabs ) {
    string tab = "" ;
    for ( int i = 0 ; i < num_of_tabs ; i++ ) {
      tab += "  " ;
    } // for

    return tab ;
  } // Create_tab()

  bool Space( const string & current, const string & previous, int i ) {
    if ( current == "[" || current == "++" || current == "--" || i == 0 ) {
      return false ;
    } // if
    else if ( ( i > 0 && mBody[ i - 1 ].mType == "identifier" && current == "(" ) ) {
      return false ;
    } // else if
    else if ( previous == "(" && current == ")" ) {
      return false ;
    } // else if

    return true ;
  } // Space()

  void Print() {
    int parentheses_counter = 0 ;
    int condition_counter = 0 ;
    int tab_counter = 0 ;
    string current = "" ;
    string previous = "" ;
    for ( int i = 0 ; i < mBody.size() ; i++ ) {
      current = mBody[ i ].mString ;
      if ( current == "(" ) {
        parentheses_counter++ ;
      } // if

      if ( previous == ";" || previous == "{" || previous == "}" || current == "}" ) {
        if ( previous == "{" ) {
          tab_counter++ ;
        } // if

        if ( current == "}" ) {
          tab_counter-- ;
        } // if

        cout << endl ;
        cout << Create_tab( tab_counter ) ;
      } // if
      else if ( condition_counter > 0 && parentheses_counter == 0 && current != "{" && previous == ")" ) {
        cout << endl ;
        cout << Create_tab( tab_counter ) ;
        for ( int i = 0 ; i < condition_counter ; i++ ) {
          cout << "  " ;
        } // for
      } // else if
      else if ( previous == "else" && current != "{" ) {
        cout << endl ;
        cout << Create_tab( tab_counter ) ;
        cout << "  " ;
      } // else if
      else if ( Space( current, previous, i ) ) {
        cout << " " ;
      } // else if

      cout << current ;
      previous = current ;
      if ( current == ")" ) {
        parentheses_counter-- ;
      } // if
      else if ( current == "while" || current == "if" ) {
        condition_counter++ ;
      } // else if
      else if ( current == ";" ) {
        condition_counter = 0 ;
      } // else if
      else if ( current == "{" && condition_counter > 0 && parentheses_counter == 0 )
        condition_counter-- ;
    } // for

    cout << endl ;
  } // Print()
}; // class Function_definition


class Definition {

  private :
  vector < Variable_definition > mVariable_definitions ;
  vector < Variable_definition > mPseudo_definitions ;
  vector < Function_definition > mFunction_definitions ;
  bool Is_defined( const vector < Variable_definition > & definitions, const Token & variable, int cLevel ) {
    for ( int i = 0 ; i < definitions.size() ; i++ ) {
      if ( definitions[ i ].mName == variable.mString && definitions[ i ].mLifetime <= cLevel ) {
        return true ;
      } // if
    } // for

    return false ;
  } // Is_defined()

  void Sort( vector < string > & array ) {
    if ( array.size() <= 1 ) {
      return ;
    } // if

    for ( int i = 0 ; i < array.size() - 1 ; i++ ) {
      for ( int j = 0 ; j < array.size() - i - 1 ; j++ ) {
        if ( array[ j ] > array[ j + 1 ] ) {
          string temp = array[ j ] ;
          array[ j ] = array[ j + 1 ] ;
          array[ j + 1 ] = temp ;
        } // if
      } // for
    } // for
  } // Sort()

  void Print( const vector < string > & array ) {
    for ( int i = 0 ; i < array.size() ; i++ ) {
      cout << array[ i ] << endl ;
    } // for
  } // Print()


  public :
  Definition() {
    Variable_definition id_cin( "cin" ) ;
    Variable_definition id_cout( "cout" ) ;
    mVariable_definitions.push_back( id_cin ) ;
    mVariable_definitions.push_back( id_cout ) ;
    Function_definition global_function_1( "ListAllVariables" ) ;
    Function_definition global_function_2( "ListAllFunctions" ) ;
    Function_definition global_function_3( "ListVariable" ) ;
    Function_definition global_function_4( "ListFunction" ) ;
    Function_definition global_function_5( "Done" ) ;
    mFunction_definitions.push_back( global_function_1 ) ;
    mFunction_definitions.push_back( global_function_2 ) ;
    mFunction_definitions.push_back( global_function_3 ) ;
    mFunction_definitions.push_back( global_function_4 ) ;
    mFunction_definitions.push_back( global_function_5 ) ;
  } // Definition()

  void Define_variable( const Token & variable, const string & type, int array_size, int currentLevel ) {
    if ( !Is_variable_defined( variable, currentLevel ) ) {
      cout << "Definition of " << variable.mString << " entered ..." << endl ;
    } // if
    else {
      cout << "New definition of " << variable.mString << " entered ..." << endl ;
      for ( int i = 0 ; i < mVariable_definitions.size() ; i++ ) {
        if ( mVariable_definitions[ i ].mName == variable.mString ) {
          if ( mVariable_definitions[ i ].mLifetime == currentLevel ) {
            mVariable_definitions.erase( mVariable_definitions.begin() + i ) ;
          } // if
        } // if
      } // for
    } // else

    Variable_definition definition( variable, type, array_size, currentLevel ) ;
    mVariable_definitions.push_back( definition ) ;
  } // Define_variable()

  void Pseudo_define_variable( const Token & variable, int array_size, int currentLevel ) {
    Variable_definition definition( variable, array_size, currentLevel ) ;
    mPseudo_definitions.push_back( definition ) ;
  } // Pseudo_define_variable()

  bool Is_variable_defined( const Token & variable, int currentLevel ) {
    if ( Is_defined( mVariable_definitions, variable, currentLevel ) ) {
      return true ;
    } // if
    else if ( Is_defined( mPseudo_definitions, variable, currentLevel ) ) {
      return true ;
    } // else if

    return false ;
  } // Is_variable_defined()

  void Define_function( const vector < Token > & expression ) {
    Function_definition definition( expression ) ;
    if ( !Is_function_defined( definition.mName ) ) {
      cout << "Definition of " << definition.mName << "() entered ..." << endl ;
    } // if
    else {
      cout << "New definition of " << definition.mName << "() entered ..." << endl ;
      for ( int i = 0 ; i < mFunction_definitions.size() ; i++ ) {
        if ( mFunction_definitions[ i ].mName == definition.mName ) {
          mFunction_definitions.erase( mFunction_definitions.begin() + i ) ;
        } // if
      } // for
    } // else

    mFunction_definitions.push_back( definition ) ;
  } // Define_function()

  bool Is_function_defined( const string & function_name ) {
    for ( int i = 0 ; i < mFunction_definitions.size() ; i++ ) {
      if ( mFunction_definitions[ i ].mName == function_name ) {
        return true ;
      } // if
    } // for

    return false ;
  } // Is_function_defined()

  void ListAllVariables() {
    vector < string > variables ;
    for ( int i = 0 ; i < mVariable_definitions.size() ; i++ ) {
      if ( !mVariable_definitions[ i ].mSystem_variable ) {
        variables.push_back( mVariable_definitions[ i ].mName ) ;
      } // if
    } // for

    Sort( variables ) ;
    Print( variables ) ;
  } // ListAllVariables()

  void ListAllFunctions() {
    vector < string > functions ;
    for ( int i = 0 ; i < mFunction_definitions.size() ; i++ ) {
      if ( !mFunction_definitions[ i ].mSystem_function ) {
        functions.push_back( mFunction_definitions[ i ].mName + "()" ) ;
      } // if
    } // for

    Sort( functions ) ;
    Print( functions ) ;
  } // ListAllFunctions()

  void ListVariable( const string & name_str ) {
    if ( name_str.length() < 3 ) {
      return ;
    } // if

    string name = name_str.substr( 1, name_str.length() - 2 ) ;
    for ( int i = 0 ; i < mVariable_definitions.size() ; i++ ) {
      if ( mVariable_definitions[ i ].mName == name ) {
        mVariable_definitions[ i ].Print() ;
      } // if
    } // for
  } // ListVariable()

  void ListFunction( const string & name_str ) {
    if ( name_str.length() < 3 ) {
      return ;
    } // if

    string name = name_str.substr( 1, name_str.length() - 2 ) ;
    for ( int i = 0 ; i < mFunction_definitions.size() ; i++ ) {
      if ( mFunction_definitions[ i ].mName == name ) {
        mFunction_definitions[ i ].Print() ;
      } // if
    } // for
  } // ListFunction()

  friend

  class Identifier_definition_checker ;
}; // class Identifier_definition_checker


class Identifier_definition_checker {
  Definition mDefinition_table ;
  string Get_expression_type( const vector < Token > & expression ) {
    if ( expression[ 0 ].mString == "int" || expression[ 0 ].mString == "char" ||
         expression[ 0 ].mString == "float" || expression[ 0 ].mString == "string" ||
         expression[ 0 ].mString == "bool" || expression[ 0 ].mString == "void" ) {
      if ( expression[ 0 ].mString == "void" ) {
        return "function_definition" ;
      } // if

      if ( expression.size() >= 3 ) {
        if ( expression[ 2 ].mString == "(" ) {
          return "function_definition" ;
        } // if

        return "definition" ;
      } // if

      return "definition" ;
    } // if
    else {
      return "statement" ;
    } // else
  } // Get_expression_type()

  void Count_brackets( const vector < Token > & expression, int & start, int & end ) {
    start = 0 ;
    end = expression.size() ;
    int levelCounter = 0 ;
    for ( int i = 0 ; i < expression.size() ; i++ ) {
      if ( expression[ i ].mString == "{" ) {
        levelCounter++ ;
        if ( start == 0 ) {
          start = i + 1 ;
        } // if
      } // if
      else if ( expression[ i ].mString == "}" ) {
        levelCounter-- ;
        if ( levelCounter == 0 ) {
          end = i ;
        } // if
      } // else if
    } // for
  } // Count_brackets()

  void Crop_function_definition( const vector < Token > & expression ) {
    vector < Token > sub_expression ;
    vector < Token > param_list ;
    bool start = false, param = false ;
    for ( int i = 0 ; i < expression.size() ; i++ ) {
      if ( expression[ i ].mString == "{" ) {
        start = true ;
      } // if
      else if ( expression[ i ].mString == "(" ) {
        param = true ;
      } // else if
      else if ( expression[ i ].mString == ")" ) {
        param = false ;
      } // else if

      if ( start ) {
        sub_expression.push_back( expression[ i ] ) ;
      } // if

      if ( param ) {
        param_list.push_back( expression[ i ] ) ;
      } // if
    } // for

    Pseudo_define_variable( param_list, 1 ) ;
    Separate_expression( sub_expression, 0 ) ;
  } // Crop_function_definition()

  void Pseudo_execute( const vector < Token > & expression, int currentLevel ) {
    string expression_type = Get_expression_type( expression ) ;
    if ( Is_simplest( expression, expression_type ) ) {
      if ( expression_type == "statement" ) {
        Check_variable_definitions( expression, currentLevel ) ;
      } // if
      else {
        Pseudo_define_variable( expression, currentLevel ) ;
      } // else
    } // if
    else {
      if ( expression_type == "statement" ) {
        vector < Token > sub_expression ;
        int start, end ;
        Count_brackets( expression, start, end ) ;
        for ( int i = start ; i < end ; i++ ) {
          sub_expression.push_back( expression[ i ] ) ;
        } // for

        Separate_expression( sub_expression, currentLevel ) ;
      } // if
    } // else
  } // Pseudo_execute()

  bool Is_simplest( const vector < Token > & expression, const string & type ) {
    if ( type == "definition" ) {
      return true ;
    } // if

    if ( type == "function_definition" ) {
      return false ;
    } // if

    for ( int i = 0 ; i < expression.size() ; i++ ) {
      if ( expression[ i ].mString == "{" ) {
        return false ;
      } // if
    } // for

    return true ;
  } // Is_simplest()

  void Separate_expression( const vector < Token > & expression, int currentLevel ) {
    vector < Token > buffer ;
    int levelCounter = 0 ;
    for ( int i = 0 ; i < expression.size() ; i++ ) {
      buffer.push_back( expression[ i ] ) ;
      if ( expression[ i ].mString == "{" ) {
        levelCounter++ ;
      } // if
      else if ( expression[ i ].mString == "}" ) {
        levelCounter-- ;
        if ( buffer.size() > 0 && levelCounter == 0 ) {
          Pseudo_execute( buffer, currentLevel + levelCounter + 1 ) ;
          buffer.clear() ;
        } // if
      } // else if
      else if ( expression[ i ].mString == ";" ) {
        if ( buffer.size() > 0 && levelCounter == 0 ) {
          Pseudo_execute( buffer, currentLevel + levelCounter ) ;
          buffer.clear() ;
        } // if
      } // else if
    } // for

    if ( buffer.size() > 0 ) {
      Pseudo_execute( buffer, currentLevel + levelCounter ) ;
    } // if
  } // Separate_expression()

  void Define_variable( const vector < Token > & expression ) {
    string type = expression[ 0 ].mString ;
    for ( int i = 0 ; i < expression.size() ; i++ ) {
      string array_size = "0" ;
      if ( i + 2 < expression.size() ) {
        if ( expression[ i + 2 ].mType == "constant" ) {
          array_size = expression[ i + 2 ].mString ;
        } // if
      } // if

      if ( expression[ i ].mType == "identifier" ) {
        mDefinition_table.Define_variable( expression[ i ], type, Tools::String_to_int( array_size ), 0 ) ;
      } // if
    } // for
  } // Define_variable()

  void Pseudo_define_variable( const vector < Token > & expression, int currentLevel ) {
    for ( int i = 0 ; i < expression.size() ; i++ ) {
      if ( expression[ i ].mType == "identifier" ) {
        mDefinition_table.Pseudo_define_variable( expression[ i ], 0, currentLevel ) ;
      } // if
    } // for
  } // Pseudo_define_variable()

  void Check_variable_definitions( const vector < Token > & expression, int currentLevel ) {
    string next_token_string = "" ;
    for ( int i = 0 ; i < expression.size() ; i++ ) {
      if ( i + 1 < expression.size() ) {
        next_token_string = expression[ i + 1 ].mString ;
      } // if
      else {
        next_token_string = "" ;
      } // else

      if ( expression[ i ].mType == "identifier" ) {
        bool is_variable = mDefinition_table.Is_variable_defined( expression[ i ], currentLevel ) ;
        bool is_function = mDefinition_table.Is_function_defined( expression[ i ].mString ) ;
        bool is_id_defined = false ;
        if ( next_token_string == "(" ) {
          is_id_defined = is_function ;
        } // if
        else if ( next_token_string == "" ) {
          if ( is_function == true || is_variable == true )
            is_id_defined = true ;
          else
            is_id_defined = false ;
        } // else if
        else {
          is_id_defined = is_variable ;
        } // else

        if ( !is_id_defined ) {
          cout << "Line " << expression[ i ].mLine << " : undefined identifier : '" ;
          cout << expression[ i ].mString << "'" << endl ;
          throw Semantic_Error() ;
        } // if
      } // if
    } // for
  } // Check_variable_definitions()

  void Define_function( const vector < Token > & expression ) {
    mDefinition_table.Define_function( expression ) ;
  } // Define_function()


  public :
  void Define_and_confirm( const vector < Token > & expression ) {
    try {
      string expression_type = Get_expression_type( expression ) ;
      if ( expression_type == "function_definition" ) {
        Crop_function_definition( expression ) ;
      } // if
      else {
        Separate_expression( expression, 0 ) ;
      } // else

      mDefinition_table.mPseudo_definitions.clear() ;
    } // try
    catch( const exception & e ) {
      mDefinition_table.mPseudo_definitions.clear() ;
      throw ;
    } // catch
  } // Define_and_confirm()

  void Execute_function( const vector < Token > & expression ) {
    if ( expression[ 0 ].mString == "ListAllVariables" ) {
      mDefinition_table.ListAllVariables() ;
    } // if
    else if ( expression[ 0 ].mString == "ListAllFunctions" ) {
      mDefinition_table.ListAllFunctions() ;
    } // else if
    else if ( expression[ 0 ].mString == "ListVariable" ) {
      if ( expression.size() > 2 ) {
        mDefinition_table.ListVariable( expression[ 2 ].mString ) ;
      } // if
    } // else if
    else if ( expression[ 0 ].mString == "ListFunction" ) {
      if ( expression.size() > 2 ) {
        mDefinition_table.ListFunction( expression[ 2 ].mString ) ;
      } // if
    } // else if
    else if ( expression[ 0 ].mString == "Done" ) {
      throw Done() ;
    } // else if
  } // Execute_function()

  void Execute( const vector < Token > & expression ) {
    string type = Get_expression_type( expression ) ;
    if ( type == "statement" ) {
      Execute_function( expression ) ;
      cout << "Statement executed ..." << endl ;
    } // if
    else if ( type == "definition" ) {
      Define_variable( expression ) ;
    } // else if
    else {
      Define_function( expression ) ;
    } // else
  } // Execute()
}; // class Identifier_definition_checker


class Expression_executor {

  private :
  Reader mReader ;
  Recursive_descent_parser mRecursive_descent_parser ;
  Identifier_definition_checker mIdentifier_definition_checker ;
  Token mBuffered_token ;

  public :
  void ExecuteNextExpression() {
    vector < Token > expression ;
    bool complete = false ;
    while ( !complete ) {
      try {
        if ( mBuffered_token.mString == "" ) {
          mBuffered_token = mReader.GetNextToken() ;
        } // if

        expression.push_back( mBuffered_token ) ;
        bool use_newest_token = true ;
        complete = mRecursive_descent_parser.Is_completed_expression( expression, use_newest_token ) ;
        if ( use_newest_token ) {
          mBuffered_token.Clear() ;
        } // if
        
        mIdentifier_definition_checker.Define_and_confirm( expression ) ;
      } // try
      catch( const exception & e ) {
        mReader.ResetAll() ;
        expression.clear() ;
        mBuffered_token.Clear() ;
        cout << "> " ;
        complete = false ;
      } // catch
    } // while

    mIdentifier_definition_checker.Execute( expression ) ;
    mReader.ResetLine() ;
  } // ExecuteNextExpression()
}; // class Expression_executor

int main() {
  Reader::GetLine() ;
  Expression_executor expression_collector ;
  cout << "Our-C running ..." << endl ;
  try {
    while ( true ) {
      cout << "> " ;
      expression_collector.ExecuteNextExpression() ;
    } // while
  } // try
  catch( const Done & e ) {
    cout << "Our-C exited ..." << endl ;
  } // catch
} // main()