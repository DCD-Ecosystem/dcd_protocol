#pragma once

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct roddcd_error_s         roddcd_error;
typedef struct roddcd_context_s       roddcd_context;
typedef struct roddcd_db_partition_s  roddcd_db_partition;
typedef struct roddcd_db_snapshot_s   roddcd_db_snapshot;
typedef struct roddcd_filter_s        roddcd_filter;
typedef struct roddcd_query_handler_s roddcd_query_handler;
typedef int                           roddcd_bool;

// Create an error object. If multiple threads use an error object, then they must synchronize it. Returns NULL on
// failure.
roddcd_error* roddcd_create_error();

// Destroy an error object. This is a no-op if error == NULL.
void roddcd_destroy_error(roddcd_error* error);

// Get last error in this object. Never returns NULL. The error object owns the returned string.
const char* roddcd_get_error(roddcd_error* error);

// Create a context. Returns NULL on failure.
roddcd_context* roddcd_create();

// Destroy a context. It is undefined behavior if the context is used between threads without synchronization, or if any
// other objects currently exist for this context. This is a no-op if context == NULL.
void roddcd_destroy(roddcd_context* context);

// Open database. num_threads is the target number of rocksdb background threads; use 0 for default. max_open_files is
// the max number of open rocksdb files; 0 to make this unlimited.
//
// It is undefined behavior if the context is used between threads without synchronization. Returns false on error.
roddcd_bool roddcd_open_db(roddcd_error* error, roddcd_context* context, const char* path,
                           roddcd_bool create_if_missing, int num_threads, int max_open_files);

// Create or open a database partition. It is undefined behavior if more than 1 partition is opened for a given prefix,
// if any partitions have overlapping prefixes, or if the context is used between threads without synchronization.
// Returns NULL on failure.
roddcd_db_partition* roddcd_create_partition(roddcd_error* error, roddcd_context* context, const char* prefix,
                                             uint32_t prefix_size);

// Destroy a partition. It is undefined behavior if the partition is used between threads without synchronization. This
// is a no-op if partition == NULL.
void roddcd_destroy_partition(roddcd_db_partition* partition);

// Create a database snapshot. Snapshots isolate changes from each other. All database reads and writes happen through
// snapshots. Snapshot objects may safely outlive partition objects.
//
// A single partition supports any number of simultaneous non-persistent snapshots, but only a single persistent
// snapshot at any time. persistent and non-persistent may coexist. Only persistent snapshots make permanent changes to
// the database.
//
// Each snapshot may be used by a different thread, even if they're created from a single partition.
//
// It is undefined behavior if more than 1 persistent snapshot exists on a partition, or if the partition is used
// between threads without synchronization. Returns NULL on failure.
roddcd_db_snapshot* roddcd_create_snapshot(roddcd_error* error, roddcd_db_partition* partition, roddcd_bool persistent);

// Destroy a snapshot. It is undefined behavior if the snapshot is used between threads without synchronization. This is
// a no-op if snapshot == NULL.
void roddcd_destroy_snapshot(roddcd_db_snapshot* snapshot);

// Refresh a snapshot so it may read recently-written database changes. This operation is invalid if the snapshot is
// persistent. It is undefined behavior if the snapshot is used between threads without synchronization.
roddcd_bool roddcd_refresh_snapshot(roddcd_error* error, roddcd_db_snapshot* snapshot);

// Start writing a block. Aborts any block in progress and rolls back reversible blocks if needed. `data` must be the
// serialized `result` type defined by the state-history plugin's ABI. Currently only supports `get_blocks_result_v0`.
// It is undefined behavior if the snapshot is used between threads without synchronization.
roddcd_bool roddcd_start_block(roddcd_error* error, roddcd_db_snapshot* snapshot, const char* data, uint64_t size);

// Finish writing a block. `data` must be the serialized `result` type defined by the state-history plugin's ABI.
// Currently only supports `get_blocks_result_v0`. If `force_write` is true, then the data will become immediately
// available to newly-created or newly-refreshed snapshots to read. If `force_write` is false, then the write may be
// delayed until a future roddcd_end_block call. It is undefined behavior if the snapshot is used between threads
// without synchronization.
roddcd_bool roddcd_end_block(roddcd_error* error, roddcd_db_snapshot* snapshot, const char* data, uint64_t size,
                             bool force_write);

// Write block info. `data` must be the serialized `result` type defined by the state-history plugin's ABI. Currently
// only supports `get_blocks_result_v0`. If `roddcd_write_block_info` returns false, the snapshot will be in an
// inconsistent state; call `start_block` to abandon the current write and start another. It is undefined behavior if
// the snapshot is used between threads without synchronization.
roddcd_bool roddcd_write_block_info(roddcd_error* error, roddcd_db_snapshot* snapshot, const char* data, uint64_t size);

// Write state-history deltas to a block. `data` must be the serialized `result` type defined by the state-history
// plugin's ABI. Currently only supports `get_blocks_result_v0`. If `shutdown` isn't null, then `roddcd_write_deltas`
// may call it during long operations. If `shutdown` returns true, then `roddcd_write_deltas` abandons the writes. If
// `roddcd_write_deltas` returns false, the snapshot will be in an inconsistent state; call `start_block` to abandon the
// current write and start another. It is undefined behavior if the snapshot is used between threads without
// synchronization.
roddcd_bool roddcd_write_deltas(roddcd_error* error, roddcd_db_snapshot* snapshot, const char* data, uint64_t size,
                                roddcd_bool (*shutdown)(void*), void* shutdown_arg);

// Create a filter. Returns NULL on failure.
roddcd_filter* roddcd_create_filter(roddcd_error* error, uint64_t name, const char* wasm_filename);

// Destroy a filter. It is undefined behavior if the filter is used between threads without synchronization. This is a
// no-op if filter == NULL.
void roddcd_destroy_filter(roddcd_filter* filter);

// Run filter. data must be the serialized `result` type defined by the state-history plugin's ABI. The `push_data`
// callback receives data from the filter's `push_data` intrinsic. If `push_data` is null, then the intrinsic is a
// no-op.
//
// If `roddcd_run_filter` returns false, the snapshot will be in an inconsistent state; call `start_block` to abandon
// the current write and start another. It is undefined behavior if `snapshot` or `filter` is used between threads
// without synchronization.
roddcd_bool roddcd_run_filter(roddcd_error* error, roddcd_db_snapshot* snapshot, roddcd_filter* filter,
                              const char* data, uint64_t size,
                              roddcd_bool (*push_data)(void* arg, const char* data, uint64_t size),
                              void* push_data_arg);

// Create a query handler. This object manages pools of resources for running queries simultaneously.
//
// Query handlers may safely outlive partition objects. It is undefined behavior if the partition is used between
// threads without synchronization. Returns NULL on failure.
//
// TODO: remove partition arg; redundant with snapshot in roddcd_query_transaction
roddcd_query_handler* roddcd_create_query_handler(roddcd_error* error, roddcd_db_partition* partition,
                                                  uint32_t max_console_size, uint32_t wasm_cache_size,
                                                  uint64_t max_exec_time_ms, const char* contract_dir);

// Destroy a query handler. It is undefined behavior if the handler is used between threads without synchronization.
// This is a no-op if handler == NULL.
void roddcd_destroy_query_handler(roddcd_query_handler* handler);

// Run a query. data is a serialized ship_protocol::packed_transaction. Returns false on serious error and sets *result
// to NULL and *result_size to 0. Otherwise, sets *result and *result_size to memory containing a serialized
// ship_protocol::transaction_trace. If the query failed, the error result will be in the transaction trace. Caller must
// use roddcd_free_result to free the memory.
//
// It is safe to use the same handler from multiple threads if:
// * The return from roddcd_create_query_handler happens-before any calls to roddcd_query_transaction
// * The return from all roddcd_query_transaction calls happens-before the call to roddcd_destroy_query_handler
//
// It is undefined behavior if `snapshot` is used between threads without synchronization.
roddcd_bool roddcd_query_transaction(roddcd_error* error, roddcd_query_handler* handler, roddcd_db_snapshot* snapshot,
                                     const char* data, uint64_t size, char** result, uint64_t* result_size);

// Frees memory from roddcd_query_transaction. Does nothing if result == NULL.
void roddcd_free_result(char* result);

#ifdef __cplusplus
}
#endif
