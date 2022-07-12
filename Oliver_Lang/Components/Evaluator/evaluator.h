#pragma once

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

#include "../Compiler/compiler.h"

namespace Olly {
    namespace eval {

        /********************************************************************************************/
        //
        //                                The Oliver Interpreter Runtime
        //
        //          The Oliver Interpreter runtime is defined below.  It is design to run
        //			as a stack inpterpreter evaluation of the code expression passed to it.
        // 
        //
        /********************************************************************************************/

        /********************************************************************************************/
        //
        //                              'function' Class Definition
        //
        //          The function class is a doubled ended queue.  It is implimented
        //          using two _node_queue_ data types representing the front and back
        //          portions of the queue.  
        //
        /********************************************************************************************/


        /********************************************************************************************/
        //
        //                                'eval' Function Definitions
        //
        /********************************************************************************************/

        class evaluator {

            typedef     std::map<str_type, let>  map_type;
            typedef     std::vector<let>	     stack_type;
            typedef     std::vector<map_type>	 closure_type;

            closure_type                _variables;
            stack_type                      _stack;
            stack_type                     _return;
            stack_type                       _code;
            size_type              _max_stack_size;

        public:
            static const size_type DEFAULT_STACK_LIMIT;

            evaluator();
            evaluator(evaluator& env) = delete;

            let eval(let exp);


        private:

            // let eval(let exp, closure_type& vars);

            void define_enclosure(let& lam);
            void define_enclosure();
            void delete_enclosure();

            void   set_expression_on_code(let exp);
            void  set_expression_on_stack(let exp);
            void set_expression_on_return(let exp);

            let get_result_stack() const;
            let get_result_queue();
            let get_eval_queue() const;

            let  get_symbol(let& var) const;
            void set_symbol(let& var, let& val);

            let get_expression_from_return();
            let get_expression_from_stack();
            let get_expression_from_code();

            void eval();

            void fundamental_operators(OP_CODE& opr);
            void    sequence_operators(OP_CODE& opr);
            void associative_operators(OP_CODE& opr);
            void       unary_operators(OP_CODE& opr);
            void      binary_operators(OP_CODE& opr);
        };

        /********************************************************************************************/
        //
        //                                'Oliver' Function Implimentation
        //
        /********************************************************************************************/

        const size_type evaluator::DEFAULT_STACK_LIMIT = 2048;

        evaluator::evaluator() : _variables(), _stack(), _return(), _code(), _max_stack_size(DEFAULT_STACK_LIMIT) {
        }

        inline let evaluator::eval(let exp) {

            if (exp.type() != "expression") {
                return nothing();
            }

            exp = unwrap_expresion(exp);

            _code.emplace_back(exp);

            define_enclosure();

            eval();

            return get_result_stack();
        }

        inline void evaluator::define_enclosure(let& lam) {

            const lambda* l = lam.cast<lambda>();

            _variables.emplace_back(l->variables());

            set_expression_on_return(op_call(OP_CODE::end_scope_op));
        }

        inline void evaluator::define_enclosure() {
            _variables.emplace_back(map_type());
        }

        inline void evaluator::delete_enclosure() {
            if (!_variables.empty()) {
                _variables.pop_back();
            }

            let d = get_expression_from_return();

            while (_return.size() && d.op_code() != OP_CODE::end_scope_op) {

                set_expression_on_stack(d);

                d = get_expression_from_return();
            }
        }

        inline void evaluator::set_expression_on_code(let exp) {

            if (_code.empty()) {
                _code.emplace_back(expression());
            }

            _code.back() = _code.back().place_lead(exp);
        }

        inline void evaluator::set_expression_on_stack(let exp) {

            if (_stack.size() < _max_stack_size) {

                _stack.emplace_back(exp);
                return;
            }

            _stack.emplace_back(error("Stack overflow."));
        }

        inline void evaluator::set_expression_on_return(let exp) {

            if (_return.size() < _max_stack_size) {

                _return.emplace_back(exp);
                return;
            }

            _return.emplace_back(error("Return overflow."));
        }

        inline let evaluator::get_result_stack() const {

            if (!_stack.empty()) {

                let result = expression();

                for (auto i = _stack.crbegin(); i != _stack.crend(); ++i) {

                    result = result.place_lead(*i);
                }

                return result;
            }

            return expression();
        }

        inline let evaluator::get_eval_queue() const {

            if (!_code.empty()) {

                let result = expression();

                for (auto i = _code.crbegin(); i != _code.crend(); ++i) {

                    result = result.place_lead(*i);
                }

                return result;
            }

            return expression();
        }

        inline let evaluator::get_symbol(let& var) const {

            str_type symbol_name = str(var);

            for (auto i = _variables.crbegin(); i != _variables.crend(); ++i) {

                auto v_itr = i->find(symbol_name);

                if (v_itr != i->end()) {
                    return v_itr->second;
                }
            }

            return error("undef_var");
        }

        inline void evaluator::set_symbol(let& var, let& val) {

            while (val.type() == "symbol") {
                val = get_symbol(val);
            }

            str_type symbol_name = repr(var);

            if (_variables.empty()) {
                _variables.emplace_back(map_type());
            }

            _variables.back()[symbol_name] = val;
        }

        inline let evaluator::get_expression_from_return() {

            if (_return.empty()) {
                return error("Return underflow.");
            }

            let val = _return.back();
            _return.pop_back();

            return val;
        }

        inline let evaluator::get_expression_from_stack() {

            if (_stack.empty()) {
                return error("Stack underflow.");
            }

            let val = _stack.back();
            _stack.pop_back();

            return val;
        }

        inline let evaluator::get_expression_from_code() {

            if (_code.empty()) {
                return error("Code underflow.");
            }

            let a = pop_lead(_code.back());

            if (expression_is_empty(_code.back())) {
                _code.pop_back();
            }

            return a;
        }

        inline void evaluator::eval() {

            do {

                let exp = get_expression_from_code();  // Get an element from the code expression.

                while (exp.type() == "symbol") {  // Get the value of an abstraction.
                    exp = get_symbol(exp);
                }

                if (exp.type() == "expression") {
                    /*
                        Any elements which are expressions are
                        placed back on to the code to have their
                        individual elements evaluated.
                    */

                    exp = unwrap_expresion(exp);

                    if (!expression_is_empty(exp)) {
                        _code.emplace_back(exp);
                    }
                }

                else if (exp.type() == "lambda") {
                    
                    let args = exp.lead();
                    let body = exp.last();

                    define_enclosure(exp);

                    while (args.is()) {

                        let var = pop_lead(args);
                        let val = get_expression_from_code();

                        if (var.type() == "symbol") {
                            set_symbol(var, val);
                        }
                    }

                    set_expression_on_code(op_call(OP_CODE::end_scope_op));
                    set_expression_on_code(body);
                }

                else if (exp.type() != "op_call" && !exp.is_nothing()) {
                    set_expression_on_stack(exp);
                }

                else {

                    OP_CODE opr = exp.op_code();

                    if (opr > OP_CODE::NOTHING_OP && opr < OP_CODE::END_OPERATORS_OP) {

                        if (opr < OP_CODE::FUNDAMENTAL_OPERATORS) {
                            fundamental_operators(opr);
                        }

                        else if (opr < OP_CODE::SEQUENTIAL_OPERATORS) {
                            sequence_operators(opr);
                        }

                        else if (opr < OP_CODE::ASSOCIATIVE_OPERATORS) {
                            associative_operators(opr);
                        }

                        else if (opr < OP_CODE::UNARY_OPERATORS) {
                            unary_operators(opr);
                        }

                        else if (opr < OP_CODE::BINARY_OPERATORS) {
                            binary_operators(opr);
                        }
                    }
                }

            } while (!_code.empty());
        }
    }  // end eval
} // end Olly

#include "fundamental_operators.h"
#include    "sequence_operators.h"
#include "associative_operators.h"
#include       "unary_operators.h"
#include      "binary_operators.h"