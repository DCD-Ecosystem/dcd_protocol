#pragma once

#include <dcd/chain/webassembly/eos-vm-oc/config.hpp>

#include <boost/asio/local/datagram_protocol.hpp>
#include <dcd/chain/webassembly/eos-vm-oc/ipc_helpers.hpp>

namespace dcd { namespace chain { namespace eosvmoc {

wrapped_fd get_connection_to_compile_monitor(int cache_fd);

}}}