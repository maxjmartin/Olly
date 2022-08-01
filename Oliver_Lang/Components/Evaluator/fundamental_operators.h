#ifndef FUNDAMENTAL_OPERATORS_H
#define FUNDAMENTAL_OPERATORS_H

/********************************************************************************************/
//
//          Copyright 2021 Max J. Martin
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

#include "evaluator.h"

namespace Olly {
    namespace eval {

        inline void evaluator::fundamental_operators(OP_CODE& opr) {

            switch (opr) {

            case OP_CODE::IDNT_op: {   // Place on to the stack an expression without evaluation.

                let exp = get_expression_from_code();

                set_expression_on_stack(exp);
            }	break;

            case OP_CODE::LET_op: {   // Define a new word.

                let val = get_expression_from_stack();
                let var = get_expression_from_stack();

                if (var.type() == "symbol") {
                    set_symbol(var, val);
                }
                else {
                    set_expression_on_code(op_call(OP_CODE::ENDL_op));
                    set_expression_on_code(op_call(OP_CODE::EMIT_op));
                    set_expression_on_code(error("Miss handled assignment!"));
                }
            }	break;

            case OP_CODE::STACK_op: {   // Print a string representation of the stack.

                let stack = get_result_stack();

                set_expression_on_stack(stack);
            }	break;

            case OP_CODE::QUEUE_op: {   // Print a string representation of the queue.

                let queue = get_eval_queue();

                set_expression_on_stack(queue);
            }	break;

            case OP_CODE::CLEAR_op: {   // Clear the designated stack like body of it elements.

                let exp = get_expression_from_code();

                OP_CODE opr = exp.op_code();

                switch (opr) {

                case OP_CODE::STACK_op: {   // Clear the stack of all ellements.

                    _stack.clear();

                }	break;

                case OP_CODE::QUEUE_op: {   // Clear the the remainder of code to execute.

                    _code.clear();

                }	break;

                default:
                    while (exp.type() == "symbol") {
                        exp = get_symbol(exp);
                    }
                    exp = exp.clear();

                    set_expression_on_code(exp);
                    break;
                }
            }	break;

            case OP_CODE::EMIT_op: {

                let val = get_expression_from_stack();

                std::cout << str(val);

            }   break;

            case OP_CODE::ENDL_op: {

                std::cout << std::endl;

            }   break;

            case OP_CODE::let_op: {  // Assign or apply a value to a variable.

                let vars = get_expression_from_code();
                let vals = get_expression_from_code();
                let oper = get_expression_from_code();

                if (oper.op_code() == OP_CODE::EQ_op || oper.op_code() == OP_CODE::BIND_op) {
                    /*
                        Simple assignment of one or more variables.
                        Functions are not evaluated before assignment.
                        Instead they are applied as a 'def' operator
                        has been called.
                    */

                    if (vars.type() != "expression") {

                        vars = expression(vars);
                        vals = expression(vals);
                    }

                    while (vars.is()) {

                        let var = pop_lead(vars);
                        let val = pop_lead(vals);

                        while (val.type() == "symbol") {
                            val = get_symbol(val);
                        }

                        set_expression_on_stack(var);

                        if (oper.op_code() == OP_CODE::BIND_op) {
                            set_expression_on_code(op_call(OP_CODE::LET_op));
                            set_expression_on_code(val);
                        }
                        else {
                            set_expression_on_stack(val);
                            set_expression_on_code(op_call(OP_CODE::LET_op));
                        }
                    }
                }
                
            } break;

            case OP_CODE::RETURN_op: {

                let exp = get_expression_from_code();

                if (exp.type() != "expression") {

                    exp = expression(exp);
                }

                let return_exp = expression();

                while (exp.is()) {
                
                    let n = pop_lead(exp);

                    if (n.type() == "symbol") {

                        n = get_symbol(n);
                    }

                    return_exp = return_exp.place_lead(n);
                }

                set_expression_on_stack(return_exp.reverse());

                do {
                    exp = get_expression_from_code();

                } while (exp.op_code() != OP_CODE::end_scope_op);

                set_expression_on_code(exp);

            } break;;

            default:
                break;
            }
        }

    }  // end eval
}
#endif // FUNDAMENTAL_OPERATORS_H