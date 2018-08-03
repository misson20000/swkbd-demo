#include "ILibraryAppletAccessor.hpp"

using namespace trn;

namespace am {

ILibraryAppletAccessor::ILibraryAppletAccessor(ipc_object_t object) : object(object) {
   
}

trn::KEvent ILibraryAppletAccessor::GetAppletStateChangedEvent() {
   trn::KEvent evt;
   ResultCode::AssertOk(
      object.SendSyncRequest<0>(
         ipc::OutHandle<KEvent, ipc::copy>(evt)));
   return std::move(evt);
}

bool ILibraryAppletAccessor::IsCompleted() {
   bool is_completed;
   ResultCode::AssertOk(
      object.SendSyncRequest<1>(
         ipc::OutRaw<bool>(is_completed)));
   return is_completed;
}

void ILibraryAppletAccessor::Start() {
   ResultCode::AssertOk(object.SendSyncRequest<10>());
}

ResultCode ILibraryAppletAccessor::GetResult() {
   auto r = object.SendSyncRequest<30>();
   if(r) {
      return 0;
   } else {
      return r.error();
   }
}

void ILibraryAppletAccessor::PushInData(IStorage &ist) {
   ResultCode::AssertOk(
      object.SendSyncRequest<100>(
         ipc::InObject(ist)));
}

IStorage ILibraryAppletAccessor::PopOutData() {
   IStorage is;
   ResultCode::AssertOk(
      object.SendSyncRequest<101>(
         ipc::OutObject(is)));
   return is;
}

void ILibraryAppletAccessor::PushInteractiveInData(IStorage &ist) {
   ResultCode::AssertOk(
      object.SendSyncRequest<103>(
         ipc::InObject(ist)));
}

IStorage ILibraryAppletAccessor::PopInteractiveOutData() {
   IStorage is;
   ResultCode::AssertOk(
      object.SendSyncRequest<104>(
         ipc::OutObject(is)));
   return is;
}

trn::KEvent ILibraryAppletAccessor::GetPopOutDataEvent() {
   trn::KEvent evt;
   ResultCode::AssertOk(
      object.SendSyncRequest<105>(
         ipc::OutHandle<KEvent, ipc::copy>(evt)));
   return std::move(evt);
}

trn::KEvent ILibraryAppletAccessor::GetPopInteractiveOutDataEvent() {
   trn::KEvent evt;
   ResultCode::AssertOk(
      object.SendSyncRequest<106>(
         ipc::OutHandle<KEvent, ipc::copy>(evt)));
   return std::move(evt);
}

} // namespace am
