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

		inline void evaluator::unary_operators(OP_CODE& opr) {

			let x = get_expression_from_stack();

			switch (opr) {

			case OP_CODE::NEG_op:
				x = x.neg();
				break;

			case OP_CODE::IS_op:
				x = boolean(x.is());
				break;

			default:
				break;
			}

			set_expression_on_stack(x);
		}

	}  // end eval
} // end Olly