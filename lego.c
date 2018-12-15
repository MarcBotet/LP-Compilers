/*
 * A n t l r  T r a n s l a t i o n  H e a d e r
 *
 * Terence Parr, Will Cohen, and Hank Dietz: 1989-2001
 * Purdue University Electrical Engineering
 * With AHPCRC, University of Minnesota
 * ANTLR Version 1.33MR33
 *
 *   antlr -gt lego.g
 *
 */

#define ANTLR_VERSION	13333
#include "pcctscfg.h"
#include "pccts_stdio.h"

#include <string>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

// struct to store information about tokens
typedef struct {
  string kind;
  string text;
} Attrib;

// function to fill token information (predeclaration)
void zzcr_attr(Attrib *attr, int type, char *text);

// fields for AST nodes
#define AST_FIELDS string kind; string text;
#include "ast.h"

// macro to create a new AST node (and function predeclaration)
#define zzcr_ast(as,attr,ttype,textt) as=createASTnode(attr,ttype,textt)
AST* createASTnode(Attrib* attr,int ttype, char *textt);
#define GENAST

#include "ast.h"

#define zzSET_SIZE 4
#include "antlr.h"
#include "tokens.h"
#include "dlgdef.h"
#include "mode.h"

/* MR23 In order to remove calls to PURIFY use the antlr -nopurify option */

#ifndef PCCTS_PURIFY
#define PCCTS_PURIFY(r,s) memset((char *) &(r),'\0',(s));
#endif

#include "ast.c"
zzASTgvars

ANTLR_INFO

#include <cstdlib>
#include <cmath>

//global structures  
AST *root;


// function to fill token information
void zzcr_attr(Attrib *attr, int type, char *text) {
  /*  if (type == ID) {
    attr->kind = "id";
    attr->text = text;  
  }
  else {*/
    attr->kind = text;
    attr->text = "";
    //  }
}

// function to create a new AST node
AST* createASTnode(Attrib* attr, int type, char* text) {
  AST* as = new AST;
  as->kind = attr->kind; 
  as->text = attr->text;
  as->right = NULL; 
  as->down = NULL;
  return as;
}


/// create a new "list" AST node with one element
AST* createASTlist(AST *child) {
  AST *as=new AST;
  as->kind="list";
  as->right=NULL;
  as->down=child;
  return as;
}

/// get nth child of a tree. Count starts at 0.
/// if no such child, returns NULL
AST* child(AST *a,int n) {
  AST *c=a->down;
  for (int i=0; c!=NULL && i<n; i++) c=c->right;
  return c;
}



/// print AST, recursively, with indentation
void ASTPrintIndent(AST *a,string s)
{
  if (a==NULL) return;
  
  cout<<a->kind;
  if (a->text!="") cout<<"("<<a->text<<")";
  cout<<endl;
  
  AST *i = a->down;
  while (i!=NULL && i->right!=NULL) {
    cout<<s+"  \\__";
    ASTPrintIndent(i,s+"  |"+string(i->kind.size()+i->text.size(),' '));
    i=i->right;
  }
  
  if (i!=NULL) {
    cout<<s+"  \\__";
    ASTPrintIndent(i,s+"   "+string(i->kind.size()+i->text.size(),' '));
    i=i->right;
  }
}

/// print AST 
void ASTPrint(AST *a)
{
  while (a!=NULL) {
    cout<<" ";
    ASTPrintIndent(a,"");
    a=a->right;
  }
}

typedef struct {
  int x, y; // punt que determina la cantonada superior esquerra
  int h,w; // dimensions
} tblock;

typedef struct {
  int n, m;
  vector< vector<int> > height; 
  map<string, tblock> blocks;
} Graella;

Graella g;

/* Alhora dimplementar el push suposaré que posa el bloc al més abaix possible enlloc
* del més amunt possible. Sinó el primer While seria un bucle infinit a on laltura de B2 
* aniria incrementant a cada iteració.
*/

void executeListInstrucctions(AST *a);

int alt(string id) {
  //falta coses!!!
  tblock t = g.blocks[id];
  return g.height[t.x][t.y];
}

void defes(string name, AST *b) {
  if (b == NULL) {
    //cout << "ERROR: No existeix la funcio " << name << endl;
    return;
  }
  AST *aux = b->down;
  if (aux->kind == name) {
    executeListInstrucctions(aux->right); //el list
    return;
  }
  // si no es cap segueixo mirant a larbre si existeix 
  defes(name,b->right);
}

bool solapament(tblock t,int nivel) {
  for (int i = t.x; i < (t.h+t.x); ++i) {
    for (int j = t.y; j < (t.w+t.y); ++j) {
      if (g.height[i][j] != nivel) return true;
    }
  }
  return false;
}

bool fora(tblock t) {
  return (t.y > g.m or t.y < 1 or t.x > g.n or t.x < 1);
}

void alturaBloque(tblock t,int nivel) {
  for (int i = t.x; i < (t.h+t.x); ++i) {
    for (int j = t.y; j < (t.w+t.y); ++j) {
      g.height[i][j] = nivel;
    }
  }
}

void write(tblock t) {
  for (int j = t.y; j < (t.w+t.y); ++j) {
    for (int i = t.x; i < (t.h+t.x); ++i) {
      cout <<"i "<<i << " j " << j <<" "<< g.height[i][j] << endl;;
    }
  }
}

void placeame(string id, AST *a) {
  tblock t;
  AST *b = a->down;
  t.h = atoi(b->kind.c_str());
  t.w = atoi((b->right)->kind.c_str());
  b = a->right->down;
  t.x = atoi(b->kind.c_str());
  t.y = atoi((b->right)->kind.c_str());
  if (fora(t) or solapament(t,0)) return;
  g.blocks[id] = t;
  alturaBloque(t,1);
}


void moviendo(AST *a) {
  string id = a->kind;
  string direction = (a->right)->kind;
  int num = atoi((a->right->right)->kind.c_str());
  tblock t = g.blocks[id];
  tblock aux = t;
  int h = alt(id);
  if (direction == "WEST") {
    t.x -= num;
    if (fora(t) or solapament(t,0)) return;
    g.blocks[id] = t;
  } else if (direction == "EAST") {  
    t.x += num;
    if (fora(t) or solapament(t,0)) return;
    g.blocks[id] = t;
  } else if (direction == "NORTH") {
    t.y -= num;
    if (fora(t) or solapament(t,0)) return;
    g.blocks[id] = t;
  } else if (direction == "SOUTH") {
    t.y += num;
    if (fora(t) or solapament(t,0)) return;
    g.blocks[id] = t;
  }
  alturaBloque(t,h);
  alturaBloque(aux,0); //ja no hI ha block
}



pair<int,int> miniumHeight(tblock t) {
  int aux = 100000000;
  int max = 0;
  for (int i = t.x; i < (t.h+t.x); ++i) {
    for (int j = t.y; j < (t.w+t.y); ++j) {
      if (g.height[i][j] < aux) aux = g.height[i][j];
      if (g.height[i][j] > max) max = g.height[i][j];
    }
  }
  return make_pair(aux,max);
}

pair<int,int> posicio(tblock up, tblock down, int alt) {
  int mida = down.h * down.w;
  int a = down.x;
  int b = down.y;
  pair<int,int> p (-1,-1);
  int cont = 0;
  int c = 0;
  int px = a,py=b;
  while (cont < mida) {
    if (g.height[a][b] == alt) {
      if (c == 0) {
        px = a; py = b;
      }
      ++c;
    } else c = 0;
    if (c == (up.h*up.w)) return make_pair(px,py);
    ++a;
    if (a >= (up.h+down.x)) {
      ++b;
      a = down.x;
      if (b >= (down.y+down.w)) break;
    }
  }
  return p;
}

void pop(string idDown, string idUp) {
  map<string, tblock>::iterator it;
  it = g.blocks.find(idUp);
  if (it == g.blocks.end()) return;
  tblock up = it->second;
  it = g.blocks.find(idDown);
  if (it == g.blocks.end()) return;
  tblock down = it->second;
  alturaBloque(up,alt(idUp)-1);
  it = g.blocks.find(idUp);
  g.blocks.erase(it);
}

void popeame(string id, AST *a) {
  string idUp = a->kind;
  string idDown = (a->right)->kind;
  pop(idDown,idUp);
}

void pusheame(string id, AST *a) {
  string idUp = a->kind;
  tblock up;
  map<string, tblock>::iterator it;
  bool b = true;
  if (idUp == "list") {
    up.h= atoi((a->down)->kind.c_str());
    up.w = atoi((a->down->right)->kind.c_str());
    b = false;
  } else {
    it = g.blocks.find(idUp);
    if (it == g.blocks.end()) return;
    up = it->second;
  }
  string idDown;
  tblock down;
  if ((a->right)->kind == "POP") {
    AST *b = a->right->down;
    AST *c = b->right;
    pop(c->kind,b->kind);
    idDown = c->kind;
    down = g.blocks[idDown];
  } else {
    idDown = (a->right)->kind;
    it = g.blocks.find(idDown);
    if (it == g.blocks.end()) return;
    down = it->second;
  }
  if (up.h > down.h or up.w > down.w) return;
  pair<int,int> palt= miniumHeight(down); //això es altura més a lesquerra
  //comprova que hi capiga 
  pair<int,int> p;
  int min = palt.first;
  int max = palt.second;
  while (true) {
    p = posicio(up,down,min);
    if (p.first == -1 and min <= max) ++min;
    else break;
  }
  if (p.first == -1) return;
  up.x = p.first;
  up.y = p.second;
  int as;
  if (b) as = alt(idUp)+alt(idDown);
  else as = alt(idDown)+1;
  alturaBloque(up,as);
  g.blocks[idUp] = up;
  if (id != idDown) g.blocks[id] = up;
}

bool fits(AST *a) {
  string id = a->kind;
  tblock t;
  t.h = atoi((a->right->down)->kind.c_str());
  t.w = atoi((a->right->down->right)->kind.c_str());
  int h = atoi((a->right->right)->kind.c_str());
  
  map<string, tblock>::iterator it;
  it = g.blocks.find(id);
  if (it == g.blocks.end()) return false;
  tblock down = it->second;
  if (t.h > down.h or t.w > down.w) return false;
  pair<int,int> p = posicio(t,down,h-1);
  if (p.first == -1) return false;
  return true;
}

bool condicio(AST *a) {
  if (a == NULL) return true;
  if (a->kind == "FITS") {
    if (not fits(a->down)) return false;
  }
  else if (a->kind == "<"){
    int e,d;
    AST *b = a->down;
    if ((b)->kind == "HEIGHT") e = alt((b->down)->kind);
    else e = atoi(b->kind.c_str());
    b = b->right;
    if ((b)->kind == "HEIGHT") d = alt((b->down)->kind);
    else d = atoi(b->kind.c_str());
    if (not (e < d)) return false;
  }
  else if (a->kind == ">") {
    int e,d;
    AST *b = a->down;
    if ((b)->kind == "HEIGHT") e = alt((b->down)->kind);
    else e = atoi(b->kind.c_str());
    b = b->right;
    if ((b)->kind == "HEIGHT") d = alt((b->down)->kind);
    else d = atoi(b->kind.c_str());
    if (not (e > d)) return false;
  } else if (a->kind == "AND") {
    if (not condicio(a->down)) return false;
    if (not condicio(a->down->right)) return false;
  }
  return true;
}

void whileando(AST *a) {
  while (condicio(a)) {
    executeListInstrucctions(a->right);
  }
}

void executeListInstrucctions(AST *a) {
  if (a == NULL) return;  
  if (a->kind == "list") executeListInstrucctions(a->down);
  else if (a->kind == "Grid") {
    AST *b = a->down;
    g.n = atoi(b->kind.c_str());
    g.m = atoi((b->right)->kind.c_str());
    g.height = vector< vector<int> > (g.n+1,vector<int>(g.m+1,0));
  } else if (a->kind == "=") {
    //aqui hi ha place i pop
    AST *b = a->down;
    string id = b->kind;
    b = b->right;
    if (b->kind == "PLACE") placeame(id,b->down);
    else if (b->kind == "POP") popeame(id,b->down);
    else if (b->kind == "PUSH") pusheame(id,b->down);
  } else if (a->kind == "MOVE") moviendo(a->down);
  else if (a->kind == "WHILE") whileando(a->down);
  else if (a->kind == "HEIGHT") {
    string id = child(a,0)->kind;
    cout << alt(id) << endl;
  } else if (a->kind == "DEF") return;
  else { //sera un id
    
     string id = a->kind;
    AST *def = root->down->right->right->down; // hauria de ser la primera funcio(DEF)
    defes(id,def);
  }
  executeListInstrucctions(a->right);
  
}

int main() {
  root = NULL;
  ANTLR(lego(&root), stdin);
  ASTPrint(root);
  executeListInstrucctions(root->down);
  
}

void
#ifdef __USE_PROTOS
lego(AST**_root)
#else
lego(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  grid(zzSTR); zzlink(_root, &_sibling, &_tail);
  ops(zzSTR); zzlink(_root, &_sibling, &_tail);
  defs(zzSTR); zzlink(_root, &_sibling, &_tail);
  (*_root)=createASTlist(_sibling);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x1);
  }
}

void
#ifdef __USE_PROTOS
grid(AST**_root)
#else
grid(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(GRID); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x2);
  }
}

void
#ifdef __USE_PROTOS
ops(AST**_root)
#else
ops(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (setwd1[LA(1)]&0x4) ) {
      ops1(zzSTR); zzlink(_root, &_sibling, &_tail);
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  (*_root)=createASTlist(_sibling);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x8);
  }
}

void
#ifdef __USE_PROTOS
defs(AST**_root)
#else
defs(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==DEF) ) {
      defs2(zzSTR); zzlink(_root, &_sibling, &_tail);
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  (*_root)=createASTlist(_sibling);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x10);
  }
}

void
#ifdef __USE_PROTOS
ops1(AST**_root)
#else
ops1(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==ID) ) {
    zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
    {
      zzBLOCK(zztasp2);
      zzMake0;
      {
      if ( (LA(1)==ASSIG) ) {
        zzmatch(ASSIG); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
        expr(zzSTR); zzlink(_root, &_sibling, &_tail);
      }
      else {
        if ( (setwd1[LA(1)]&0x20) ) {
        }
        else {zzFAIL(1,zzerr1,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
      }
      zzEXIT(zztasp2);
      }
    }
  }
  else {
    if ( (LA(1)==MOVE) ) {
      {
        zzBLOCK(zztasp2);
        zzMake0;
        {
        zzmatch(MOVE); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
        mueveme(zzSTR); zzlink(_root, &_sibling, &_tail);
        zzEXIT(zztasp2);
        }
      }
    }
    else {
      if ( (LA(1)==HEIGHT) ) {
        altura(zzSTR); zzlink(_root, &_sibling, &_tail);
      }
      else {
        if ( (LA(1)==WHILE) ) {
          {
            zzBLOCK(zztasp2);
            zzMake0;
            {
            zzmatch(WHILE); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
            mentre(zzSTR); zzlink(_root, &_sibling, &_tail);
            zzEXIT(zztasp2);
            }
          }
        }
        else {zzFAIL(1,zzerr2,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
      }
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x40);
  }
}

void
#ifdef __USE_PROTOS
expr(AST**_root)
#else
expr(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==PLACE) ) {
    {
      zzBLOCK(zztasp2);
      zzMake0;
      {
      zzmatch(PLACE); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
      cord(zzSTR); zzlink(_root, &_sibling, &_tail);
      zzmatch(AT);  zzCONSUME;
      cord(zzSTR); zzlink(_root, &_sibling, &_tail);
      zzEXIT(zztasp2);
      }
    }
  }
  else {
    if ( (setwd1[LA(1)]&0x80) ) {
      pushpop(zzSTR); zzlink(_root, &_sibling, &_tail);
    }
    else {zzFAIL(1,zzerr3,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x1);
  }
}

void
#ifdef __USE_PROTOS
altura(AST**_root)
#else
altura(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(HEIGHT); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(LPAR);  zzCONSUME;
  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(RPAR);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x2);
  }
}

void
#ifdef __USE_PROTOS
pushpop(AST**_root)
#else
pushpop(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  id(zzSTR); zzlink(_root, &_sibling, &_tail);
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==PUSH) ) {
      zzmatch(PUSH); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
    }
    else {
      if ( (LA(1)==POP) ) {
        zzmatch(POP); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
      }
      else {zzFAIL(1,zzerr4,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  part2(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x4);
  }
}

void
#ifdef __USE_PROTOS
part2(AST**_root)
#else
part2(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==PUSH) ) {
      zzmatch(PUSH); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
      zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
    }
    else {
      if ( (LA(1)==POP) ) {
        zzmatch(POP); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
        zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
      }
      else {
        if ( (setwd2[LA(1)]&0x8) ) {
        }
        else {zzFAIL(1,zzerr5,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
      }
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x10);
  }
}

void
#ifdef __USE_PROTOS
mueveme(AST**_root)
#else
mueveme(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==WEST) ) {
      zzmatch(WEST); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
    }
    else {
      if ( (LA(1)==EAST) ) {
        zzmatch(EAST); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
      }
      else {
        if ( (LA(1)==NORTH) ) {
          zzmatch(NORTH); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
        }
        else {
          if ( (LA(1)==SOUTH) ) {
            zzmatch(SOUTH); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
          }
          else {zzFAIL(1,zzerr6,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
      }
    }
    zzEXIT(zztasp2);
    }
  }
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x20);
  }
}

void
#ifdef __USE_PROTOS
id(AST**_root)
#else
id(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==ID) ) {
    zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  }
  else {
    if ( (LA(1)==LPAR) ) {
      cord(zzSTR); zzlink(_root, &_sibling, &_tail);
    }
    else {zzFAIL(1,zzerr7,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x40);
  }
}

void
#ifdef __USE_PROTOS
cord(AST**_root)
#else
cord(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(LPAR);  zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(COMA);  zzCONSUME;
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(RPAR); 
  (*_root)=createASTlist(_sibling);
 zzCONSUME;

  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd2, 0x80);
  }
}

void
#ifdef __USE_PROTOS
mentre(AST**_root)
#else
mentre(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(LPAR);  zzCONSUME;
  cond(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(RPAR);  zzCONSUME;
  zzmatch(LCLAU);  zzCONSUME;
  ops(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(RCLAU);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x1);
  }
}

void
#ifdef __USE_PROTOS
cond(AST**_root)
#else
cond(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  boolea(zzSTR); zzlink(_root, &_sibling, &_tail);
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    if ( (LA(1)==AND) ) {
      zzmatch(AND); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
      cond(zzSTR); zzlink(_root, &_sibling, &_tail);
    }
    else {
      if ( (LA(1)==RPAR) ) {
      }
      else {zzFAIL(1,zzerr8,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
    }
    zzEXIT(zztasp2);
    }
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x2);
  }
}

void
#ifdef __USE_PROTOS
boolea(AST**_root)
#else
boolea(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (setwd3[LA(1)]&0x4) ) {
    {
      zzBLOCK(zztasp2);
      zzMake0;
      {
      nums(zzSTR); zzlink(_root, &_sibling, &_tail);
      {
        zzBLOCK(zztasp3);
        zzMake0;
        {
        if ( (LA(1)==MAJOR) ) {
          zzmatch(MAJOR); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
        }
        else {
          if ( (LA(1)==MENOR) ) {
            zzmatch(MENOR); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
          }
          else {zzFAIL(1,zzerr9,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
        }
        zzEXIT(zztasp3);
        }
      }
      nums(zzSTR); zzlink(_root, &_sibling, &_tail);
      zzEXIT(zztasp2);
      }
    }
  }
  else {
    if ( (LA(1)==FITS) ) {
      {
        zzBLOCK(zztasp2);
        zzMake0;
        {
        fiteame(zzSTR); zzlink(_root, &_sibling, &_tail);
        zzEXIT(zztasp2);
        }
      }
    }
    else {zzFAIL(1,zzerr10,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x8);
  }
}

void
#ifdef __USE_PROTOS
fiteame(AST**_root)
#else
fiteame(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(FITS); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(LPAR);  zzCONSUME;
  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(COMA);  zzCONSUME;
  restfits(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(RPAR);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x10);
  }
}

void
#ifdef __USE_PROTOS
restfits(AST**_root)
#else
restfits(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  nums(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(COMA);  zzCONSUME;
  nums(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(COMA); 
  (*_root)=createASTlist(_sibling);
 zzCONSUME;

  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x20);
  }
}

void
#ifdef __USE_PROTOS
nums(AST**_root)
#else
nums(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  if ( (LA(1)==NUM) ) {
    zzmatch(NUM); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  }
  else {
    if ( (LA(1)==HEIGHT) ) {
      altura(zzSTR); zzlink(_root, &_sibling, &_tail);
    }
    else {zzFAIL(1,zzerr11,&zzMissSet,&zzMissText,&zzBadTok,&zzBadText,&zzErrk); goto fail;}
  }
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x40);
  }
}

void
#ifdef __USE_PROTOS
defs2(AST**_root)
#else
defs2(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  zzmatch(DEF); zzsubroot(_root, &_sibling, &_tail); zzCONSUME;
  zzmatch(ID); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  ops(zzSTR); zzlink(_root, &_sibling, &_tail);
  zzmatch(ENDEF);  zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd3, 0x80);
  }
}
