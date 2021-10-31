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

#include "evaluator.h"

namespace Olly {
	namespace eval {

		inline void evaluator::function_operators(OP_CODE& opr) {

			switch (opr) {

			case OP_CODE::end_scope_op: {    // Delete the current enclosure scope.
				delete_enclosure();
			}	break;

			case OP_CODE::ASSIGN_op: {    // Delete the current enclosure scope.

				let val = get_expression_from_stack();
				let var = get_expression_from_code();

				if (var.type() == "symbol") {
					set_symbol(var, val);
				}
			}	break;

			case OP_CODE::let_op: {  // Presupose the definition of a variable.  

				let vars = get_expression_from_code();
				let vals = get_expression_from_code();
				let oper = get_expression_from_code();

				if (oper.op_code() == OP_CODE::EQ_op) {

					if (vars.type() != "expression") {

						vars = expression(vars);
						vals = expression(vals);
					}

					while (vars.is()) {

						let var = pop_lead(vars);
						let val = pop_lead(vals);

						if (val.type() == "lambda") {
							set_expression_on_code(val);
							set_expression_on_code(var);
							set_expression_on_code(op_call(OP_CODE::def_op));
						}

						else {
							set_expression_on_code(var);
							set_expression_on_code(op_call(OP_CODE::ASSIGN_op));
							set_expression_on_code(val);
						}
					}
				}
				else if (oper.op_code() == OP_CODE::BIND_op) {

					let lam = get_symbol(vars);

					if (lam.type() == "lambda") {

						lambda l = lam.copy<lambda>();

						while (vals.is()) {

							let a = pop_lead(vals);
							let b = pop_lead(vals);
							let op = pop_lead(vals);

							if (op.op_code() == OP_CODE::EQ_op) {

								l.bind_variable(a, b);
							}
						}
						lam = l;

						set_symbol(vars, lam);
					}
				}
			}	break;

			case OP_CODE::def_op: {  // Presupose the definition of a constant or variable.  

				let var = get_expression_from_code();
				let args = get_expression_from_code();
				let body = get_expression_from_code();

				auto lam = lambda(args, body);

				if (_variables.size() > 1) {
					/*
						Define the lambda's internal closure.

						Ignore global scope for the purpose of setting the values of the enclosure.
						Get the current scope and bind it to the defined function.
					*/

					lam.bind_scope(_variables.back());
				}

				lam.bind_variable(var, lam);  // Provide a self reference for recursion.  

				lam.bind_variable(string("self"), var);  // Identify the self refrence.  

				set_expression_on_stack(lam);
				set_expression_on_code(var);
				set_expression_on_code(op_call(OP_CODE::ASSIGN_op));
			}	break;

			default:
				break;
			}
		}

	}  // end eval
} // end Olly