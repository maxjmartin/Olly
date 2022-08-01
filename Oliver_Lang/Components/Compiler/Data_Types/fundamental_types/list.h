#ifndef LIST_H
#define LIST_H

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

namespace Olly {

    /********************************************************************************************/
//
//                               'list' Class Definition
//
//          The list class is implimented using Lisp inspired data lists.  It
//          is used to define the data lists as in Lisp.  
//
/********************************************************************************************/

    class list {

        let _lead;
        let _last;

        int_type _size_lead;
        int_type _size_last;

    public:

        list();
        list(const list& l);
        list(let x);
        list(let x, let y);
        virtual ~list();

        friend str_type           _type_(const list& self);
        friend bool_type            _is_(const list& self);
        friend real_type          _comp_(const list& self, const let& other);

        friend void                _str_(stream_type& out, const list& self);
        friend void               _repr_(stream_type& out, const list& self);

        friend size_type          _size_(const list& self);
        friend let                _lead_(const list& self);
        friend let                _last_(const list& self);

        friend let          _place_lead_(const list& self, const let& other);
        friend let          _place_last_(const list& self, const let& other);
        friend let           _drop_lead_(const list& self);
        friend let           _drop_last_(const list& self);

        friend let             _reverse_(const list& self);
        friend let                 _add_(const list& self, const let& other);
        friend bool_type           _has_(const list& self, const let& other);
        friend let               _clear_(const list& self);

    private:
        void balance();
    };

    /********************************************************************************************/
    //
    //                                 'list' Class Implimentation
    //
    /********************************************************************************************/

    list::list() : _lead(expression()), _last(expression()), _size_lead(0), _size_last(0) {
    }

    list::list(const list& l) : _lead(l._lead), _last(l._last), _size_lead(l._size_lead), _size_last(l._size_last) {
    }

    list::list(let x) : _lead(expression(x)), _last(expression()), _size_lead(0), _size_last(0) {

        if (!_lead.lead().is_nothing()) {
            _size_lead += 1;
        }
    }

    list::list(let x, let y) : _lead(expression(x)), _last(expression(y)), _size_lead(0), _size_last(0) {

        if (!_lead.lead().is_nothing()) {
            _size_lead += 1;
        }

        if (!_lead.last().is_nothing()) {
            _size_last += 1;
        }
    }

    list::~list() {
    }

    std::string _type_(const list& self) {
        return "list";
    }

    bool_type _is_(const list& self) {

        if (self._lead.lead().is_nothing() && self._last.lead().is_nothing()) {
            return false;
        }

        return true;
    }

    real_type _comp_(const list& self, const let& other) {

        const list* ptr = other.cast<list>();

        if (ptr) {

            return (self._lead.comp(ptr->_lead) + self._last.comp(ptr->_last));
        }

        return NOT_A_NUMBER;
    }

    void _str_(stream_type& out, const list& self) {

        if (!_is_(self)) {
            out << "[]";
            return;
        }

        let a = self._lead;
        let b = self._last.reverse();

        bool_type next = a.is();

        out << "[";

        while (next) {
            a.lead().str(out);

            a = a.drop_lead();

            next = a.is();

            out << " ";
        }

        next = b.is();

        while (next) {
            b.lead().str(out);

            b = b.drop_lead();

            next = b.is();

            out << " ";
        }

        out.seekp(-1, out.cur);
        out << "]";
    }

    void _repr_(stream_type& out, const list& self) {

        if (!_is_(self)) {
            out << "[]";
            return;
        }

        let a = self._lead;
        let b = self._last.reverse();

        bool_type next = a.is();

        out << "[";

        while (next) {
            a.lead().repr(out);

            a = a.drop_lead();

            next = a.is();

            out << " ";
        }

        next = b.is();

        while (next) {
            b.lead().repr(out);

            b = b.drop_lead();

            next = b.is();

            out << " ";
        }

        out.seekp(-1, out.cur);
        out << "]";
    }

    size_type _size_(const list& self) {
        return static_cast<size_type>(self._size_lead + self._size_last);
    }

    let _lead_(const list& self) {

        return (self._lead.is() ? self._lead.lead() : self._last.lead());
    }

    let _last_(const list& self) {

        return (self._last.is() ? self._last.lead() : self._lead.lead());
    }

    let _place_lead_(const list& self, const let& other) {

        if (other.is_nothing()) {
            return self;
        }

        list a = self;

        if (!a._last.is()) {
            a.balance();
        }
        a._lead = a._lead.place_lead(other);
        a._size_lead += 1;

        return a;
    }

    let _place_last_(const list& self, const let& other) {

        if (other.is_nothing()) {
            return self;
        }

        list a = self;

        if (!a._lead.is()) {
            a.balance();
        }
        a._last = a._last.place_lead(other);
        a._size_last += 1;

        return a;
    }

    let _drop_lead_(const list& self) {

        if (!_is_(self)) {
            return self;
        }

        list a = self;

        if (!a._lead.is() && a._last.is()) {
            a.balance();
        }

        if (a._lead.is()) {
            a._lead = a._lead.drop_lead();
        }
        else {
            a._last = a._last.drop_lead();
        }

        return a;
    }

    let _drop_last_(const list& self) {

        if (!_is_(self)) {
            return self;
        }

        list a = self;

        if (!a._last.is() && a._lead.is()) {
            a.balance();
        }

        if (a._last.is()) {
            a._last = a._last.drop_lead();
        }
        else {
            a._lead = a._lead.drop_lead();
        }

        return a;
    }

    let _reverse_(const list& self) {

        if (_atomic_(self)) {

            return self;
        }

        list a;

        a._last = self._lead;
        a._lead = self._last;
        
        a._size_last = self._size_lead;
        a._size_lead = self._size_last;

        return a;
    }

    void list::balance() {

        if (_lead.is() && !_last.is()) {

            _size_last = _size_lead - 1;

            while (_size_lead-- > 1) {
                _last = _last.place_lead(pop_lead(_lead));
            }

            return;
        }
        else 

        if (_last.is() && !_lead.is()) {

            _size_lead = _size_last - 1;

            while (_size_last-- > 1) {
                _lead = _lead.place_lead(pop_lead(_last));
            }
        }
    }

    let _add_(const list& self, const let& other) {
        return _place_last_(self, other);
    }

    bool_type _has_(const list& self, const let& other) {
        
        let l = self;

        while (l.is()) {

            if (other == pop_lead(l)) {
                return true;
            }
        }

        return false;
    }

    let _clear_(const list& self) {

        return list();
    }
}
#endif // LIST_H