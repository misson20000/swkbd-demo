#pragma once

#include<libtransistor/types.h>
#include<libtransistor/cpp/ipcclient.hpp>

namespace am {

class ITransferStorageAccessor {
  public:
   ITransferStorageAccessor() = default;
   ITransferStorageAccessor(ipc_object_t object);

   size_t GetSize();
   uint64_t GetHandle(handle_t &out);
   trn::ipc::client::Object object;
};

} // namespace am
