#pragma once

#include <dcd/chain/webassembly/dcd-vm-oc/config.hpp>

#include <boost/asio/local/datagram_protocol.hpp>
#include <dcd/chain/webassembly/dcd-vm-oc/ipc_helpers.hpp>

namespace dcd { namespace chain { namespace dcdvmoc {

wrapped_fd get_connection_to_compile_monitor(int cache_fd);

}}}