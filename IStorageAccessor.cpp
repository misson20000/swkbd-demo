#include "IStorageAccessor.hpp"

using namespace trn;

namespace am {

IStorageAccessor::IStorageAccessor(ipc_object_t object) : object(object) {
   
}

size_t IStorageAccessor::GetSize() {
   size_t size;
   ResultCode::AssertOk(
      object.SendSyncRequest<0>(
         ipc::OutRaw<uint64_t>(size)));
   return size;
}

void IStorageAccessor::Write(size_t offset, const uint8_t *data, size_t size) {
   ResultCode::AssertOk(
      object.SendSyncRequest<10>(
         ipc::InRaw<uint64_t>(offset),
         ipc::Buffer<uint8_t, 0x21>((uint8_t*) data, size)));
}

void IStorageAccessor::Read(size_t offset, uint8_t *data, size_t size) {
   ResultCode::AssertOk(
      object.SendSyncRequest<11>(
         ipc::InRaw<uint64_t>(offset),
         ipc::Buffer<uint8_t, 0x22>(data, size)));
}

} // namespace am
