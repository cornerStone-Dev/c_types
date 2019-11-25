
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
	p_s->error = 1;
}*/

%extra_context {ParserState * p_s}

%token_type {Token}
%default_type {Token}

%syntax_error {
	printf("syntax_error\n");
	p_s->status = -1;
	p_s->n_m=0;
	while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
}



input ::= typelist. {printf("parse done, input exhausted\n");}

typelist ::= typelist etype.
typelist ::= etype.

etype ::= TYPEDEF STRUCT IDENT LBLOCK memlist RBLOCK IDENT(A) SEMI. {
	printf("type accepted\n");
	p_s->type_name=A.s;
	p_s->tnl=A.l;
	p_s->status = 1;
}
memlist ::= memlist mem.
memlist ::= mem.

mem ::= IDENT(A) idend. {
	p_s->mem_type[p_s->n_m] = A.s;
	p_s->mtl[p_s->n_m] = A.l;
	p_s->ptr[p_s->n_m] = 0;
	p_s->n_m+=1;
}
mem ::= IDENT(B) ptr IDENT(A) SEMI. {
	p_s->mem_name[p_s->n_m] = A.s;
	p_s->mnl[p_s->n_m] = A.l;
	p_s->mem_type[p_s->n_m] = B.s;
	p_s->mtl[p_s->n_m] = B.l;
	p_s->n_m+=1;
}

idend ::= IDENT(A) SEMI. {
	p_s->mem_name[p_s->n_m] = A.s;
	p_s->mnl[p_s->n_m] = A.l;
}

ptr ::= ptr STAR. {p_s->ptr[p_s->n_m] = 2;}
ptr ::= STAR.     {p_s->ptr[p_s->n_m] = 1;}


//idlist(A) ::= IDENT(X). //{A = X; /*A-overwrites-X*/ } not needed as this is default

/*ptrlist ::= ptrlist ptr.
ptrlist ::= ptr.

ptr ::= STAR.
ptr ::= .*/

//def_open ::= TYPEDEF STRUCT IDENT LBLOCK.
/*def_open ::= struct_def LBLOCK.
struct_def ::= open_type IDENT. 
open_type ::= TYPEDEF STRUCT.*/



