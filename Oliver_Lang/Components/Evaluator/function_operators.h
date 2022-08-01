#ifndef FUNCTION_OPERATORS_H
#define FUNCTION_OPERATORS_H

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

        inline void evaluator::function_operators(OP_CODE& opr) {

            switch (opr) {

            case OP_CODE::end_scope_op: {   // Delete the current enclosure scope.
                delete_enclosure();
            }	break;




            case OP_CODE::let_op: {  // Assign or apply a value to a variable.

                let vars = get_expression_from_code();
                let vals = get_expression_from_code();
                let oper = get_expression_from_code();

                if (oper.op_code() == OP_CODE::EQ_op) {
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

                        if (val.type() == "lambda") {
                            set_expression_on_code(val.last());
                            set_expression_on_code(val.lead());
                            set_expression_on_code(var);
                            set_expression_on_code(op_call(OP_CODE::def_op));
                        }

                        else {
                            set_expression_on_code(var);
                            set_expression_on_code(op_call(OP_CODE::LET_op));
                            set_expression_on_code(val);
                        }
                    }
                }

                else if (oper.op_code() == OP_CODE::BIND_op) {
                    /*
                        Accept a series of one or more pairs
                        of assignment to bind to the enclosed 
                        variables of a function.  
                    */

                    let lam = get_symbol(vars);

                    if (lam.type() == "lambda") {

                        lambda l = lam.copy<lambda>();

                        while (vals.is()) {

                            let a  = pop_lead(vals);
                            let b  = pop_lead(vals);
                            let op = pop_lead(vals);

                            if (op.op_code() == OP_CODE::EQ_op) {

                                l.bind_variable(a, b);
                            }
                        }
                        lam = l;

                        set_symbol(vars, lam);
                    }
                }


                else if (oper.op_code() == OP_CODE::APPLY_op) {
                    /*
                        The apply operator functions the same as
                        a normal let call, but applies the result
                        of a function instead of defining a new
                        function instance.  
                    */

                    // We can reuse the apply_op as all other instances
                    // of the operator were compiled out of the code.
                    let oper = op_call(OP_CODE::apply_op);

                    set_expression_on_code(vars);
                    set_expression_on_code(oper);
                    set_expression_on_code(vals);
                }
            }	break;

            case OP_CODE::apply_op: {
                /*
                    Part two of the applications process.  The result
                    of the function is gotten from the stack.  Then a
                    let assignment is written to the code to execute.
                */

                let vals = get_expression_from_stack();
                let vars = get_expression_from_code();

                set_expression_on_code(op_call(OP_CODE::EQ_op));
                set_expression_on_code(vals);
                set_expression_on_code(vars);
                set_expression_on_code(op_call(OP_CODE::let_op));
            } break;




            case OP_CODE::def_op: {  // Presupose the definition of a function.  

                let var = get_expression_from_code();
                let args = get_expression_from_code();
                let body = get_expression_from_code();

                auto lam = lambda(args, body);

                if (_variables.size() > 1) {
                    /*
                        Define the function's internal closure.

                        Ignore global scope for the purpose of setting the values of 
                        the enclosure.  Get the current scope and bind it to the defined 
                        function.
                    */

                    lam.bind_scope(_variables.back());
                }

                lam.bind_variable(var, lam);  // Provide a self reference for recursion.  

                lam.bind_variable(string("self"), var);  // Identify the self refrence.  

                set_expression_on_stack(lam);
                set_expression_on_code(var);
                set_expression_on_code(op_call(OP_CODE::LET_op));
            }	break;




            case OP_CODE::return_op: {  // Return an expression from a function, then quite the function.

                let queue = get_result_queue();

                set_expression_on_code(queue.reverse());
                set_expression_on_code(op_call(OP_CODE::end_scope_op));
            }   break;




            default:
                break;
            }
        }

        inline let evaluator::get_result_queue() {

            let args = get_expression_from_code();

            if (args.type() != "expression") {
                args = expression(args);
            }

            let queue = expression();

            while (args.is()) {

                let a = pop_lead(args);

                while (a.type() == "symbol") {
                    a = get_symbol(a);
                }

                queue = queue.place_lead(a);
            }

            let end = op_call(OP_CODE::end_scope_op);
            let itr = get_expression_from_code();

            while (end != itr) {
                itr = get_expression_from_code();
            }
            
            return queue;
        }

    }  // end eval
}
#endif // FUNCTION_OPERATORS_H