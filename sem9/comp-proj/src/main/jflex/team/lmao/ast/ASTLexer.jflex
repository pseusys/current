package team.lmao.ast;

import java_cup.runtime.ComplexSymbolFactory.Location;
import java_cup.runtime.*;
import java.io.Reader;


%%

%class ASTLexer

%line
%column

%cup

%eofval{
    return symbol("eof", ASTParserSym.EOF);
%eofval}

/* Declarations */
%{
    private ComplexSymbolFactory factory;

    public ASTLexer(Reader in, ComplexSymbolFactory sf){
        this(in);
    	factory = sf;
    }

    private Symbol symbol(String token, int type) {
        Location left = new Location(yyline + 1, yycolumn + 1, (int) yychar);
        Location right = new Location(yyline + 1, yycolumn + yylength(), (int) yychar + yylength());
        return factory.newSymbol(token, type, left, right);
    }

    private Symbol symbol(String token, int type, Object value) {
        Location left = new Location(yyline + 1, yycolumn + 1, (int) yychar);
        Location right = new Location(yyline + 1, yycolumn + yylength(), (int) yychar + yylength());
        return factory.newSymbol(token, type, left, right, value);
    }
%}


/* Macro Declarations */

space = [ \t\n\r]
digit = [0-9]
lower = [a-z]
upper = [A-Z]
sign = "+"|"-"
comment = "(*" [^*] ~"*)"


%%

<YYINITIAL> {
    {space}+                                                  { }
    {comment}                                                 {  }
    "("                                                       { return symbol("lpar", ASTParserSym.LPAREN); }
    ")"                                                       { return symbol("rpar", ASTParserSym.RPAREN); }
    "true"                                                    { return symbol("true", ASTParserSym.BOOL, true); }
    "false"                                                   { return symbol("false", ASTParserSym.BOOL, false); }
    "not"                                                     { return symbol("not", ASTParserSym.NOT); }

    {sign}? {digit}+                                          { return symbol("int", ASTParserSym.INT, Integer.valueOf(yytext())); }
    {sign}? 0 [xX] ({digit}|[a-f]|[A-F])+                  { return symbol("int", ASTParserSym.INT, Integer.decode(yytext())); }
    {sign}? {digit}+ ("." {digit}*)? ([eE] {sign}? {digit}+)? { return symbol("float", ASTParserSym.FLOAT, Float.valueOf(yytext())); }

    "-"                                                       { return symbol("sub", ASTParserSym.MINUS); }
    "+"                                                       { return symbol("add", ASTParserSym.PLUS); }
    "-."                                                      { return symbol("fsub", ASTParserSym.MINUS_DOT); }
    "+."                                                      { return symbol("fadd", ASTParserSym.PLUS_DOT); }
    "*."                                                      { return symbol("fmul", ASTParserSym.AST_DOT); }
    "/."                                                      { return symbol("fdiv", ASTParserSym.SLASH_DOT); }
    "="                                                       { return symbol("eq", ASTParserSym.EQUAL); }
    "<>"                                                      { return symbol("lg", ASTParserSym.LESS_GREATER); }
    "<="                                                      { return symbol("le", ASTParserSym.LESS_EQUAL); }
    ">="                                                      { return symbol("ge", ASTParserSym.GREATER_EQUAL); }
    "<"                                                       { return symbol("l", ASTParserSym.LESS); }
    ">"                                                       { return symbol("g", ASTParserSym.GREATER); }
    "if"                                                      { return symbol("if", ASTParserSym.IF); }
    "then"                                                    { return symbol("then", ASTParserSym.THEN); }
    "else"                                                    { return symbol("else", ASTParserSym.ELSE); }
    "let"                                                     { return symbol("let", ASTParserSym.LET); }
    "in"                                                      { return symbol("in", ASTParserSym.IN); }
    "rec"                                                     { return symbol("rec", ASTParserSym.REC); }
    ","                                                       { return symbol("com", ASTParserSym.COMMA); }
    "_"                                                       { return symbol("idt", ASTParserSym.IDENT, Id.gen()); }
    "Array.create"                                            { return symbol("arr", ASTParserSym.ARRAY_CREATE); }
    "."                                                       { return symbol("dot", ASTParserSym.DOT); }
    "<-"                                                      { return symbol("lm", ASTParserSym.LESS_MINUS); }
    ";"                                                       { return symbol("sem", ASTParserSym.SEMICOLON); }
    eof                                                       { return symbol("eof", ASTParserSym.EOF); }

    {lower} ({digit}|{lower}|{upper}|"_")*                    { return symbol("fun", ASTParserSym.IDENT, new Id(yytext())); }
}
[^]                                                           { throw new Error("Illegal character <" + yytext() + ">"); }
