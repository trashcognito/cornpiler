add_test(
    NAME arr_test
    COMMAND cornpiler arr_test.crn -o ${CMAKE_CURRENT_BINARY_DIR}/arr_test.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
    NAME bottlesofbeer99
    COMMAND cornpiler bottlesofbeer99.crn -o ${CMAKE_CURRENT_BINARY_DIR}/bottlesofbeer99.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
    NAME else_if_test
    COMMAND cornpiler else_if_test.crn -o ${CMAKE_CURRENT_BINARY_DIR}/else_if_test.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
    NAME else_test
    COMMAND cornpiler else_test.crn -o ${CMAKE_CURRENT_BINARY_DIR}/else_test.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
    NAME err_test
    COMMAND cornpiler err_test.crn -o ${CMAKE_CURRENT_BINARY_DIR}/err_test.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
    NAME extern_test
    COMMAND cornpiler extern_test.crn -o ${CMAKE_CURRENT_BINARY_DIR}/extern_test.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
    NAME factorial
    COMMAND cornpiler factorial.crn -o ${CMAKE_CURRENT_BINARY_DIR}/factorial.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
    NAME fib
    COMMAND cornpiler fib.crn -o ${CMAKE_CURRENT_BINARY_DIR}/fib.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
    NAME global_test
    COMMAND cornpiler global_test.crn -o ${CMAKE_CURRENT_BINARY_DIR}/global_test.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
    NAME guessing_game
    COMMAND cornpiler guessing_game.crn -o ${CMAKE_CURRENT_BINARY_DIR}/guessing_game.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
    NAME helloworld
    COMMAND cornpiler helloworld.crn -o ${CMAKE_CURRENT_BINARY_DIR}/helloworld.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
    NAME in_number
    COMMAND cornpiler in_number.crn -o ${CMAKE_CURRENT_BINARY_DIR}/in_number.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
    NAME loop_test
    COMMAND cornpiler loop_test.crn -o ${CMAKE_CURRENT_BINARY_DIR}/loop_test.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
    NAME negtest
    COMMAND cornpiler negtest.crn -o ${CMAKE_CURRENT_BINARY_DIR}/negtest.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
    NAME primes
    COMMAND cornpiler primes.crn -o ${CMAKE_CURRENT_BINARY_DIR}/primes.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
    NAME ptr_ptr_test
    COMMAND cornpiler ptr_ptr_test.crn -o ${CMAKE_CURRENT_BINARY_DIR}/ptr_ptr_test.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
    NAME ptr_test
    COMMAND cornpiler ptr_test.crn -o ${CMAKE_CURRENT_BINARY_DIR}/ptr_test.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
    NAME return_test
    COMMAND cornpiler return_test.crn -o ${CMAKE_CURRENT_BINARY_DIR}/return_test.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
    NAME vartest
    COMMAND cornpiler vartest.crn -o ${CMAKE_CURRENT_BINARY_DIR}/vartest.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)