add_test(
    NAME arr_test
    COMMAND cornpiler ${SOURCE_DIR}/tests/arr_test.crn -o arr_test.o
)
add_test(
    NAME bottlesofbeer99
    COMMAND cornpiler ${SOURCE_DIR}/tests/bottlesofbeer99.crn -o bottlesofbeer99.o
)
add_test(
    NAME else_if_test
    COMMAND cornpiler ${SOURCE_DIR}/tests/else_if_test.crn -o else_if_test.o
)
add_test(
    NAME else_test
    COMMAND cornpiler ${SOURCE_DIR}/tests/else_test.crn -o else_test.o
)
add_test(
    NAME err_test
    COMMAND cornpiler ${SOURCE_DIR}/tests/err_test.crn -o err_test.o
)
add_test(
    NAME extern_test
    COMMAND cornpiler ${SOURCE_DIR}/tests/extern_test.crn -o extern_test.o
)
add_test(
    NAME factorial
    COMMAND cornpiler ${SOURCE_DIR}/tests/factorial.crn -o factorial.o
)
add_test(
    NAME fib
    COMMAND cornpiler ${SOURCE_DIR}/tests/fib.crn -o fib.o
)
add_test(
    NAME global_test
    COMMAND cornpiler ${SOURCE_DIR}/tests/global_test.crn -o global_test.o
)
add_test(
    NAME guessing_game
    COMMAND cornpiler ${SOURCE_DIR}/tests/guessing_game.crn -o guessing_game.o
)
add_test(
    NAME helloworld
    COMMAND cornpiler ${SOURCE_DIR}/tests/helloworld.crn -o helloworld.o
)
add_test(
    NAME in_number
    COMMAND cornpiler ${SOURCE_DIR}/tests/in_number.crn -o in_number.o
)
add_test(
    NAME loop_test
    COMMAND cornpiler ${SOURCE_DIR}/tests/loop_test.crn -o loop_test.o
)
add_test(
    NAME negtest
    COMMAND cornpiler ${SOURCE_DIR}/tests/negtest.crn -o negtest.o
)
add_test(
    NAME primes
    COMMAND cornpiler ${SOURCE_DIR}/tests/primes.crn -o primes.o
)
add_test(
    NAME ptr_ptr_test
    COMMAND cornpiler ${SOURCE_DIR}/tests/ptr_ptr_test.crn -o ptr_ptr_test.o
)
add_test(
    NAME ptr_test
    COMMAND cornpiler ${SOURCE_DIR}/tests/ptr_test.crn -o ptr_test.o
)
add_test(
    NAME return_test
    COMMAND cornpiler ${SOURCE_DIR}/tests/return_test.crn -o return_test.o
)
add_test(
    NAME vartest
    COMMAND cornpiler ${SOURCE_DIR}/tests/vartest.crn -o vartest.o
)