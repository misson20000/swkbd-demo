#include<codecvt>
#include<locale>

#include<libtransistor/cpp/types.hpp>
#include<libtransistor/cpp/svc.hpp>
#include<libtransistor/cpp/ipcclient.hpp>
#include<libtransistor/cpp/ipc/sm.hpp>
#include<libtransistor/cpp/waiter.hpp>
#include<libtransistor/types.h>
#include<libtransistor/runtime_config.h>
#include<libtransistor/loader_config.h>
#include<libtransistor/alloc_pages.h>
#include<libtransistor/svc.h>
#include<libtransistor/ipc.h>
#include<libtransistor/err.h>
#include<libtransistor/util.h>
#include<libtransistor/internal_util.h>
#include<libtransistor/ipc/sm.h>
#include<libtransistor/ipc/am.h>

#include<string.h>
#include<stdio.h>

#include "ILibraryAppletCreator.hpp"
#include "ILibraryAppletAccessor.hpp"
#include "IStorage.hpp"
#include "IStorageAccessor.hpp"

#include "keyboard.h"

extern "C" {
runconf_stdio_override_t _trn_runconf_stdio_override = _TRN_RUNCONF_STDIO_OVERRIDE_TWILI;
}

using namespace trn;

static void ConfigureKeyboard(swkbd_config_t &config, int preset);
static void ShowKeyboard(am::ILibraryAppletCreator &ilac, int preset);

int main() {
   // obtain ILibraryAppletCreator
   trn::service::SM sm = ResultCode::AssertOk(trn::service::SM::Initialize());
   
   ipc::client::Object iasaps = ResultCode::AssertOk(
      sm.GetService("appletAE")); // IAllSystemAppletProxiesService
   
   ipc_domain_t domain;
   ResultCode::AssertOk(ipc_convert_to_domain(&iasaps.object, &domain));

   ipc::client::Object ilap; // ILibraryAppletProxy
   ResultCode::AssertOk(
      iasaps.SendSyncRequest<200>( // OpenLibraryAppletProxyOld
         ipc::InPid(),
         ipc::InRaw<uint64_t>(0),
         ipc::InHandle<handle_t, ipc::copy>(0xffff8001),
         ipc::OutObject(ilap)));
   
   am::ILibraryAppletCreator ilac;
   ResultCode::AssertOk(
      ilap.SendSyncRequest<11>( // GetLibraryAppletCreator
         ipc::OutObject(ilac)));

   ShowKeyboard(ilac, 4);
   
   return 0;
}

static void ShowKeyboard(am::ILibraryAppletCreator &ilac, int preset) {
   // create keyboard applet
   //   0x11: nn::applet::AppletId_LibraryAppletSwkbd
   //    0x0: nn::applet::LibraryAppletMode_AllForeground
   am::ILibraryAppletAccessor ilaa = ilac.CreateLibraryApplet(0x11, 0);

   // prepare common arguments
   am::IStorage common_args = ilac.CreateStorage(0x20);
   {
      am::IStorageAccessor access = common_args.Open();
      uint32_t common_args_header[] = {0x1, 0x20}; // {version?, size?}
      access.Write(0, (uint8_t*) common_args_header, sizeof(common_args_header));
      
      struct {
         uint32_t unknown1;
         uint32_t theme_color;
         uint8_t unknown2;
         uint64_t system_tick;
      } common_args_data = {0};
      
      static_assert(sizeof(common_args_data) == 0x18, "invalid common args data");
      
      common_args_data.unknown1 = 5;
      common_args_data.theme_color = 0;
      common_args_data.unknown2 = 0;
      common_args_data.system_tick = svcGetSystemTick();
      
      access.Write(sizeof(common_args_header), (uint8_t*) &common_args_data, sizeof(common_args_data));
   }
   ilaa.PushInData(common_args);

   // prepare transfer memory
   work_buf_t *work_buf = (work_buf_t*) alloc_pages(0x1000, 0x1000, nullptr);
   memset(work_buf, 0, 0x1000);
   
   char16_t initial_string[] = u"Initial string";
   memcpy(work_buf->initial_string, initial_string, sizeof(initial_string));

   KTransferMemory tmem(work_buf, 0x1000, 0, true);
   
   // prepare config
   swkbd_config_t swkbd_config = {0};
   ConfigureKeyboard(swkbd_config, preset);

   swkbd_config.initial_string_offset = offsetof(work_buf_t, initial_string);
   swkbd_config.initial_string_size = sizeof(initial_string);

   char16_t header_text[] = u"Header Text";
   char16_t sub_text[] = u"Sub text";
   char16_t guide_text[] = u"Guide text";
   char16_t ok_text[] = u"OK text";
   memcpy(swkbd_config.header_text, header_text, sizeof(header_text));
   memcpy(swkbd_config.sub_text, sub_text, sizeof(sub_text));
   memcpy(swkbd_config.guide_text, guide_text, sizeof(guide_text));
   memcpy(swkbd_config.ok_text, ok_text, sizeof(ok_text));
   
   am::IStorage config_storage = ilac.CreateStorage(0x3e0);
   {
      am::IStorageAccessor access = config_storage.Open();
      static_assert(sizeof(swkbd_config) == 0x3e0, "invalid swkbd_config size");
      access.Write(0, (uint8_t*) &swkbd_config, sizeof(swkbd_config));
   }
   ilaa.PushInData(config_storage);

   am::IStorage work_buf_storage = ilac.CreateTransferMemoryStorage(true, 0x1000, tmem);
   ilaa.PushInData(work_buf_storage);

   // launch applet
   bool run = true;
   
   trn::Waiter waiter;

   KEvent applet_event = ilaa.GetAppletStateChangedEvent();
   auto wh = waiter.Add(applet_event, [&]() {
                                         printf("state changed\n");
                                         if(ilaa.IsCompleted()) {
                                            printf("completed: 0x%x\n", ilaa.GetResult().code);
                                            run = false;
                                            return false;
                                         }
                                         return true;
                                      });

   // for UTF-16 <-> UTF-8 conversion
   std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
   
   // if swkbd_config.text_check_enabled is set, this is used to validate text
   // when it is submitted.
   KEvent interactive_event = ilaa.GetPopInteractiveOutDataEvent();
   printf("interactive event: 0x%x\n", interactive_event.handle);
   auto wh2 = waiter.Add(
      interactive_event,
      [&]() {
         printf("interactive out data event signalled\n");
         std::u16string string;
         {
            am::IStorage storage = ilaa.PopInteractiveOutData();
            am::IStorageAccessor accessor = storage.Open();
            
            size_t size = accessor.GetSize();
            printf("size: 0x%lx\n", size);
            
            size_t text_size;
            accessor.Read(0, (uint8_t*) &text_size, sizeof(text_size));
            
            char16_t *buf = new char16_t[(size-sizeof(text_size))/sizeof(*buf)];
            accessor.Read(sizeof(text_size), (uint8_t*) buf, size - sizeof(text_size));
            string = std::u16string(buf);
            delete[] buf;
         }
         
         printf("check string: '%s'\n", convert.to_bytes(string).c_str());

         am::IStorage response = ilac.CreateStorage(0x7d8);
         {
            am::IStorageAccessor access = response.Open();
            uint32_t result = 1;
            access.Write(0, (uint8_t*) &result, sizeof(result));
            char16_t message[] = u"what"; // swkbd shows this message in a dialog box
            access.Write(4, (uint8_t*) message, sizeof(message));
         }
         ilaa.PushInteractiveInData(response);
         return true;
      });
   
   KEvent out_event = ilaa.GetPopOutDataEvent();
   printf("out event: 0x%x\n", out_event.handle);
   auto wh3 = waiter.Add(
      out_event,
      [&]() {
         printf("pop out data event signalled\n");
         am::IStorage storage = ilaa.PopOutData();
         am::IStorageAccessor accessor = storage.Open();
         
         size_t size = accessor.GetSize();
         printf("size: 0x%lx\n", size);

         // 0: success
         // 1: cancel
         uint32_t code;
         accessor.Read(0, (uint8_t*) &code, sizeof(code));
         
         char16_t *buf = new char16_t[(size-sizeof(code))/sizeof(*buf)];
         accessor.Read(sizeof(code), (uint8_t*) buf, size - sizeof(code));
         std::u16string string(buf);
         delete[] buf;
         
         printf("result code: 0x%x\n", code);
         printf("result string: '%s'\n", convert.to_bytes(string).c_str());
         
         return true;
      });
   
   printf("launching...\n");
   ilaa.Start();
   printf("launched\n");
   while(run) {
      ResultCode::AssertOk(waiter.Wait(3000000000));
   }
   
   printf("exiting\n");
}

/*
 * If max_length is <= 32, you get a single-line layout that displays
 * header text and sub text. Otherwise, you get a multi-line layout that
 * does not show header or subtext.
 */

static void ConfigureKeyboard(swkbd_config_t &config, int preset) {
   config = {0};
   config.left_optional_symbol_key = 0; // not sure what these do?
   config.right_optional_symbol_key = 0;
   switch(preset) {
   case 4: // testing
      config.text_check_enabled = 1;
      config.text_check_callback = 1;
   case 0: // default
      config.field_0 = 2;
      config.field_1A = 0;
      config.field_1C = 0;
      config.field_20 = 1;
      config.max_length = 0;
      config.field_3B0 = 0;
      config.field_3B4 = 0;
      config.field_3B8 = 1;
      config.field_3BC = 1;
      config.field_3BE = 1;
      break;
   case 1: // password
      config.field_0 = 2;
      config.field_1A = 0;
      config.field_1C = 0;
      config.field_20 = 1;
      config.max_length = 0;
      config.field_3B0 = 0;
      config.field_3B4 = 1;
      config.field_3B8 = 0;
      config.field_3BC = 0;
      config.field_3BE = 1;
      break;
   case 2: // username
      config.field_0 = 0;
      config.field_1A = 0;
      config.field_1C = 0x100;
      config.field_20 = 1;
      config.max_length = 0;
      config.field_3B0 = 0;
      config.field_3B4 = 0;
      config.field_3B8 = 0;
      config.field_3BC = 0;
      config.field_3BE = 1;
      break;
   case 3: // download code
      config.field_0 = 0;
      config.field_1A = 0;
      config.field_1C = 0x80;
      config.field_20 = 1;
      config.max_length = 0;
      config.field_3B0 = 0;
      config.field_3B4 = 0;
      config.field_3B8 = 0;
      config.field_3BC = 0;
      config.field_3BE = 1;
      break;
   }
}
