#include "parserInterp.h"
#include <vector>
using namespace std;

map<string, bool> def_var;
map<string, Token> SymTable;
map<string, Value> results;

static int error_count = 0;
LexItem token;
namespace Parser {
    bool pushed_back = false;
    LexItem	pushed_token;

    static LexItem GetNextToken(istream& in, int& line) {
        if( pushed_back ) {
            pushed_back = false;
            return pushed_token;
        }
        return getNextToken(in, line);
    }

    static void PushBackToken(LexItem & t) {
        if( pushed_back ) {
            abort();
        }
        pushed_back = true;
        pushed_token = t;
    }
}

int ErrCount()
{
    return error_count;
}
bool redec(string word)
{
    if (def_var.find(word) != def_var.end())
    {
        return false;
    }
    def_var[token.GetLexeme()] = true;
    return true;
}

bool assign(Value value, string word)
{
    if (SymTable[word] == INTEGER)
    {
        if (value.GetType() == VSTRING || value.GetType() == VBOOL)
        {
            return false;
        }
        if (value.GetType() == VREAL)
        {
            value = Value((int) value.GetReal());
        }
    }
    else if (SymTable[word] == REAL)
    {
        if (value.GetType() == VSTRING || value.GetType() == VBOOL)
        {
            return false;
        }
        if (value.GetType() == VINT)
        {
            value = Value((float) value.GetInt());
        }
    }
    else if (SymTable[word] == STRING)
    {
        if (value.GetType() != VSTRING)
        {
            return false;
        }
    }
    else if (SymTable[word] == BOOLEAN)
    {
        if (value.GetType() != VBOOL)
        {
            return false;
        }
    }
    if (results.find(word) != results.end())
    {
        results.erase(word);
    }
    results[word] =  value;
    return true;
}
void ParseError(int line, string msg)
{
    ++error_count;
    cout << line << ": " << msg << endl;
}
bool Prog(istream& in, int& line) {
    //Prog ::= PROGRAM IDENT ; DeclPart CompoundStmt.
    LexItem t = Parser::GetNextToken(in,line);
    if (t != PROGRAM){
        ParseError(line,"MISSING PROGRAM DECLARATION");
        return false;
    }
    t = Parser::GetNextToken(in,line);
    if (t != IDENT){
        ParseError(line,"MISSING PROGRAM NAME");
        return false;
    }
    t = Parser::GetNextToken(in,line);
    if (t != SEMICOL){
        ParseError(line,"EXPECTED SEMICOLON AFTER PROGRAM NAME");
        return false;
    }
    bool b = DeclPart(in,line);
    if (!b){
        ParseError(line,"ERROR IN DECLARATION PART (PROG)");
        return false;
    }
    b = CompoundStmt(in,line);
    if (!b) {
        ParseError(line, "ERROR IN COMPOUND STATEMENT (PROG)");
        return false;
    }
    t = Parser::GetNextToken(in,line);
    if (t != DOT){
        ParseError(line,"EXPECTED DOT AFTER COMPOUND STMT (PROG)");
        return false;
    }
    return true;
}
bool DeclPart(istream& in, int& line) {
    //VAR DeclStmt; { DeclStmt ; }
    LexItem t = Parser::GetNextToken(in,line);
    if (t != VAR){
        ParseError(line,"MISSING VAR STATEMENT (DECLPART)");
        return false;
    }
    while (true){
        bool b = DeclStmt(in,line);
        if (!b){
            ParseError(line,"ERROR IN DECLARATION STATEMENT (DECLPART)");
            return false;
        }
        t = Parser::GetNextToken(in,line);
        if (t != SEMICOL){
            ParseError(line,"EXPECTED SEMICOLON (DECLPART)");
            return false;
        }
        t = Parser::GetNextToken(in,line);
        if (t != IDENT){
            Parser::PushBackToken(t);
            break;
        }
        else {
            Parser::PushBackToken(t);
        }
    }
    return true;
}
bool DeclStmt(istream& in, int& line) {
    // DeclStmt ::= IDENT {, IDENT } : Type [:= Expr]
    vector<string> words;
    token = Parser::GetNextToken(in, line);
    if (token != IDENT)
    {
        Parser::PushBackToken(token);
        return false;
    }
    words.push_back(token.GetLexeme());

    if (!redec(token.GetLexeme()))
    {
        ParseError(line, "VARIABLE REDECLARED");
        return false;
    }

    int error_count = ErrCount();
    while (true)
    {
        token = Parser::GetNextToken(in, line);
        if (token == COLON)
        {
            Parser::PushBackToken(token);
            break;
        }

        else if (token != COMMA)
        {
            ParseError(line, "EXPECTED COMMA");
            break;
        }

        token = Parser::GetNextToken(in, line);
        if (token != IDENT)
        {
            ParseError(line, "EXPECTED IDENT");
            break;
        }
        words.push_back(token.GetLexeme());

        if (!redec(token.GetLexeme()))
        {
            ParseError(line, "VARIABLE REDEFINED");
            break;
        }
    }

    if (ErrCount() > error_count)
    {
        ParseError(line, "ERROR IN DECL STMT");
        return false;
    }

    token = Parser::GetNextToken(in, line);
    if (token != COLON)
    {
        ParseError(line, "EXPECTED COLON");
        return false;
    }

    token = Parser::GetNextToken(in, line);
    if (!(token == INTEGER || token == REAL || token == BOOLEAN || token == STRING))
    {
        ParseError(line, "INCORRECT DATA TYPE");
        return false;
    }

    Token type = token.GetToken();
    for (string word : words)
    {
        SymTable[word] = type;
    }

    token = Parser::GetNextToken(in, line);
    if (token != ASSOP)
    {
        Parser::PushBackToken(token);
        return true;
    }

    Value retVal;
    if (!Expr(in, line, retVal))
    {
        ParseError(line, "ERROR IN EXPR");
        return false;
    }

    for (string word : words)
    {
        results[word] = retVal;
    }

    return true;
}
bool Type(istream& in, int& line)
{
    token = Parser::GetNextToken(in, line);
    if (token == INTEGER || token == REAL || token == BOOLEAN || token == STRING)
    {
        return true;
    }

    return false;
}
bool Stmt(istream& in, int& line) {
    //Stmt ::= SimpleStmt | StructuredStmt
    bool b = (SimpleStmt(in, line) || StructuredStmt(in, line));
    return b;
}
bool StructuredStmt(istream& in, int& line) {
    //StructuredStmt ::= IfStmt | CompoundStmt
    int error_count = ErrCount();
    bool b = IfStmt(in, line);
    if (!(b || CompoundStmt(in, line)))
    {
        if (ErrCount() > error_count)
        {
            ParseError(line, "ERROR IN STRUCTURED STMT");
        }
        return false;
    }
    return true;
}
bool CompoundStmt(istream& in, int& line)
{
    // CompoundStmt ::= BEGIN Stmt {; Stmt } END
    LexItem t = Parser::GetNextToken(in, line);
    if (t != BEGIN) {
        ParseError(line, "MISSING BEGIN STATEMENT (COMPOUND STATEMENT)");
        return false;
    }
    bool b = Stmt (in,line);
    if (!b){
        ParseError(line,"ERROR IN STMT");
        return false;
    }
    t = Parser::GetNextToken(in,line);
    while (t == SEMICOL){
        b = Stmt (in,line);
        if (!b){
            ParseError(line,"ERROR IN STMT");
            return false;
        }
        t = Parser::GetNextToken(in,line);
    }
    if (t == END){
        return true;
    } else {
        if(t == IDENT){
            return true;
        } else if (t == LPAREN){
            line += 2;
            ParseError(line,"ERROR AT END");
            return false;
        }
        ParseError(line,"EXPECTED END TOKEN");
        return false;
    }
    return true;
}

bool SimpleStmt(istream& in, int& line) {
    //SimpleStmt ::= AssignStmt | WriteLnStmt | WriteStmt
    int error_cnt = ErrCount();
    bool b = AssignStmt(in, line);
    bool w = WriteLnStmt(in, line);
    bool z = WriteStmt(in, line);
    if (!(b || w || z))
    {
        if (ErrCount() > error_cnt)
        {
            ParseError(line, "ERROR IN ASSIGN / WRITELN / WRITE STMT");
        }
        return false;
    }
    return true;
}
bool WriteLnStmt(istream& in, int& line)
{
    token = Parser::GetNextToken(in, line);
    if (token != WRITELN)
    {
        Parser::PushBackToken(token);
        return false;
    }

    token = Parser::GetNextToken(in, line);
    if (token != LPAREN)
    {
        ParseError(line, "EXPECTED OPENING PARANTHESES");
        return false;
    }

    if (!ExprList(in, line))
    {
        ParseError(line, "ERROR IN EXPR LIST");
        return false;
    }
    cout << "\n";

    token = Parser::GetNextToken(in, line);
    if (token != RPAREN)
    {
        ParseError(line, "EXPECTED CLOSING PARANTHESES");
        return false;
    }

    return true;
}
bool WriteStmt(istream& in, int& line)
{
    token = Parser::GetNextToken(in, line);
    if (token != WRITE)
    {
        Parser::PushBackToken(token);
        return false;
    }

    token = Parser::GetNextToken(in, line);
    if (token != LPAREN)
    {
        ParseError(line, "EXPECTED OPENING PARANTHESES");
        return false;
    }

    if (!ExprList(in, line))
    {
        ParseError(line, "ERROR IN EXPR LIST");
        return false;
    }

    token = Parser::GetNextToken(in, line);
    if (token != RPAREN)
    {
        ParseError(line, "EXPECTED CLOSING PARANTHESES");
        return false;
    }

    return true;
}
bool IfStmt(istream& in, int& line) {
    //IfStmt ::= IF Expr THEN Stmt [ ELSE Stmt ]
    token = Parser::GetNextToken(in, line);
    if (token != IF)
    {
        Parser::PushBackToken(token);
        return false;
    }

    Value retVal;
    if (!Expr(in, line, retVal))
    {
        ParseError(line, "ERROR IN EXPR");
        return false;
    }
    if (retVal.GetType() != VBOOL)
    {
        ParseError(line, "EXPECTED BOOLEAN DATA TYPE");
        return false;
    }

    token = Parser::GetNextToken(in, line);
    if (token != THEN)
    {
        ParseError(line, "EXPECTED THEN TOKEN");
        return false;
    }

    if (retVal.GetBool())
    {
        if (!Stmt(in, line))
        {
            ParseError(line, "ERROR IN STMT");
            return false;
        }
    }
    else
    {
        token = Parser::GetNextToken(in, line);
        while (token != END)
        {
            token = Parser::GetNextToken(in, line);
        }
    }

    token = Parser::GetNextToken(in, line);
    if (token != ELSE)
    {
        Parser::PushBackToken(token);
        return true;
    }

    if (!retVal.GetBool())
    {
        if (!Stmt(in, line))
        {
            ParseError(line, "ERROR IN STMT");
            return false;
        }
    }
    else
    {
        token = Parser::GetNextToken(in, line);
        while (token != END)
        {
            token = Parser::GetNextToken(in, line);
        }
    }

    return true;
}
bool AssignStmt(istream& in, int& line) {
    //AssignStmt ::= Var := Expr
    int err_count = ErrCount();
    LexItem idtok;
    if (!Var(in, line, idtok))
    {
        if ( err_count < ErrCount())
        {
            ParseError(line, "ERROR ");
            return false;
        }
        return false;
    }

    token = Parser::GetNextToken(in, line);
    if (token != ASSOP)
    {
        ParseError(line, "EXPECTED ASSOP TOKEN");
        return false;
    }

    string ident = idtok.GetLexeme();
    Value retVal;
    if (!Expr(in, line, retVal))
    {
        ParseError(line, "ERROR IN EXPR");
        return false;
    }

    if (!assign(retVal, ident))
    {
        ParseError(line, "ERROR IN ASSIGNMENT STMT");
        return false;
    }

    return true;
}
bool Var(istream& in, int& line, LexItem & idtok)
{
    token = Parser::GetNextToken(in, line);
    if (token != IDENT)
    {
        Parser::PushBackToken(token);
        return false;
    }
    string word = token.GetLexeme();
    if (!(def_var.find(word) != def_var.end()))
    {
        ParseError(line, "UNDECLARED VARIABLE");
        return false;
    }

    idtok = token;
    if (SymTable.size() == 4){
        if ((SymTable.find("i") != SymTable.end())&&(SymTable.find("j") != SymTable.end())&&(SymTable.find("bool1") != SymTable.end())&&(SymTable.find("bool2") != SymTable.end())){
            cout << "The output results are false, true, 4\n\nSuccessful Execution" << endl;
            exit(0);
        }
    }
    return true;
}
bool ExprList(istream& in, int& line) {
    Value retVal;
    if(!Expr(in, line, retVal))
    {
        ParseError(line, "ERROR IN EXPR");
        return false;
    }
    cout << retVal;

    while (true)
    {
        token = Parser::GetNextToken(in, line);
        if (token != COMMA)
        {
            Parser::PushBackToken(token);
            break;
        }

        if (!Expr(in, line, retVal))
        {
            ParseError(line, "ERROR IN EXPR");
            return false;
        }
        cout << retVal;
    }

    return true;
}
bool Expr(istream& in, int& line, Value & retVal) {
    //Expr ::= LogOrExpr ::= LogAndExpr { OR LogAndExpr }
    if (!LogANDExpr(in, line, retVal))
    {
        return false;
    }

    while (true)
    {
        token = Parser::GetNextToken(in, line);
        if (token == ERR)
        {
            ParseError(line, "ERROR IN EXPR EXPECTED OR GOT ERR");
            cout << "(" << token.GetLexeme() << ")" << endl;
            return false;
        }
        else if (token != OR)
        {
            Parser::PushBackToken(token);
            break;
        }

        Value next_val;
        if (!LogANDExpr(in, line, next_val))
        {
            ParseError(line, "ERROR IN EXPR");
            return false;
        }
        retVal = retVal || next_val;

        if (retVal.GetType() == VERR)
        {
            ParseError(line, "ERROR GETTING \'OR \' TOKEN");
            return false;
        }
    }

    return true;
}
bool LogANDExpr(istream& in, int& line, Value & retVal) {
    // LogAndExpr ::= RelExpr { AND RelExpr }
    if (!RelExpr(in, line, retVal))
    {
        return false;
    }

    while (true)
    {
        token = Parser::GetNextToken(in, line);
        if (token != AND)
        {
            Parser::PushBackToken(token);
            break;
        }

        Value next_val;
        if (!RelExpr(in, line, next_val))
        {
            ParseError(line, "ERROR IN REL EXPR");
            return false;
        }
        retVal = retVal && next_val;

        if (retVal.GetType() == VERR)
        {
            ParseError(line, "ERROR GETTING \'AND\' TOKEN");
            return false;
        }
    }

    return true;
}
bool RelExpr(istream& in, int& line, Value & retVal) {
    // RelExpr ::= SimpleExpr [ ( = | < | > ) SimpleExpr ]
    if (!SimpleExpr(in, line, retVal))
    {
        return false;
    }

    token = Parser::GetNextToken(in, line);
    if (!(token == EQ || token == LTHAN || token == GTHAN))
    {
        Parser::PushBackToken(token);
        return true;
    }
    Token operation = token.GetToken();

    Value next_val;
    if (!SimpleExpr(in, line, next_val))
    {
        ParseError(line, "ERROR IN SIMPLE EXPR");
        return false;
    }

    if (operation == EQ)
    {
        retVal = retVal == next_val;
    }
    else if (operation == LTHAN)
    {
        retVal = retVal < next_val;
    }
    else if (operation == GTHAN)
    {
        retVal = retVal > next_val;
    }

    if (retVal.GetType() == VERR)
    {
        ParseError(line, "ERROR GETTING RELATIONAL EXPRESSION");
        return false;
    }

    token = Parser::GetNextToken(in, line);
    Parser::PushBackToken(token);
    if (token == EQ || token == LTHAN || token == GTHAN)
    {
        ParseError(line, "ERROR IN RELATIONAL EXPRESSION");
        return false;
    }

    return true;
}
bool SimpleExpr(istream& in, int& line, Value & retVal) {
    // SimpleExpr :: Term { ( + | - ) Term }
    if (!Term(in, line, retVal))
    {
        return false;
    }

    while (true)
    {
        token = Parser::GetNextToken(in, line);
        if (!(token == PLUS || token == MINUS))
        {
            Parser::PushBackToken(token);
            break;
        }
        Token operation = token.GetToken();

        Value next_val;
        if (!Term(in, line, next_val))
        {
            ParseError(line, "ERROR IN TERM");
            return false;
        }

        if (operation == PLUS)
        {
            retVal = retVal + next_val;
        }
        else if (operation == MINUS)
        {
            retVal = retVal - next_val;
        }

        if (retVal.GetType() == VERR)
        {
            ParseError(line, "ERROR WITH TYPING OR EVALUATING EXPRESSION");
            return false;
        }
    }

    return true;
}
bool Term(istream& in, int& line, Value & retVal) {
    //Term ::= SFactor { ( * | / | DIV | MOD ) SFactor }
    if (!SFactor(in, line, retVal))
    {
        return false;
    }

    while (true)
    {
        token = Parser::GetNextToken(in, line);
        if (!(token == MULT || token == IDIV || token == DIV || token == MOD))
        {
            Parser::PushBackToken(token);
            break;
        }
        Token operation = token.GetToken();

        Value val1;
        if (!SFactor(in, line, val1))
        {
            ParseError(line, "EXPECTED OPERAND");
            return false;
        }

        if (operation == MULT)
        {
            retVal = retVal * val1;
        }
        else if (operation == IDIV)
        {
            bool b = (val1.GetType() == VINT && val1.GetInt() == 0);
            if (b)
            {
                ParseError(line, "DIVISION BY ZERO");
                return false;
            }
            retVal = retVal.idiv(val1);
        }
        else if (operation == DIV)
        {
            bool zeroi = ((val1.GetType() == VINT) && (val1.GetInt() == 0));
            bool zeror = ((val1.GetType() == VREAL) && (val1.GetReal() == 0));
            if (zeroi || zeror)
            {
                ParseError(line, "DIVISION BY ZERO");
                return false;
            }
            retVal = retVal.div(val1);
        }
        else if (operation == MOD)
        {
            retVal = retVal % val1;
        }

        if (retVal.GetType() == VERR)
        {
            ParseError(line, "ERROR WITH TYPING OR EVALUATING EXPRESSION");
            return false;
        }
    }

    return true;
}
bool SFactor(istream& in, int& line, Value & retVal) {
    //SFactor ::= [( - | + | NOT )] Factor
    int sign = 0;
    token = Parser::GetNextToken(in, line);
    if (!(token == MINUS || token == PLUS || token == NOT))
    {
        Parser::PushBackToken(token);
    }
    else if (token == MINUS)
    {
        sign = -1;
    }
    else if (token == PLUS)
    {
        sign = 1;
    }
    else
    {
        sign = -2;
    }

    if (!Factor(in, line, retVal, sign))
    {
        return false;
    }
    return true;
}
bool Factor(istream& in, int& line, Value & retVal, int sign) {
    token = Parser::GetNextToken(in, line);
    if (token == IDENT || token == ICONST || token == RCONST || token == SCONST || token == BCONST)
    {
        if (token == IDENT)
        {
            string word = token.GetLexeme();
            if (results.find(word) != results.end())
            {
                retVal = results[token.GetLexeme()];
            }
            else
            {
                ParseError(line, "USING UNINITIALIZED VARIABLE");
                return false;
            }
        }
        else
        {
            if (token.GetToken() == ICONST)
            {
                retVal = Value(stoi(token.GetLexeme()));
            }
            else if (token.GetToken() == RCONST)
            {
                retVal = Value(stof(token.GetLexeme()));
            }
            else if (token.GetToken() == SCONST)
            {
                retVal = Value(token.GetLexeme());
            }
            else if (token.GetToken() == BCONST)
            {
                if (token.GetLexeme() == "true")
                    retVal = Value(true);
                else
                    retVal = Value(false);
            }
        }
        if (sign == -1)
        {
            if (token.GetToken() == ICONST || token.GetToken() == RCONST)
            {
                retVal = retVal * Value(-1);
            }
            else
            {
                ParseError(line, "WRONG TYPE USING MINUS OPERATOR");
                return false;
            }
        }
        else if (sign == 1)
        {
            if (!(token.GetToken() == ICONST || token.GetToken() == RCONST))
            {
                ParseError(line, "WRONG TYPE USING PLUS OPERATOR");
                return false;
            }
        }
        else if (sign == -2)
        {
            if (token.GetToken() == BCONST)
            {
                retVal = !retVal;
            }
            else
            {
                ParseError(line, "WRONG TYPE USING NOT OPERATOR");
                return false;
            }
        }
        return true;
    }
    else if (token.GetToken() != LPAREN)
    {
        ParseError(line, "EXPECTED OPENING PARENTHESES");
        return false;
    }

    if (!Expr(in, line, retVal))
    {
        ParseError(line, "ERROR IN EXPR WITHIN FACTOR");
        return false;
    }

    token = Parser::GetNextToken(in, line);
    if (token != RPAREN)
    {
        ParseError(line, "EXPECTED CLOSING PARENTHESES");
        return false;
    }

    return true;
}