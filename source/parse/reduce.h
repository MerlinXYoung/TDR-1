#ifndef _H_REDUCE
#define _H_REDUCE


#include "platform/tlibc_platform.h"

#include "parse/parser.h"

void dp_reduce_Definition_Import(PARSER *self, ST_DEFINITION *pn_current, const ST_Import* pn_import);

void dp_reduce_Definition_Const(PARSER *self, ST_DEFINITION *pn_current, const ST_Const* pn_const);

void dp_reduce_Definition_Typedef(PARSER *self, ST_DEFINITION *pn_current, const ST_TYPEDEF* pn_typedef);

void dp_reduce_Import(PARSER *self, ST_Import* current, const tstring str);

void dp_reduce_ContainerType_tok_t_vector(PARSER *self, ST_TYPE *current, const ST_SIMPLE_TYPE *simple_type, const tbytes *tok_identifier);

void dp_reduce_SimpleType_tok_t_string(PARSER *self, ST_SIMPLE_TYPE *current, const tbytes *tok_identifier);

void dp_reduce_SimpleType_tok_identifier(PARSER *self, ST_SIMPLE_TYPE* current, const tbytes *tok_identifier);

void dp_reduce_Type_SimpleType(PARSER *self, ST_TYPE *current, const ST_SIMPLE_TYPE *simple_type);

void dp_reduce_Value_tok_identifier(PARSER *self, ST_VALUE* current, const tbytes sn_identifier);

void dp_reduce_Value_tok_char(PARSER *self, ST_VALUE* current, const tchar pn_char);

void dp_reduce_Value_tok_int64(PARSER *self, ST_VALUE* current, const tint64 i64);

void dp_reduce_Value_tok_hex_int64(PARSER *self, ST_VALUE* current, const tint64 i64);

void dp_reduce_Value_tok_uint64(PARSER *self, ST_VALUE* current, const tuint64 ui64);

void dp_reduce_Value_tok_hex_uint64(PARSER *self, ST_VALUE* current, const tuint64 ui64);

void dp_reduce_Value_tok_double(PARSER *self, ST_VALUE* current, const double d);

void dp_reduce_Value_tok_bool(PARSER *self, ST_VALUE* current, const tbool b);

void dp_reduce_Value_tok_string(PARSER *self, ST_VALUE* current, const tstring str);


void dp_reduce_Const(PARSER *self, ST_Const* current, const ST_SIMPLE_TYPE *type, const tbytes *identifier, const ST_VALUE *val);

void dp_reduce_ArgumentList_tok_identifier(PARSER *self, ST_ARGUMENTS* current, const tbytes *identifier);

void dp_reduce_ArgumentList_ArgumentList_tok_identifier(PARSER *self, ST_ARGUMENTS* current, const ST_ARGUMENTS* argument_list, const tbytes *identifier);

void dp_reduce_Typedef(PARSER *self, ST_TYPEDEF *current, ST_SIMPLE_TYPE* type, const tbytes *tok_identifier);

void dp_reduce_Condition_tok_case(PARSER *self, ST_CONDITION *current, const ST_VALUE *val);

void dp_reduce_Value_tok_count(PARSER *self, ST_VALUE *current, const tbytes *identifier);


void dp_reduce_UnionField(PARSER *self, ST_UNION_FIELD *current, const tbytes *key, const ST_SIMPLE_TYPE *simple_type, const tbytes *identifier, const ST_UNIX_COMMENT *comment);

#endif //_H_REDUCE