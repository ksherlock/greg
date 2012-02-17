/* Copyright (c) 2007 by Ian Piumarta
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the 'Software'),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, provided that the above copyright notice(s) and this
 * permission notice appear in all copies of the Software.  Acknowledgement
 * of the use of this Software in supporting documentation would be
 * appreciated but is not required.
 * 
 * THE SOFTWARE IS PROVIDED 'AS IS'.  USE ENTIRELY AT YOUR OWN RISK.
 * 
 * Last edited: 2007-08-31 13:55:23 by piumarta on emilia.local
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "greg.h"

static int yyl(void)
{
  static int prev= 0;
  return ++prev;
}

static void charClassSet  (unsigned char bits[], int c) { bits[c >> 3] |=  (1 << (c & 7)); }
static void charClassClear(unsigned char bits[], int c) { bits[c >> 3] &= ~(1 << (c & 7)); }

typedef void (*setter)(unsigned char bits[], int c);

static int readChar(unsigned char **cp)
{
  unsigned char *cclass = *cp;
  int c= *cclass++, i = 0;
  if ('\\' == c && *cclass)
        {
    c= *cclass++;
    if (c >= '0' && c <= '9')
      {
        unsigned char oct= 0;
        for (i= 2; i >= 0; i--) {
          if (!(c >= '0' && c <= '9'))
            break;
          oct= (oct * 8) + (c - '0');
          c= *cclass++;
        }
        cclass--;
        c= oct;
        goto done;
      }

    switch (c)
      {
      case 'a':  c= '\a'; break;        /* bel */
      case 'b':  c= '\b'; break;        /* bs */
      case 'e':  c= '\e'; break;        /* esc */
      case 'f':  c= '\f'; break;        /* ff */
      case 'n':  c= '\n'; break;        /* nl */
      case 'r':  c= '\r'; break;        /* cr */
      case 't':  c= '\t'; break;        /* ht */
      case 'v':  c= '\v'; break;        /* vt */
      default:          break;
      }
        }

done:
  *cp = cclass;
  return c;
}

static char *makeCharClass(unsigned char *cclass)
{
  unsigned char  bits[32];
  setter         set;
  int            c, prev= -1;
  static char    string[256];
  char          *ptr;

  if ('^' == *cclass)
    {
      memset(bits, 255, 32);
      set= charClassClear;
      ++cclass;
    }
  else
    {
      memset(bits, 0, 32);
      set= charClassSet;
    }
  while (0 != (c= readChar(&cclass)))
    {
      if ('-' == c && *cclass && prev >= 0)
        {
          for (c= readChar(&cclass); prev <= c; ++prev)
            set(bits, prev);
          prev= -1;
        }
      else
  {
    set(bits, prev= c);
  }
    }

  ptr= string;
  for (c= 0;  c < 32;  ++c)
    ptr += sprintf(ptr, "\\%03o", bits[c]);

  return string;
}

static void begin(void)         { fprintf(output, "\n  {"); }
static void end(void)           { fprintf(output, "\n  }"); }
static void label(int n)        { fprintf(output, "\n  l%d:;\t", n); }
static void jump(int n)         { fprintf(output, "  goto l%d;", n); }
static void save(int n)         { fprintf(output, "  int yypos%d= G->pos, yythunkpos%d= G->thunkpos;", n, n); }
static void restore(int n)      { fprintf(output,     "  G->pos= yypos%d; G->thunkpos= yythunkpos%d;", n, n); }

static void callErrBlock(Node * node) {
    fprintf(output, " { YY_XTYPE YY_XVAR = (YY_XTYPE) G->data; int yyindex = G->offset + G->pos; %s; }", ((struct Any*) node)->errblock);
}

static void Node_compile_c_ko(Node *node, int ko)
{
  assert(node);
  switch (node->type)
    {
    case Rule:
      fprintf(stderr, "\ninternal error #1 (%s)\n", node->rule.name);
      exit(1);
      break;

    case Dot:
      fprintf(output, "  if (!yymatchDot(G)) goto l%d;", ko);
      break;

    case Name:
      {
        fprintf(output, "  if (!yy_%s(G)) {", node->name.rule->rule.name);
        if(((struct Any*) node)->errblock) {
            callErrBlock(node);
        }
        fprintf(output, " goto l%d; }", ko);
        if (node->name.variable)
          fprintf(output, "  yyDo(G, yySet, %d, 0);", node->name.variable->variable.offset);
      }
      break;

    case Character:
    case String:
      {
        int len= strlen(node->string.value);
        if (1 == len || (2 == len && '\\' == node->string.value[0]))
          fprintf(output, "  if (!yymatchChar(G, '%s')) goto l%d;", node->string.value, ko);
        else
          fprintf(output, "  if (!yymatchString(G, \"%s\")) goto l%d;", node->string.value, ko);
      }
      break;

    case Class:
      fprintf(output, "  if (!yymatchClass(G, (unsigned char *)\"%s\")) goto l%d;", makeCharClass(node->cclass.value), ko);
      break;

    case Action:
      fprintf(output, "  yyDo(G, yy%s, yytextpos?yytextpos:G->begin, G->end);", node->action.name);
      break;

    case Predicate:
      fprintf(output, "  yyText(G, yytextpos?yytextpos:G->begin, G->end);  if (!(%s)) goto l%d;", node->action.text, ko);
      break;

    case Alternate:
      {
        int ok= yyl();
        begin();
        save(ok);
        for (node= node->alternate.first;  node;  node= node->alternate.next)
          if (node->alternate.next)
            {
              int next= yyl();
              Node_compile_c_ko(node, next);
              jump(ok);
              label(next);
              restore(ok);
            }
          else
            Node_compile_c_ko(node, ko);
        end();
        label(ok);
      }
      break;

    case Sequence:
      for (node= node->sequence.first;  node;  node= node->sequence.next)
        Node_compile_c_ko(node, ko);
      break;

    case PeekFor:
      {
        int ok= yyl();
        begin();
        save(ok);
        Node_compile_c_ko(node->peekFor.element, ko);
        restore(ok);
        end();
      }
      break;

    case PeekNot:
      {
        int ok= yyl();
        begin();
        save(ok);
        Node_compile_c_ko(node->peekFor.element, ok);
        jump(ko);
        label(ok);
        restore(ok);
        end();
      }
      break;

    case Query:
      {
        int qko= yyl(), qok= yyl();
        begin();
        save(qko);
        Node_compile_c_ko(node->query.element, qko);
        jump(qok);
        label(qko);
        restore(qko);
        end();
        label(qok);
      }
      break;

    case Star:
      {
        int again= yyl(), out= yyl();
        label(again);
        begin();
        save(out);
        Node_compile_c_ko(node->star.element, out);
        jump(again);
        label(out);
        restore(out);
        end();
      }
      break;

    case Plus:
      {
        int again= yyl(), out= yyl();
        Node_compile_c_ko(node->plus.element, ko);
        label(again);
        begin();
        save(out);
        Node_compile_c_ko(node->plus.element, out);
        jump(again);
        label(out);
        restore(out);
        end();
      }
      break;

    default:
      fprintf(stderr, "\nNode_compile_c_ko: illegal node type %d\n", node->type);
      exit(1);
    }
}


static int countVariables(Node *node)
{
  int count= 0;
  while (node)
    {
      ++count;
      node= node->variable.next;
    }
  return count;
}

static void defineVariables(Node *node)
{
  int count= 0;
  while (node)
    {
      fprintf(output, "#define %s G->val[%d]\n", node->variable.name, --count);
      node->variable.offset= count;
      node= node->variable.next;
    }
}

static void undefineVariables(Node *node)
{
  while (node)
    {
      fprintf(output, "#undef %s\n", node->variable.name);
      node= node->variable.next;
    }
}


static void Rule_compile_c2(Node *node)
{
  assert(node);
  assert(Rule == node->type);

  if (!node->rule.expression)
    fprintf(stderr, "rule '%s' used but not defined\n", node->rule.name);
  else
    {
      int ko= yyl(), safe;

      if ((!(RuleUsed & node->rule.flags)) && (node != start))
        fprintf(stderr, "rule '%s' defined but not used\n", node->rule.name);

      safe= ((Query == node->rule.expression->type) || (Star == node->rule.expression->type));

      fprintf(output, "\nYY_RULE(int) yy_%s(GREG *G)\n{", node->rule.name);
      fprintf(output, "  int yytextpos = 0; yytextpos=yytextpos;\n");
      if (!safe) save(0);
      if (node->rule.variables)
        fprintf(output, "  yyDo(G, yyPush, %d, 0);", countVariables(node->rule.variables));
      fprintf(output, "\n  yyprintf((stderr, \"%%s\\n\", \"%s\"));", node->rule.name);
      Node_compile_c_ko(node->rule.expression, ko);
      fprintf(output, "\n  yyprintf((stderr, \"  ok   %%s @ %%s\\n\", \"%s\", G->buf+G->pos));", node->rule.name);
      if (node->rule.variables)
        fprintf(output, "  yyDo(G, yyPop, %d, 0);", countVariables(node->rule.variables));
      fprintf(output, "\n  return 1;");
      if (!safe)
        {
          label(ko);
          restore(0);
          fprintf(output, "\n  yyprintf((stderr, \"  fail %%s @ %%s\\n\", \"%s\", G->buf+G->pos));", node->rule.name);
          fprintf(output, "\n  return 0;");
        }
      fprintf(output, "\n}");
    }

  if (node->rule.next)
    Rule_compile_c2(node->rule.next);
}


void Rule_compile_c_header(void)
{
  extern const char *header;
  fprintf(output, "/* A recursive-descent parser generated by greg %d.%d.%d */\n", GREG_MAJOR, GREG_MINOR, GREG_LEVEL);
  fprintf(output, "\n");
  fprintf(output, "%s", header);
  fprintf(output, "#define YYRULECOUNT %d\n", ruleCount);
}

int consumesInput(Node *node)
{
  if (!node) return 0;

  switch (node->type)
    {
    case Rule:
      {
        int result= 0;
        if (RuleReached & node->rule.flags)
          fprintf(stderr, "possible infinite left recursion in rule '%s'\n", node->rule.name);
        else
          {
            node->rule.flags |= RuleReached;
            result= consumesInput(node->rule.expression);
            node->rule.flags &= ~RuleReached;
          }
        return result;
      }
      break;

    case Dot:           return 1;
    case Name:          return consumesInput(node->name.rule);
    case Character:
    case String:        return strlen(node->string.value) > 0;
    case Class:         return 1;
    case Action:        return 0;
    case Predicate:     return 0;

    case Alternate:
      {
        Node *n;
        for (n= node->alternate.first;  n;  n= n->alternate.next)
          if (!consumesInput(n))
            return 0;
      }
      return 1;

    case Sequence:
      {
        Node *n;
        for (n= node->alternate.first;  n;  n= n->alternate.next)
          if (consumesInput(n))
            return 1;
      }
      return 0;

    case PeekFor:       return 0;
    case PeekNot:       return 0;
    case Query:         return 0;
    case Star:          return 0;
    case Plus:          return consumesInput(node->plus.element);

    default:
      fprintf(stderr, "\nconsumesInput: illegal node type %d\n", node->type);
      exit(1);
    }
  return 0;
}


void Rule_compile_c(Node *node)
{
  extern const char *preamble;
  extern const char *footer;
  
  Node *n;

  for (n= rules;  n;  n= n->rule.next)
    consumesInput(n);

  fprintf(output, "%s", preamble);
  for (n= node;  n;  n= n->rule.next)
    fprintf(output, "YY_RULE(int) yy_%s(GREG *G); /* %d */\n", n->rule.name, n->rule.id);
  fprintf(output, "\n");
  for (n= actions;  n;  n= n->action.list)
    {
      fprintf(output, "YY_ACTION(void) yy%s(GREG *G, char *yytext, int yyleng, yythunk *thunk, YY_XTYPE YY_XVAR)\n{\n", n->action.name);
      defineVariables(n->action.rule->rule.variables);
      fprintf(output, "  yyprintf((stderr, \"do yy%s\\n\"));\n", n->action.name);
      fprintf(output, "  %s;\n", n->action.text);
      undefineVariables(n->action.rule->rule.variables);
      fprintf(output, "}\n");
    }
  Rule_compile_c2(node);
  
  fprintf(output, "%s", footer);

  fprintf(output,
    "YY_PARSE(int) YY_NAME(parse)(GREG *G)\n"
    "{\n"
    "  return YY_NAME(parse_from)(G, yy_%s);\n"
    "}\n",
    start->rule.name
  );
  
}
