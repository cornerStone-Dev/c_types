/* here is the code start */

#include <stdio.h>
#include <stdlib.h>   
#include <string.h>
#include <stdint.h>

#define NDEBUG
#define Parse_ENGINEALWAYSONSTACK

#include "std_types.h"

typedef struct token_s{
	u8 * s;
	u32  l;
} Token;

typedef struct parser_s{
	u8 * type_name;
	u8 * mem_name[64];
	u8 * mem_type[64];
	u32  mnl[64];
	u32  mtl[64];
	u8   ptr[64];
	u32  tnl;
	s32  status;
	u32  n_m;
} ParserState;


#include "sqlite3/sqlite3.h"
#include "tool_output/c_type_gram.h"
#include "tool_output/c_type_lex.c"
#include "tool_output/c_type_gram.c"

#define INTPUT_FILE "input/test_types.h"
#define OUTPUT_FILE "output/type_macros.h"

static sqlite3 * db;

/* stmt enum */
enum g_enum_stmts {
	g_insrt_cntype,
	g_sel_cnMemNum,
	size_of_stmt_array
};

static sqlite3_stmt * g_stmtAr[size_of_stmt_array];
//"CREATE TABLE cnt_type(typeName TEXT PRIMARY KEY, mem_num INTEGER) WITHOUT ROWID;"

#define SELECT_CMN "SELECT mem_num FROM cnt_type WHERE typeName=?;"
//static sqlite3_stmt * g_stmtArray[g_search_for_email];

#define INSERT_CNTYPE \
"INSERT INTO cnt_type VALUES(@nm, @mn);"
//static sqlite3_stmt * g_stmtArray[g_insert_registration];

#define SELECT_LOGIN_INFO \
		"SELECT Auth_id, accessLevel, sesCookie, passSalt, passHash FROM " \
		"AuthTable WHERE userEmail=? OR Auth_id=1 ORDER BY Auth_id DESC LIMIT 1;"
//static sqlite3_stmt * g_stmtArray[g_select_login_info];

#define UPDATE_PASSWORD "UPDATE AuthTable SET sesCookie=?, passHash=? WHERE Auth_id=?;"
//static sqlite3_stmt * g_stmtArray[g_update_password];

#define ALL_SQL_STATEMENTS \
		INSERT_CNTYPE \
		SELECT_CMN 

static s32
has_cnt_ending(u8 * mem_type)
{
	/* find '_' if it is there */
	while ( (*mem_type!='_') && (*mem_type!=0) ) {mem_type++;}
	
	if (*mem_type!='_') {
		return 0;
	}
	mem_type++;
	if (*mem_type!='c') {
		return 0;
	}
	mem_type++;
	if (*mem_type!='n') {
		return 0;
	}
	mem_type++;
	if (*mem_type!='t') {
		return 0;
	}
	return 1;
	
}

static s32
has_cnt_provided(ParserState * p_s, unsigned char * mem_type, u8 * cnt_type_loc, u32 mtl)
{
	s32 stm_state;
	s32 mem_num;

	sqlite3_bind_text(
		g_stmtAr[g_sel_cnMemNum],
		1,
		(char *)mem_type,
		mtl,
		SQLITE_STATIC);
	stm_state = sqlite3_step(g_stmtAr[g_sel_cnMemNum]);
	if(stm_state == SQLITE_DONE) {
		return -1;
	}
	mem_num = sqlite3_column_int(g_stmtAr[g_sel_cnMemNum], 0);
	sqlite3_reset(g_stmtAr[g_sel_cnMemNum]);
	return mem_num;
}

/* now that we have a correct parse walk the parse to generate code */
static int semantic_actions_Wstate(ParserState * p_s, unsigned char * output)
{
	/* cache chars */
	/* need information on struct members */
	unsigned char * mem_name;
	unsigned char * mem_type;
	u8 cnt_type_location[64];
	u32 mtl;
	
	s32 x, match;
	
	printf("type name = %s\n", p_s->type_name);
	for(x=0;x<p_s->n_m;x++) {
		printf("mem_name = %s\n", p_s->mem_name[x]);
	}
	
	/* output number of members */
	output+=sprintf((char*)output, "#define %s_numMembers  = %d\n",p_s->type_name, p_s->n_m);
	
	/* output names */
	output+=sprintf((char*)output, "#define %s_memberNames ", p_s->type_name);
	
	output+=sprintf((char*)output, "const uint8 * const %s_memberNames[] ={\\\n",p_s->type_name);
	
	for(x=0;x<p_s->n_m;x++) {
		output+=sprintf((char*)output, "\"%s\",\\\n",p_s->mem_name[x]);
	}
	output+=sprintf((char*)output, "};\n");
	
	/* output toJson function */
	output+=sprintf((char*)output, "#define %s_toJSON_Func \\\n", p_s->type_name);
	
	output+=sprintf((char*)output, 
	"uint8_t * %s_toJSON(uint8_t * b, const %s * t) \\\n"
	, p_s->type_name, p_s->type_name);
	output+=sprintf((char*)output, "{ \\\n");
	output+=sprintf((char*)output, "\tb = (uint8_t *)stpcpy((char *)b, \"{\"); \\\n");
	/* print out members to JSON */
	for(x=0;x<p_s->n_m;x++) {
		mem_type = p_s->mem_type[x];
		mem_name = p_s->mem_name[x];
		mtl = p_s->mtl[x];
		/* check if this is a custom type that starts with a capital letter */
		if ( (*mem_type >= 'A') && (*mem_type <= 'Z') ) {
			printf("custom type\n");
			/* this has the custom type prefix */
			/*check if this is a "number of" member for a pointer type
			  that is immediately following */
			if ( p_s->ptr[x]==0 && has_cnt_ending(mem_type) ) {
				/* this is a number of type, use for loop generation */
				sqlite3_bind_text(
					g_stmtAr[g_insrt_cntype],
					1,
					(char *)mem_type,
					mtl-4,
					SQLITE_STATIC);
				sqlite3_bind_int(g_stmtAr[g_insrt_cntype], 2, x);
				printf("insert x = %d\n", x);
				sqlite3_step(g_stmtAr[g_insrt_cntype]);
				sqlite3_reset(g_stmtAr[g_insrt_cntype]);
				goto print_out;
			}
			if ( p_s->ptr[x]==0 ) { /* single instance of type */
				if ( x > 0 ) {
						output+=sprintf((char*)output, "\tb = (uint8_t *)stpcpy((char *)b, \",\"); \\\n");
					}
				output+=sprintf((char*)output,
				"\tb+=sprintf((char *)b, \"\\\"%s\\\":\"); \\\n" // 5
				"\tb = %s_toJSON(b, &t->%s); \\\n"
				,p_s->mem_name[x],
				mem_type,
				p_s->mem_name[x]
				);
				continue;
			}
			
			/* this is a custom type pointer */
			match = has_cnt_provided(p_s, mem_type, cnt_type_location, mtl);
			printf("match = %d\n", match);
			if ( match>=0 ) {
				/* generate for loop for printing out custom type array */
				output+=sprintf((char*)output, 
				"\tif(t->%s > 0) {\\\n" //1
				"\t\tconst %s  * tmp_%d = t->%s;\\\n" // 2 3 4
				"\t\tb = (uint8_t *)stpcpy((char *)b, \",\"); \\\n"
				"\t\tb+=sprintf((char *)b, \"\\\"%s\\\":\"); \\\n" // 5
				"\t\tb = (uint8_t *)stpcpy((char *)b, \"[\"); \\\n"
				"\t\tfor(int i=t->%s; i>0; i--) { \\\n" // 6
				"\t\t\tb = %s_toJSON(b, tmp_%d); \\\n" //7 8
				"\t\t\tif (i > 1) { \\\n"
				"\t\t\t\tb = (uint8_t *)stpcpy((char *)b, \",\"); \\\n"
				"\t\t\t\ttmp_%d++; \\\n" // 9
				"\t\t\t} \\\n"
				"\t\t} \\\n"
				"\t\tb = (uint8_t *)stpcpy((char *)b, \"]\"); \\\n"
				"\t} \\\n"
				,
				p_s->mem_name[match], // 1
				mem_type, // 2
				x, // 3
				p_s->mem_name[x], // 4
				p_s->mem_name[x], // 5
				p_s->mem_name[match], // 6
				mem_type, // 7
				x, // 8
				x // 9
				);
			}
			/* finished with special type, next */
			continue;
		}
		printf("regular type\n");
print_out:
		/* output comma */
		if ( x > 0 ) {
			output+=sprintf((char*)output, 
			"\tif ( NP_C(t->%s) || ((t->%s)!=0) ) { \\\n" /* if */
			"\t\tb = (uint8_t *)stpcpy((char *)b, \",\"); \\\n" /* comma */,
			mem_name,
			mem_name
			);
			/* output member */
			output+=sprintf((char*)output,
			"\t\tb+=sprintf((char *)b, \"\\\"%s\\\":\"); \\\n" /* name */
			"\t\tb+=sprintf((char *)b, FMT(t->%s), t->%s); \\\n" /* value */
			"\t}\\\n" /* end */
			,mem_name,
			mem_name,
			mem_name
			);
		} else {
			/* output member */
			output+=sprintf((char*)output,
			"\tif ( NP_C(t->%s) || ((t->%s)!=0) ) { \\\n" /* if */
			"\t\tb+=sprintf((char *)b, \"\\\"%s\\\":\"); \\\n" /* name */
			"\t\tb+=sprintf((char *)b, FMT(t->%s), t->%s); \\\n" /* value */
			"\t}\\\n" /* end */
			,mem_name,
			mem_name,
			mem_name,
			mem_name,
			mem_name
			);
		}
	}
	output+=sprintf((char*)output, "\tb = (uint8_t *)stpcpy((char *)b, \"}\"); \\\n");
	output+=sprintf((char*)output, "\treturn b; \\\n");
	output+=sprintf((char*)output, "} \n");

	return 0;
}

static void
sematic_analysis(ParserState * p_s, unsigned char * output)
{
	
}

static void
null_terminate(ParserState * p_s)
{
	for(s32 x = 0;x<p_s->n_m;x++) 
	{
		*(p_s->mem_type[x]+p_s->mtl[x])=0;
		*(p_s->mem_name[x]+p_s->mnl[x])=0;
	}
	/* null terminate type name */
	*(p_s->type_name+p_s->tnl)=0;
}

static void 
prepare_slq_statements(void)
{
	/* alternative loop based init */
	const char * tail = 0;
	int i=0;
	
	sqlite3_prepare_v2(db,  ALL_SQL_STATEMENTS, -1, &g_stmtAr[i], &tail);
	for (i=1;i < size_of_stmt_array; i++){
		sqlite3_prepare_v2(db,  tail, -1, &g_stmtAr[i], &tail);
	}
	return;
}

static s32
execute_sql(const char * request)
{
	sqlite3_stmt * sql_statement;
	int n;
	
	/* prepare SQL query */
	sqlite3_prepare_v2(db,  request, 
						-1, &sql_statement, NULL);
	/* execute sql statement */					
	n = sqlite3_step(sql_statement);
	
	/* all work has completed */
	sqlite3_finalize(sql_statement);
	return n;
}

int main(int argc, char **argv)
{
	
	const unsigned char * data;
	void *pEngine;     /* The LEMON-generated LALR(1) parser */
	yyParser sEngine;  /* Space to hold the Lemon-generated Parser object */
	/*const unsigned char *data3 = "this is s come garbage 321 typedef struct BibleStudies {"
	"int numStudents;";*/ /* IMMUTABLE will fail if tested with */
	/*"ChurchGuest CT_ChurchGuest_guest;"
	"const uint8_t * studyDescription;"
	"const uint8_t * studyPrayer;} BibleStudy; ";*/
	//unsigned char *lex_stack[100];
	unsigned char output_string[4096] = {0};
	unsigned char * output = output_string;
	Token token = {0};
	int tmp_token;
	ParserState p_s = {0};
	FILE * pFile, * outputFile;
	size_t lSize;
	unsigned char * buffer;
	size_t result;
	

	/* output generic funtions */
	output+=sprintf((char*)output, 
	"#define FMT(x) _Generic((x), \\\n"
	"char: \"%%c\", \\\n"
	"signed char: \"%%hhd\", \\\n"
	"unsigned char: \"%%hhu\", \\\n"
	"signed short: \"%%hd\", \\\n"
	"unsigned short: \"%%hu\", \\\n"
	"signed int: \"%%d\", \\\n"
	"unsigned int: \"%%u\", \\\n"
	"long int: \"%%ld\", \\\n"
	"unsigned long int: \"%%lu\", \\\n"
	"long long int: \"%%lld\", \\\n"
	"unsigned long long int: \"%%llu\", \\\n"
	"float: \"%%f\", \\\n"
	"double: \"%%f\", \\\n"
	"long double: \"%%Lf\", \\\n"
	"char *: \"\\\"%%s\\\"\", \\\n"
	"unsigned char *: \"\\\"%%s\\\"\", \\\n"
	"const char *: \"\\\"%%s\\\"\", \\\n"
	"const unsigned char *: \"\\\"%%s\\\"\", \\\n"
	"void *: \"%%p\")\n");
	
	output+=sprintf((char*)output, 
	"#define NP_C(x) _Generic((x), \\\n"
	"char: 1, \\\n"
	"signed char: 1, \\\n"
	"unsigned char: 1, \\\n"
	"signed short: 1, \\\n"
	"unsigned short: 1, \\\n"
	"signed int: 1, \\\n"
	"unsigned int: 1, \\\n"
	"long int: 1, \\\n"
	"unsigned long int: 1, \\\n"
	"long long int: 1, \\\n"
	"unsigned long long int: 1, \\\n"
	"float: 1, \\\n"
	"double: 1, \\\n"
	"long double: 1, \\\n"
	"default: 0)\n");
	
	
	sqlite3_open(":memory:", &db);
	execute_sql("CREATE TABLE cnt_type(typeName TEXT PRIMARY KEY, mem_num INTEGER) WITHOUT ROWID;");
	
	/* prepare database statements */
	prepare_slq_statements();
	
	pFile = fopen ( INTPUT_FILE, "rb" );
	if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
	
	outputFile = fopen ( OUTPUT_FILE, "w" );
	if (outputFile==NULL) {fputs ("File error",stderr); exit (1);}
	
	// obtain file size:
	fseek (pFile , 0 , SEEK_END);
	lSize = ftell (pFile);
	rewind (pFile);

	// allocate memory to contain the whole file:
	buffer = (unsigned char*) malloc (sizeof(char)*lSize+1);
	if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}
	data = buffer;
	// copy the file into the buffer:
	result = fread (buffer,1,lSize,pFile);
	if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
	
	buffer[lSize]=0;
	
	/*pParser = ParseAlloc( malloc, &p_s );*/
	pEngine = &sEngine;
	ParseInit(pEngine, &p_s);

#ifndef NDEBUG
	ParseTrace(stdout, "debug:: ");
#endif

	printf("starting parse\n");
	p_s.status = 0;
	do {
		tmp_token = lex(&data, &token);

		Parse(pEngine, tmp_token, token);
		
		/* if there was a parse failure reset stack */
		if (p_s.status == -1) {
			printf("error actions\n");
			p_s.status = 0;
			
		} else if (p_s.status == 1) {
			/* parser has found a type */
			//semantic_actions(&token, &output_string);
			null_terminate(&p_s);
			semantic_actions_Wstate(&p_s, output);
			fwrite (output_string , sizeof(char), strlen((const char *)output_string), outputFile);
			
			/*printf("type name = %s\n", p_s.type_name);
			for(x=0;x<p_s.n_m;x++) {
				printf("mem_name = %s\n", p_s.mem_name[x]);
			}*/
			
			p_s.n_m = 0;
			output = output_string;
			
			if (tmp_token==0) {
				break;
			} else {
				p_s.status = 0;
			}
		} 
		
	} while (tmp_token != 0);
	
	

	
	
	/* flush file out of cache and close both files */
	fflush (outputFile); 
	fclose (outputFile);
	fclose (pFile);
	/* free memory that stored copy of file */
	free (buffer);
	/* free parser memory */
	/*ParseFree(pParser, free );*/
	ParseFinalize(pEngine);
	
	return 0;
}
