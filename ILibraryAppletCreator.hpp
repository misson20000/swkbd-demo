#pragma once

#include<libtransistor/types.h>
#include<libtransistor/cpp/ipcclient.hpp>

namespace am {

class ILibraryAppletAccessor;
class IStorage;

class ILibraryAppletCreator {
  public:
   ILibraryAppletCreator() = default;
   ILibraryAppletCreator(ipc_object_t object);

   ILibraryAppletAccessor CreateLibraryApplet(uint32_t a, uint32_t b);
   void TerminateAllLibraryApplets();
   bool AreAnyLibraryAppletsLeft();
   IStorage CreateStorage(size_t size);
   IStorage CreateTransferMemoryStorage(bool unknown, uint64_t size, handle_t tmem);
   IStorage CreateTransferMemoryStorage(bool unknown, uint64_t size, trn::KTransferMemory &tmem);
   IStorage CreateHandleStorage(uint64_t unknown, handle_t handle);
   trn::ipc::client::Object object;
};

} // namespace am
