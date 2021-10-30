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

#include "../../let/let.h"
#include "text_reader.h"

namespace Olly {
	namespace parser {

		/********************************************************************************************/
		//
		//                              'token_reader' class definition
		//
		//        The token_reader class simply manages iteration over a series of text tokens.
		//
		/********************************************************************************************/

		class token_reader {

		public:

			token_reader(tokens_type& input_code);
			virtual ~token_reader();

			str_type next();
			str_type peek();

			bool_type is() const;

			bool_type check_if_bounded(str_type left, str_type right);

		private:

			tokens_type& _code;
			str_type      _word;
			int_type         _i;

			token_reader() = delete;
			token_reader(const token_reader& obj) = delete;
		};


		token_reader::token_reader(tokens_type& input_code) : _code(input_code), _i(0), _word("") {

			if (is()) {
				_word = _code[_i];
			}
		}

		token_reader::~token_reader() {
		}

		str_type token_reader::next() {

			if (is()) {

				str_type t = _word;

				_i += 1;

				_word = is() ? _code[_i] : "";

				return t;
			}
			else {
				_i = static_cast<int_type>(_code.size());
			}

			return "";
		}

		str_type token_reader::peek() {
			/*
				Return the current value of '_c'.
			*/
			return _word;
		}

		bool_type token_reader::is() const {
			/*
				Return true if the file is not eof
				and in good condition.
			*/
			return (_i < _code.size());
		}

		bool_type token_reader::check_if_bounded(str_type left, str_type right) {

			if (is()) {

				if (_code.back() == right) {
					_code.pop_back();

					if (_code.back() == right && _word == left) {
						return true;
					}
					else {
						_code.push_back(right);
					}
				}
			}
			return false;
		}
	}  // end parser
} // end Olly