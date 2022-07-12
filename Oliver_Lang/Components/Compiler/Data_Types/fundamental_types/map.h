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

#include "./Components/Compiler/Data_Types/fundamental_types/support_types/integer_value.h"

namespace Olly {

    /********************************************************************************************/
    //
    //                               'map' Class Definition
    //
    //          The map class is implimented using Lisp inspired data maps.  It
    //          is used to define the data maps as in Lisp.  
    //
    /********************************************************************************************/

    class map {

        let _node;

    public:

        map();
        map(const map& l);
        map(let exp);
        map(let x, let y);
        virtual ~map();

        friend str_type           _type_(const map& self);
        friend bool_type            _is_(const map& self);
        friend real_type          _comp_(const map& self, const let& other);

        friend void                _str_(stream_type& out, const map& self);
        friend void               _repr_(stream_type& out, const map& self);

        friend size_type          _size_(const map& self);
        friend bool_type           _has_(const map& self, const let& key);
        friend let                 _set_(const map& self, const let& key, const let& value);
        friend let                 _get_(const map& self, const let& key);
        friend let               _clear_(const map& self);

    private:
        void balance();
        let  get_list() const;
        let  get_list(let node) const;

        let make_pair(let a, let b);
        let make_node(let p, let l, let r);

        let set_value(let node, let key, let value);
        let set_branch(let node, let key, let value);

        size_type calculate_height(let node);

        let rotate(let node);
    };

    /********************************************************************************************/
    //
    //                                 'map' Class Implimentation
    //
    /********************************************************************************************/

    map::map() : _node(expression()) {
    }

    map::map(const map& s) : _node(s._node) {
    }

    map::map(let exp) : _node(expression()) {

        while (exp.is()) {

            let key = pop_lead(exp);
            let val = pop_lead(exp);
            let opr = pop_lead(exp);

            if (opr.op_code() == OP_CODE::EQ_op) {

                _node = set_value(_node, key, val);
            }
        }
        // print("node = " + str(_node));
    }

    map::map(let x, let y) : _node() {
    }

    map::~map() {
    }

    std::string _type_(const map& self) {
        return "map";
    }

    bool_type _is_(const map& self) {
        return self._node.is();
    }

    real_type _comp_(const map& self, const let& other) {

        const map* ptr = other.cast<map>();

        if (ptr) {

            return self._node.comp(ptr->_node);
        }

        return NOT_A_NUMBER;
    }

    void _str_(stream_type& out, const map& self) {

        if (!_is_(self)) {
            out << "{}";
            return;
        }

        out << "{";

        let l = self.get_list();

        while (l.is()) {

            let p = pop_lead(l);

            let k = pop_lead(p);
            let v = pop_lead(p);

            k.str(out);

            out << " = ";

            v.str(out);

            out << ", ";
        }

        out.seekp(-2, out.cur);
        out << "}";
    }

    void _repr_(stream_type& out, const map& self) {

        if (!_is_(self)) {
            out << "{}";
            return;
        }

        out << "{";

        let l = self.get_list();

        while (l.is()) {

            let p = pop_lead(l);

            let k = pop_lead(p);
            let v = pop_lead(p);

            k.repr(out);

            out << " = ";

            v.repr(out);

            out << ", ";
        }

        out.seekp(-2, out.cur);
        out << "}";
    }

    size_type _size_(const map& self) {

        let list = self.get_list();

        return list.size();
    }

    bool_type _has_(const map& self, const let& key) {

        let node = self._node;

        while (node.is()) {

            let pair = first(node);

            if (first(pair) == key) {
                return true;
            }

            if (key < first(pair)) {

                node = second(node);  // left
            }
            else {
                node = third(node);  // right
            }
        }

        return false;
    }

    let _set_(const map& self, const let& key, const let& value) {

        if (!_is_(self)) {
            return map(key, value);
        }

        map m = self;

        m._node = m.set_value(m._node, key, value);

        return m;
    }

    let _get_(const map& self, const let& key) {

        let node = self._node;

        while (node.is()) {

            let pair = first(node);

            if (first(pair) == key) {
                return second(pair);
            }

            if (key < first(pair)) {

                node = second(node);  // left
            }
            else {
                node = third(node);  // right
            }
        }

        return nothing();
    }

    let _clear_(const map& self) {
        return map();
    }

    inline void map::balance() {
    }

    inline let map::get_list() const {
        return get_list(_node);
    }

    inline let map::get_list(let node) const {

        let buffer = expression();

        let result = expression();

        let n = node;

        while (n.is() || buffer.is()) {

            while (n.is()) {  // Loop through all left side branches.

                buffer = buffer.place_lead(n);

                n = second(n);
            }

            n = pop_lead(buffer);

            // For other effects place manipulation code here.

            result = result.place_lead(n.lead());

            // End code manipulation.

            n = third(n);  // Get any right side branches that may exist.
        }

        return result;
    }

    inline let map::make_pair(let a, let b) {

        let e = expression();
        e = e.place_lead(b);
        e = e.place_lead(a);

        return e;
    }

    inline let map::set_value(let node, let key, let value) {

        let new_node = set_branch(node, key, value);

        if (new_node.is_nothing()) {

            auto left_right = std::vector<bool_type>();
            auto queue      = std::vector<let>();

            while (new_node.is_nothing()) {

                queue.push_back(node);

                let pair = first(node);

                if (first(pair) < key) {  // left

                    left_right.push_back(true);

                    node = second(node);

                    new_node = set_branch(node, key, value);
                }
                else if (first(pair) > key) {  // right

                    left_right.push_back(false);

                    node = third(node);

                    new_node = set_branch(node, key, value);
                }
            }

            // Check for and perform rotation.
            if (!queue.empty()) {

                let parent_node = queue.back();
                queue.pop_back();

                bool_type push_left = left_right.back();
                left_right.pop_back();

                if (push_left) {
                    new_node = make_node(first(parent_node), new_node, third(parent_node));
                }
                else {
                    new_node = make_node(first(parent_node), second(parent_node), new_node);
                }
                new_node = rotate(new_node);
            }
            
            // print("new_node = " + str(new_node));

            // Balance map here.  
            //size_type  left_height = calculate_height(second(new_node));
            //size_type right_height = calculate_height( third(new_node));

            //std::cout << " left_height = " <<  left_height << std::endl;
            //std::cout << "right_height = " << right_height << std::endl;

            auto j = left_right.crbegin();

            for (auto i = queue.crbegin(); i != queue.crend(); ++i, ++j) {

                if (*j) {
                    new_node = make_node(first(*i), new_node, third(*i));

                    new_node = rotate(new_node);
                }
                else {
                    new_node = make_node(first(*i), second(*i), new_node);
                }
            }
        }
        // print("new_node = " + str(new_node));

        return new_node;
    }

    inline let map::set_branch(let node, let key, let value) {

        if (!node.is()) {
            return make_node(make_pair(key, value), expression(), expression());
        }

        let pair = first(node);

        if (first(pair) == key) {

            return make_node(make_pair(key, value), second(node), third(node));
        }

        return nothing();
    }

    inline let map::make_node(let p, let l, let r) {

        let e = expression();
        e = e.place_lead(integer_type());
        e = e.place_lead(r);
        e = e.place_lead(l);
        e = e.place_lead(p);

        return e;
    }

    inline size_type map::calculate_height(let node) {
        return get_list(node).size();
    }

    inline let map::rotate(let node) {

        let temp = node;

        size_type  left_height = calculate_height(second(node));
        size_type right_height = calculate_height( third(node));

        //std::cout << " left height = " << left_height << std::endl;
        //std::cout << "right height = " << right_height << std::endl;
        
        if (left_height > right_height) {  // Left rotation.

            let new_node = second(node);

            node = make_node(first(node), third(new_node), third(node));

            node = make_node(first(new_node), second(new_node), node);
        }

        else if (left_height < right_height) {  // Right rotation.

            let new_node = third(node);

            node = make_node(first(node), second(node), second(new_node));

            node = make_node(first(new_node), node, third(new_node));
        }

        if (temp != node) {
            print("old node = " + str(temp));
            print("new node = " + str(node));
        }
        
        return node;
    }
}