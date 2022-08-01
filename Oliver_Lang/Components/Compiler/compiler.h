#ifndef COMPILER_H
#define COMPILER_H

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

#include <string>

#include "../text_reader.h"
#include "Data_Types/let.h"
#include "Data_Types/fundamental_types/expression.h"
#include "Data_Types/fundamental_types/error.h"
#include "Data_Types/fundamental_types/lambda.h"
#include "Data_Types/fundamental_types/list.h"
#include "Data_Types/fundamental_types/logical_term.h"
#include "Data_Types/fundamental_types/number.h"
#include "Data_Types/fundamental_types/op_call.h"
#include "Data_Types/fundamental_types/map.h"
#include "Data_Types/fundamental_types/string.h"
#include "Data_Types/fundamental_types/symbol.h"

namespace Olly {

        /********************************************************************************************/
        //
        //                                    'parser' class definition
        //
        //        The parser class accepts individual characters from an instance of
        //        the text_reader.  The parser then appends each character together
        //        until a specific combination of characters or an individual character
        //        is recognized as invoking special behavior.  Else all words are
        //        defined by being split at whitespace.  
        //
        /********************************************************************************************/

        // The regex below is currently not in use.  Will see later integration.  
        // static const regex_t  REAL_REGEX("((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?((e|E)((\\+|-)?)[[:digit:]]+)?", std::regex_constants::ECMAScript | std::regex_constants::optimize);

        class compiler {

            std::vector<std::string>    _tokens;
            let                         _code;

        public:

            compiler(std::string text);
            compiler(std::vector<std::string> text);
            virtual ~compiler();

            let compile();

        private:

            compiler() = delete;
            compiler(const compiler& obj) = delete;

            bool_type is_prefix_unary_operator(OP_CODE opr) const;
            bool_type is_infix_binary_operator(OP_CODE opr) const;

            let get_postfix_operator(OP_CODE opr) const;
            let get_infix_operator(OP_CODE opr) const;

            void place_term(let t);

            str_type collect_string(tokens_type::const_iterator& word, const str_type& stop);
        };



        compiler::compiler(std::string text) : _tokens(), _code(expression()) {

            text_reader text_file(text);

            // _code = _code.place_lead(expression());
            while (text_file.is()) {
                _tokens.push_back(text_file.get_line());
            }
        }

        compiler::compiler(std::vector<std::string> text) : _tokens(text), _code(expression()) {
        }

        compiler::~compiler() {
        }

        let compiler::compile() {
            _code = _code.place_lead(expression());

            auto word = _tokens.begin();

            while (word != _tokens.cend()) {

                if (*word == "(" || *word == "[") {
                    _code = _code.place_lead(expression());
                    *word = "";
                }

                if (*word == "{") {
                    _code = _code.place_lead(expression());
                    place_term(op_call(OP_CODE::map_op));
                }

                else if (*word == "\'") {
                    str_type str = collect_string(word, "\'");

                    place_term(number(str));
                }

                else if (*word == "\"") {
                    str_type str = collect_string(word, "\"");

                    place_term(string(str));
                }

                else if (*word == ")" || *word == "]" || *word == "}") {

                    let terms = pop_lead(_code);

                    let exp;
                        
                    if (*word == ")") {
                        exp = expression();
                    }
                    else if (*word == "]") {
                        exp = list();
                    }
                    else if (*word == "}") {
                        exp = expression();
                    }

                    while (terms.is()) {

                        let term = pop_lead(terms);

                        if (term.op_code() == OP_CODE::function_op) {
                            // Define an anonymous function.

                            let a = pop_lead(exp);
                            let b = pop_lead(exp);

                            //if (b.op_code() == OP_CODE::start_scope_op) {
                            //    
                            //    b = pop_lead(exp).place_lead(b);
                            //}

                            exp = exp.place_lead(lambda(a, b));
                        }

                        else if (is_prefix_unary_operator(term.op_code())) {
                            // Convert prefix unary operators to postix unary.

                            let a = pop_lead(exp);
                            let b = get_postfix_operator(term.op_code());

                            let p = expression();

                            p = p.place_lead(b);
                            p = p.place_lead(a);

                            exp = exp.place_lead(p);
                        }

                        else if (is_infix_binary_operator(term.op_code())) {
                            // Convert infix operators to postfix operators.

                            let a = pop_lead(exp);
                            let b = get_infix_operator(term.op_code());

                            exp = exp.place_lead(b);
                            exp = exp.place_lead(a);
                        }

                        else {  // Place the term on the expression, list, or map.
                            exp = exp.place_lead(term);
                        }
                    }
                    if (exp.lead().op_code() == OP_CODE::map_op) {

                        let args = exp.drop_lead();

                        place_term(map(args));
                    }
                    else {
                        place_term(exp);
                    }
                }

                else if (*word != "") {

                    str_type lower_case = to_lower(*word);

                    auto it = OPERATORS.find(lower_case);

                    if (it != OPERATORS.end()) {

                        let opr = op_call(it->second);

                        place_term(opr);
                    }
                    else {

                        str_type upper_case = to_upper(*word);

                        auto it = OPERATORS.find(upper_case);

                        if (it != OPERATORS.end()) {

                            let opr = op_call(it->second);

                            place_term(opr);
                        }
                        else {

                            if (upper_case == "TRUE" || upper_case == "FALSE" ||
                                upper_case == "1" || upper_case == "0" ||
                                upper_case == "UNDEF" || upper_case == "UNDEFINED") {

                                place_term(boolean(upper_case));
                            }

                            //if (upper_case == "ELSE") {
                            //    place_term(boolean(true));
                            //}

                            else if (upper_case != "NOTHING" && upper_case != "NONE") {

                                place_term(symbol(*word));
                            }
                        }
                    }
                }

                ++word;
            } 

            return _code.lead().reverse();
        }

        bool_type compiler::is_prefix_unary_operator(OP_CODE opr) const {

            if (opr > OP_CODE::PREFIX_OPERATORS_START && opr < OP_CODE::PREFIX_OPERATORS_STOP) {
                return true;
            }
            return false;
        }

        bool_type compiler::is_infix_binary_operator(OP_CODE opr) const {

            if (opr > OP_CODE::INFIX_OPERATORS_START && opr < OP_CODE::INFIX_OPERATORS_STOP) {
                return true;
            }
            return false;
        }

        let compiler::get_postfix_operator(OP_CODE opr) const {

            let op;

            switch (opr) {

            case OP_CODE::neg_op:
                op = op_call(OP_CODE::NEG_op);
                break;

            case OP_CODE::POS_op:
                op = op_call(OP_CODE::POS_op);
                break;

            default:
                break;
            }

            return op;
        }

        let compiler::get_infix_operator(OP_CODE opr) const {

            let op;
            // TODO Must review if all these operators are in use.  

            switch (opr) {
                
            case OP_CODE::bind_op:
                op = op_call(OP_CODE::BIND_op);
                break;

            case OP_CODE::apply_op:
                op = op_call(OP_CODE::APPLY_op);
                break;

            case OP_CODE::and_op:
                op = op_call(OP_CODE::AND_op);
                break;

            case OP_CODE::or_op:
                op = op_call(OP_CODE::OR_op);
                break;

            case OP_CODE::xor_op:
                op = op_call(OP_CODE::XOR_op);
                break;

            case OP_CODE::eq_op:
                op = op_call(OP_CODE::EQ_op);
                break;

            case OP_CODE::ne_op:
                op = op_call(OP_CODE::NE_op);
                break;

            case OP_CODE::ge_op:
                op = op_call(OP_CODE::GE_op);
                break;

            case OP_CODE::gt_op:
                op = op_call(OP_CODE::GT_op);
                break;

            case OP_CODE::le_op:
                op = op_call(OP_CODE::LE_op);
                break;

            case OP_CODE::lt_op:
                op = op_call(OP_CODE::LT_op);
                break;

            case OP_CODE::add_op:
                op = op_call(OP_CODE::ADD_op);
                break;

            case OP_CODE::sub_op:
                op = op_call(OP_CODE::SUB_op);
                break;

            case OP_CODE::mul_op:
                op = op_call(OP_CODE::MUL_op);
                break;

            case OP_CODE::div_op:
                op = op_call(OP_CODE::DIV_op);
                break;

            case OP_CODE::mod_op:
                op = op_call(OP_CODE::MOD_op);
                break;

            case OP_CODE::fdiv_op:
                op = op_call(OP_CODE::FDIV_op);
                break;

            case OP_CODE::rem_op:
                op = op_call(OP_CODE::REM_op);
                break;

            case OP_CODE::pow_op:
                op = op_call(OP_CODE::POW_op);
                break;
                
            default:
                break;
            }

            return op;
        }

        void compiler::place_term(let t) {

            let terms = pop_lead(_code);

            terms = terms.place_lead(t);

            _code = _code.place_lead(terms);
        }

        str_type compiler::collect_string(tokens_type::const_iterator& word, const str_type& stop) {

            str_type text = "";

            ++word;

            while (*word != stop && word != _tokens.cend()) {

                text += *word;

                ++word;
            }

            return text;
        }

}
#endif // COMPILER_H