/*execute.c*/

//
// << WHAT IS THE PURPOSE OF THIS FILE??? >>
//
// Jonathan Kong 
// Norhtwestern University 
// CS 211
// Fall 2024 
// 
// Starter code: Prof. Joe Hummel
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>  // true, false
#include <string.h>
#include <assert.h>

#include "programgraph.h"
#include "ram.h"
#include "execute.h"


//
// Public functions:
//

//true if executes successfully, false if not 
bool execute_assignment(struct STMT* stmt, struct RAM* memory) {
  char* var_name = stmt->types.assignment->var_name; 
  char* string_rhs = NULL; 
  int line = stmt->line;

  struct VALUE* rhs = stmt->types.assignment->rhs; 
  struct RAM_VALUE i;

  if (rhs->value_type==VALUE_EXPR) {
    struct EXPR* expr = rhs->types.expr; 

    if (expr->isBinaryExpr) {
      int result; 
      bool success = execute_binary_expression(expr, &result, memory); 
      if (!success) {
        printf("**SEMANTIC ERROR at line %d\n", line);
        return false; 
      }
      i.types.i=result; 
      i.value_type=RAM_TYPE_INT; 
      ram_write_cell_by_name(memory, i, var_name); 
    } else {
    string_rhs = expr->lhs->element->element_value;
    if (expr->lhs->expr_type==UNARY_ELEMENT) {
      if (expr->lhs->element->element_type==ELEMENT_INT_LITERAL) {
          int num = atoi(string_rhs); 
          i.types.i=num; 
          i.value_type=RAM_TYPE_INT; 
          ram_write_cell_by_name(memory, i, var_name);
      } else if (expr->lhs->element->element_type==ELEMENT_IDENTIFIER) {
        struct RAM_VALUE* val = ram_read_cell_by_name(memory, string_rhs); 
        if (val==NULL) {
          printf("**SEMANTIC ERROR: name '%s' is not defined (line %d)\n", string_rhs, line);
          return false; 
        }
        i.types.i=val->types.i;
        i.value_type=RAM_TYPE_INT; 
        ram_write_cell_by_name(memory, i, var_name); 
      } 
    }
    }
  }
  return true; 
} 

bool execute_function_call(struct STMT* stmt, struct RAM* memory) {
  struct ELEMENT* element = stmt->types.function_call->parameter; 
  int line = stmt->line; 


  if (element==NULL) {
    printf("\n"); 
  } else if (element->element_type==ELEMENT_INT_LITERAL) {
    char* str_literal = element->element_value; 
    int num_literal = atoi(str_literal); 
    printf("%d\n", num_literal); 
  } else if (element->element_type==ELEMENT_IDENTIFIER) {
    char* identifier = element->element_value;  
    struct RAM_VALUE* cell_ram_value = ram_read_cell_by_name(memory, identifier); 
    if (cell_ram_value==NULL) {
      printf("**SEMANTIC ERROR: name '%s' is not defined (line %d)\n", identifier, line); 
      return false; 
    }

    int return_value_from_memory = cell_ram_value->types.i; 
        printf("%d\n", return_value_from_memory);
  }
  else if (element->element_type==ELEMENT_STR_LITERAL) { 
    char* str_literal = element->element_value; 
    printf("%s\n", str_literal); 
  }
  return true; 
}

bool retrive_value(struct UNARY_EXPR* expr, int* result, struct RAM* memory) {
  char* string_value = expr->element->element_value; 
  if (expr->element->element_type==ELEMENT_INT_LITERAL) {
    int num = atoi(string_value); 
    *result=num; 
  } else if (expr->element->element_type==ELEMENT_IDENTIFIER) {
    struct RAM_VALUE* cell_ram_value = ram_read_cell_by_name(memory, string_value); 
    if (cell_ram_value==NULL) {
      return false; 
    }
    int return_value_from_memory=cell_ram_value->types.i; 
    *result=return_value_from_memory; 
  }
  return true; 
}

//compute result and then do *result=res
bool execute_binary_expression(struct EXPR* expr, int* result, struct RAM* memory) {
  struct UNARY_EXPR* lhs=expr->lhs; 
  struct UNARY_EXPR* rhs=expr->rhs; 
  int result_lhs; 
  int result_rhs; 
  bool lhs_success = retrive_value(lhs, &result_lhs, memory); 
  bool rhs_success = retrive_value(lhs, &result_rhs, memory); 
  if (!lhs_success || !rhs_success) {
    return false; 
  }
  int res; 
  *result=res; 
  return true; 
}

//
// execute
//
// Given a nuPython program graph and a memory, 
// executes the statements in the program graph.
// If a semantic error occurs (e.g. type error),
// and error message is output, execution stops,
// and the function returns.
//

void execute(struct STMT* program, struct RAM* memory)
{
  struct STMT* stmt = program; 
  while (stmt!=NULL) {
    if (stmt->stmt_type==STMT_ASSIGNMENT) {
      bool success = execute_assignment(stmt, memory);
      if (!success) {
        return;
      }
      stmt=stmt->types.assignment->next_stmt; 
    } else if (stmt->stmt_type==STMT_FUNCTION_CALL) {
      bool success = execute_function_call(stmt, memory); 
      if (!success) {
        return; 
      }
      stmt=stmt->types.function_call->next_stmt; 
    } else if (stmt->stmt_type==STMT_PASS) {
      stmt=stmt->types.pass->next_stmt; 
    } 
  }
}
