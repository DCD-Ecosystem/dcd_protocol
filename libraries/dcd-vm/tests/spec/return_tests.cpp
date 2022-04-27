// Generated by spec_test_generator.  DO NOT MODIFY THIS FILE.

#include <algorithm>
#include <vector>
#include <iostream>
#include <iterator>
#include <cmath>
#include <cstdlib>
#include <catch2/catch.hpp>
#include <utils.hpp>
#include <wasm_config.hpp>
#include <dcd/vm/backend.hpp>

using namespace dcd;
using namespace dcd::vm;
extern wasm_allocator wa;

BACKEND_TEST_CASE( "Testing wasm <return_0_wasm>", "[return_0_wasm_tests]" ) {
   using backend_t = backend<standalone_function_t, TestType>;
   auto code = read_wasm( std::string(wasm_directory) + "return.0.wasm");
   backend_t bkend( code, &wa );

   CHECK(!bkend.call_with_return("env", "type-i32"));
   CHECK(!bkend.call_with_return("env", "type-i64"));
   CHECK(!bkend.call_with_return("env", "type-f32"));
   CHECK(!bkend.call_with_return("env", "type-f64"));
   CHECK(bkend.call_with_return("env", "type-i32-value")->to_ui32() == UINT32_C(1));
   CHECK(bkend.call_with_return("env", "type-i64-value")->to_ui64() == UINT32_C(2));
   CHECK(bit_cast<uint32_t>(bkend.call_with_return("env", "type-f32-value")->to_f32()) == UINT32_C(1077936128));
   CHECK(bit_cast<uint64_t>(bkend.call_with_return("env", "type-f64-value")->to_f64()) == UINT64_C(4616189618054758400));
   CHECK(!bkend.call_with_return("env", "nullary"));
   CHECK(bit_cast<uint64_t>(bkend.call_with_return("env", "unary")->to_f64()) == UINT64_C(4613937818241073152));
   CHECK(bkend.call_with_return("env", "as-func-first")->to_ui32() == UINT32_C(1));
   CHECK(bkend.call_with_return("env", "as-func-mid")->to_ui32() == UINT32_C(2));
   CHECK(!bkend.call_with_return("env", "as-func-last"));
   CHECK(bkend.call_with_return("env", "as-func-value")->to_ui32() == UINT32_C(3));
   CHECK(!bkend.call_with_return("env", "as-block-first"));
   CHECK(!bkend.call_with_return("env", "as-block-mid"));
   CHECK(!bkend.call_with_return("env", "as-block-last"));
   CHECK(bkend.call_with_return("env", "as-block-value")->to_ui32() == UINT32_C(2));
   CHECK(bkend.call_with_return("env", "as-loop-first")->to_ui32() == UINT32_C(3));
   CHECK(bkend.call_with_return("env", "as-loop-mid")->to_ui32() == UINT32_C(4));
   CHECK(bkend.call_with_return("env", "as-loop-last")->to_ui32() == UINT32_C(5));
   CHECK(bkend.call_with_return("env", "as-br-value")->to_ui32() == UINT32_C(9));
   CHECK(!bkend.call_with_return("env", "as-br_if-cond"));
   CHECK(bkend.call_with_return("env", "as-br_if-value")->to_ui32() == UINT32_C(8));
   CHECK(bkend.call_with_return("env", "as-br_if-value-cond")->to_ui32() == UINT32_C(9));
   CHECK(bkend.call_with_return("env", "as-br_table-index")->to_ui64() == UINT32_C(9));
   CHECK(bkend.call_with_return("env", "as-br_table-value")->to_ui32() == UINT32_C(10));
   CHECK(bkend.call_with_return("env", "as-br_table-value-index")->to_ui32() == UINT32_C(11));
   CHECK(bkend.call_with_return("env", "as-return-value")->to_ui64() == UINT32_C(7));
   CHECK(bkend.call_with_return("env", "as-if-cond")->to_ui32() == UINT32_C(2));
   CHECK(bkend.call_with_return("env", "as-if-then", UINT32_C(1), UINT32_C(6))->to_ui32() == UINT32_C(3));
   CHECK(bkend.call_with_return("env", "as-if-then", UINT32_C(0), UINT32_C(6))->to_ui32() == UINT32_C(6));
   CHECK(bkend.call_with_return("env", "as-if-else", UINT32_C(0), UINT32_C(6))->to_ui32() == UINT32_C(4));
   CHECK(bkend.call_with_return("env", "as-if-else", UINT32_C(1), UINT32_C(6))->to_ui32() == UINT32_C(6));
   CHECK(bkend.call_with_return("env", "as-select-first", UINT32_C(0), UINT32_C(6))->to_ui32() == UINT32_C(5));
   CHECK(bkend.call_with_return("env", "as-select-first", UINT32_C(1), UINT32_C(6))->to_ui32() == UINT32_C(5));
   CHECK(bkend.call_with_return("env", "as-select-second", UINT32_C(0), UINT32_C(6))->to_ui32() == UINT32_C(6));
   CHECK(bkend.call_with_return("env", "as-select-second", UINT32_C(1), UINT32_C(6))->to_ui32() == UINT32_C(6));
   CHECK(bkend.call_with_return("env", "as-select-cond")->to_ui32() == UINT32_C(7));
   CHECK(bkend.call_with_return("env", "as-call-first")->to_ui32() == UINT32_C(12));
   CHECK(bkend.call_with_return("env", "as-call-mid")->to_ui32() == UINT32_C(13));
   CHECK(bkend.call_with_return("env", "as-call-last")->to_ui32() == UINT32_C(14));
   CHECK(bkend.call_with_return("env", "as-call_indirect-func")->to_ui32() == UINT32_C(20));
   CHECK(bkend.call_with_return("env", "as-call_indirect-first")->to_ui32() == UINT32_C(21));
   CHECK(bkend.call_with_return("env", "as-call_indirect-mid")->to_ui32() == UINT32_C(22));
   CHECK(bkend.call_with_return("env", "as-call_indirect-last")->to_ui32() == UINT32_C(23));
   CHECK(bkend.call_with_return("env", "as-local.set-value")->to_ui32() == UINT32_C(17));
   CHECK(bkend.call_with_return("env", "as-local.tee-value")->to_ui32() == UINT32_C(1));
   CHECK(bkend.call_with_return("env", "as-global.set-value")->to_ui32() == UINT32_C(1));
   CHECK(bit_cast<uint32_t>(bkend.call_with_return("env", "as-load-address")->to_f32()) == UINT32_C(1071225242));
   CHECK(bkend.call_with_return("env", "as-loadN-address")->to_ui64() == UINT32_C(30));
   CHECK(bkend.call_with_return("env", "as-store-address")->to_ui32() == UINT32_C(30));
   CHECK(bkend.call_with_return("env", "as-store-value")->to_ui32() == UINT32_C(31));
   CHECK(bkend.call_with_return("env", "as-storeN-address")->to_ui32() == UINT32_C(32));
   CHECK(bkend.call_with_return("env", "as-storeN-value")->to_ui32() == UINT32_C(33));
   CHECK(bit_cast<uint32_t>(bkend.call_with_return("env", "as-unary-operand")->to_f32()) == UINT32_C(1079613850));
   CHECK(bkend.call_with_return("env", "as-binary-left")->to_ui32() == UINT32_C(3));
   CHECK(bkend.call_with_return("env", "as-binary-right")->to_ui64() == UINT32_C(45));
   CHECK(bkend.call_with_return("env", "as-test-operand")->to_ui32() == UINT32_C(44));
   CHECK(bkend.call_with_return("env", "as-compare-left")->to_ui32() == UINT32_C(43));
   CHECK(bkend.call_with_return("env", "as-compare-right")->to_ui32() == UINT32_C(42));
   CHECK(bkend.call_with_return("env", "as-convert-operand")->to_ui32() == UINT32_C(41));
   CHECK(bkend.call_with_return("env", "as-memory.grow-size")->to_ui32() == UINT32_C(40));
}

BACKEND_TEST_CASE( "Testing wasm <return_1_wasm>", "[return_1_wasm_tests]" ) {
   using backend_t = backend<standalone_function_t, TestType>;
   auto code = read_wasm( std::string(wasm_directory) + "return.1.wasm");
   CHECK_THROWS_AS(backend_t(code, nullptr), std::exception);
}

BACKEND_TEST_CASE( "Testing wasm <return_10_wasm>", "[return_10_wasm_tests]" ) {
   using backend_t = backend<standalone_function_t, TestType>;
   auto code = read_wasm( std::string(wasm_directory) + "return.10.wasm");
   CHECK_THROWS_AS(backend_t(code, nullptr), std::exception);
}

BACKEND_TEST_CASE( "Testing wasm <return_11_wasm>", "[return_11_wasm_tests]" ) {
   using backend_t = backend<standalone_function_t, TestType>;
   auto code = read_wasm( std::string(wasm_directory) + "return.11.wasm");
   CHECK_THROWS_AS(backend_t(code, nullptr), std::exception);
}

BACKEND_TEST_CASE( "Testing wasm <return_12_wasm>", "[return_12_wasm_tests]" ) {
   using backend_t = backend<standalone_function_t, TestType>;
   auto code = read_wasm( std::string(wasm_directory) + "return.12.wasm");
   CHECK_THROWS_AS(backend_t(code, nullptr), std::exception);
}

BACKEND_TEST_CASE( "Testing wasm <return_13_wasm>", "[return_13_wasm_tests]" ) {
   using backend_t = backend<standalone_function_t, TestType>;
   auto code = read_wasm( std::string(wasm_directory) + "return.13.wasm");
   CHECK_THROWS_AS(backend_t(code, nullptr), std::exception);
}

BACKEND_TEST_CASE( "Testing wasm <return_14_wasm>", "[return_14_wasm_tests]" ) {
   using backend_t = backend<standalone_function_t, TestType>;
   auto code = read_wasm( std::string(wasm_directory) + "return.14.wasm");
   CHECK_THROWS_AS(backend_t(code, nullptr), std::exception);
}

BACKEND_TEST_CASE( "Testing wasm <return_15_wasm>", "[return_15_wasm_tests]" ) {
   using backend_t = backend<standalone_function_t, TestType>;
   auto code = read_wasm( std::string(wasm_directory) + "return.15.wasm");
   CHECK_THROWS_AS(backend_t(code, nullptr), std::exception);
}

BACKEND_TEST_CASE( "Testing wasm <return_16_wasm>", "[return_16_wasm_tests]" ) {
   using backend_t = backend<standalone_function_t, TestType>;
   auto code = read_wasm( std::string(wasm_directory) + "return.16.wasm");
   CHECK_THROWS_AS(backend_t(code, nullptr), std::exception);
}

BACKEND_TEST_CASE( "Testing wasm <return_17_wasm>", "[return_17_wasm_tests]" ) {
   using backend_t = backend<standalone_function_t, TestType>;
   auto code = read_wasm( std::string(wasm_directory) + "return.17.wasm");
   CHECK_THROWS_AS(backend_t(code, nullptr), std::exception);
}

BACKEND_TEST_CASE( "Testing wasm <return_18_wasm>", "[return_18_wasm_tests]" ) {
   using backend_t = backend<standalone_function_t, TestType>;
   auto code = read_wasm( std::string(wasm_directory) + "return.18.wasm");
   CHECK_THROWS_AS(backend_t(code, nullptr), std::exception);
}

BACKEND_TEST_CASE( "Testing wasm <return_19_wasm>", "[return_19_wasm_tests]" ) {
   using backend_t = backend<standalone_function_t, TestType>;
   auto code = read_wasm( std::string(wasm_directory) + "return.19.wasm");
   CHECK_THROWS_AS(backend_t(code, nullptr), std::exception);
}

BACKEND_TEST_CASE( "Testing wasm <return_2_wasm>", "[return_2_wasm_tests]" ) {
   using backend_t = backend<standalone_function_t, TestType>;
   auto code = read_wasm( std::string(wasm_directory) + "return.2.wasm");
   CHECK_THROWS_AS(backend_t(code, nullptr), std::exception);
}

BACKEND_TEST_CASE( "Testing wasm <return_20_wasm>", "[return_20_wasm_tests]" ) {
   using backend_t = backend<standalone_function_t, TestType>;
   auto code = read_wasm( std::string(wasm_directory) + "return.20.wasm");
   CHECK_THROWS_AS(backend_t(code, nullptr), std::exception);
}

BACKEND_TEST_CASE( "Testing wasm <return_3_wasm>", "[return_3_wasm_tests]" ) {
   using backend_t = backend<standalone_function_t, TestType>;
   auto code = read_wasm( std::string(wasm_directory) + "return.3.wasm");
   CHECK_THROWS_AS(backend_t(code, nullptr), std::exception);
}

BACKEND_TEST_CASE( "Testing wasm <return_4_wasm>", "[return_4_wasm_tests]" ) {
   using backend_t = backend<standalone_function_t, TestType>;
   auto code = read_wasm( std::string(wasm_directory) + "return.4.wasm");
   CHECK_THROWS_AS(backend_t(code, nullptr), std::exception);
}

BACKEND_TEST_CASE( "Testing wasm <return_5_wasm>", "[return_5_wasm_tests]" ) {
   using backend_t = backend<standalone_function_t, TestType>;
   auto code = read_wasm( std::string(wasm_directory) + "return.5.wasm");
   CHECK_THROWS_AS(backend_t(code, nullptr), std::exception);
}

BACKEND_TEST_CASE( "Testing wasm <return_6_wasm>", "[return_6_wasm_tests]" ) {
   using backend_t = backend<standalone_function_t, TestType>;
   auto code = read_wasm( std::string(wasm_directory) + "return.6.wasm");
   CHECK_THROWS_AS(backend_t(code, nullptr), std::exception);
}

BACKEND_TEST_CASE( "Testing wasm <return_7_wasm>", "[return_7_wasm_tests]" ) {
   using backend_t = backend<standalone_function_t, TestType>;
   auto code = read_wasm( std::string(wasm_directory) + "return.7.wasm");
   CHECK_THROWS_AS(backend_t(code, nullptr), std::exception);
}

BACKEND_TEST_CASE( "Testing wasm <return_8_wasm>", "[return_8_wasm_tests]" ) {
   using backend_t = backend<standalone_function_t, TestType>;
   auto code = read_wasm( std::string(wasm_directory) + "return.8.wasm");
   CHECK_THROWS_AS(backend_t(code, nullptr), std::exception);
}

BACKEND_TEST_CASE( "Testing wasm <return_9_wasm>", "[return_9_wasm_tests]" ) {
   using backend_t = backend<standalone_function_t, TestType>;
   auto code = read_wasm( std::string(wasm_directory) + "return.9.wasm");
   CHECK_THROWS_AS(backend_t(code, nullptr), std::exception);
}
