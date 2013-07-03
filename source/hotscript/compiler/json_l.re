#include <stdio.h>

#include "hotpot/hp_platform.h"
#include "hotpot/hp_error.h"
#include "json_y.h"
#include "json_parser.h"
#include "json_l.h"
#include "hotscript/hotlex.h"

static void hotscript_reserved_keyword(char* keyword)
{
	//yyerror(&yylloc, hotscript_parameter, "Cannot use reserved language keyword: \"%s\"\n", keyword);
}



static void hexToDigit(unsigned int * val, const unsigned char * hex)
{
    unsigned int i;
    for (i=0;i<4;i++) {
        unsigned char c = hex[i];
        if (c >= 'A') c = (c & ~0x20) - 7;
        c -= '0';
        *val = (*val << 4) | c;
    }
}

static size_t Utf32toUtf8(unsigned int codepoint, char * utf8Buf) 
{
    if (codepoint < 0x80) {
        utf8Buf[0] = (char) codepoint;
        utf8Buf[1] = 0;
        return 1;
    } else if (codepoint < 0x0800) {
        utf8Buf[0] = (char) ((codepoint >> 6) | 0xC0);
        utf8Buf[1] = (char) ((codepoint & 0x3F) | 0x80);
        utf8Buf[2] = 0;
        return 2;
    } else if (codepoint < 0x10000) {
        utf8Buf[0] = (char) ((codepoint >> 12) | 0xE0);
        utf8Buf[1] = (char) (((codepoint >> 6) & 0x3F) | 0x80);
        utf8Buf[2] = (char) ((codepoint & 0x3F) | 0x80);
        utf8Buf[3] = 0;
        return 3;
    } else if (codepoint < 0x200000) {
        utf8Buf[0] =(char)((codepoint >> 18) | 0xF0);
        utf8Buf[1] =(char)(((codepoint >> 12) & 0x3F) | 0x80);
        utf8Buf[2] =(char)(((codepoint >> 6) & 0x3F) | 0x80);
        utf8Buf[3] =(char)((codepoint & 0x3F) | 0x80);
        utf8Buf[4] = 0;
        return 4;
    } else {
        utf8Buf[0] = '?';
        utf8Buf[1] = 0;
        //error?
        return 2;
    }
}
#include "hotscript/hotlex.h"

hpint32 json_lex_scan(SCANNER *self, YYLTYPE *yylloc, YYSTYPE * yylval)
{
restart:
	if(YYCURSOR >= YYLIMIT)
	{
		return 0;
	}
	yylloc->first_line = self->yylineno;
	yylloc->first_column = self->yycolumn;
	yytext = YYCURSOR;
/*!re2c
re2c:yyfill:check = 0;


newline			("\r"|"\n"|"\r\n")
whitespace		[ \t\r\n]*
symbols			[,:\[\]\{\}]
string_begin	['\"']
intconstant		([+-]?[0-9]+)
LNUM			[0-9]+
DNUM			([0-9]*"."[0-9]+)|([0-9]+"."[0-9]*)
EXPONENT_DNUM	(({LNUM}|{DNUM})[eE][+-]?{LNUM})


<!*> := yyleng = YYCURSOR - yytext; scanner_process(self);

<*> "<%" { return tok_script_begin; }

<INITIAL>{symbols} {
	return *yytext;
}

<INITIAL>{DNUM}|{EXPONENT_DNUM}
{	
	//这里有问题
	yylval->type = E_HP_BYTES;
	yylval->val.bytes.ptr = yytext;
	yylval->val.bytes.len = yyleng;
	return tok_double;
}

<INITIAL>{intconstant}
{
	return tok_integer;
}

<INITIAL>"true"
{
	return tok_true;
}

<INITIAL>"false"
{
	return tok_false;
}

<INITIAL>"null"
{
	return tok_null;
}
	
<INITIAL>{string_begin} {
  char mark = *yytext;
  //YYCURSOR只比yytext大1哦~

  //最大字符串的限制
  char *str = malloc(1024);
  yylval->val.bytes.ptr = str;
  yylval->type = E_HP_BYTES;
  yylval->val.bytes.len = 0;  
  while (YYCURSOR < YYLIMIT)
  {
	unsigned char ch = *YYCURSOR;
	++YYCURSOR;
    switch (ch)
    {
      case '\\':
		++YYCURSOR;
		if (YYCURSOR >= YYLIMIT) {
			return 0;
		}
        switch (*YYCURSOR)
        {
          case '"':
			str[(yylval->val.bytes.len)++] = '"';
            continue;
          case '\\':
			str[(yylval->val.bytes.len)++] = '\\';
            continue;
          case '/':
			str[(yylval->val.bytes.len)++] = '/';
            continue;
          case 'b':
			str[(yylval->val.bytes.len)++] = '\b';
            continue;
          case 'f':
			str[(yylval->val.bytes.len)++] = '\f';
            continue;
          case 'n':
			str[(yylval->val.bytes.len)++] = '\n';
            continue;
          case 'r':
			str[(yylval->val.bytes.len)++] = '\r';
            continue;
          case 't':
			str[(yylval->val.bytes.len)++] = '\t';
            continue;
		  case 'u':
		  {
			char hex_number[5];
			unsigned int i;
			unsigned int d;
			for(i = 0;i < 4; ++i)
			{
				++YYCURSOR;
				if (YYCURSOR >= YYLIMIT) {
					return 0;
				}
				hex_number[i] = *YYCURSOR;
			}
			//这里要进行错误处理
			hexToDigit(&d, hex_number);
			yylval->val.bytes.len += Utf32toUtf8(d, yylval->val.bytes.ptr + (yylval->val.bytes.len));
			
			continue;
		  }
          default:
            //yyerror("bad escape character\n");
            return -1;
        }
        break;
      default:
        if (ch == mark)
        {
			str[yylval->val.bytes.len] = 0;
			yylval->type = E_HP_BYTES;
			return tok_string;
        }
        else
        {
          str[(yylval->val.bytes.len)++] = ch;
        }
    }
  }
}

<INITIAL>. | '\n' {
	goto restart;
}

*/
return 0;
}
