#include "ILibraryAppletCreator.hpp"

#include "ILibraryAppletAccessor.hpp"

using namespace trn;

namespace am {

ILibraryAppletCreator::ILibraryAppletCreator(ipc_object_t object) : object(object) {
   
}

ILibraryAppletAccessor ILibraryAppletCreator::CreateLibraryApplet(uint32_t a, uint32_t b) {
   ILibraryAppletAccessor ilaa;
   ResultCode::AssertOk(
      object.SendSyncRequest<0>(
         ipc::InRaw<uint32_t>(a),
         ipc::InRaw<uint32_t>(b),
         ipc::OutObject(ilaa)));
   return ilaa;
}

IStorage ILibraryAppletCreator::CreateStorage(size_t size) {
   IStorage is;
   ResultCode::AssertOk(
      object.SendSyncRequest<10>(
         ipc::InRaw<uint64_t>(size),
         ipc::OutObject(is)));
   return is;
}

IStorage ILibraryAppletCreator::CreateTransferMemoryStorage(bool unknown, uint64_t size, handle_t tmem) {
   IStorage is;
   ResultCode::AssertOk(
      object.SendSyncRequest<11>(
         ipc::InRaw<bool>(unknown),
         ipc::InRaw<uint64_t>(size),
         ipc::InHandle<handle_t, ipc::copy>(tmem),
         ipc::OutObject(is)));
   return is;
}

IStorage ILibraryAppletCreator::CreateTransferMemoryStorage(bool unknown, uint64_t size, KTransferMemory &tmem) {
   return CreateTransferMemoryStorage(unknown, size, tmem.handle);
}

} // namespace am
