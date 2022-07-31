##################################################
#
#   Define project main function configuration.
#
##################################################
add_executable (Oliver "main_function.cpp" "main_function.h")

##################################################
#
#     Add tests and install targets if needed.
#
##################################################
enable_testing()

add_test(NAME Runs COMMAND Oliver 25)                   # Does the program run?

add_test(NAME HelpDef COMMAND Oliver)                   # Does the help message work?
set_tests_properties(HelpDef
    PROPERTIES PASS_REGULAR_EXPRESSION "Usage:.*number"
)

function(do_test target arg result)                     # Define a helper function to
  add_test(NAME Comp${arg} COMMAND ${target} ${arg})    # test input text strings to an
  set_tests_properties(Comp${arg}                       # output result string.
    PROPERTIES PASS_REGULAR_EXPRESSION ${result}
    )
endfunction()

##################################################
#
#        Define 'do_test' invocations.
#
##################################################
do_test(Oliver '4' "4")