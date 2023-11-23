/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_Y_TAB_HPP_INCLUDED
# define YY_YY_Y_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ADD = 258,                     /* ADD  */
    SUB = 259,                     /* SUB  */
    MUL = 260,                     /* MUL  */
    DIV = 261,                     /* DIV  */
    SEMICOLON = 262,               /* SEMICOLON  */
    LPAREN = 263,                  /* LPAREN  */
    RPAREN = 264,                  /* RPAREN  */
    AND = 265,                     /* AND  */
    OR = 266,                      /* OR  */
    NOT = 267,                     /* NOT  */
    GREATER = 268,                 /* GREATER  */
    LESS = 269,                    /* LESS  */
    GREATER_EQ = 270,              /* GREATER_EQ  */
    LESS_EQ = 271,                 /* LESS_EQ  */
    EQUAL = 272,                   /* EQUAL  */
    NOT_EQUAL = 273,               /* NOT_EQUAL  */
    ASSIGN = 274,                  /* ASSIGN  */
    FN_CALL = 275,                 /* FN_CALL  */
    LET = 276,                     /* LET  */
    COLON = 277,                   /* COLON  */
    COMMA = 278,                   /* COMMA  */
    LBRACKET = 279,                /* LBRACKET  */
    RBRACKET = 280,                /* RBRACKET  */
    LBRACE = 281,                  /* LBRACE  */
    RBRACE = 282,                  /* RBRACE  */
    STRUCT = 283,                  /* STRUCT  */
    FN = 284,                      /* FN  */
    ARROW = 285,                   /* ARROW  */
    RET = 286,                     /* RET  */
    DOT = 287,                     /* DOT  */
    CONTINUE = 288,                /* CONTINUE  */
    BREAK = 289,                   /* BREAK  */
    IF = 290,                      /* IF  */
    ELSE = 291,                    /* ELSE  */
    WHILE = 292,                   /* WHILE  */
    INT = 293,                     /* INT  */
    ID = 294,                      /* ID  */
    NUM = 295                      /* NUM  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define ADD 258
#define SUB 259
#define MUL 260
#define DIV 261
#define SEMICOLON 262
#define LPAREN 263
#define RPAREN 264
#define AND 265
#define OR 266
#define NOT 267
#define GREATER 268
#define LESS 269
#define GREATER_EQ 270
#define LESS_EQ 271
#define EQUAL 272
#define NOT_EQUAL 273
#define ASSIGN 274
#define FN_CALL 275
#define LET 276
#define COLON 277
#define COMMA 278
#define LBRACKET 279
#define RBRACKET 280
#define LBRACE 281
#define RBRACE 282
#define STRUCT 283
#define FN 284
#define ARROW 285
#define RET 286
#define DOT 287
#define CONTINUE 288
#define BREAK 289
#define IF 290
#define ELSE 291
#define WHILE 292
#define INT 293
#define ID 294
#define NUM 295

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 17 "parser.yacc"

  A_pos pos;
  A_tokenId tokenId;
  A_tokenNum tokenNum;
  A_type type;
  A_rightValList rightValList;
  A_fnCall fnCall;
  A_indexExpr indexExpr;
  A_arrayExpr arrayExpr;
  A_memberExpr memberExpr;
  A_exprUnit exprUnit;
  A_arithBiOpExpr arithBiOpExpr;
  A_arithUExpr arithUExpr;
  A_arithExpr arithExpr;
  A_boolBiOpExpr boolBiOpExpr;
  A_boolUOpExpr boolUOpExpr;
  A_boolExpr boolExpr;
  A_comExpr comExpr;
  A_boolUnit boolUnit;
  A_rightVal rightVal;
  A_rightVal rightValOpt;
  A_leftVal leftVal;
  A_assignStmt assignStmt;
  A_varDeclScalar varDeclScalar;
  A_varDeclArray varDeclArray;
  A_varDecl varDecl;
  A_varDefScalar varDefScalar;
  A_varDefArray varDefArray;
  A_varDef varDef;
  A_varDeclStmt varDeclStmt;
  A_varDeclList varDeclList;
  A_structDef structDef;
  A_fnDecl fnDecl;
  A_paramDecl paramDecl;
  A_codeBlockStmtList codeBlockStmtList;
  A_fnDef fnDef;
  A_ifStmt ifStmt;
  A_whileStmt whileStmt;
  A_callStmt callStmt;
  A_returnStmt returnStmt;
  A_codeBlockStmt codeBlockStmt;
  A_fnDeclStmt fnDeclStmt;
  A_programElement programElement;
  A_programElementList programElementList;
  A_program program;

#line 194 "y.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_Y_TAB_HPP_INCLUDED  */
