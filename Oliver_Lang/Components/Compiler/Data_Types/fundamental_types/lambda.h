#ifndef LAMBDA_H
#define LAMBDA_H

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

#include "expression.h"

namespace Olly {

    /********************************************************************************************/
    //
    //                              'lambda' Class Definition
    //
    //          The lambda class is an ananymous lambda in Oliver. It consists of
    //          arguments, a body, and a scope.  Individual variables can be bound to
    //          the scope of a lambda after its definition.  
    //
    /********************************************************************************************/

    typedef     std::map<str_type, let>    map_type;

    class lambda {

        let    _args;
        let    _body;

        map_type  _scope;

    public:

        lambda();
        lambda(const lambda& exp);
        lambda(let exp);
        lambda(let args, let body);
        virtual ~lambda();

        friend str_type           _type_(const lambda& self);
        friend bool_type            _is_(const lambda& self);
        friend real_type          _comp_(const lambda& self, const let& other);
        friend void                _str_(stream_type& out, const lambda& self);
        friend void               _repr_(stream_type& out, const lambda& self);

        friend size_type          _size_(const lambda& self);
        friend let                _lead_(const lambda& self);
        friend let                _last_(const lambda& self);

        void bind_scope(const map_type& scope);
        void bind_variable(let var, let val);

        map_type variables() const;

        void print_enclosure() const;
    };

    /********************************************************************************************/
    //
    //                               'lambda' Class Implimentation
    //
    /********************************************************************************************/

    lambda::lambda() : _args(expression()), _body(expression()), _scope() {
    }

    lambda::lambda(const lambda& exp) : _args(exp._args), _body(exp._body), _scope(exp._scope) {
    }

    lambda::lambda(let exp) : _args(), _body(), _scope() {

        const lambda* l = exp.cast<lambda>();

        if (l) {
            _args = l->_args;
            _body = l->_body;
            _scope = l->_scope;
        }
    }

    lambda::lambda(let args, let body) : _args(args), _body(), _scope() {
    }

    lambda::~lambda() {
    }

    std::string _type_(const lambda& self) {
        return "lambda";
    }

    bool_type _is_(const lambda& self) {

        if (self._args.is() || self._body.is()) {
            return true;
        }

        return false;
    }

    real_type _comp_(const lambda& self, const let& other) {

        const lambda* e = other.cast<lambda>();

        if (e) {

            if ((self._args == e->_args) && (self._body == e->_body)) {
                return 0.0;
            }
        }

        return NOT_A_NUMBER;
    }

    void _str_(stream_type& out, const lambda& self) {

        if (!_is_(self)) {
            out << "lambda():;";
            return;
        }

        out << "lambda";

        self._args.str(out);

        out << ":";

        let body = self._body;

        if (body.is()) {
            out << " ";
        }

        while (body.is()) {
            pop_lead(body).str(out);
            out << " ";
        }

        out << ";";
    }

    void _repr_(stream_type& out, const lambda& self) {

        if (!_is_(self)) {
            out << "lambda():;";
            return;
        }

        out << "lambda";

        self._args.repr(out);

        out << ": ";

        let body = self._body;

        while (body.is()) {
            pop_lead(body).repr(out);
            out << " ";
        }

        out << ";";
    }

    size_type _size_(const lambda& self) {
        return self._args.size();
    }

    let _lead_(const lambda& self) {

        if (!self._args.is()) {
            return nothing();
        }

        return self._args;
    }

    let _last_(const lambda& self) {

        if (!self._body.is()) {
            return nothing();
        }

        return self._body;
    }

    inline void Olly::lambda::bind_scope(const map_type& scope) {

        auto name_loc = scope.find("self");

        auto name = (name_loc == scope.cend()) ? "" : str(name_loc->second);

        for (auto itr : scope) {

            if (itr.first != name) {

                _scope[itr.first] = itr.second;
            }
        }
    }

    inline void lambda::bind_variable(let var, let val) {
        _scope[str(var)] = val;
    }

    inline map_type Olly::lambda::variables() const {
        return _scope;
    }

    inline void Olly::lambda::print_enclosure() const {

        for (auto itr : _scope) {
            std::cout << itr.first << " = " << str(itr.second) << std::endl;
        }
    }

}
#endif // LAMBDA_H