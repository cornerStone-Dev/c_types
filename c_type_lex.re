/*!re2c re2c:flags:i = 1; */         // re2c block with configuration that turns off #line directives
                                     //
//#include <stdio.h>                   //    C/C++ code


/*!max:re2c*/                        // directive that defines YYMAXFILL (unused)
/*!re2c                              // start of re2c block
    
	mcm = "/*" ([^*] | ("*" [^/]))* "*""/";
    scm = "//" [^\n]* "\n";
    wsp = ([ \t\v\n\r] | scm | mcm)+;
	struct =   "struct";
	typedef =  "typedef";
	//const =    "const";
	//volatile = "volatile";
	//restrict = "restrict";
	//atomic =   "_Atomic";
	lblock =   "{";
	rblock =   "}";
	semi =     ";";
	atstar = ("@" | "*");
	id = [a-zA-Z_][a-zA-Z_0-9]*;
	
*/                                   // end of re2c block

static int lex(const u8 **YYCURSOR_p, Context * c) // YYCURSOR is defined as a function parameter
{                                    //
    const u8 * YYMARKER;    // YYMARKER is defined as a local variable
	//const u8 * YYCTXMARKER; // YYCTXMARKER is defined as a local variable
	const u8 * YYCURSOR;    // YYCURSOR is defined as a local variable
	const u8 * start;
	start = *YYCURSOR_p;
	YYCURSOR = *YYCURSOR_p;

loop: // label for looping within the lexxer

    /*!re2c                          // start of re2c block
    re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
    re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
                                     //
    * {  start =YYCURSOR; goto loop; }//   default rule with its semantic action
    [\x00] { return 0; }             // EOF rule with null sentinal
    
    wsp {
		*(c->string_end) = 0;
        start =YYCURSOR;
		goto loop;
    }
	
	struct {                         //   a normal rule with its semantic action
        //printf("struct\n"); 
		*YYCURSOR_p = YYCURSOR;
		return STRUCT;               //     ... semantic action (continued)
    }
	
	typedef {
        //printf("typedef\n");  
		*YYCURSOR_p = YYCURSOR;
		return TYPEDEF;
    }
	
	lblock {
        //printf("lblock\n"); 
        *(c->string_end) = 0; 
		*YYCURSOR_p = YYCURSOR;
		return LBLOCK;
    }
	
	rblock {
        //printf("rblock\n");  
		*YYCURSOR_p = YYCURSOR;
		return RBLOCK;
    }
	
	semi {
		// record an atstar 
		*(c->string_end) = 0;
        //printf("semi\n");  
		*YYCURSOR_p = YYCURSOR;
		return SEMI;
    }
	
	atstar {
		// record an atstar
        //printf("atstar\n");
        *(c->string_end) = 0;
		*YYCURSOR_p = YYCURSOR;
		return ATSIGN;
    }
	
	id {
		// record string 
		c->string_start = (u8 *)start;
		c->string_end = (u8 *)YYCURSOR;
        //printf("id_end\n");    
		*YYCURSOR_p = YYCURSOR;
		return IDENT;
    }
                                     //
    */                               // end of re2c block
}  



