#ifndef ASSOCIATIVE_OPERATORS_H
#define ASSOCIATIVE_OPERATORS_H

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

        inline void evaluator::associative_operators(OP_CODE& opr) {

            switch (opr) {

            case OP_CODE::HAS_op: {

                let y = get_expression_from_stack();
                let x = get_expression_from_stack();

                x = boolean(x.has(y));

                set_expression_on_stack(x);

            }   break;

            case OP_CODE::GET_op: {

                let y = get_expression_from_stack();
                let x = get_expression_from_stack();

                x = x.get(y);

                set_expression_on_stack(x);

            }   break;

            case OP_CODE::SET_op: {

                let z = get_expression_from_stack();
                let y = get_expression_from_stack();
                let x = get_expression_from_stack();

                while (x.type() == "symbol") {
                    x = get_symbol(x);
                }

                x = x.set(y, z);

                set_expression_on_stack(x);

            }   break;

            case OP_CODE::DEL_op: {

                let y = get_expression_from_stack();
                let x = get_expression_from_stack();

                x = x.del(y);

                set_expression_on_stack(x);

            }   break;

            default:
                break;
            }
        }

    }  // end eval
}
#endif // ASSOCIATIVE_OPERATORS_H