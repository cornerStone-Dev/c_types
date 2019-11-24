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
	const =    "const";
	volatile = "volatile";
	restrict = "restrict";
	atomic =   "_Atomic";
	lblock =   "{";
	rblock =   "}";
	semi =     ";";
	star =     "*";
	id = [a-zA-Z_][a-zA-Z_0-9]*;
	
*/                                   // end of re2c block

static int lex(const u8 **YYCURSOR_p, Token * t) // YYCURSOR is defined as a function parameter
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
		start =YYCURSOR;
		goto loop;
	}
	
	struct {
		*YYCURSOR_p = YYCURSOR;
		return STRUCT;
	}
	
	typedef { 
		*YYCURSOR_p = YYCURSOR;
		return TYPEDEF;
	}
	
		const {
		// skip type qualifiers
		start =YYCURSOR;
		goto loop;
	}
	
	volatile {
		// skip type qualifiers
		start =YYCURSOR;
		goto loop;
	}
	
	restrict {
		// skip type qualifiers
		start =YYCURSOR;
		goto loop;
	}
	
	atomic {
		// skip type qualifiers
		start =YYCURSOR;
		goto loop;
	}
	
	lblock {
		*YYCURSOR_p = YYCURSOR;
		return LBLOCK;
	}
	
	rblock { 
		*YYCURSOR_p = YYCURSOR;
		return RBLOCK;
	}
	
	semi {
		*YYCURSOR_p = YYCURSOR;
		return SEMI;
	}
	
	star {
		*YYCURSOR_p = YYCURSOR;
		return STAR;
	}
	
	id {
		// record string 
		t->s = (u8 *)start;
		t->l = (u32)(YYCURSOR - start); 
		*YYCURSOR_p = YYCURSOR;
		return IDENT;
	}

	*/                               // end of re2c block
}  

