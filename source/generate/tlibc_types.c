#include "generate/tlibc_types.h"
#include "generator.h"
#include "version.h"

#include <stdio.h>
#include <string.h>



static TD_ERROR_CODE on_document_begin(GENERATOR *super, const char *file_name)
{
	generator_open(super, file_name, TLIBC_TYPES_SUFFIX);

	generator_print(super, "/**\n");
    generator_print(super, " * Autogenerated by TData Compiler (%s)\n", TDATA_VERSION);
    generator_print(super, " *\n");
    generator_print(super, " * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING\n");
    generator_print(super, " *  @generated\n");
    generator_print(super, " */\n");
	generator_print(super, "\n");



	generator_print(super, "#ifndef _H_%s\n", super->document_name);
	generator_print(super, "#define _H_%s\n", super->document_name);
	generator_print(super, "\n");
	generator_print(super, "#include \"platform/tlibc_platform.h\"\n");
	
	generator_print(super, "\n");
	generator_print(super, "\n");
	return E_TD_NOERROR;
}

static TD_ERROR_CODE on_document_end(GENERATOR *super, const char *file_name)
{	
	generator_print(super, "\n");
	generator_print(super, "#endif //_H_%s\n", super->document_name);
	generator_print(super, "\n");

	generator_close(super);
	return E_TD_NOERROR;
}

static TD_ERROR_CODE _on_import(TLIBC_TYPES_GENERATOR *self, const ST_Import *de_import)
{
	char name[MAX_PACKAGE_NAME_LENGTH];	
	strncpy(name, de_import->package_name, MAX_PACKAGE_NAME_LENGTH);
	name[MAX_PACKAGE_NAME_LENGTH - 1] = 0;
	generator_replace_extension(name, MAX_PACKAGE_NAME_LENGTH, TLIBC_TYPES_SUFFIX);
	generator_print(&self->super, "#include \"%s\"\n", name);

	return E_TD_NOERROR;
}




static TD_ERROR_CODE _on_const(TLIBC_TYPES_GENERATOR *self, const ST_Const *de_const)
{
	generator_print(&self->super, "\n");
	generator_print(&self->super, "#define %s ", de_const->identifier);
	generator_print_value(&self->super, &de_const->val);
	generator_print(&self->super, "\n");
	return E_TD_NOERROR;
}

static TD_ERROR_CODE _on_enum(TLIBC_TYPES_GENERATOR *self, const ST_ENUM *de_enum)
{
	tuint32 i;
	generator_print(&self->super, "\n");
	generator_print(&self->super, "typedef enum _%s %s;\n", de_enum->name, de_enum->name);
	generator_print(&self->super, "enum _%s\n", de_enum->name);
	generator_print(&self->super, "{\n");
	for(i = 0;i < de_enum->enum_def_list_num; ++i)
	{
		generator_print(&self->super, "\t%s = ", de_enum->enum_def_list[i].identifier);
		generator_print_value(&self->super, &de_enum->enum_def_list[i].val);
		generator_print(&self->super, ",");
		if(de_enum->enum_def_list[i].comment.text[0])
		{
			generator_print(&self->super, "//%s", de_enum->enum_def_list[i].comment.text);
		}
		generator_print(&self->super, "\n");
	}
	generator_print(&self->super, "};\n");

	return E_TD_NOERROR;
}

static TD_ERROR_CODE _on_field_list(TLIBC_TYPES_GENERATOR *self, const ST_FIELD_LIST *field_list)
{
	tuint32 i;
	for(i = 0; i < field_list->field_list_num; ++i)
	{
		if(field_list->field_list[i].type.type == E_SNT_CONTAINER)
		{
			//为vector对象自动生成一个计数器对象, 所以一个vector类型的对象需要占用两个符号。
			if(field_list->field_list[i].type.ct == E_CT_VECTOR)
			{
				generator_print(&self->super, "\ttuint16 %s_num;\n", field_list->field_list[i].identifier);
				generator_print(&self->super, "\t");
				generator_print_simple_type(&self->super, &field_list->field_list[i].type.vector_type);
				generator_print(&self->super, " %s", field_list->field_list[i].identifier);				
				generator_print(&self->super, "[%s]", field_list->field_list[i].type.vector_length);
				if(field_list->field_list[i].type.vector_type.st == E_ST_STRING)
				{
					generator_print(&self->super, "[%s]", field_list->field_list[i].type.vector_type.string_length);
				}
			}
		}
		else
		{
			if(field_list->field_list[i].type.st.st == E_ST_STRING)
			{
				generator_print(&self->super, "\t");
				generator_print_simple_type(&self->super, &field_list->field_list[i].type.st);
				generator_print(&self->super, " %s", field_list->field_list[i].identifier);
				generator_print(&self->super, "[%s]", field_list->field_list[i].type.st.string_length);
			}
			else
			{
				generator_print(&self->super, "\t");
				generator_print_simple_type(&self->super, &field_list->field_list[i].type.st);
				generator_print(&self->super, " %s", field_list->field_list[i].identifier);
			}
		}

		generator_print(&self->super, ";");
		if(field_list->field_list[i].comment.text[0])
		{
			generator_print(&self->super, "//%s", field_list->field_list[i].comment.text);
		}
		generator_print(&self->super, "\n");
	}

	return E_TD_NOERROR;
}

static TD_ERROR_CODE _on_struct(TLIBC_TYPES_GENERATOR *self, const ST_STRUCT *de_struct)
{
	generator_print(&self->super, "\n");
	generator_print(&self->super, "typedef struct _%s %s;\n", de_struct->name, de_struct->name);
	generator_print(&self->super, "struct _%s\n", de_struct->name);
	generator_print(&self->super, "{\n");
	_on_field_list(self, &de_struct->field_list);
	generator_print(&self->super, "};\n");

	return E_TD_NOERROR;
}


static TD_ERROR_CODE _on_union_field_list(TLIBC_TYPES_GENERATOR *self, const ST_UNION_FIELD_LIST *union_field_list)
{
	tuint32 i;
	for(i = 0; i < union_field_list->union_field_list_num; ++i)
	{
		generator_print(&self->super, "\t");
		generator_print_simple_type(&self->super, &union_field_list->union_field_list[i].simple_type);
		generator_print(&self->super, " %s", union_field_list->union_field_list[i].name);
		if(union_field_list->union_field_list[i].simple_type.st == E_ST_STRING)
		{
			generator_print(&self->super, "[%s]", union_field_list->union_field_list[i].simple_type.string_length);
		}
		generator_print(&self->super, ";");
		if(union_field_list->union_field_list[i].comment.text[0])
		{
			generator_print(&self->super, "//%s", union_field_list->union_field_list[i].comment.text);
		}
		generator_print(&self->super, "\n");
	}

	return E_TD_NOERROR;
}

static TD_ERROR_CODE _on_union(TLIBC_TYPES_GENERATOR *self, const ST_UNION *de_union)
{
	generator_print(&self->super, "\n");
	generator_print(&self->super, "typedef union _%s %s;\n", de_union->name, de_union->name);
	generator_print(&self->super, "union _%s\n", de_union->name);
	generator_print(&self->super, "{\n");
	_on_union_field_list(self, &de_union->union_field_list);
	generator_print(&self->super, "};\n");

	return E_TD_NOERROR;
}

//只能typedef除了string之外的SimpleType
static TD_ERROR_CODE _on_typedef(TLIBC_TYPES_GENERATOR *self, const ST_TYPEDEF *de_typedef)
{
	generator_print(&self->super, "\n");
	generator_print(&self->super, "typedef ");
	generator_print_type(&self->super, &de_typedef->type);
	generator_print(&self->super, " %s", de_typedef->name);
	generator_print(&self->super, ";\n");
	return E_TD_NOERROR;
}

static TD_ERROR_CODE _on_comment(TLIBC_TYPES_GENERATOR *self, const ST_UNIX_COMMENT *de_unix_comment)
{
	if(de_unix_comment->text[0])
	{
		generator_print(&self->super, "//%s\n", de_unix_comment->text);
	}
	return E_TD_NOERROR;
}

static TD_ERROR_CODE on_definition(GENERATOR *super, const ST_DEFINITION *definition)
{
	TLIBC_TYPES_GENERATOR *self = TLIBC_CONTAINER_OF(super, TLIBC_TYPES_GENERATOR, super);
	switch(definition->type)
	{
		case E_DT_IMPORT:
			return _on_import(self, &definition->definition.de_import);				
		case E_DT_CONST:
			return _on_const(self, &definition->definition.de_const);
		case E_DT_ENUM:
			return _on_enum(self, &definition->definition.de_enum);
		case E_DT_STRUCT:
			return _on_struct(self, &definition->definition.de_struct);
		case E_DT_UNION:
			return _on_union(self, &definition->definition.de_union);
		case E_DT_TYPEDEF:
			return _on_typedef(self, &definition->definition.de_typedef);
		case E_DT_UNIX_COMMENT:
			return _on_comment(self, &definition->definition.de_unix_comment);
		default:
			return E_TD_ERROR;
	}
}

void tlibc_types_generator_init(TLIBC_TYPES_GENERATOR *self)
{
	generator_init(&self->super);

	self->super.on_document_begin = on_document_begin;
	self->super.on_document_end = on_document_end;
	self->super.on_definition = on_definition;
}
