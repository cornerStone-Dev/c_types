
/*
STANDARD C
typedef struct BibleStudies {
    const uint8_t * studentName;
    const uint8_t * studyDescription;
    const uint8_t * studyPrayer;
} BibleStudy;

BibleStudy biblestudy1;
biblestudy1.studentName = "John Doe";

************************************************
C$$ Add type interspection
int numMembers = biblestudy1.numMembers;
=
int numMembers = 3;

biblestudy1.Members[0].name;
=
"studentName";




convertToJson(biblestudy1);
{"studentName":"John Doe",...





*/
%include{
#define YYNOERRORRECOVERY 1
}
/* not active when YYNOERRORRECOVERY is defined */
/*%parse_failure {
	printf("parse_failure\n");
	parser_state->error = 1;
}*/

%extra_context {ParserState * parser_state}

%syntax_error {
	printf("syntax_error\n");
	parser_state->status = -1;
	parser_state->num_members=0;
	while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
}



input ::= typelist. {printf("parse done, input exhausted\n");}

typelist ::= typelist etype.
typelist ::= etype.

etype ::= TYPEDEF STRUCT IDENT LBLOCK memlist RBLOCK IDENT(A) SEMI. {
	printf("type accepted\n");
	parser_state->type_name=A;
	parser_state->status = 1;
}
memlist ::= memlist mem.
memlist ::= mem.

mem ::= idlist idend.
mem ::= idlist ptr idlist(A) SEMI. {
	parser_state->member_name[parser_state->num_members] = A;
	parser_state->num_members+=1;
}

idlist(A) ::= idlist IDENT(B). {A = B;}
idlist ::= IDENT.

idend ::= IDENT(A) SEMI. {
	parser_state->member_name[parser_state->num_members] = A;
	parser_state->num_members+=1;
}

ptr ::= ptr ATSIGN.
ptr ::= ATSIGN.


//idlist(A) ::= IDENT(X). //{A = X; /*A-overwrites-X*/ } not needed as this is default

/*ptrlist ::= ptrlist ptr.
ptrlist ::= ptr.

ptr ::= ATSIGN.
ptr ::= .*/

//def_open ::= TYPEDEF STRUCT IDENT LBLOCK.
/*def_open ::= struct_def LBLOCK.
struct_def ::= open_type IDENT. 
open_type ::= TYPEDEF STRUCT.*/



