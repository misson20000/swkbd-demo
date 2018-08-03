#pragma once

#include<libtransistor/types.h>
#include<libtransistor/cpp/ipcclient.hpp>

#include "IStorage.hpp"

namespace am {

class ILibraryAppletAccessor {
  public:
   ILibraryAppletAccessor() = default;
   ILibraryAppletAccessor(ipc_object_t object);

   trn::KEvent GetAppletStateChangedEvent();
   bool IsCompleted();
   void Start();
   trn::ResultCode GetResult();
   void PushInData(IStorage &ist);
   IStorage PopOutData();
   void PushExtraStorage(IStorage &ist);
   void PushInteractiveInData(IStorage &ist);
   IStorage PopInteractiveOutData();
   trn::KEvent GetPopOutDataEvent();
   trn::KEvent GetPopInteractiveOutDataEvent();
   trn::ipc::client::Object object;
};

} // namespace am
