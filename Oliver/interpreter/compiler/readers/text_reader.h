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


#include <fstream>
#include <mutex>
#include "../../let/let.h"

namespace Olly {
	namespace parser {

		/********************************************************************************************/
		//
		//                              'text_reader' class definition
		//
		//        The text_reader class opens a file and then passes each individual character
		//        to the parser for evaluation.  If a valid file is not opened then the string
		//        itself is iterated over for reading.  
		//
		/********************************************************************************************/

		class text_reader {

			typedef		char						char_t;
			typedef		std::fstream				file_t;
			typedef		std::recursive_mutex		mutex_t;
			typedef     std::lock_guard<mutex_t>    lock_t;

			char_t      _c;
			file_t		_input;
			bool_type   _is_file;
			stream_type _stream;
			mutex_t		_mutex;

		public:

			text_reader(const str_type& inp);
			virtual ~text_reader();

			char_t next();
			char_t peek();

			str_type get_line();

			bool_type is();

			bool_type is_file();

		private:
			text_reader();
			text_reader(const text_reader& obj) = delete;
		};

		text_reader::text_reader() : _c('\0'), _input(), _is_file(false), _stream(), _mutex() {
		}

		text_reader::text_reader(const str_type& input_code) : _c('\0'), _input(), _is_file(true), _stream(), _mutex() {
			/*
				Open a file 'file_name' and retrieve its first character
				Setting the value of the character to '_c'.  So long as
				the file is not empty.
			*/
			lock_t lock(_mutex);

			_input = file_t(input_code, file_t::in);

			if (_input.is_open() && _input.good() && !_input.eof()) {

				_c = _input.get();
			}
			else {
				/*
					Not a file, so send the text to a stream to be
					treated just like a file.
				*/
				_is_file = false;
				_stream << input_code;

				_c = _stream.get();
			}
		}

		text_reader::~text_reader() {
			_input.close();
		}

		text_reader::char_t text_reader::next() {
			/*
				As long as the file is not at the end of the file,
				return the current value of '_c' and set the next
				value of '_c'.  Return a null character if the
				file is eof.
			*/

			if (_is_file) {
				lock_t lock(_mutex);

				char_t t = _c;

				if (is()) {
					_input >> std::noskipws >> _c;
				}

				return t;
			}


			if (is()) {

				char_t t = _c;

				_stream >> std::noskipws >> _c;

				return t;
			}

			return '\0';
		}

		text_reader::char_t text_reader::peek() {
			/*
				Return the current value of '_c'.
			*/
			return _c;
		}

		bool_type text_reader::is() {
			/*
				Return true if the file is not eof
				and in good condition.
			*/

			if (_is_file) {
				return (_input.is_open() && _input.good() && !_input.eof());
			}
			return (!_stream.eof() && _stream.good());
		}

		bool_type text_reader::is_file() {
			/*
				Return true if the file is not eof
				and in good condition.
			*/

			return _is_file;
		}

		str_type text_reader::get_line() {

			if (_is_file) {
				lock_t lock(_mutex);

				if (is()) {

					str_type line;

					std::getline(_input, line);

					lrtrim(line);

					if (line.empty()) {
						return "";
					}

					return line;
				}
			}

			return "";
		}

	}  // end parser
} // end Olly