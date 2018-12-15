#ifndef tokens_h
#define tokens_h
/* tokens.h -- List of labelled tokens and stuff
 *
 * Generated from: lego.g
 *
 * Terence Parr, Will Cohen, and Hank Dietz: 1989-2001
 * Purdue University Electrical Engineering
 * ANTLR Version 1.33MR33
 */
#define zzEOF_TOKEN 1
#define NUM 2
#define GRID 3
#define PUSH 4
#define ASSIG 5
#define MOVE 6
#define PLACE 7
#define AT 8
#define POP 9
#define HEIGHT 10
#define DEF 11
#define ENDEF 12
#define LPAR 13
#define RPAR 14
#define MAJOR 15
#define MENOR 16
#define WHILE 17
#define AND 18
#define EAST 19
#define WEST 20
#define NORTH 21
#define SOUTH 22
#define FITS 23
#define LCLAU 24
#define RCLAU 25
#define COMA 26
#define ID 27
#define SPACE 28

#ifdef __USE_PROTOS
void lego(AST**_root);
#else
extern void lego();
#endif

#ifdef __USE_PROTOS
void grid(AST**_root);
#else
extern void grid();
#endif

#ifdef __USE_PROTOS
void ops(AST**_root);
#else
extern void ops();
#endif

#ifdef __USE_PROTOS
void defs(AST**_root);
#else
extern void defs();
#endif

#ifdef __USE_PROTOS
void ops1(AST**_root);
#else
extern void ops1();
#endif

#ifdef __USE_PROTOS
void expr(AST**_root);
#else
extern void expr();
#endif

#ifdef __USE_PROTOS
void altura(AST**_root);
#else
extern void altura();
#endif

#ifdef __USE_PROTOS
void pushpop(AST**_root);
#else
extern void pushpop();
#endif

#ifdef __USE_PROTOS
void part2(AST**_root);
#else
extern void part2();
#endif

#ifdef __USE_PROTOS
void mueveme(AST**_root);
#else
extern void mueveme();
#endif

#ifdef __USE_PROTOS
void id(AST**_root);
#else
extern void id();
#endif

#ifdef __USE_PROTOS
void cord(AST**_root);
#else
extern void cord();
#endif

#ifdef __USE_PROTOS
void mentre(AST**_root);
#else
extern void mentre();
#endif

#ifdef __USE_PROTOS
void cond(AST**_root);
#else
extern void cond();
#endif

#ifdef __USE_PROTOS
void boolea(AST**_root);
#else
extern void boolea();
#endif

#ifdef __USE_PROTOS
void fiteame(AST**_root);
#else
extern void fiteame();
#endif

#ifdef __USE_PROTOS
void restfits(AST**_root);
#else
extern void restfits();
#endif

#ifdef __USE_PROTOS
void nums(AST**_root);
#else
extern void nums();
#endif

#ifdef __USE_PROTOS
void defs2(AST**_root);
#else
extern void defs2();
#endif

#endif
extern SetWordType zzerr1[];
extern SetWordType zzerr2[];
extern SetWordType zzerr3[];
extern SetWordType setwd1[];
extern SetWordType zzerr4[];
extern SetWordType zzerr5[];
extern SetWordType zzerr6[];
extern SetWordType zzerr7[];
extern SetWordType setwd2[];
extern SetWordType zzerr8[];
extern SetWordType zzerr9[];
extern SetWordType zzerr10[];
extern SetWordType zzerr11[];
extern SetWordType setwd3[];
