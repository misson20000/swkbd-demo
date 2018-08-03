#pragma once

#include<libtransistor/types.h>
#include<libtransistor/cpp/ipcclient.hpp>

namespace am {

class IStorageAccessor;
class ITransferStorageAccessor;

class IStorage {
  public:
   IStorage() = default;
   IStorage(ipc_object_t object);

   IStorageAccessor Open();
   ITransferStorageAccessor OpenTransferStorage();
   trn::ipc::client::Object object;
};

} // namespace am
