#ifndef SEQUENCE_OPERATOR_H
#define SEQUENCE_OPERATOR_H

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

        inline void evaluator::sequence_operators(OP_CODE& opr) {

            switch (opr) {

            case OP_CODE::LEAD_op: {

                let x = get_expression_from_stack();

                x = x.lead();

                set_expression_on_stack(x);

            }   break;

            case OP_CODE::LAST_op: {

                let x = get_expression_from_stack();

                x = x.last();

                set_expression_on_stack(x);

            }   break;

            case OP_CODE::PLACE_op: {

                let y = get_expression_from_stack();
                let x = get_expression_from_stack();

                let opr = get_expression_from_code();

                auto op_code = opr.op_code();

                if (op_code == OP_CODE::LEAD_op) {

                    x = y.place_lead(x);
                }
                else if (op_code == OP_CODE::LAST_op) {

                    x = x.place_last(y);
                }
                else {
                    x = error("Invalid object placement.");
                }

                set_expression_on_stack(x);

            }   break;

            case OP_CODE::DROP_op: {

                let x = get_expression_from_stack();

                let opr = get_expression_from_code();

                auto op_code = opr.op_code();

                if (op_code == OP_CODE::LEAD_op) {

                    x = x.drop_lead();
                }
                else if (op_code == OP_CODE::LAST_op) {

                    x = x.drop_last();
                }
                else {
                    x = error("Invalid object drop.");
                }

                set_expression_on_stack(x);

            }   break;

            case OP_CODE::place_lead_op: {

                let x = get_expression_from_code();

                let place = op_call(OP_CODE::PLACE_op);
                let lead  = op_call(OP_CODE::LEAD_op);

                set_expression_on_code(lead);
                set_expression_on_code(place);
                set_expression_on_code(x);

            }   break;

            case OP_CODE::place_last_op: {

                let x = get_expression_from_code();

                let place = op_call(OP_CODE::PLACE_op);
                let last = op_call(OP_CODE::LAST_op);

                set_expression_on_code(last);
                set_expression_on_code(place);
                set_expression_on_code(x);

            }   break;

            case OP_CODE::drop_lead_op: {

                let x = get_expression_from_code();

                let drop = op_call(OP_CODE::DROP_op);
                let lead = op_call(OP_CODE::LEAD_op);

                set_expression_on_code(lead);
                set_expression_on_code(drop);
                set_expression_on_code(x);

            }   break;

            case OP_CODE::drop_last_op: {

                let drop = op_call(OP_CODE::DROP_op);
                let last = op_call(OP_CODE::LAST_op);

                set_expression_on_code(last);
                set_expression_on_code(drop);

            }   break;

            default:
                break;
            }
        }

    }  // end eval
}
#endif // SEQUENCE_OPERATOR_H