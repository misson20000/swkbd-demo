#pragma once

#include<libtransistor/types.h>
#include<libtransistor/cpp/ipcclient.hpp>

namespace am {

class IStorageAccessor {
  public:
   IStorageAccessor() = default;
   IStorageAccessor(ipc_object_t object);

   size_t GetSize();
   void Write(size_t offset, const uint8_t *data, size_t size);
   void Read(size_t offset, uint8_t *data, size_t size);
   trn::ipc::client::Object object;
};

} // namespace am
