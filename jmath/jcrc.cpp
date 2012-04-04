/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "Stdafx.h"
#include "jcrc.h"

namespace jmath {

uint8_t crc_8[256] = {
		0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
		0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
		0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
		0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
		0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
		0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
		0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
		0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
		0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
		0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
		0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
		0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
		0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
		0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
		0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
		0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
		0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
		0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
		0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
		0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
		0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
		0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
		0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
		0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
		0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
		0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
		0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
		0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
		0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
		0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
		0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
		0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

uint16_t crc_16[256] = {
		0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
		0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7,
		0x1081, 0x0108, 0x3393, 0x221A, 0x56A5, 0x472C, 0x75B7, 0x643E,
		0x9CC9, 0x8D40, 0xBFDB, 0xAE52, 0xDAED, 0xCB64, 0xF9FF, 0xE876,
		0x2102, 0x308B, 0x0210, 0x1399, 0x6726, 0x76AF, 0x4434, 0x55BD,
		0xAD4A, 0xBCC3, 0x8E58, 0x9FD1, 0xEB6E, 0xFAE7, 0xC87C, 0xD9F5,
		0x3183, 0x200A, 0x1291, 0x0318, 0x77A7, 0x662E, 0x54B5, 0x453C,
		0xBDCB, 0xAC42, 0x9ED9, 0x8F50, 0xFBEF, 0xEA66, 0xD8FD, 0xC974,
		0x4204, 0x538D, 0x6116, 0x709F, 0x0420, 0x15A9, 0x2732, 0x36BB,
		0xCE4C, 0xDFC5, 0xED5E, 0xFCD7, 0x8868, 0x99E1, 0xAB7A, 0xBAF3,
		0x5285, 0x430C, 0x7197, 0x601E, 0x14A1, 0x0528, 0x37B3, 0x263A,
		0xDECD, 0xCF44, 0xFDDF, 0xEC56, 0x98E9, 0x8960, 0xBBFB, 0xAA72,
		0x6306, 0x728F, 0x4014, 0x519D, 0x2522, 0x34AB, 0x0630, 0x17B9,
		0xEF4E, 0xFEC7, 0xCC5C, 0xDDD5, 0xA96A, 0xB8E3, 0x8A78, 0x9BF1,
		0x7387, 0x620E, 0x5095, 0x411C, 0x35A3, 0x242A, 0x16B1, 0x0738,
		0xFFCF, 0xEE46, 0xDCDD, 0xCD54, 0xB9EB, 0xA862, 0x9AF9, 0x8B70,
		0x8408, 0x9581, 0xA71A, 0xB693, 0xC22C, 0xD3A5, 0xE13E, 0xF0B7,
		0x0840, 0x19C9, 0x2B52, 0x3ADB, 0x4E64, 0x5FED, 0x6D76, 0x7CFF,
		0x9489, 0x8500, 0xB79B, 0xA612, 0xD2AD, 0xC324, 0xF1BF, 0xE036,
		0x18C1, 0x0948, 0x3BD3, 0x2A5A, 0x5EE5, 0x4F6C, 0x7DF7, 0x6C7E,
		0xA50A, 0xB483, 0x8618, 0x9791, 0xE32E, 0xF2A7, 0xC03C, 0xD1B5,
		0x2942, 0x38CB, 0x0A50, 0x1BD9, 0x6F66, 0x7EEF, 0x4C74, 0x5DFD,
		0xB58B, 0xA402, 0x9699, 0x8710, 0xF3AF, 0xE226, 0xD0BD, 0xC134,
		0x39C3, 0x284A, 0x1AD1, 0x0B58, 0x7FE7, 0x6E6E, 0x5CF5, 0x4D7C,
		0xC60C, 0xD785, 0xE51E, 0xF497, 0x8028, 0x91A1, 0xA33A, 0xB2B3,
		0x4A44, 0x5BCD, 0x6956, 0x78DF, 0x0C60, 0x1DE9, 0x2F72, 0x3EFB,
		0xD68D, 0xC704, 0xF59F, 0xE416, 0x90A9, 0x8120, 0xB3BB, 0xA232,
		0x5AC5, 0x4B4C, 0x79D7, 0x685E, 0x1CE1, 0x0D68, 0x3FF3, 0x2E7A,
		0xE70E, 0xF687, 0xC41C, 0xD595, 0xA12A, 0xB0A3, 0x8238, 0x93B1,
		0x6B46, 0x7ACF, 0x4854, 0x59DD, 0x2D62, 0x3CEB, 0x0E70, 0x1FF9,
		0xF78F, 0xE606, 0xD49D, 0xC514, 0xB1AB, 0xA022, 0x92B9, 0x8330,
		0x7BC7, 0x6A4E, 0x58D5, 0x495C, 0x3DE3, 0x2C6A, 0x1EF1, 0x0F78
};

uint32_t crc_32[256] = {
	0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
	0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61, 0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
	0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
	0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
	0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039, 0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
	0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
	0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
	0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1, 0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
	0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
	0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
	0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde, 0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
	0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
	0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
	0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6, 0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
	0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
	0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
	0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637, 0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
	0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
	0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
	0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff, 0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
	0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
	0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
	0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7, 0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
	0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
	0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
	0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8, 0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
	0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
	0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
	0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0, 0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
	0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
	0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
	0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668, 0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4,
};

uint64_t crc_64[256] = {
	0x0000000000000000LL, 0x42f0e1eba9ea3693LL, 0x85e1c3d753d46d26LL, 0xc711223cfa3e5bb5LL, 0x493366450e42ecdfLL, 0x0bc387aea7a8da4cLL, 0xccd2a5925d9681f9LL, 0x8e224479f47cb76aLL,
	0x9266cc8a1c85d9beLL, 0xd0962d61b56fef2dLL, 0x17870f5d4f51b498LL, 0x5577eeb6e6bb820bLL, 0xdb55aacf12c73561LL, 0x99a54b24bb2d03f2LL, 0x5eb4691841135847LL, 0x1c4488f3e8f96ed4LL,
	0x663d78ff90e185efLL, 0x24cd9914390bb37cLL, 0xe3dcbb28c335e8c9LL, 0xa12c5ac36adfde5aLL, 0x2f0e1eba9ea36930LL, 0x6dfeff5137495fa3LL, 0xaaefdd6dcd770416LL, 0xe81f3c86649d3285LL,
	0xf45bb4758c645c51LL, 0xb6ab559e258e6ac2LL, 0x71ba77a2dfb03177LL, 0x334a9649765a07e4LL, 0xbd68d2308226b08eLL, 0xff9833db2bcc861dLL, 0x388911e7d1f2dda8LL, 0x7a79f00c7818eb3bLL,
	0xcc7af1ff21c30bdeLL, 0x8e8a101488293d4dLL, 0x499b3228721766f8LL, 0x0b6bd3c3dbfd506bLL, 0x854997ba2f81e701LL, 0xc7b97651866bd192LL, 0x00a8546d7c558a27LL, 0x4258b586d5bfbcb4LL,
	0x5e1c3d753d46d260LL, 0x1cecdc9e94ace4f3LL, 0xdbfdfea26e92bf46LL, 0x990d1f49c77889d5LL, 0x172f5b3033043ebfLL, 0x55dfbadb9aee082cLL, 0x92ce98e760d05399LL, 0xd03e790cc93a650aLL,
	0xaa478900b1228e31LL, 0xe8b768eb18c8b8a2LL, 0x2fa64ad7e2f6e317LL, 0x6d56ab3c4b1cd584LL, 0xe374ef45bf6062eeLL, 0xa1840eae168a547dLL, 0x66952c92ecb40fc8LL, 0x2465cd79455e395bLL,
	0x3821458aada7578fLL, 0x7ad1a461044d611cLL, 0xbdc0865dfe733aa9LL, 0xff3067b657990c3aLL, 0x711223cfa3e5bb50LL, 0x33e2c2240a0f8dc3LL, 0xf4f3e018f031d676LL, 0xb60301f359dbe0e5LL,
	0xda050215ea6c212fLL, 0x98f5e3fe438617bcLL, 0x5fe4c1c2b9b84c09LL, 0x1d14202910527a9aLL, 0x93366450e42ecdf0LL, 0xd1c685bb4dc4fb63LL, 0x16d7a787b7faa0d6LL, 0x5427466c1e109645LL,
	0x4863ce9ff6e9f891LL, 0x0a932f745f03ce02LL, 0xcd820d48a53d95b7LL, 0x8f72eca30cd7a324LL, 0x0150a8daf8ab144eLL, 0x43a04931514122ddLL, 0x84b16b0dab7f7968LL, 0xc6418ae602954ffbLL,
	0xbc387aea7a8da4c0LL, 0xfec89b01d3679253LL, 0x39d9b93d2959c9e6LL, 0x7b2958d680b3ff75LL, 0xf50b1caf74cf481fLL, 0xb7fbfd44dd257e8cLL, 0x70eadf78271b2539LL, 0x321a3e938ef113aaLL,
	0x2e5eb66066087d7eLL, 0x6cae578bcfe24bedLL, 0xabbf75b735dc1058LL, 0xe94f945c9c3626cbLL, 0x676dd025684a91a1LL, 0x259d31cec1a0a732LL, 0xe28c13f23b9efc87LL, 0xa07cf2199274ca14LL,
	0x167ff3eacbaf2af1LL, 0x548f120162451c62LL, 0x939e303d987b47d7LL, 0xd16ed1d631917144LL, 0x5f4c95afc5edc62eLL, 0x1dbc74446c07f0bdLL, 0xdaad56789639ab08LL, 0x985db7933fd39d9bLL,
	0x84193f60d72af34fLL, 0xc6e9de8b7ec0c5dcLL, 0x01f8fcb784fe9e69LL, 0x43081d5c2d14a8faLL, 0xcd2a5925d9681f90LL, 0x8fdab8ce70822903LL, 0x48cb9af28abc72b6LL, 0x0a3b7b1923564425LL,
	0x70428b155b4eaf1eLL, 0x32b26afef2a4998dLL, 0xf5a348c2089ac238LL, 0xb753a929a170f4abLL, 0x3971ed50550c43c1LL, 0x7b810cbbfce67552LL, 0xbc902e8706d82ee7LL, 0xfe60cf6caf321874LL,
	0xe224479f47cb76a0LL, 0xa0d4a674ee214033LL, 0x67c58448141f1b86LL, 0x253565a3bdf52d15LL, 0xab1721da49899a7fLL, 0xe9e7c031e063acecLL, 0x2ef6e20d1a5df759LL, 0x6c0603e6b3b7c1caLL,
	0xf6fae5c07d3274cdLL, 0xb40a042bd4d8425eLL, 0x731b26172ee619ebLL, 0x31ebc7fc870c2f78LL, 0xbfc9838573709812LL, 0xfd39626eda9aae81LL, 0x3a28405220a4f534LL, 0x78d8a1b9894ec3a7LL,
	0x649c294a61b7ad73LL, 0x266cc8a1c85d9be0LL, 0xe17dea9d3263c055LL, 0xa38d0b769b89f6c6LL, 0x2daf4f0f6ff541acLL, 0x6f5faee4c61f773fLL, 0xa84e8cd83c212c8aLL, 0xeabe6d3395cb1a19LL,
	0x90c79d3fedd3f122LL, 0xd2377cd44439c7b1LL, 0x15265ee8be079c04LL, 0x57d6bf0317edaa97LL, 0xd9f4fb7ae3911dfdLL, 0x9b041a914a7b2b6eLL, 0x5c1538adb04570dbLL, 0x1ee5d94619af4648LL,
	0x02a151b5f156289cLL, 0x4051b05e58bc1e0fLL, 0x87409262a28245baLL, 0xc5b073890b687329LL, 0x4b9237f0ff14c443LL, 0x0962d61b56fef2d0LL, 0xce73f427acc0a965LL, 0x8c8315cc052a9ff6LL,
	0x3a80143f5cf17f13LL, 0x7870f5d4f51b4980LL, 0xbf61d7e80f251235LL, 0xfd913603a6cf24a6LL, 0x73b3727a52b393ccLL, 0x31439391fb59a55fLL, 0xf652b1ad0167feeaLL, 0xb4a25046a88dc879LL,
	0xa8e6d8b54074a6adLL, 0xea16395ee99e903eLL, 0x2d071b6213a0cb8bLL, 0x6ff7fa89ba4afd18LL, 0xe1d5bef04e364a72LL, 0xa3255f1be7dc7ce1LL, 0x64347d271de22754LL, 0x26c49cccb40811c7LL,
	0x5cbd6cc0cc10fafcLL, 0x1e4d8d2b65facc6fLL, 0xd95caf179fc497daLL, 0x9bac4efc362ea149LL, 0x158e0a85c2521623LL, 0x577eeb6e6bb820b0LL, 0x906fc95291867b05LL, 0xd29f28b9386c4d96LL,
	0xcedba04ad0952342LL, 0x8c2b41a1797f15d1LL, 0x4b3a639d83414e64LL, 0x09ca82762aab78f7LL, 0x87e8c60fded7cf9dLL, 0xc51827e4773df90eLL, 0x020905d88d03a2bbLL, 0x40f9e43324e99428LL,
	0x2cffe7d5975e55e2LL, 0x6e0f063e3eb46371LL, 0xa91e2402c48a38c4LL, 0xebeec5e96d600e57LL, 0x65cc8190991cb93dLL, 0x273c607b30f68faeLL, 0xe02d4247cac8d41bLL, 0xa2dda3ac6322e288LL,
	0xbe992b5f8bdb8c5cLL, 0xfc69cab42231bacfLL, 0x3b78e888d80fe17aLL, 0x7988096371e5d7e9LL, 0xf7aa4d1a85996083LL, 0xb55aacf12c735610LL, 0x724b8ecdd64d0da5LL, 0x30bb6f267fa73b36LL,
	0x4ac29f2a07bfd00dLL, 0x08327ec1ae55e69eLL, 0xcf235cfd546bbd2bLL, 0x8dd3bd16fd818bb8LL, 0x03f1f96f09fd3cd2LL, 0x41011884a0170a41LL, 0x86103ab85a2951f4LL, 0xc4e0db53f3c36767LL,
	0xd8a453a01b3a09b3LL, 0x9a54b24bb2d03f20LL, 0x5d45907748ee6495LL, 0x1fb5719ce1045206LL, 0x919735e51578e56cLL, 0xd367d40ebc92d3ffLL, 0x1476f63246ac884aLL, 0x568617d9ef46bed9LL,
	0xe085162ab69d5e3cLL, 0xa275f7c11f7768afLL, 0x6564d5fde549331aLL, 0x279434164ca30589LL, 0xa9b6706fb8dfb2e3LL, 0xeb46918411358470LL, 0x2c57b3b8eb0bdfc5LL, 0x6ea7525342e1e956LL,
	0x72e3daa0aa188782LL, 0x30133b4b03f2b111LL, 0xf7021977f9cceaa4LL, 0xb5f2f89c5026dc37LL, 0x3bd0bce5a45a6b5dLL, 0x79205d0e0db05dceLL, 0xbe317f32f78e067bLL, 0xfcc19ed95e6430e8LL,
	0x86b86ed5267cdbd3LL, 0xc4488f3e8f96ed40LL, 0x0359ad0275a8b6f5LL, 0x41a94ce9dc428066LL, 0xcf8b0890283e370cLL, 0x8d7be97b81d4019fLL, 0x4a6acb477bea5a2aLL, 0x089a2aacd2006cb9LL,
	0x14dea25f3af9026dLL, 0x562e43b4931334feLL, 0x913f6188692d6f4bLL, 0xd3cf8063c0c759d8LL, 0x5dedc41a34bbeeb2LL, 0x1f1d25f19d51d821LL, 0xd80c07cd676f8394LL, 0x9afce626ce85b507LL
};

CRC::CRC():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jnath::CRC");
}

CRC::~CRC()
{
}

uint8_t CRC::Calculate8(uint8_t *begin, int count) 
{
	uint8_t crc = 0x00;
	
	while (count-- > 0) {
		crc = crc_8[crc ^ (uint8_t)(*begin++)];
	}

	return crc ^ 0x00;
}

uint16_t CRC::Calculate16(uint16_t *begin, int count) 
{
	uint16_t crc = 0xffff;
	
	while (count-- > 0) {
		crc = (crc << 8) ^ crc_16[(crc >> 8) ^ ((uint16_t)(*begin++))];
	}
	
	return crc ^ 0x0000;
}

uint32_t CRC::Calculate32(uint32_t *begin, int count) 
{	
	uint32_t crc = 0xffffffff;
	
	while (count-- > 0) {
		crc = (crc << 8) ^ crc_32[(crc >> 24) ^ ((uint32_t)(*begin++))];
	}
	
	return crc ^ 0xffffffff;
}

uint64_t CRC::Calculate64(uint64_t *begin, int count) 
{	
	uint64_t crc = 0xffffffffffffffffLL;
	
	while (count-- > 0) {
		crc = (crc << 8) ^ crc_32[(crc >> 56) ^ ((uint64_t)(*begin++))];
	}
	
	return ~crc;
}

}
