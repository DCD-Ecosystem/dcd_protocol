// copyright defined in abidcd/LICENSE.txt

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct abidcd_context_s abidcd_context;
typedef int abidcd_bool;

// Create a context. The context holds all memory allocated by functions in this header. Returns null on failure.
abidcd_context* abidcd_create();

// Destroy a context.
void abidcd_destroy(abidcd_context* context);

// Get last error. Never returns null. The context owns the returned string.
const char* abidcd_get_error(abidcd_context* context);

// Get generated binary. The context owns the returned memory. Functions return null on error; use abidcd_get_error to
// retrieve error.
int abidcd_get_bin_size(abidcd_context* context);
const char* abidcd_get_bin_data(abidcd_context* context);

// Convert generated binary to hex. The context owns the returned string. Returns null on error; use abidcd_get_error to
// retrieve error.
const char* abidcd_get_bin_hex(abidcd_context* context);

// Name conversion. The context owns the returned memory. Functions return null on error; use abidcd_get_error to
// retrieve error.
uint64_t abidcd_string_to_name(abidcd_context* context, const char* str);
const char* abidcd_name_to_string(abidcd_context* context, uint64_t name);

// Set abi (JSON format). Returns false on error.
abidcd_bool abidcd_set_abi(abidcd_context* context, uint64_t contract, const char* abi);

// Set abi (binary format). Returns false on error.
abidcd_bool abidcd_set_abi_bin(abidcd_context* context, uint64_t contract, const char* data, size_t size);

// Set abi (hex format). Returns false on error.
abidcd_bool abidcd_set_abi_hex(abidcd_context* context, uint64_t contract, const char* hex);

// Get the type name for an action. The context owns the returned memory. Returns null on error; use abidcd_get_error
// to retrieve error.
const char* abidcd_get_type_for_action(abidcd_context* context, uint64_t contract, uint64_t action);

// Get the type name for a table. The context owns the returned memory. Returns null on error; use abidcd_get_error
// to retrieve error.
const char* abidcd_get_type_for_table(abidcd_context* context, uint64_t contract, uint64_t table);

// Get the definition for a kv table in json. The context owns the returned memory. Returns null on error; use
// abidcd_get_error to retrieve error.
const char* abidcd_get_kv_table_def(abidcd_context* context, uint64_t contract, uint64_t table);

// Get the type name for an action_result. The context owns the returned memory. Returns null on error; use
// abidcd_get_error to retrieve error.
const char* abidcd_get_type_for_action_result(abidcd_context* context, uint64_t contract, uint64_t action_result);

// Convert json to binary. Use abidcd_get_bin_* to retrieve result. Returns false on error.
abidcd_bool abidcd_json_to_bin(abidcd_context* context, uint64_t contract, const char* type, const char* json);

// Convert json to binary. Allow json field reordering. Use abidcd_get_bin_* to retrieve result. Returns false on error.
abidcd_bool abidcd_json_to_bin_reorderable(abidcd_context* context, uint64_t contract, const char* type,
                                           const char* json);

// Convert binary to json. The context owns the returned string. Returns null on error; use abidcd_get_error to retrieve
// error.
const char* abidcd_bin_to_json(abidcd_context* context, uint64_t contract, const char* type, const char* data,
                               size_t size);

// Convert hex to json. The context owns the returned memory. Returns null on error; use abidcd_get_error to retrieve
// error.
const char* abidcd_hex_to_json(abidcd_context* context, uint64_t contract, const char* type, const char* hex);

// Convert abi json to bin, Use abidcd_get_bin_* to retrieve result. Returns false on error.
abidcd_bool abidcd_abi_json_to_bin(abidcd_context* context, const char* json);

// Convert abi bin to json, The context.result_str has the result, Returns null on error; use abidcd_get_error to
// retrieve
const char* abidcd_abi_bin_to_json(abidcd_context* context, const char* abi_bin_data, const size_t abi_bin_data_size);

#ifdef __cplusplus
}
#endif
