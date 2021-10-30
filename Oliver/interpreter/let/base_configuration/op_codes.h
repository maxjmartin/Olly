#pragma once

/********************************************************************************************/
//
//          Copyright 2019 Max J. Martin
//
//          This file is part of Oliver.
//
//          Oliver is free software : you can redistribute it and / or modify
//          it under the terms of the GNU General Public License as published by
//          the Free Software Foundation, either version 3 of the License, or
//          (at your option) any later version.
//
//          Oliver is distributed in the hope that it will be useful,
//          but WITHOUT ANY WARRANTY; without even the implied warranty of
//          MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//          GNU General Public License for more details.
//
//          You should have received a copy of the GNU General Public License
//          along with Oliver.If not, see < https://www.gnu.org/licenses/>.
//
/********************************************************************************************/

#include "system_fundamentals.h"

namespace Olly {

    /********************************************************************************************/
    //
    //                                 Interpreter Operator ENUM
    //
    /********************************************************************************************/

    const enum class OP_CODE {
        NOTHING_OP = 0,

        PREFIX_OPERATORS_START,

            neg_op,

        PREFIX_OPERATORS_STOP,

        INFIX_OPERATORS_START,

            and_op, or_op, xor_op,
            add_op, sub_op, mul_op, div_op, mod_op, fdiv_op, rem_op, pow_op,

        INFIX_OPERATORS_STOP,

        /*******************************************************************************/

            ASSIGN_op, lambda_op, end_scope_op, let_op, def_op, bind_op,

        FUNCTION_OPERATORS,

            NEG_op, IS_op,

        UNARY_OPERATORS,

            AND_op,  OR_op, XOR_op, 
            ADD_op, SUB_op, MUL_op, DIV_op, MOD_op, FDIV_op, REM_op, POW_op,

        BINARY_OPERATORS,

        /**********  UNSORTED **********/

        return_op, relent_op,

        eq_op, ne_op, lt_op, le_op, gt_op, ge_op,

        INFIX_BINARY_OPERATORS,

        EQ_op, NE_op, LT_op, LE_op, GT_op, GE_op,

        POSTFIX_BINARY_OPERATORS,

        STACK_op, DEPTH_op, MAX_DEPTH_op, SET_STACK_op,

        STACK_OPERATORS,

        SIZE_op, len_op, LEAD_op, lead_op, LAST_op, last_op,
        PLACE_LEAD_op, place_lead_op, PLACE_LAST_op, place_last_op,
        SHIFT_LEAD_op, shift_lead_op, SHIFT_LAST_op, shift_last_op,
        iterable_op, GET_op, SET_op, index_op, iter_op, ITER_op,

        SEQUENCE_OPERATORS,

        PRINT_op,

        IO_OPERATORS,





        imply_op, else_op, cond_op, is_const_op, is_var_op, LOOP_op, loop_op,

        EXTENDED_LOGIC_OPERATORS,

        // ADD ABSTRACTION OPERATORS HERE

        ABSTRACTION_OPERATORS,

        BREAK_op, END_op,

        EVALUATION_OPERATORS,

        // Not sorted.

        IN_EQ_op, IN_NE_op, IN_LT_op, IN_LE_op, IN_GT_op, IN_GE_op,
        IS_TRUE_op, IF_TRUE_op, if_op,

        END_OPERATORS_OP
    };

    /********************************************************************************************/
    //
    //                                 Interpreter Operator Map
    //
    /********************************************************************************************/

    static const std::map<str_type, OP_CODE> OPERATORS = {

        { "none",            OP_CODE::NOTHING_OP },
        { "nothing",         OP_CODE::NOTHING_OP },

        { "ASSIGN",           OP_CODE::ASSIGN_op },    { "let",                 OP_CODE::let_op },
        { "def",                 OP_CODE::def_op },    { "==",                 OP_CODE::bind_op },
        { "lambda",            OP_CODE::lambda_op},

        { "ADD",                 OP_CODE::ADD_op },    { "SUB",                 OP_CODE::SUB_op },
        { "MUL",                 OP_CODE::MUL_op },    { "DIV",                 OP_CODE::DIV_op },
        { "MOD",                 OP_CODE::MOD_op },    { "FDIV",               OP_CODE::FDIV_op },
        { "REM",                 OP_CODE::REM_op },    { "POW",                 OP_CODE::POW_op },

        { "+",                   OP_CODE::add_op },    { "-",                   OP_CODE::sub_op },
        { "*",                   OP_CODE::mul_op },    { "/",                   OP_CODE::div_op },
        { "%",                   OP_CODE::mod_op },    { "//",                 OP_CODE::fdiv_op },
        { "%%",                  OP_CODE::rem_op },    { "**",                  OP_CODE::pow_op },

        { "AND",                 OP_CODE::AND_op },    { "OR",                   OP_CODE::OR_op },
        { "XOR",                 OP_CODE::XOR_op },    { "NEG",                 OP_CODE::NEG_op },
        { "?",                    OP_CODE::IS_op },

        { "and",                 OP_CODE::and_op },    { "or",                   OP_CODE::or_op },
        { "xor",                 OP_CODE::xor_op },    { "neg",                 OP_CODE::neg_op },

        /**********  UNSORTED **********/


        { "return",           OP_CODE::return_op },    { "relent",           OP_CODE::relent_op },                                      
          

        { "=",                    OP_CODE::eq_op },    { "<=",                   OP_CODE::le_op },
        { "!=",                   OP_CODE::ne_op },    { ">",                    OP_CODE::gt_op },
        { "<",                    OP_CODE::lt_op },    { ">=",                   OP_CODE::ge_op },

        { "_STACK_",           OP_CODE::STACK_op },
        { "_DEPTH_",           OP_CODE::DEPTH_op },
        { "_MAX_DEPTH_",   OP_CODE::MAX_DEPTH_op },
        { "_SET_STACK_",   OP_CODE::SET_STACK_op },

        { "size",               OP_CODE::SIZE_op },
        { "lead",               OP_CODE::lead_op },
        { "last",               OP_CODE::last_op },
        { "-->",          OP_CODE::place_lead_op },
        { "<--",          OP_CODE::place_last_op },
        { "<<<",          OP_CODE::shift_lead_op },
        { ">>>",          OP_CODE::shift_last_op },
        { "itr?",           OP_CODE::iterable_op },
        { ".",                 OP_CODE::index_op },

        { "PRINT",             OP_CODE::PRINT_op },

        /****************************************************************************/





        { "loop",         OP_CODE::loop_op },
        { "imply",       OP_CODE::imply_op },   { "else",        OP_CODE::else_op },


        { "cond",         OP_CODE::cond_op },
        { "const?",   OP_CODE::is_const_op },
        { "var?",       OP_CODE::is_var_op },



    };

} // end
