#ifndef _PARSER_H_
#define _PARSER_H_

int parser(FILE *);

/*  funkce odpovidajici neterminalum v pravidlech */
int start(Token *, FILE *);
int program(Token *, FILE *);
int fnc_dec(Token *, FILE *);
int id_fnc(Token *, FILE *);
int params_dec(Token *, FILE *);
int params_dec2(Token *, FILE *);
int return_type(Token *, FILE *);
int data_type(Token *, FILE *);
int fnc_call(Token *, FILE *);
int value(Token *, FILE *);
int value2(Token *, FILE *);
int value_last(Token *, FILE *);
int id_var(Token *, FILE *);
int fnc_def(Token *, FILE *);
int fnc_head(Token *, FILE *);
int fnc_def2(Token *, FILE *);
int params_def(Token *, FILE *);
int params_def2(Token *, FILE *);
int var_def(Token *, FILE *);
int return_(Token *, FILE *);
int return_void(Token *, FILE *);
int fnc_body(Token *, FILE *);
int statement(Token *, FILE *);
int var_dec(Token *, FILE *);
int var_dec_init(Token *, FILE *);
int var_dec_init2(Token *, FILE *);
int var_assign(Token *, FILE *);
int if_(Token *, FILE *);
int loop(Token *, FILE *);
int statements(Token *, FILE *);

/*  pomocne funkce  */
int get_non_white_tokens(Token *, FILE *);

#endif
