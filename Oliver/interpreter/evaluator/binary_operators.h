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

		inline void evaluator::binary_operators(OP_CODE& opr) {

			let y = get_expression_from_stack();
			let x = get_expression_from_stack();

			switch (opr) {

			case OP_CODE::AND_op:
				x = x.l_and(y);
				break;

			case OP_CODE::OR_op:
				x = x.l_or(y);
				break;

			case OP_CODE::XOR_op:
				x = x.l_xor(y);
				break;

			case OP_CODE::EQ_op:
				x = boolean(x == y);
				break;

			case OP_CODE::NE_op:
				x = boolean(x != y);
				break;

			case OP_CODE::GT_op:
				x = boolean(x > y);
				break;

			case OP_CODE::GE_op:
				x = boolean(x >= y);
				break;

			case OP_CODE::LT_op:
				x = boolean(x < y);
				break;

			case OP_CODE::LE_op:
				x = boolean(x <= y);
				break;

			case OP_CODE::ADD_op:
				x = x + y;
				break;

			case OP_CODE::SUB_op:
				x = x - y;
				break;

			case OP_CODE::MUL_op:
				x = x * y;
				break;

			case OP_CODE::DIV_op:
				x = x / y;
				break;

			case OP_CODE::MOD_op:
				x = x % y;
				break;

			case OP_CODE::FDIV_op:
				x = x.f_div(y);
				break;

			case OP_CODE::REM_op:
				x = x.rem(y);
				break;

			case OP_CODE::POW_op:
				x = x.pow(y);
				break;

			default:
				break;
			}

			set_expression_on_stack(x);
		}

	}  // end eval
} // end Olly