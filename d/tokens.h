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
#define STRT 2
#define END 3
#define SALES 4
#define ID 5
#define SPACE 6

#ifdef __USE_PROTOS
void lego(AST**_root);
#else
extern void lego();
#endif

#ifdef __USE_PROTOS
void st(AST**_root);
#else
extern void st();
#endif

#endif
extern SetWordType setwd1[];
