#ifndef TEXT_PARSER_H
#define TEXT_PARSER_H

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

#include <iostream>
#include <string>
#include <vector>
#include "text_reader.h"

namespace Olly {

        /********************************************************************************************/
        //
        //                                    'text_parser' class definition
        //
        //        The text_parser class accepts individual characters from an instance of
        //        the text_reader.  The text_parser then appends each character together
        //        until a specific combination of characters or an individual character
        //        is recognized as invoking special behavior.  Else all words are
        //        defined by being split at whitespace.  
        //
        /********************************************************************************************/

        // The regex below is currently not in use.  Will see later integration.  
        // static const regex_t  REAL_REGEX("((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?((e|E)((\\+|-)?)[[:digit:]]+)?", std::regex_constants::ECMAScript | std::regex_constants::optimize);

        class text_parser {

            text_reader	                _input;    // The lexical code to parse.
            std::vector<std::string>	_text;	   // The parsed code read from the input.
            bool			            _skip;     // Identifies if a token exists within the bounds of a comment block.
            char                        _c;        // The current character being handled.

        public:

            text_parser(std::string input);
            virtual ~text_parser();

            std::vector<std::string> parse();

        private:

            text_parser() = delete;
            text_parser(const text_parser& obj) = delete;

            int is_regex_escape_char(const char& c);
            int is_string_escape_char(const char& c);

            void process_word(std::string& word);

            void handle_leading_whitespace();
            void handle_comment_operator(std::string& word);
            void handle_unary_negation_operator(std::string& word);
            void handle_unary_addition_operator(std::string& word);
            void handle_pass_through_operator(std::string& word);
            void handle_numeric_identifier(std::string& word);
            void handle_string_identifier(std::string& word);
            void handle_regex_identifier(std::string& word);
            void handle_io_format_identifier(std::string& word);
            void handle_paren_expression_identifier(std::string& word);
            void handle_colon_expression_identifier(std::string& word);
            void handle_list_identifier(std::string& word);
            void handle_map_identifier(std::string& word);

            std::string read_string();
            std::string read_number();
            std::string read_format();
            std::string read_regex();
            std::string list_op(const char& c);
            std::string map_op(const char& c);
            std::string expression_op(const char& c);

            void skip_comment_line();

            bool whitespace_char(char c);
            
            std::string pop_text();

            static const std::vector<std::string> ENCLOSURE_WORDS;
        };

        /********************************************************************************************/
        //
        //   text_parser implimentation.
        //
        /********************************************************************************************/

        const std::vector<std::string> text_parser::ENCLOSURE_WORDS = {
            "(", ")", "'", "'", "\"", "\"", "[", "]", "{", "}", "`", "`"
    };

    text_parser::text_parser(std::string input) : _input(input), _text(), _skip(false), _c('\0') {
    }

    text_parser::~text_parser() {
    }

    std::vector<std::string> text_parser::parse() {
        /*
            Ensure that the file has content to parse.
        */

        if (!_input.is()) {
            return _text;
        }

        handle_leading_whitespace();

        std::string  word = "(";

        process_word(word);

        while (_input.is()) {
            /*
                Grab each character and split each group
                of characters on whitespace or on a few
                special characters which have a specific
                semantic meaning.
            */

            _c = _input.next();

            if (!_skip) {

                if (whitespace_char(_c) || _c == ',') {

                    process_word(word);
                }

                else if (_c == '#') {
                    handle_comment_operator(word);
                }

                else if (word == "" && _c == '-') {
                    handle_unary_negation_operator(word);
                }

                else if (word == "" && _c == '+') {
                    handle_unary_addition_operator(word);
                }

                else if (_c == '@') {
                    handle_pass_through_operator(word);
                }

                else if (_c == '\'') {
                    handle_numeric_identifier(word);
                }

                else if (_c == '"') {
                    handle_string_identifier(word);
                }

                else if (_c == '\\') {
                    handle_regex_identifier(word);
                }

                else if (_c == '`') {
                    handle_io_format_identifier(word);
                }

                else if (_c == '(' || _c == ')') {
                    handle_paren_expression_identifier(word);
                }

                else if (_c == ':' || _c == ';') {
                    handle_colon_expression_identifier(word);
                }

                else if (_c == '[' || _c == ']') {
                    handle_list_identifier(word);
                }

                else if (_c == '{' || _c == '}') {
                    handle_map_identifier(word);
                }

                else {
                    word += _c;
                }
            }
            else if (_c == '#') {
                handle_comment_operator(word);
            }
        }

        process_word(word);

        word = ")";

        process_word(word);

        return _text;
    }

    int text_parser::is_regex_escape_char(const char& c) {
        /*
            Determine if an encountered character
            could be a part of an escaped char
            like '\t' or '\"' for examples.
        */

        for (char o : "\\") {  // -[]/{}()*+?.\\^$|
            // TODO - flesh this out.  

            if (c == o) {
                return true;
            }
        }

        return false;
    }

    int text_parser::is_string_escape_char(const char& c) {
        /*
            Determine if an encountered character
            could be a part of an escaped char
            like '\t' or '\"' for examples.
        */

        for (char o : "\'\"\\abfnrtv") {

            if (c == o) {
                return true;
            }
        }

        return false;
    }

    void text_parser::process_word(std::string& word) {
        /*
            Check that we are not within a comment block.
            Else ensure we have a word to handle and place
            it on the back of the text queue.
        */

        if (word != "") {
            _text.push_back(word);
            word.clear();
        }
    }

    void text_parser::handle_leading_whitespace() {

        while (_input.is()) {
            /*
                Loop through any whitespace at the start
                of the file.
            */

            if (!whitespace_char(_input.peek())) {
                break;
            }


            _input.next();
        }
    }

    void text_parser::handle_comment_operator(std::string& word) {
        /*
                    A comment has probably been encountered.
                    '#'  comment to the end of the line.
                    '##' comments everything up to a second '##'.
                    '#!' Reserved to implement a preprocessor type system?
                */

        process_word(word);

        if (_input.peek() == '#') {
            /*
                Comment blocks are handled by setting a
                boolean value to identify when a comment
                is found.  Else a function is called to
                handle the comment up to the next line.
            */

            if (_skip) {

                _skip = false;
            }
            else {
                _skip = true;
            }

            while (_input.peek() == '#' && _input.is()) {

                _input.next();
            }
        }

        else {
            skip_comment_line();
        }
    }

    void text_parser::handle_unary_negation_operator(std::string& word) {

        if (_input.peek() == '=') {
            word = "-=";
            process_word(word);
            _c = _input.next();
            _c = ' ';
        }

        else if (_input.peek() == '-') {

            auto c = _input.next();

            if (_input.peek() == '>' || _input.peek() == '<') {

                word = "";
                word.push_back(_c);
                word.push_back(c);
                word.push_back(_input.next());

                process_word(word);
                _c = ' ';
            }
            else {
                word = "neg";
                process_word(word);

                word = "neg";
                process_word(word);
            }
        }

        else if (_input.peek() != ' ') {
            word = "neg";
            process_word(word);
        }

        else {
            word = "-";
            process_word(word);
        }
    }

    void text_parser::handle_unary_addition_operator(std::string& word) {

        if (_input.peek() != ' ') {
            word = "pos";
            process_word(word);
        }
        else {
            word = "+";
            process_word(word);
        }
    }

    void text_parser::handle_pass_through_operator(std::string& word) {
        /*
            We encountered a pass operator.

        */
        process_word(word);

        word = "@";
        process_word(word);
    }

    void text_parser::handle_numeric_identifier(std::string& word) {
        /*
            A number was encountered.
        */

        process_word(word);

        _text.push_back("\'");
        _text.push_back(read_number());
        _text.push_back("\'");
    }

    void text_parser::handle_string_identifier(std::string& word) {
        /*
            A string identification was encountered.
            Process the string, and add it to _text
            so long as we are not in a comment block.
        */

        process_word(word);

        _text.push_back("\"");
        _text.push_back(read_string());
        _text.push_back("\"");
    }

    void text_parser::handle_regex_identifier(std::string& word) {
        /*
            A regex identification was encountered.
            Process the regex, and add it to _text
            so long as we are not in a comment block.
        */

        process_word(word);

        _text.push_back("\\");
        _text.push_back(read_regex());
        _text.push_back("\\");
    }

    void text_parser::handle_io_format_identifier(std::string& word) {
        /*
            A format identification was encountered.
            Process the format, and add it to _text
            so long as we are not in a comment block.
        */

        process_word(word);

        _text.push_back("`");
        _text.push_back(read_format());
        _text.push_back("`");
    }

    void text_parser::handle_paren_expression_identifier(std::string& word) {
        /*
            An expression operator was encountered.
            Process the expression, and add it to _text
            so long as we are not in a comment block.
        */

        process_word(word);

        _text.push_back(expression_op(_c));
    }

    void text_parser::handle_colon_expression_identifier(std::string& word) {
        /*
            An expression operator was encountered.
            Process the expression, and add it to _text
            so long as we are not in a comment block.
        */

        process_word(word);

        if (_input.peek() == ':' && _c == ':') {
            _c = _input.next();
            _text.push_back("::");
            _c = ' ';
        }
        else {
            _text.push_back(expression_op(_c));
            _c = ' ';
        }
    }

    void text_parser::handle_list_identifier(std::string& word) {
        /*
            A list operator was encountered.
            Process the list, and add it to _text
            so long as we are not in a comment block
        */

        process_word(word);

        _text.push_back(list_op(_c));

        _c = ' ';
    }

    void text_parser::handle_map_identifier(std::string& word) {
        /*
            An map operator was encountered.
            Process the map, and add it to _text
            so long as we are not in a comment block
        */

        process_word(word);

        if (_c == '{') {
            word = "{";
        }

        else {
            word = "}";
        }

        process_word(word);
        _c = ' ';
    }

    std::string text_parser::read_format() {
        /*
            Read each character including
            whitespace and form a word to
            be used to define a format
            map.
        */

        std::string str;

        char c;

        while (_input.is()) {

            c = _input.next();

            if (c == '`') {
                break;
            }

            else {
                str += c;
            }
        }

        if (str.empty()) {
            return "";
        }

        return str;
    }

    std::string text_parser::read_string() {
        /*
            Read each character including
            whitespace and form a word to
            be used to define a string of
            characters.

            When a '\' character is found
            check to see if it is used to
            define an escaped character.
            If it is append the escaped
            character to the string.
        */

        bool escaped = false;

        std::string str;

        char c;

        while (_input.is()) {

            c = _input.next();

            if (escaped) {

                switch (c) {

                case '\\':
                    str += '\\';
                    break;

                case 'a':
                    str += '\a';
                    break;

                case 'b':
                    str += '\b';
                    break;

                case 'f':
                    str += '\f';
                    break;

                case 'n':
                    str += '\n';
                    break;

                case 'r':
                    str += '\r';
                    break;

                case 't':
                    str += '\t';
                    break;

                case 'v':
                    str += '\v';
                    break;

                default:
                    str += c;
                    break;
                }

                escaped = false;
            }
            else if ((c == '\\') && (is_string_escape_char(_input.peek()))) {

                escaped = true;
            }
            else if (c == '"') {
                break;
            }
            else {
                str += c;
            }
        }

        return str;
    }

    std::string text_parser::read_number() {
        /*
            Read each character including
            whitespace and form a word to
            be used to define a string of
            characters.

            When a '\' character is found
            check to see if it is used to
            define an escaped character.
            If it is append the escaped
            character to the string.
        */

        std::string str;

        char c;

        while (_input.is()) {

            c = _input.next();

            if (c == '\'') {
                break;
            }
            else {
                str += c;
            }
        }

        return str;
    }

    std::string text_parser::read_regex() {
        /*
            Read each character including
            whitespace and form a word to
            be used to define a regex of
            characters.

            When a '\' character is found
            check to see if it is used to
            define an escaped character.
            If it is append the escaped
            character to the regex.
        */

        bool escaped = false;

        std::string str;

        char c;

        while (_input.is()) {

            c = _input.next();

            if (escaped) {

                if (c == '\\') {
                    str += '\\';
                }
                else {
                    str += c;
                }

                escaped = false;
            }
            else if ((c == '\\') && (is_string_escape_char(_input.peek()))) {

                escaped = true;
            }
            else if (c == '\\' && !escaped) {
                break;
            }
            else {
                str += c;
            }
        }

        return str;
    }

    std::string text_parser::list_op(const char& c) {
        /*
            Validate which list operator is provided.
        */
        if (c == '[') {

            return "[";
        }

        return "]";
    }

    std::string text_parser::map_op(const char& c) {
        /*
            Validate which map operator is provided.
        */
        if (c == '{') {

            return "{";
        }

        return "}";
    }

    std::string text_parser::expression_op(const char& c) {
        /*
            Validate which expression operator is provided.
        */

        if (c == '(' || c == ':') {

            return "(";
        }

        return ")";
    }

    void text_parser::skip_comment_line() {
        /*
            Skip all other characters until a newline is encountered.
        */
        char c;

        while (_input.is()) {

            c = _input.next();

            if (c == '\n') {
                break;
            }
        }
    }

    bool text_parser::whitespace_char(char c) {

        if (c < 32) {
            return true;
        }

        if (::isspace(c)) {
            return true;
        }

        return false;
    }

    inline std::string text_parser::pop_text() {

        if (!_text.empty()) {

            std::string word = _text.back();
            _text.pop_back();

            return word;
        }

        return "";
    }
    
}
#endif // TEXT_PARSER_H