package team.lmao.asml;

import team.lmao.asml.expressions.*;

import java_cup.runtime.ComplexSymbolFactory.Location;
import java_cup.runtime.*;
import java.io.Reader;


%%

%class ASMLLexer

%line
%column

%cup

%eofval{
    return symbol("eof", ASMLParserSym.EOF);
%eofval}


/* Declarations */

%{
    private ComplexSymbolFactory factory;

    public ASMLLexer(Reader in, ComplexSymbolFactory sf){
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


%%

<YYINITIAL> {
    {space}+                                                  { }
    "("                                                       { return symbol("lpar", ASMLParserSym.LPAREN); }
    ")"                                                       { return symbol("rpar", ASMLParserSym.RPAREN); }
    "+"                                                       { return symbol("plus", ASMLParserSym.PLUS); }
    "="                                                       { return symbol("eq", ASMLParserSym.EQUAL); }
    "=."                                                      { return symbol("feq", ASMLParserSym.FEQUAL); }
    "<="                                                      { return symbol("le", ASMLParserSym.LE); }
    "<=."                                                     { return symbol("fle", ASMLParserSym.FLE); }
    ">="                                                      { return symbol("ge", ASMLParserSym.GE); }
    ">=."                                                     { return symbol("fge", ASMLParserSym.FGE); }
    "if"                                                      { return symbol("if", ASMLParserSym.IF); }
    "then"                                                    { return symbol("then", ASMLParserSym.THEN); }
    "else"                                                    { return symbol("else", ASMLParserSym.ELSE); }
    "let"                                                     { return symbol("let", ASMLParserSym.LET); }
    "in"                                                      { return symbol("in", ASMLParserSym.IN); }
    "."                                                       { return symbol("dot", ASMLParserSym.DOT); }
    "neg"                                                     { return symbol("neg", ASMLParserSym.NEG); }
    "fneg"                                                    { return symbol("fneg", ASMLParserSym.FNEG); }
    "mem"                                                     { return symbol("mem", ASMLParserSym.MEM); }
    "fmul"                                                    { return symbol("fmul", ASMLParserSym.FMUL); }
    "fdiv"                                                    { return symbol("fdiv", ASMLParserSym.FDIV); }
    "sub"                                                     { return symbol("sub", ASMLParserSym.SUB); }
    "fsub"                                                    { return symbol("fsub", ASMLParserSym.FSUB); }
    "add"                                                     { return symbol("add", ASMLParserSym.ADD); }
    "fadd"                                                    { return symbol("fadd", ASMLParserSym.FADD); }
    "<-"                                                      { return symbol("arr", ASMLParserSym.ASSIGN); }
    "call"                                                    { return symbol("call", ASMLParserSym.CALL); }
    "new"                                                     { return symbol("new", ASMLParserSym.NEW); }
    "nop"                                                     { return symbol("nop", ASMLParserSym.NOP); }
    "call_closure"                                            { return symbol("apc", ASMLParserSym.CALCLO); }
    "_"                                                       { return symbol("und", ASMLParserSym.UNDERSC); }
    eof                                                       { return symbol("eof", ASMLParserSym.EOF); }

    {sign}? {digit}+                                          { return symbol("int", ASMLParserSym.INT, new ASMLIntegerLiteral(Integer.valueOf(yytext()))); }
    {sign}? 0 [xX] ({digit}|[a-f]|[A-F])+                  { return symbol("int", ASMLParserSym.INT, new ASMLIntegerLiteral(Integer.decode(yytext()))); }
    {sign}? {digit}+ ("." {digit}*)? ([eE] {sign}? {digit}+)? { return symbol("float", ASMLParserSym.FLOAT, new ASMLFloatLiteral(Float.valueOf(yytext()))); }

    {lower} ({digit}|{lower}|{upper}|"_")*                    { return symbol("ident", ASMLParserSym.IDENT, new ASMLIdentifier(yytext())); }
    "_" ({digit}|{lower}|{upper}|"_")*                        { return symbol("label", ASMLParserSym.LABEL, new ASMLLabel(yytext())); }
    "%self"                                                   { return symbol("ident", ASMLParserSym.IDENT, new ASMLIdentifier(yytext())); }
}
[^]                                                         { throw new Error("Illegal character <" + yytext() + ">"); }
