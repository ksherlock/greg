const char *preamble = 
  "/* begin preamble.template */\n"
  "\n"
  "#ifndef YY_ALLOC\n"
  "#define YY_ALLOC(N, D) malloc(N)\n"
  "#endif\n"
  "#ifndef YY_CALLOC\n"
  "#define YY_CALLOC(N, S, D) calloc(N, S)\n"
  "#endif\n"
  "#ifndef YY_REALLOC\n"
  "#define YY_REALLOC(B, N, D) realloc(B, N)\n"
  "#endif\n"
  "#ifndef YY_FREE\n"
  "#define YY_FREE free\n"
  "#endif\n"
  "#ifndef YY_LOCAL\n"
  "#define YY_LOCAL(T)     static T\n"
  "#endif\n"
  "#ifndef YY_ACTION\n"
  "#define YY_ACTION(T)    static T\n"
  "#endif\n"
  "#ifndef YY_RULE\n"
  "#define YY_RULE(T)      static T\n"
  "#endif\n"
  "#ifndef YY_PARSE\n"
  "#define YY_PARSE(T)     T\n"
  "#endif\n"
  "#ifndef YY_NAME\n"
  "#define YY_NAME(N) yy##N\n"
  "#endif\n"
  "#ifndef YY_INPUT\n"
  "#define YY_INPUT(buf, result, max_size, D)              \\\n"
  "  {                                                     \\\n"
  "    int yyc= getchar();                                 \\\n"
  "    result= (EOF == yyc) ? 0 : (*(buf)= yyc, 1);        \\\n"
  "    yyprintf((stderr, \"<%c>\", yyc));                    \\\n"
  "  }\n"
  "#endif\n"
  "#ifndef YY_BEGIN\n"
  "#define YY_BEGIN        ( yytextpos= G->begin= G->pos, 1)\n"
  "#endif\n"
  "#ifndef YY_END\n"
  "#define YY_END          ( G->end= G->pos, 1)\n"
  "#endif\n"
  "#ifdef YY_DEBUG\n"
  "# define yyprintf(args) fprintf args\n"
  "#else\n"
  "# define yyprintf(args)\n"
  "#endif\n"
  "#ifndef YYSTYPE\n"
  "#define YYSTYPE int\n"
  "#endif\n"
  "#ifndef YY_XTYPE\n"
  "#define YY_XTYPE void *\n"
  "#endif\n"
  "#ifndef YY_XVAR\n"
  "#define YY_XVAR yyxvar\n"
  "#endif\n"
  "\n"
  "#ifndef YY_STACK_SIZE\n"
  "#define YY_STACK_SIZE 128\n"
  "#endif\n"
  "\n"
  "#ifndef YY_BUFFER_START_SIZE\n"
  "#define YY_BUFFER_START_SIZE 1024\n"
  "#endif\n"
  "\n"
  "#ifndef YY_PART\n"
  "#define yydata G->data\n"
  "#define yy G->ss\n"
  "\n"
  "struct _yythunk; // forward declaration\n"
  "typedef void (*yyaction)(struct _GREG *G, char *yytext, int yyleng, struct _yythunk *thunkpos, YY_XTYPE YY_XVAR);\n"
  "typedef struct _yythunk { int begin, end;  yyaction  action;  struct _yythunk *next; } yythunk;\n"
  "\n"
  "typedef struct _GREG {\n"
  "  char *buf;\n"
  "  int buflen;\n"
  "  int   offset;\n"
  "  int   pos;\n"
  "  int   limit;\n"
  "  char *text;\n"
  "  int   textlen;\n"
  "  int   begin;\n"
  "  int   end;\n"
  "  yythunk *thunks;\n"
  "  int   thunkslen;\n"
  "  int thunkpos;\n"
  "  YYSTYPE ss;\n"
  "  YYSTYPE *val;\n"
  "  YYSTYPE *vals;\n"
  "  int valslen;\n"
  "  YY_XTYPE data;\n"
  "} GREG;\n"
  "\n"
  "YY_LOCAL(int) yyrefill(GREG *G)\n"
  "{\n"
  "  int yyn;\n"
  "  while (G->buflen - G->pos < 512)\n"
  "    {\n"
  "      G->buflen *= 2;\n"
  "      G->buf= (char*)YY_REALLOC(G->buf, G->buflen, G->data);\n"
  "    }\n"
  "  YY_INPUT((G->buf + G->pos), yyn, (G->buflen - G->pos), G->data);\n"
  "  if (!yyn) return 0;\n"
  "  G->limit += yyn;\n"
  "  return 1;\n"
  "}\n"
  "\n"
  "YY_LOCAL(int) yymatchDot(GREG *G)\n"
  "{\n"
  "  if (G->pos >= G->limit && !yyrefill(G)) return 0;\n"
  "  ++G->pos;\n"
  "  return 1;\n"
  "}\n"
  "\n"
  "YY_LOCAL(int) yymatchChar(GREG *G, int c)\n"
  "{\n"
  "  if (G->pos >= G->limit && !yyrefill(G)) return 0;\n"
  "  if ((unsigned char)G->buf[G->pos] == c)\n"
  "    {\n"
  "      ++G->pos;\n"
  "      yyprintf((stderr, \"  ok   yymatchChar(%c) @ %s\\n\", c, G->buf+G->pos));\n"
  "      return 1;\n"
  "    }\n"
  "  yyprintf((stderr, \"  fail yymatchChar(%c) @ %s\\n\", c, G->buf+G->pos));\n"
  "  return 0;\n"
  "}\n"
  "\n"
  "YY_LOCAL(int) yymatchString(GREG *G, const char *s)\n"
  "{\n"
  "  int yysav= G->pos;\n"
  "  while (*s)\n"
  "    {\n"
  "      if (G->pos >= G->limit && !yyrefill(G)) return 0;\n"
  "      if (G->buf[G->pos] != *s)\n"
  "        {\n"
  "          G->pos= yysav;\n"
  "          return 0;\n"
  "        }\n"
  "      ++s;\n"
  "      ++G->pos;\n"
  "    }\n"
  "  return 1;\n"
  "}\n"
  "\n"
  "YY_LOCAL(int) yymatchClass(GREG *G, unsigned char *bits)\n"
  "{\n"
  "  int c;\n"
  "  if (G->pos >= G->limit && !yyrefill(G)) return 0;\n"
  "  c= (unsigned char)G->buf[G->pos];\n"
  "  if (bits[c >> 3] & (1 << (c & 7)))\n"
  "    {\n"
  "      ++G->pos;\n"
  "      yyprintf((stderr, \"  ok   yymatchClass @ %s\\n\", G->buf+G->pos));\n"
  "      return 1;\n"
  "    }\n"
  "  yyprintf((stderr, \"  fail yymatchClass @ %s\\n\", G->buf+G->pos));\n"
  "  return 0;\n"
  "}\n"
  "\n"
  "YY_LOCAL(void) yyDo(GREG *G, yyaction action, int begin, int end)\n"
  "{\n"
  "  while (G->thunkpos >= G->thunkslen)\n"
  "    {\n"
  "      G->thunkslen *= 2;\n"
  "      G->thunks= (yythunk*)YY_REALLOC(G->thunks, sizeof(yythunk) * G->thunkslen, G->data);\n"
  "    }\n"
  "  G->thunks[G->thunkpos].begin=  begin;\n"
  "  G->thunks[G->thunkpos].end=    end;\n"
  "  G->thunks[G->thunkpos].action= action;\n"
  "  ++G->thunkpos;\n"
  "}\n"
  "\n"
  "YY_LOCAL(int) yyText(GREG *G, int begin, int end)\n"
  "{\n"
  "  int yyleng= end - begin;\n"
  "  if (yyleng <= 0)\n"
  "    yyleng= 0;\n"
  "  else\n"
  "    {\n"
  "      while (G->textlen < (yyleng - 1))\n"
  "        {\n"
  "          G->textlen *= 2;\n"
  "          G->text= (char*)YY_REALLOC(G->text, G->textlen, G->data);\n"
  "        }\n"
  "      memcpy(G->text, G->buf + begin, yyleng);\n"
  "    }\n"
  "  G->text[yyleng]= '\\0';\n"
  "  return yyleng;\n"
  "}\n"
  "\n"
  "YY_LOCAL(void) yyDone(GREG *G)\n"
  "{\n"
  "  int pos;\n"
  "  for (pos= 0; pos < G->thunkpos; ++pos)\n"
  "    {\n"
  "      yythunk *thunk= &G->thunks[pos];\n"
  "      int yyleng= thunk->end ? yyText(G, thunk->begin, thunk->end) : thunk->begin;\n"
  "      yyprintf((stderr, \"DO [%d] %p %s\\n\", pos, thunk->action, G->text));\n"
  "      thunk->action(G, G->text, yyleng, thunk, G->data);\n"
  "    }\n"
  "  G->thunkpos= 0;\n"
  "}\n"
  "\n"
  "YY_LOCAL(void) yyCommit(GREG *G)\n"
  "{\n"
  "  if ((G->limit -= G->pos))\n"
  "    {\n"
  "      memmove(G->buf, G->buf + G->pos, G->limit);\n"
  "    }\n"
  "  G->offset += G->pos;\n"
  "  G->begin -= G->pos;\n"
  "  G->end -= G->pos;\n"
  "  G->pos= G->thunkpos= 0;\n"
  "}\n"
  "\n"
  "YY_LOCAL(int) yyAccept(GREG *G, int tp0)\n"
  "{\n"
  "  if (tp0)\n"
  "    {\n"
  "      fprintf(stderr, \"accept denied at %d\\n\", tp0);\n"
  "      return 0;\n"
  "    }\n"
  "  else\n"
  "    {\n"
  "      yyDone(G);\n"
  "      yyCommit(G);\n"
  "    }\n"
  "  return 1;\n"
  "}\n"
  "\n"
  "YY_LOCAL(void) yyPush(GREG *G, char *text, int count, yythunk *thunk, YY_XTYPE YY_XVAR) { G->val += count; }\n"
  "YY_LOCAL(void) yyPop(GREG *G, char *text, int count, yythunk *thunk, YY_XTYPE YY_XVAR)  { G->val -= count; }\n"
  "YY_LOCAL(void) yySet(GREG *G, char *text, int count, yythunk *thunk, YY_XTYPE YY_XVAR)  { G->val[count]= G->ss; }\n"
  "\n"
  "#endif /* YY_PART */\n"
  "\n"
  "#define YYACCEPT        yyAccept(G, yythunkpos0)\n"
  "\n"
  "/* end preamble.template */"
;
