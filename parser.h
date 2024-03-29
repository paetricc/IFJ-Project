/**
 * Projekt: IFJ2021
 *
 * @brief Implementace top-down syntakticke anylzy shora dolu.
 *
 * @author Šimon Vacek xvacek10@stud.fit.vutbr.cz
*/

#ifndef _PARSER_H_
#define _PARSER_H_

#include "symtable.h"
#include "builtIn.h"
#include "generate_code.h"
#include "expression.h"

int parser(FILE *);

/*  funkce odpovidajici neterminalum v pravidlech */
int start(Token *, FILE *);
int program(Token *, FILE *);
int fnc_dec(Token *, FILE *);
int params_dec(Token *, FILE *, bst_node_t *);
int params_dec2(Token *, FILE *, bst_node_t *);
int return_type(Token *, FILE *, bst_node_t *);
int data_type(Token *, FILE *, bst_node_t *, SLLElementPtr_Param, int);
int fnc_call(Token *, FILE *);
int value(Token *, FILE *, bst_node_t *, SLLElementPtr_Param *);
int value2(Token *, FILE *, bst_node_t *, SLLElementPtr_Param *);
int value_last(Token *, FILE *, bst_node_t *, SLLElementPtr_Param *);
int fnc_def(Token *, FILE *);
int fnc_head(Token *, FILE *, bst_node_t **);
int fnc_def2(Token *, FILE *, bst_node_t **);
int params_def(Token *, FILE *, bst_node_t *, SLLElementPtr_Param *);
int params_def2(Token *, FILE *, bst_node_t *, SLLElementPtr_Param *);
int var_def(Token *, FILE *, bst_node_t **);
int var_defParam(Token *, FILE *, bst_node_t *, SLLElementPtr_Param *);
int return_(Token *, FILE *, Data_type, bool *);
int fnc_body(Token *, FILE *, Data_type, bool *);
int statement(Token *, FILE *, Data_type, bool *);
int var_dec(Token *, FILE *);
int var_dec_init(Token *, FILE *, bst_node_t *, char *);
int var_assign(Token *, FILE *, bst_node_t *, char *);
int if_(Token *, FILE *, Data_type, bool *);
int loop(Token *, FILE *, Data_type, bool *);
int statements(Token *, FILE *, Data_type, bool *);

/*  pomocne funkce  */
int get_non_white_token(Token *, FILE *);
int checkDeclaredFncs(bst_node_t *);
int writeFncCall(Token *, FILE *);

#endif

/** @endcode */