#include "IStorage.hpp"

#include "IStorageAccessor.hpp"

using namespace trn;

namespace am {

IStorage::IStorage(ipc_object_t object) : object(object) {
   
}

IStorageAccessor IStorage::Open() {
   IStorageAccessor isa;
   ResultCode::AssertOk(
      object.SendSyncRequest<0>(
         ipc::OutObject(isa)));
   return isa;
}

} // namespace am
