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

#include "../let.h"

namespace Olly {

	/********************************************************************************************/
	//
	//                                 '_node_' Class Definition
	//
	//          The _node_ class is implimented using Lisp inspired data nodes.  It
	//          is used to define the data lists as in Lisp.  
	//
	/********************************************************************************************/

	class _node_ {

		let _data;
		let _next;

	public:

		_node_();
		_node_(const _node_& exp);
		_node_(let obj);
		virtual ~_node_();

		friend str_type           _type_(const _node_& self);
		friend bool_type            _is_(const _node_& self);
		friend real_type          _comp_(const _node_& self, const let& other);

		friend void                _str_(stream_type& out, const _node_& self);
		friend void               _repr_(stream_type& out, const _node_& self);

		friend size_type          _size_(const _node_& self);
		friend let                _lead_(const _node_& self);
		friend let                _last_(const _node_& self);
		friend let          _place_lead_(const _node_& self, const let& other);
		friend let          _shift_lead_(const _node_& self);
		friend let             _reverse_(const _node_& self);
	};

	/********************************************************************************************/
	//
	//                                 '_node_' Class Implimentation
	//
	/********************************************************************************************/

	_node_::_node_() : _data(), _next() {
	}

	_node_::_node_(const _node_& exp) : _data(exp._data), _next(exp._next) {
	}

	_node_::_node_(let object) : _data(object), _next() {
	}

	_node_::~_node_() {
	}

	std::string _type_(const _node_& self) {
		return "_node_";
	}

	bool_type _is_(const _node_& self) {

		if (self._data.is_nothing()) {
			return false;
		}

		return true;
	}

	real_type _comp_(const _node_& self, const let& other) {

		const _node_* p = other.cast<_node_>();

		if (p) {

			let a = self;
			let b = *p;

			while (a.is() && b.is()) {

				if (a.lead() != b.lead()) {
					return NOT_A_NUMBER;
				}

				a = a.shift_lead();
				b = b.shift_lead();
			}

			if (!a.is() && !b.is()) {
				return 0.0;
			}
		}

		return NOT_A_NUMBER;
	}

	void _str_(stream_type& out, const _node_& self) {

		if (!_is_(self)) {
			return;
		}

		let e = self;

		bool_type next = false;

		do {
			out << str(e.lead());

			e = e.shift_lead();

			next = e.is();

			if (next) {
				out << " ";
			}

		} while (next);
	}

	void _repr_(stream_type& out, const _node_& self) {

		if (!_is_(self)) {
			return;
		}

		let e = self;

		bool_type next = false;

		do {
			out << repr(e.lead());

			e = e.shift_lead();

			next = e.is();

			if (next) {
				out << " ";
			}

		} while (next);
	}

	size_type _size_(const _node_& self) {

		if (!_is_(self)) {
			return 0;
		}

		int_type size = 1;

		let next = self._next;

		while (next.is()) {

			size += 1;

			next = next.shift_lead();
		}

		return size;
	}

	let _lead_(const _node_& self) {

		return self._data;
	}

	let _last_(const _node_& self) {

		if (self._next.is_nothing()) {

			return self;
		}

		let next = self;

		while (next.shift_lead().is()) {

			next = next.shift_lead();
		}

		return next;
	}

	let _place_lead_(const _node_& self, const let& other) {

		if (other.is_nothing()) {
			return self;
		}

		_node_ a(other);

		if (_is_(self)) {

			a._next = self;
		}

		return a;
	}

	let _shift_lead_(const _node_& self) {

		if (self._next.is_nothing()) {

			return _node_();
		}

		return self._next;
	}

	let _reverse_(const _node_& self) {

		if (_atomic_(self)) {

			return self;
		}

		let a = _node_();

		let next = self;

		while (next.is()) {

			a = a.place_lead(next.lead());

			next = next.shift_lead();
		}

		return a;
	}

	/********************************************************************************************/
	//
	//                              '_node_expr_' Class Definition
	//
	//          The _node_expr_ class is a Lisp linked list inspired data type.  Which
	//          manages a list of _node_ objects.  Strictly speaking this class is not
	//          necessary for this project.  The _node_ class alone would suffice to 
	//          allow all of the necessary functions and data types to be implimented on 
	//          thier own.  But this class does allow for an extra layer of abstraction, 
	//          and facilitates easier code comprehension.  
	//
	/********************************************************************************************/

	class _node_expr_ {

		let        _queue;
		size_type  _len;

	public:

		_node_expr_();
		_node_expr_(const _node_expr_& exp);
		_node_expr_(let x);
		_node_expr_(let x, let y);
		virtual ~_node_expr_();

		friend str_type           _type_(const _node_expr_& self);
		friend bool_type            _is_(const _node_expr_& self);
		friend real_type          _comp_(const _node_expr_& self, const let& other);

		friend void                _str_(stream_type& out, const _node_expr_& self);
		friend void               _repr_(stream_type& out, const _node_expr_& self);

		friend size_type          _size_(const _node_expr_& self);
		friend let                _lead_(const _node_expr_& self);
		friend let                _last_(const _node_expr_& self);
		friend let          _place_lead_(const _node_expr_& self, const let& other);
		friend let          _shift_lead_(const _node_expr_& self);
		friend let             _reverse_(const _node_expr_& self);
	};

	/********************************************************************************************/
	//
	//                               '_node_expr_' Class Implimentation
	//
	/********************************************************************************************/

	_node_expr_::_node_expr_() : _queue(_node_()), _len(0) {
	}

	_node_expr_::_node_expr_(const _node_expr_& exp) : _queue(exp._queue), _len(exp._len) {
	}

	_node_expr_::_node_expr_(let x) : _queue(_node_(x)), _len(0) {
		if (_queue.is()) {
			_len = 1;
		}
	}

	_node_expr_::_node_expr_(let x, let y) : _queue(_node_()), _len(0) {

		_queue = _queue.place_lead(y);

		if (_queue.size() > _len) {
			_len += 1;
		}

		_queue = _queue.place_lead(x);

		if (_queue.size() > _len) {
			_len += 1;
		}
	}

	_node_expr_::~_node_expr_() {
	}

	std::string _type_(const _node_expr_& self) {
		return "_node_expr_";
	}

	bool_type _is_(const _node_expr_& self) {

		if (self._len) {
			return true;
		}

		return false;
	}

	real_type _comp_(const _node_expr_& self, const let& other) {

		const _node_expr_* e = other.cast<_node_expr_>();

		if (e) {

			if (self._len == e->_len) {
				return (self._queue == e->_queue);
			}
		}

		return NOT_A_NUMBER;
	}

	void _str_(stream_type& out, const _node_expr_& self) {

		self._queue.str(out);
	}

	void _repr_(stream_type& out, const _node_expr_& self) {

		self._queue.repr(out);
	}

	size_type _size_(const _node_expr_& self) {

		return self._len;
	}

	let _lead_(const _node_expr_& self) {

		return self._queue.lead();
	}

	let _last_(const _node_expr_& self) {

		return self._queue.last();
	}

	let _place_lead_(const _node_expr_& self, const let& other) {

		if (other.is_nothing()) {
			return self;
		}

		_node_expr_ e = self;

		e._queue = e._queue.place_lead(other);
		e._len += 1;

		return e;
	}

	let _shift_lead_(const _node_expr_& self) {

		if (self._len == 0) {
			return nothing();
		}

		_node_expr_ e = self;

		e._queue = e._queue.shift_lead();
		e._len -= 1;

		return e;
	}

	let _reverse_(const _node_expr_& self) {

		if (self._len < 2) {
			return self;
		}

		_node_expr_ e = self;

		e._queue = e._queue.reverse();

		return e;
	}

	/********************************************************************************************/
	//
	//                              'expression' Class Definition
	//
	//          The expression class is a Lisp linked list inspired data type.  Which
	//          manages a list of _node_ objects.  Strictly speaking this class is not
	//          necessary for this project.  The _node_ class alone would suffice to 
	//          allow all of the necessary functions and data types to be implimented on 
	//          thier own.  But this class does allow for an extra layer of abstraction, 
	//          and facilitates easier code comprehension.  
	//
	/********************************************************************************************/

	class expression {

		let        _expr;

	public:

		expression();
		expression(const expression& exp);
		expression(let x);
		expression(let x, let y);
		virtual ~expression();

		friend str_type           _type_(const expression& self);
		friend bool_type            _is_(const expression& self);
		friend real_type          _comp_(const expression& self, const let& other);

		friend void                _str_(stream_type& out, const expression& self);
		friend void               _repr_(stream_type& out, const expression& self);

		friend size_type          _size_(const expression& self);
		friend let                _lead_(const expression& self);
		friend let                _last_(const expression& self);
		friend let          _place_lead_(const expression& self, const let& other);
		friend let          _shift_lead_(const expression& self);
		friend let             _reverse_(const expression& self);
	};

	/********************************************************************************************/
	//
	//                               'expression' Class Implimentation
	//
	/********************************************************************************************/

	expression::expression() : _expr(_node_expr_()) {
	}

	expression::expression(const expression& exp) : _expr(exp._expr) {
	}

	expression::expression(let x) : _expr(_node_expr_(x)) {
	}

	expression::expression(let x, let y) : _expr(_node_expr_(x, y)) {
	}

	expression::~expression() {
	}

	std::string _type_(const expression& self) {
		return "expression";
	}

	bool_type _is_(const expression& self) {

		return self._expr.is();
	}

	real_type _comp_(const expression& self, const let& other) {

		return self._expr.comp(other);
	}

	void _str_(stream_type& out, const expression& self) {

		if (!_is_(self)) {
			out << "()";
			return;
		}

		out << "(";

		self._expr.str(out);

		out << ")";
	}

	void _repr_(stream_type& out, const expression& self) {

		if (!_is_(self)) {
			out << "()";
			return;
		}

		out << "(";

		self._expr.repr(out);

		out << ")";
	}

	size_type _size_(const expression& self) {

		return self._expr.size();
	}

	let _lead_(const expression& self) {

		return self._expr.lead();
	}

	let _last_(const expression& self) {

		return self._expr.last();
	}

	let _place_lead_(const expression& self, const let& other) {

		if (other.is_nothing()) {
			return self;
		}

		expression e = self;

		e._expr = e._expr.place_lead(other);

		return e;
	}

	let _shift_lead_(const expression& self) {

		if (self._expr.size() == 0) {
			return nothing();
		}

		expression e = self;

		e._expr = e._expr.shift_lead();

		return e;
	}

	let _reverse_(const expression& self) {

		if (self._expr.size() < 2) {
			return self;
		}

		expression e = self;

		e._expr = e._expr.reverse();

		return e;
	}
}