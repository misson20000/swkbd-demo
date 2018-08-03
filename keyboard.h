#pragma once

struct swkbd_config_t {
   uint32_t field_0;
   char16_t ok_text[9];
   uint16_t left_optional_symbol_key;
   uint16_t right_optional_symbol_key;
   uint8_t field_1A;
       uint8_t gap1A; // alignment
   uint32_t field_1C; // character set disable
   uint32_t field_20; // cursor position (0: beginning, 1: end)
   char16_t header_text[65];
   char16_t sub_text[129];
   char16_t guide_text[257];
       uint8_t gap3AA[2]; // alignment
   uint32_t max_length;
   uint32_t field_3B0;
   uint32_t field_3B4; // is password
   uint32_t field_3B8;
   uint16_t field_3BC;
   uint8_t field_3BE; // draw background
       uint8_t gap_3BF; // alignment
   uint32_t initial_string_offset;
   uint32_t initial_string_size;
   uint32_t user_dictionary_offset;
   uint32_t user_dictionary_length;
   uint8_t text_check_enabled;
       uint8_t gap_3D1[7]; // alignment
	uint64_t text_check_callback; // not sure why this is sent to swkbd
};

/* field_1C: character set disable bitmask
 *  0x01: disable Return
 *  0x02: disable ' ' and Return
 *  0x04: disable '@' and Return
 *  0x08: disable '%'
 *  0x10: disable '\', '/', and Return
 *  0x20: ???
 *  0x40: disable numbers
 *  0x80: for download codes
 */
   
static_assert(offsetof(swkbd_config_t, initial_string_offset) == 0x3c0);
static_assert(offsetof(swkbd_config_t, initial_string_size) == 0x3c4);
static_assert(offsetof(swkbd_config_t, user_dictionary_offset) == 0x3c8);
static_assert(offsetof(swkbd_config_t, user_dictionary_length) == 0x3cc);

struct work_buf_t {
   uint8_t pad1[20];
   char16_t initial_string[500];
   uint8_t pad2[1504];
   char16_t user_dictionary[500]; // not sure how long this is
};
static_assert(sizeof(work_buf_t) <= 0x1000, "work_buf_t must fit in a single page"); // there are cases in the sdk code where this can be size 0xd000
static_assert(offsetof(work_buf_t, initial_string) == 20, "initial string offset should be 20");
