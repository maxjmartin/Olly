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

#include "../compiler/parser.h"

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


		parser::token_reader  _text;
		let                   _code;

	public:

		compiler(tokens_type& text);
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

		str_type collect_string(const str_type& stop);
	};

	/********************************************************************************************/
	//
	//                              'compiler' class implimentation
	//
	/********************************************************************************************/

	inline compiler::compiler(tokens_type& text) : _text(text), _code(expression()) {
		_code = _code.place_lead(expression());
	}

	inline compiler::~compiler() {
	}

	inline bool_type compiler::is_prefix_unary_operator(OP_CODE opr) const {

		if (opr > OP_CODE::PREFIX_OPERATORS_START && opr < OP_CODE::PREFIX_OPERATORS_STOP) {
			return true;
		}
		return false;
	}

	inline bool_type compiler::is_infix_binary_operator(OP_CODE opr) const {

		if (opr > OP_CODE::INFIX_OPERATORS_START && opr < OP_CODE::INFIX_OPERATORS_STOP) {
			return true;
		}
		return false;
	}

	inline let compiler::get_postfix_operator(OP_CODE opr) const {

		let op;

		switch (opr) {

		case OP_CODE::neg_op:
			op = op_call(OP_CODE::NEG_op);
			break;

		default:
			break;
		}

		return op;
	}

	inline let compiler::get_infix_operator(OP_CODE opr) const {

		let op;

		switch (opr) {

		case OP_CODE::and_op:
			op = op_call(OP_CODE::AND_op);
			break;

		case OP_CODE::or_op:
			op = op_call(OP_CODE::OR_op);
			break;

		case OP_CODE::xor_op:
			op = op_call(OP_CODE::XOR_op);
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

	inline let compiler::compile() {

		if (!_text.is()) {
			return unwrap_expresion(_code);
		}

		str_type word = _text.next();

		if (word == "(") { 
			_code = _code.place_lead(expression());
		}
		else if (word == ")") {

			let terms = pop_lead(_code);
			let exp = expression();

			while (terms.is()) {

				let term = pop_lead(terms);

				if (term.op_code() == OP_CODE::lambda_op) {
					// Define an anonymous function.

					let a = pop_lead(exp);
					let b = pop_lead(exp);

					exp = exp.place_lead(lambda(a, b));
				}

				else if (is_prefix_unary_operator(term.op_code())) {

					let a = pop_lead(exp);
					let b = get_postfix_operator(term.op_code());

					let p = expression();

					p = p.place_lead(b);
					p = p.place_lead(a);

					exp = exp.place_lead(p);
				}

				else if (is_infix_binary_operator(term.op_code())) {

					let a = pop_lead(exp);
					let b = get_infix_operator(term.op_code());

					exp = exp.place_lead(b);
					exp = exp.place_lead(a);
				}
\
				else {
					exp = exp.place_lead(term);
				}
			}

			terms = pop_lead(_code);

			terms = terms.place_lead(exp);

			_code = _code.place_lead(terms);
		}

		else if (word == "\'") {
			str_type str = collect_string("\'");

			place_term(number(str));
		}

		else if (word == "\"") {
			str_type str = collect_string("\"");

			place_term(string(str));
		}

		else if (word != "") {

			auto it = OPERATORS.find(word);

			if (it != OPERATORS.end()) {

				let opr = op_call(it->second);

				place_term(opr);
			}
			else {

				str_type upper_case = to_upper(word);

				if (upper_case == "TRUE" || upper_case == "FALSE" ||
					upper_case == "1" || upper_case == "0" ||
					upper_case == "UNDEF" || upper_case == "UNDEFINED") {

					place_term(boolean(upper_case));
				}

				//if (upper_case == "ELSE") {
				//	exp = exp.place_lead(boolean(true));
				//}

				else if (upper_case != "NOTHING") {

					place_term(symbol(word));
				}
			}
		}

		return compile();
	}

	inline void compiler::place_term(let t) {

		let terms = pop_lead(_code);

		terms = terms.place_lead(t);

		_code = _code.place_lead(terms);
	}

	str_type compiler::collect_string(const str_type& stop) {

		str_type word = _text.next();
		str_type text = "";

		while (word != stop && word != "") {

			if (word != "") {
				text += word;
				word = _text.next();
			}
		}

		return text;
	}

	//let parser::compile() {

	//	token_reader code(_text);

	//	if (!code.is()) {
	//		return expression();
	//	}

	//	str_type word;
	//	let exp = expression();

	//	while (code.is()) {

	//		word = code.next();

	//		compile(word, code, exp);
	//	}

	//	return exp.reverse();
	//}

	//void parser::compile(str_type& word, token_reader& code, let& exp) {


	//	//if (word == ":") {

	//	//	let e = statement();

	//	//	// e = e.place_lead(op_call(OP_CODE::begin_scope_op));

	//	//	while (code.is() && word != ";") {

	//	//		word = code.next();

	//	//		compile(word, code, e);
	//	//	}
	//	//	word = "";

	//	//	// e = e.place_lead(op_call(OP_CODE::end_scope_op));

	//	//	exp = exp.place_lead(e.reverse());

	//	//	return;
	//	//}

	//	//if (word == "[") {

	//	//	let e = expression();

	//	//	while (code.is() && word != "]") {

	//	//		word = code.next();

	//	//		compile(word, code, e);
	//	//	}
	//	//	word = "";

	//	//	let l = list(e.reverse());

	//	//	exp = exp.place_lead(l);

	//	//	return;
	//	//}

	//	//if (word == "{") {

	//	//	let e = expression();

	//	//	while (code.is() && word != "}") {

	//	//		word = code.next();

	//	//		compile(word, code, e);
	//	//	}
	//	//	word = "";

	//	//	// exp = exp.place_lead(set(e));

	//	//	return;
	//	//}

	//	if (word != "") {

	//		auto it = OPERATORS.find(word);

	//		if (it != OPERATORS.end()) {

	//			let opr = op_call(it->second);

	//			if (it->second < OP_CODE::PREFIX_UNARY_OPERATORS) {
	//				/*
	//					Handle prefix unary operators by placing the next code expression
	//					and it within it own expression.  This ensures that binary operators
	//					execute in the right order.
	//				*/
	//				let e = expression();
	//				word = code.next();
	//				compile(word, code, e);
	//				word = "";

	//				opr = e.place_lead(opr);
	//			}

	//			exp = exp.place_lead(opr);
	//			return;
	//		}

	//		str_type upper_case = to_upper(word);

	//		if (upper_case == "TRUE" || upper_case == "FALSE" ||
	//			upper_case == "1" || upper_case == "0" ||
	//			upper_case == "UNDEF" || upper_case == "UNDEFINED") {

	//			exp = exp.place_lead(boolean(upper_case));
	//			return;
	//		}

	//		if (upper_case == "ELSE") {
	//			exp = exp.place_lead(boolean(true));
	//		}

	//		else if (upper_case != "NOTHING") {
	//			exp = exp.place_lead(symbol(word));
	//		}
	//	}
	//	return;
	//}
} // end Olly