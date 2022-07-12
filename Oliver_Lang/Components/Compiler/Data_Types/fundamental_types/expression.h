#pragma once

/********************************************************************************************/
//
//			Copyright 2019 Max J. Martin
//
//			This file is part of Oliver.
//			
//			Oliver is free software : you can redistribute it and / or modify
//			it under the terms of the GNU General Public License as published by
//			the Free Software Foundation, either version 3 of the License, or
//			(at your option) any later version.
//			
//			Oliver is distributed in the hope that it will be useful,
//			but WITHOUT ANY WARRANTY; without even the implied warranty of
//			MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//			GNU General Public License for more details.
//			
//			You should have received a copy of the GNU General Public License
//			along with Oliver.If not, see < https://www.gnu.org/licenses/>.
//			
/********************************************************************************************/

#include <concepts>
#include <iterator>
#include "../let.h"

namespace Olly {

    /********************************************************************************************/
    //
    //                               'expression' Class Definition
    //
    //          The expression class is implimented using Lisp inspired data nodes.  It
    //          is used to define the data sets as in Lisp.  
    //
    /********************************************************************************************/

    class expression {

        let _data;
        let _next;

    public:

        expression();
        expression(const expression& exp);
        expression(let obj);
        virtual ~expression();

        friend str_type           _type_(const expression& self);
        friend bool_type            _is_(const expression& self);
        friend real_type          _comp_(const expression& self, const let& other);

        friend void                _str_(stream_type& out, const expression& self);
        friend void               _repr_(stream_type& out, const expression& self);

        friend size_type          _size_(const expression& self);
        friend let                _lead_(const expression& self);
        friend let          _place_lead_(const expression& self, const let& other);
        friend let           _drop_lead_(const expression& self);
        friend let             _reverse_(const expression& self);
    };

    /********************************************************************************************/
    //
    //                                 'expression' Class Implimentation
    //
    /********************************************************************************************/

    expression::expression() : _data(), _next() {
    }

    expression::expression(const expression& exp) : _data(exp._data), _next(exp._next) {
    }

    expression::expression(let object) : _data(object), _next() {
    }

    expression::~expression() {
    }

    std::string _type_(const expression& self) {
        return "expression";
    }

    bool_type _is_(const expression& self) {

        if (self._data.is_nothing()) {
            return false;
        }

        return true;
    }

    real_type _comp_(const expression& self, const let& other) {

        const expression* ptr = other.cast<expression>();

        if (ptr) {

            let a = self;
            let b = *ptr;

            while (a.is() && b.is()) {

                if (a.lead() != b.lead()) {
                    return NOT_A_NUMBER;
                }

                a = a.drop_lead();
                b = b.drop_lead();
            }

            if (!a.is() && !b.is()) {
                return 0.0;
            }
        }

        return NOT_A_NUMBER;
    }

    void _str_(stream_type& out, const expression& self) {

        if (!_is_(self)) {
            out << "()";
            return;
        }

        let e = self;

        bool_type next = false;

        out << "(";

        do {
            e.lead().str(out);

            e = e.drop_lead();

            next = e.is();

            if (next) {
                out << " ";
            }

        } while (next);

        out << ")";
        // out.seekp(-1, out.cur);
    }

    void _repr_(stream_type& out, const expression& self) {

        if (!_is_(self)) {
            out << "()";
            return;
        }

        let e = self;

        bool_type next = false;

        out << "(";

        do {
            e.lead().repr(out);

            e = e.drop_lead();

            next = e.is();

            if (next) {
                out << " ";
            }

        } while (next);

        out << ")";
    }

    size_type _size_(const expression& self) {

        if (!_is_(self)) {
            return 0;
        }

        int_type size = 1;

        let next = self._next;

        while (next.is()) {

            size += 1;

            next = next.drop_lead();
        }

        return size;
    }

    let _lead_(const expression& self) {

        return self._data;
    }

    let _place_lead_(const expression& self, const let& other) {

        if (other.is_nothing()) {
            return self;
        }

        expression a(other);

        if (_is_(self)) {

            a._next = self;
        }

        return a;
    }

    let _drop_lead_(const expression& self) {

        if (self._next.is_nothing()) {

            return expression();
        }

        return self._next;
    }

    let _reverse_(const expression& self) {

        if (self._next.is_nothing()) {
            return self;
        }

        let a = expression();

        let next = self;

        while (next.is()) {

            a = a.place_lead(next.lead());

            next = next.drop_lead();
        }

        return a;
    }
}