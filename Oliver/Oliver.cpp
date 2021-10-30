// Olly.cpp : Defines the entry point for the application.
//

#include "Olly.h"

int main(Olly::int_type argc, char** argv) {

	try {
		if (argc == 1) {
			return 0;
		}

		if (argc == 2) {

			Olly::let code;

			{
				Olly::tokens_type code_tokens;

				{
					Olly::parser::parser lex(argv[1]);
					code_tokens = lex.parse();
				}

				{
					Olly::compiler comp(code_tokens);
					code = comp.compile();
				}
			}

			Olly::print("input   = " + str(code));

			{
				Olly::eval::evaluator olly;
				code = olly.eval(code);
			}

			Olly::print("output  = " + str(code));
		}
	}
	catch (std::exception& e) {
		std::cerr << "Error during runtime: " << e.what() << std::endl;
	}
	return 0;
}
