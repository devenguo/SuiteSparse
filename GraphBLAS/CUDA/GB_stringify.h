//------------------------------------------------------------------------------
// GB_stringify.h: prototype definitions for using C helpers 
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2021, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

// This file is #include'd only in the GraphBLAS/CUDA/GB*.cu source files.

#ifndef GB_CUDA_STRINGIFY_H
#define GB_CUDA_STRINGIFY_H

//------------------------------------------------------------------------------
// for GB_binop_flip and related methods
//------------------------------------------------------------------------------

#include "GB_binop.h"

//------------------------------------------------------------------------------
// length of strings for building semiring code and names
//------------------------------------------------------------------------------

#define GB_CUDA_STRLEN 2048

//------------------------------------------------------------------------------
// GB_stringify_semiring: build all strings for a semiring
//------------------------------------------------------------------------------

void GB_stringify_semiring     // build a semiring (name and code)
(
    // input:
    FILE *fp,               // File to write macros, assumed open already
    GrB_Semiring semiring,  // the semiring to stringify
    bool flipxy,            // multiplier is: mult(a,b) or mult(b,a)
    GrB_Type ctype,         // the type of C
    GrB_Type mtype,         // the type of M, or NULL if no mask
    GrB_Type atype,         // the type of A
    GrB_Type btype,         // the type of B
    bool Mask_struct,       // mask is structural
    bool Mask_comp,         // mask is complemented
    int C_sparsity,         // sparsity structure of C
    int M_sparsity,         // sparsity structure of M
    int A_sparsity,         // sparsity structure of A
    int B_sparsity          // sparsity structure of B
) ;

void GB_enumify_semiring   // enumerate a semiring
(
    // output:
    uint64_t *scode,        // unique encoding of the entire semiring
    // input:
    GrB_Semiring semiring,  // the semiring to enumify
    bool flipxy,            // multiplier is: mult(a,b) or mult(b,a)
    GrB_Type ctype,         // the type of C
    GrB_Type mtype,         // the type of M, or NULL if no mask
    GrB_Type atype,         // the type of A
    GrB_Type btype,         // the type of B
    bool Mask_struct,       // mask is structural
    bool Mask_comp,         // mask is complemented
    int C_sparsity,         // sparsity structure of C
    int M_sparsity,         // sparsity structure of M
    int A_sparsity,         // sparsity structure of A
    int B_sparsity          // sparsity structure of B
) ;

void GB_macrofy_semiring   // construct all macros for a semiring
(
    // input:
    FILE *fp,                   // target file to write, already open
    uint64_t scode
) ;

//------------------------------------------------------------------------------
// GB_stringify_mask: define macros that access the mask matrix M
//------------------------------------------------------------------------------

void GB_stringify_mask     // return string to define mask macros
(
    // input:
    FILE *fp,                   // File to write macros, assumed open already
    const GB_Type_code mcode,   // typecode of the mask matrix M,
                                // or 0 if M is not present
    bool Mask_struct,           // true if M structural, false if valued
    bool Mask_comp              // true if M complemented
) ;

void GB_enumify_mask       // return enum to define mask macros
(
    // output:
    int *mask_ecode,            // enumified mask
    // input
    const GB_Type_code mcode,   // typecode of the mask matrix M,
                                // or 0 if M is not present
    bool Mask_struct,           // true if M structural, false if valued
    bool Mask_comp              // true if M complemented
) ;

void GB_macrofy_mask       // return enum to define mask macros
(
    // input
    FILE *fp,                   // File to write macros, assumed open already
    int mask_ecode              // enumified mask
) ;

//------------------------------------------------------------------------------
// GB_stringify_monoid and supporting methods
//------------------------------------------------------------------------------

void GB_enumify_monoid  // enumerate a monoid
(
    // outputs:
    int *add_ecode,     // binary op as an enum
    int *id_ecode,      // identity value as an enum
    int *term_ecode,    // terminal value as an enum
    // inputs:
    int add_opcode,     // must be a built-in binary operator from a monoid
    int zcode           // type of the monoid (x, y, and z)
) ;

void GB_macrofy_monoid  // construct the macros for a monoid
(
    // inputs:
    FILE *fp,           // File to write macros, assumed open already
    int add_ecode,      // binary op as an enum
    int id_ecode,       // identity value as an enum
    int term_ecode,     // terminal value as an enum (< 30 is terminal)
    bool is_term
) ;

//------------------------------------------------------------------------------
// GB_stringify_binop and supporting methods
//------------------------------------------------------------------------------

void GB_stringify_binop
(
    // input:
    FILE *fp,                 // File to write macros, assumed open already
    const char *macro_name,   // name of macro to construct
    GB_Opcode opcode,   // opcode of GraphBLAS operator to convert into a macro
    GB_Type_code xcode, // op->xtype->code of the operator
    bool for_semiring,  // if true: op is a multiplier in a semiring
    bool flipxy         // if true, use mult(y,x) else mult(x,y)
) ;

void GB_enumify_binop
(
    // output:
    int *ecode,         // enumerated operator, range 0 to 110; -1 on failure
    // input:
    GB_Opcode opcode,   // opcode of GraphBLAS operator to convert into a macro
    GB_Type_code zcode, // op->xtype->code of the operator
    bool for_semiring   // true for A*B, false for A+B or A.*B
) ;

void GB_charify_binop
(
    // output:
    const char **op_string,   // string defining the operator (NULL if failure)
    // input:
    int ecode           // from GB_enumify_binop
) ;

void GB_macrofy_binop
(
    // input:
    FILE *fp,                   // File to write macros, assumed open already
    const char *macro_name,     // name of macro to construct
    const char *op_string,            // string defining the operator
    bool flipxy                 // if true, use mult(y,x) else mult(x,y)
) ;

//------------------------------------------------------------------------------
// GB_stringify_identity and supporting methods
//------------------------------------------------------------------------------

void GB_stringify_identity     // return string for identity value
(
    // input:
    FILE *fp,               // File to write macros, assumed open already
    GB_Opcode opcode,       // must be a built-in binary operator from a monoid
    GB_Type_code zcode      // type code of the binary operator
) ;

void GB_enumify_identity       // return enum of identity value
(
    // output:
    int *ecode,             // enumerated identity, 0 to 17 (-1 if fail)
    // input:
    GB_Opcode opcode,       // built-in binary opcode of a monoid
    GB_Type_code zcode      // type code used in the opcode we want
) ;

const char *GB_charify_identity_or_terminal // return string encoding the value
(
    // input:
    int ecode                   // enumerated identity/terminal value
) ;

void GB_macrofy_identity
(
    // input:
    FILE *fp,                   // File to write macros, assumed open already
    const char *value_string    // string defining the identity value
) ;

//------------------------------------------------------------------------------
// GB_stringify_terminal and supporting methods
//------------------------------------------------------------------------------

void GB_stringify_terminal         // return strings to check terminal
(
    // outputs:
    bool *is_monoid_terminal,           // true if monoid is terminal
    // inputs:
    FILE *fp,                           // File to write macros, assumed open
    const char *terminal_expression_macro_name,     // name of expression macro
    const char *terminal_statement_macro_name,      // name of statement macro
    GB_Opcode opcode,    // must be a built-in binary operator from a monoid
    GB_Type_code zcode   // type code of the binary operator
) ;

void GB_enumify_terminal       // return enum of terminal value
(
    // output:
    bool *is_monoid_terminal,   // true if monoid is terminal
    int *ecode,                 // enumerated terminal, 0 to 31 (-1 if fail)
    // input:
    GB_Opcode opcode,           // built-in binary opcode of a monoid
    GB_Type_code zcode          // type code used in the opcode we want
) ;

void GB_charify_terminal_expression    // string for terminal expression
(
    // output:
    char *terminal_expression,          // string with terminal expression
    // input:
    const char *terminal_string,        // string with terminal value
    bool is_monoid_terminal,            // true if monoid is terminal
    int ecode                           // ecode of monoid operator
) ;

void GB_charify_terminal_statement // string for terminal statement
(
    // output:
    char *terminal_statement,           // string with terminal statement
    // input:
    const char *terminal_string,        // string with terminal value
    bool is_monoid_terminal,            // true if monoid is terminal
    int ecode                           // ecode of monoid operator
) ;

void GB_macrofy_terminal_expression    // macro for terminal expression
(
    // intput:
    FILE *fp,                          // File to write macros, assumed open
    const char *terminal_expression_macro_name,
    const char *terminal_expression
) ;

void GB_macrofy_terminal_statement     // macro for terminal statement
(
    // intput:
    FILE *fp,                          // File to write macro, assumed open
    const char *terminal_statement_macro_name,
    const char *terminal_statement
) ;

//------------------------------------------------------------------------------
// GB_stringify_load: return a string to load/typecast macro
//------------------------------------------------------------------------------

void GB_stringify_load         // return a string to load/typecast macro
(
    // input:
    FILE *fp,                       // File to write macros, assumed open already
    const char *load_macro_name,    // name of macro to construct
    bool is_pattern                 // if true, load/cast does nothing
) ;

//------------------------------------------------------------------------------
// GB_stringify_opcode: name of unary/binary opcode
//------------------------------------------------------------------------------

const char *GB_stringify_opcode    // name of unary/binary opcode
(
    GB_Opcode opcode    // opcode of GraphBLAS unary or binary operator
) ;

//------------------------------------------------------------------------------
// GB_stringify_sparsity: define macros for sparsity structure
//------------------------------------------------------------------------------

void GB_stringify_sparsity  // construct macros for sparsity structure
(
    // input:
    FILE *fp,               // output file for macros that define the sparsity structure
                            // assumed to be open already
    char *matrix_name,      // "C", "M", "A", or "B"
    int A_sparsity          // GxB_SPARSE, GxB_HYPERSPARSE, GxB_BITMAP, GxB_FULL
) ;

void GB_enumify_sparsity    // enumerate the sparsity structure of a matrix
(
    // output:
    int *ecode,             // enumerated sparsity structure
    // input:
    int A_sparsity          // GxB_SPARSE, GxB_HYPERSPARSE, GxB_BITMAP, GxB_FULL
) ;

void GB_macrofy_sparsity    // construct macros for sparsity structure
(
    // input:
    FILE *fp,
    char *matrix_name,      // "C", "M", "A", or "B"
    int ecode
) ;

#endif

