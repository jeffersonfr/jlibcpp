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
#include "jmpeg/jmpeglib.h"

std::string GetTableDescription(int pid, int tid)
{
  if (pid == TS_PAT_PID && tid == TS_PAT_TABLE_ID) {
    return "Program Association Table";
  } else if (pid == TS_CAT_PID && tid == TS_CAT_TABLE_ID) {
    return "Condition Access Table";
  } else if (pid == TS_NIT_PID && tid == TS_NIT_TABLE_ID) {
    return "Network Information Table";
  } else if (pid == TS_BAT_PID && tid == TS_BAT_TABLE_ID) {
    return "Bouquet Association Table";
  } else if (pid == TS_TSDT_PID && tid == TS_TSDT_TABLE_ID) {
    return "Transport Stream Descriptor Table";
  } else if (pid == TS_SDT_PID && tid == TS_SDT_TABLE_ID) {
    return "Service Descriptor Table";
  } else if (pid == TS_TDT_PID && tid == TS_TDT_TABLE_ID) {
    return "Time Descriptor Table";
  } else if (pid == TS_TDT_PID && tid == TS_TOT_TABLE_ID) {
    return "Time Offset Table";
  } else if (pid == TS_EIT_PID) {
	  if (tid == 0x4e) {
		  return "Event Information Table [present & following]";
    } else if (tid == 0x4f) {
		  return "Event Information Table [present & following - other service]";
    } else if (tid >= 0x50 && tid <= 0x5f) {
		  return "Event Information Table [schedule]";
    } else if (tid >= 0x60 && tid <= 0x6f) {
		  return "Event Information Table [schedule - other service]";
    }
  } else if (pid == TS_RST_PID && tid == TS_RST_TABLE_ID) {
    return "Running Status Table";
  } else { // INFO:: considering only table id information
    if (tid == TS_PAT_TABLE_ID) {
			return "Program Association Table ?";
    } else if (tid == TS_CAT_TABLE_ID) {
			return "Condicional Access Table ?";
    } else if (tid == TS_PMT_TABLE_ID) {
			return "Program Map Table";
    } else if (tid == TS_TSDT_TABLE_ID) {
			return "Transport Stream Description";
    } else if (tid == TS_METADATA_TABLE_ID) {
			return "Metadata Table";
    } else if (tid == TS_NIT_TABLE_ID) {
			return "Network Information Table ?";
    } else if (tid == TS_SDT_TABLE_ID) {
			return "Service Description Table";
    } else if (tid == TS_BAT_TABLE_ID) {
			return "Bouquet Association Table";
    } else if (tid == TS_TDT_TABLE_ID) {
			return "Time Description Table";
    } else if (tid == TS_TOT_TABLE_ID) {
			return "Time Offset Table";
    } else if (tid == 0x4e) {
		  return "Event Information Table [present & following]";
    } else if (tid == 0x4f) {
		  return "Event Information Table [present & following - other service]";
    } else if (tid >= 0x50 && tid <= 0x5f) {
		  return "Event Information Table [schedule]";
    } else if (tid >= 0x60 && tid <= 0x6f) {
		  return "Event Information Table [schedule - other service]";
    } else if (tid == 0x3a) {
			return "DSM-CC Sections containing multi-protocol encapsulated data";
    } else if (tid == 0x3b) {
			return "DSM-CC Sections containing U-N Messages, except Download Data Messages";
    } else if (tid == 0x3c) {
			return "DSM-CC Sections containing Download Data Messages";
    } else if (tid == 0x3d) {
			return "DSM-CC Sections containing Stream Descriptors";
    } else if (tid == 0x3e) {
			return "DSM-CC Sections containing private data";
    } else if (tid == 0x3f) {
			return "DSM-CC Addressable Sections";
		}
  }

	return "Unknown Table";
}

std::string GetStreamTypeDescription(int stream_type)
{
	if (stream_type > 0x1c && stream_type < 0x7e) {
		switch (stream_type) {
			case 0x24: return "HEVC video stream";
			case 0x25: return "HEVC temporal video subset (profile Annex A H.265)";
			case 0x42: return "AVS Video";
			default:
				break;
		}

		return "H.220.0/13818-1 reserved";
	} else if (stream_type >= 0x80 && stream_type <= 0xff) {
		switch (stream_type) {
			case 0x81: return "User private (commonly Dolby/AC-3 in ATSC)";
			default:
				break;
		}

		return "User private";
	} else {
		switch (stream_type) {
			case 0x00: return "Reserved";
			case 0x01: return "11172-2 video (MPEG-1)";
			case 0x02: return "H.262/13818-2 video (MPEG-2) or 11172-2 constrained video";
			case 0x03: return "11172-3 audio (MPEG-1)";
			case 0x04: return "13818-3 audio (MPEG-2)";
			case 0x05: return "H.222.0/13818-1 private sections";
			case 0x06: return "H.222.0/13818-1 PES private data (maybe Dolby/AC-3 in DVB)";
			case 0x07: return "13522 MHEG";
			case 0x08: return "H.222.0/13818-1 Annex A - DSM CC";
			case 0x09: return "H.222.1";
			case 0x0A: return "13818-6 type A (Multi-protocol Encapsulation)";
			case 0x0B: return "13818-6 type B (DSM-CC U-N Messages)";
			case 0x0C: return "13818-6 type C (DSM-CC Stream Descriptors)";
			case 0x0D: return "13818-6 type D (DSM-CC Sections (any type, including private data))";
			case 0x0E: return "H.222.0/13818-1 auxiliary";
			case 0x0F: return "13818-7 Audio with ADTS transport syntax";
			case 0x10: return "14496-2 Visual (MPEG-4 part 2 video)";
			case 0x11: return "14496-3 Audio with LATM transport syntax (14496-3/AMD 1)";
			case 0x12: return "14496-1 SL-packetized or FlexMux stream in PES packets";
			case 0x13: return "14496-1 SL-packetized or FlexMux stream in 14496 sections";
			case 0x14: return "ISO/IEC 13818-6 Synchronized Download Protocol";
			case 0x15: return "Metadata in PES packets";
			case 0x16: return "Metadata in metadata_sections";
			case 0x17: return "Metadata in 13818-6 Data Carousel";
			case 0x18: return "Metadata in 13818-6 Object Carousel";
			case 0x19: return "Metadata in 13818-6 Synchronized Download Protocol";
			case 0x1A: return "13818-11 MPEG-2 IPMP stream";
			case 0x1B: return "H.264/14496-10 video (MPEG-4/AVC)";
			case 0x7F: return "IPMP stream";
			default: 
				break;
		}
	}

	return "Unrecognised";
}

std::string GetComponentDescription(int stream_content, int component_type)
{
  std::string description = "undefined";

  if (stream_content == 0x00 and component_type >= 0x00 and component_type <= 0xff) {
    description = "reserved";
  } else if (stream_content == 0x01 and component_type == 0x00) {
    description = "reserved";
  } else if (stream_content == 0x01 and component_type == 0x01) {
    description = "MPEG2 vídeo 480i(525i), relação de aspecto 4:3";
  } else if (stream_content == 0x01 and component_type == 0x02) {
    description = "MPEG2 vídeo 480i(525i), relação de aspecto 16:9 com vetor de pan";
  } else if (stream_content == 0x01 and component_type == 0x03) {
    description = "MPEG2 vídeo 480i(525i), relação de aspecto 16:9 sem vetor de pan";
  } else if (stream_content == 0x01 and component_type == 0x04) {
    description = "MPEG2 vídeo 480i(525i), > relação de aspecto 16:9";
  } else if (stream_content == 0x01 and component_type >= 0x05 and component_type <= 0xa0) {
    description = "reserved";
  } else if (stream_content == 0x01 and component_type == 0xa1) {
    description = "MPEG2 vídeo 480p(525p), relação de aspecto 4:3";
  } else if (stream_content == 0x01 and component_type == 0xa2) {
    description = "MPEG2 vídeo 480p(525p), relação de aspecto 16:9 com vetor de pan";
  } else if (stream_content == 0x01 and component_type == 0xa3) {
    description = "MPEG2 vídeo 480p(525p), relação de aspecto 16:9 sem vetor de pan";
  } else if (stream_content == 0x01 and component_type == 0xa4) {
    description = "MPEG2 vídeo 480p(525p), > relação de aspecto 16:9";
  } else if (stream_content == 0x01 and component_type >= 0xa5 and component_type <= 0xb0) {
    description = "reserved";
  } else if (stream_content == 0x01 and component_type == 0xb1) {
    description = "MPEG2 vídeo 1080i(1125i), relação de aspecto 4:3";
  } else if (stream_content == 0x01 and component_type == 0xb2) {
    description = "MPEG2 vídeo 1080i(1125i), relação de aspecto 16:9 com vetor de pan";
  } else if (stream_content == 0x01 and component_type == 0xb3) {
    description = "MPEG2 vídeo 1080i(1125i), relação de aspecto 16:9 sem vetor de pan";
  } else if (stream_content == 0x01 and component_type == 0xb4) {
    description = "MPEG2 vídeo 1080i(1125i), > relação de aspecto 16:9";
  } else if (stream_content == 0x01 and component_type >= 0xb5 and component_type <= 0xc0) {
    description = "reserved";
  } else if (stream_content == 0x01 and component_type == 0xc1) {
    description = "MPEG2 vídeo 720p(750p), relação de aspecto 4:3";
  } else if (stream_content == 0x01 and component_type == 0xc2) {
    description = "MPEG2 vídeo 720p(750p), relação de aspecto 16:9 com vetor de pan";
  } else if (stream_content == 0x01 and component_type == 0xc3) {
    description = "MPEG2 vídeo 720p(750p), relação de aspecto 16:9 sem vetor de pan";
  } else if (stream_content == 0x01 and component_type == 0xc4) {
    description = "MPEG2 vídeo 720p(750p), > relação de aspecto 16:9";
  } else if (stream_content == 0x01 and component_type >= 0xc5 and component_type <= 0xd0) {
    description = "reserved";
  } else if (stream_content == 0x01 and component_type == 0xd1) {
    description = "MPEG2 vídeo 240p, relação de aspecto 4:3";
  } else if (stream_content == 0x01 and component_type == 0xd2) {
    description = "MPEG2 vídeo 240p, relação de aspecto 4:3 com vetor de pan";
  } else if (stream_content == 0x01 and component_type == 0xd3) {
    description = "MPEG2 vídeo 240p, relação de aspecto 4:3, sem vetor de pan";
  } else if (stream_content == 0x01 and component_type == 0xd4) {
    description = "MPEG2 vídeo 240p, > relação de aspecto 16:9";
  } else if (stream_content == 0x01 and component_type >= 0xd5 and component_type <= 0xe0) {
    description = "reserved";
  } else if (stream_content == 0x01 and component_type == 0xe1) {
    description = "MPEG2 vídeo 1080p(1125p), relação de aspecto 4:3";
  } else if (stream_content == 0x01 and component_type == 0xe2) {
    description = "MPEG2 vídeo 1080p(1125p), relação de aspecto 16:9 com vetor de pan";
  } else if (stream_content == 0x01 and component_type == 0xe3) {
    description = "MPEG2 vídeo 1080p(1125p), relação de aspecto 16:9 sem vetor de pan";
  } else if (stream_content == 0x01 and component_type == 0xe4) {
    description = "MPEG2 vídeo 1080p(1125p), > relação de aspecto 16:9";
  } else if (stream_content == 0x01 and component_type >= 0xe5 and component_type <= 0xff) {
    description = "reserved";
  } else if (stream_content == 0x02 and component_type == 0x00) {
    description = "reserved";
  } else if (stream_content == 0x02 and component_type == 0x01) {
    description = "AAC MPEG2 áudio, modo 1/0 (single mono)";
  } else if (stream_content == 0x02 and component_type == 0x02) {
    description = "AAC MPEG2 áudio, modo 1/0 + 1/0 (dual mono)";
  } else if (stream_content == 0x02 and component_type == 0x03) {
    description = "AAC MPEG2 áudio, modo 2/0 (estéreo)";
  } else if (stream_content == 0x02 and component_type == 0x04) {
    description = "AAC MPEG2 áudio, modo 2/1";
  } else if (stream_content == 0x02 and component_type == 0x05) {
    description = "AAC MPEG2 áudio, modo 3/0";
  } else if (stream_content == 0x02 and component_type == 0x06) {
    description = "AAC MPEG2 áudio, modo 2/2";
  } else if (stream_content == 0x02 and component_type == 0x07) {
    description = "AAC MPEG2 áudio, modo 3/1";
  } else if (stream_content == 0x02 and component_type == 0x08) {
    description = "AAC MPEG2 áudio, modo 3/2";
  } else if (stream_content == 0x02 and component_type == 0x09) {
    description = "AAC MPEG2 áudio, modo 3/2 + LFE";
  } else if (stream_content == 0x02 and component_type >= 0x0a and component_type <= 0x3f) {
    description = "reserved";
  } else if (stream_content == 0x02 and component_type == 0x40) {
    description = "AAC MPEG2 descrição de áudio para deficientes visuais";
  } else if (stream_content == 0x02 and component_type == 0x41) {
    description = "AAC MPEG2 áudio com áudio elevado para deficientes auditivos";
  } else if (stream_content == 0x02 and component_type >= 0x42 and component_type <= 0xaf) {
    description = "reserved";
  } else if (stream_content == 0x02 and component_type >= 0xb0 and component_type <= 0xfe) {
    description = "user defined";
  } else if (stream_content == 0x02 and component_type == 0xff) {
    description = "reserved";
  
  
  
  
  
  } else if (stream_content == 0x03 and component_type == 0x00) {
    description = "reserved for future use";
  } else if (stream_content == 0x03 and component_type == 0x01) {
    description = "EBU Teletext subtitles";
  } else if (stream_content == 0x03 and component_type == 0x02) {
    description = "associated EBU Teletext";
  } else if (stream_content == 0x03 and component_type == 0x03) {
    description = "VBI data";
  } else if (stream_content == 0x03 and component_type >= 0x04 and component_type <= 0x0f) {
    description = "reserved for future use";
  } else if (stream_content == 0x03 and component_type >= 0x10) {
    description = "DVB subtitles (normal) with no monitor aspect ratio criticality";
  } else if (stream_content == 0x03 and component_type >= 0x11) {
    description = "DVB subtitles (normal) for display on 4:3 aspect ratio monitor";
  } else if (stream_content == 0x03 and component_type >= 0x12) {
    description = "DVB subtitles (normal) for display on 16:9 aspect ratio monitor";
  } else if (stream_content == 0x03 and component_type >= 0x13) {
    description = "DVB subtitles (normal) for display on 2.21:1 aspect ratio monitor";
  } else if (stream_content == 0x03 and component_type >= 0x14) {
    description = "DVB subtitles (normal) for display on a high definition monitor";
  } else if (stream_content == 0x03 and component_type >= 0x15 and component_type <= 0x1f) {
    description = "reserved for future use";
  } else if (stream_content == 0x03 and component_type >= 0x20) {
    description = "DVB subtitles (for the hard of hearing) with no monitor aspect ratio criticality ";
  } else if (stream_content == 0x03 and component_type >= 0x21) {
    description = "DVB subtitles (for the hard of hearing) for display on 4:3 aspect ratio monitor";
  } else if (stream_content == 0x03 and component_type >= 0x22) {
    description = "DVB subtitles (for the hard of hearing) for display on 16:9 aspect ratio monitor";
  } else if (stream_content == 0x03 and component_type >= 0x23) {
    description = "DVB subtitles (for the hard of hearing) for display on 2.21:1 aspect ratio monitor";
  } else if (stream_content == 0x03 and component_type >= 0x24) {
    description = "DVB subtitles (for the hard of hearing) for display on a high definition monitor";
  } else if (stream_content == 0x03 and component_type >= 0x25 and component_type <= 0x2f) {
    description = "reserved for future use";
  } else if (stream_content == 0x03 and component_type >= 0x30) {
    description = "Open (in-vision) sign language interpretation for the deaf";
  } else if (stream_content == 0x03 and component_type >= 0x31) {
    description = "Closed sign language interpretation for the deaf";
  } else if (stream_content == 0x03 and component_type >= 0x32 and component_type <= 0x3f) {
    description = "reserved for future use";
  } else if (stream_content == 0x03 and component_type >= 0x40) {
    description = "video up-sampled from standard definition source material";
  } else if (stream_content == 0x03 and component_type >= 0x41 and component_type <= 0xaf) {
    description = "reserved for future use";
  } else if (stream_content == 0x03 and component_type >= 0xb0 and component_type <= 0xfe) {
    description = "user defined";
  } else if (stream_content == 0x03 and component_type >= 0xff) {
    description = "reserved for future use";
  } else if (stream_content == 0x04 and component_type >= 0x00 and component_type <= 0x7f) {
    description = "reserved for AC-3 audio modes (refer to table D.1)";
  } else if (stream_content == 0x04 and component_type >= 0x80 and component_type <= 0xff) {
    description = "reserved for enhanced AC-3 audio modes (refer to table D.1)";
  } else if (stream_content == 0x05 and component_type == 0x00) {
    description = "reserved";
  } else if (stream_content == 0x05 and component_type == 0x01) {
    description = "H264/AVC vídeo 480i(525i), relação de aspecto 4:3";
  } else if (stream_content == 0x05 and component_type == 0x02) {
    description = "H264/AVC vídeo 480i(525i), relação de aspecto 16:9 com vetor de pan";
  } else if (stream_content == 0x05 and component_type == 0x03) {
    description = "H264/AVC vídeo 480i(525i), relação de aspecto 16:9 sem vetor de pan";
  } else if (stream_content == 0x05 and component_type == 0x04) {
    description = "H264/AVC vídeo 480i(525i), > relação de aspecto 16:9";
  } else if (stream_content == 0x05 and component_type >= 0x05 and component_type <= 0xa0) {
    description = "reserved";
  } else if (stream_content == 0x05 and component_type == 0xa1) {
    description = "H264/AVC vídeo 480p(525p), relação de aspecto 4:3";
  } else if (stream_content == 0x05 and component_type == 0xa2) {
    description = "H264/AVC vídeo 480p(525p), relação de aspecto 16:9, com vetor de pan";
  } else if (stream_content == 0x05 and component_type == 0xa3) {
    description = "H264/AVC vídeo 480p(525p), relação de aspecto 16:9, sem vetor de pan";
  } else if (stream_content == 0x05 and component_type == 0xa4) {
    description = "H264/AVC vídeo 480p(525p), > relação de aspecto 16:9";
  } else if (stream_content == 0x05 and component_type >= 0xa5 and component_type <= 0xb0) {
    description = "reserved";
  } else if (stream_content == 0x05 and component_type == 0xb1) {
    description = "H264/AVC vídeo 1080i(1125i), relação de aspecto 4:3";
  } else if (stream_content == 0x05 and component_type == 0xb2) {
    description = "H264/AVC vídeo 1080i(1125i), relação de aspecto 16:9, com vetor de pan";
  } else if (stream_content == 0x05 and component_type == 0xb3) {
    description = "H264/AVC vídeo 1080i(1125i), relação de aspecto 16:9, sem vetor de pan";
  } else if (stream_content == 0x05 and component_type == 0xb4) {
    description = "H264/AVC vídeo 1080i(1125i), > relação de aspecto 16:9";
  } else if (stream_content == 0x05 and component_type >= 0xb5 and component_type <= 0xc0) {
    description = "reserved";
  } else if (stream_content == 0x05 and component_type == 0xc1) {
    description = "H264/AVC vídeo 720p(750p), relação de aspecto 4:3";
  } else if (stream_content == 0x05 and component_type == 0xc2) {
    description = "H264/AVC vídeo 720p(750p), relação de aspecto 16:9, com vetor de pan";
  } else if (stream_content == 0x05 and component_type == 0xc3) {
    description = "H264/AVC vídeo 720p(750p), relação de aspecto 16:9, sem vetor de pan";
  } else if (stream_content == 0x05 and component_type == 0xc4) {
    description = "H264/AVC vídeo 720p(750p), > relação de aspecto 16:9";
  } else if (stream_content == 0x05 and component_type >= 0xc5 and component_type <= 0xd0) {
    description = "reserved";
  } else if (stream_content == 0x05 and component_type == 0xd1) {
    description = "H264/AVC vídeo 240p, relação de aspecto 4:3";
  } else if (stream_content == 0x05 and component_type == 0xd2) {
    description = "H264/AVC vídeo 240p, relação de aspecto 16:9 com vetor de pan";
  } else if (stream_content == 0x05 and component_type == 0xd3) {
    description = "H264/AVC vídeo 240p, relação de aspecto 16:9 sem vetor de pan";
  } else if (stream_content == 0x05 and component_type == 0xd4) {
    description = "H264/AVC vídeo 240p, > relação de aspecto 16:9";
  } else if (stream_content == 0x05 and component_type >= 0xd5 and component_type <= 0xe0) {
    description = "reserved";
  } else if (stream_content == 0x05 and component_type == 0xe1) {
    description = "H264/AVC vídeo 1080p(1125p), relação de aspecto 4:3";
  } else if (stream_content == 0x05 and component_type == 0xe2) {
    description = "H264/AVC vídeo 1080p(1125p), relação de aspecto 16:9 com vetor de pan";
  } else if (stream_content == 0x05 and component_type == 0xe3) {
    description = "H264/AVC vídeo 1080p(1125p), relação de aspecto 16:9 sem vetor de pan";
  } else if (stream_content == 0x05 and component_type == 0xe4) {
    description = "H264/AVC vídeo 1080p(1125p), > relação de aspecto 16:9";
  } else if (stream_content == 0x05 and component_type >= 0xe5 and component_type <= 0xff) {
    description = "reserved";
  } else if (stream_content == 0x06 and component_type == 0x00) {
    description = "reserved";
  } else if (stream_content == 0x06 and component_type == 0x01) {
    description = "HE-AAC MPEG4 áudio, modo 1/0 (single mono)";
  } else if (stream_content == 0x06 and component_type == 0x02) {
    description = "HE-AAC MPEG4 áudio, modo 1/0 + 1/0 (dual mono)";
  } else if (stream_content == 0x06 and component_type == 0x03) {
    description = "HE-AAC MPEG4 áudio, modo 2/0 (estéreo)";
  } else if (stream_content == 0x06 and component_type == 0x04) {
    description = "HE-AAC MPEG4 áudio, modo 2/1";
  } else if (stream_content == 0x06 and component_type == 0x05) {
    description = "HE-AAC MPEG4 áudio, modo 3/0";
  } else if (stream_content == 0x06 and component_type == 0x06) {
    description = "HE-AAC MPEG4 áudio, modo 2/2";
  } else if (stream_content == 0x06 and component_type == 0x07) {
    description = "HE-AAC MPEG4 áudio, modo 3/1";
  } else if (stream_content == 0x06 and component_type == 0x08) {
    description = "HE-AAC MPEG4 áudio, modo 3/2";
  } else if (stream_content == 0x06 and component_type == 0x09) {
    description = "HE-AAC MPEG4 áudio, modo 3/2 + LFE";
  } else if (stream_content == 0x06 and component_type >= 0x0a and component_type <= 0x3f) {
    description = "reserved";
  } else if (stream_content == 0x06 and component_type == 0x40) {
    description = "HE-AAC MPEG4 descrição de pure audio para deficientes visuais";
  } else if (stream_content == 0x06 and component_type == 0x41) {
    description = "HE-AAC MPEG4 áudio com áudio elevado para deficientes auditivos";
  } else if (stream_content == 0x06 and component_type == 0x42) {
    description = "HE-AAC MPEG4 descrição de mixed audio para deficientes visuais";
  } else if (stream_content == 0x06 and component_type == 0x43) {
    description = "HE-AAC v2 MPEG4 áudio, modo 1/0 (mono)";
  } else if (stream_content == 0x06 and component_type == 0x44) {
    description = "HE-AAC v2 MPEG4 áudio, modo 2/0 (estéreo)";
  } else if (stream_content == 0x06 and component_type == 0x45) {
    description = "HE-AAC v2 MPEG4 descrição de pure audio para deficientes visuais";
  } else if (stream_content == 0x06 and component_type == 0x46) {
    description = "HE-AAC MPEG4 v2 áudio com áudio elevado para deficientes auditivos";
  } else if (stream_content == 0x06 and component_type == 0x47) {
    description = "HE-AAC MPEG4 v2 descrição de mixed audio para deficientes visuais";
  } else if (stream_content == 0x06 and component_type >= 0x48 and component_type <= 0x50) {
    description = "reserved";
  } else if (stream_content == 0x06 and component_type == 0x51) {
    description = "AAC MPEG4 áudio, modo 1/0 (single mono)";
  } else if (stream_content == 0x06 and component_type == 0x52) {
    description = "AAC MPEG4 áudio, modo 1/0 + 1/0 (dual mono)";
  } else if (stream_content == 0x06 and component_type == 0x53) {
    description = "AAC MPEG4 áudio, modo 2/0 (estéreo)";
  } else if (stream_content == 0x06 and component_type == 0x54) {
    description = "AAC MPEG4 áudio, modo 2/1";
  } else if (stream_content == 0x06 and component_type == 0x55) {
    description = "AAC MPEG4 áudio, modo 3/0";
  } else if (stream_content == 0x06 and component_type == 0x56) {
    description = "AAC MPEG4 áudio, modo 2/2";
  } else if (stream_content == 0x06 and component_type == 0x57) {
    description = "AAC MPEG4 áudio, modo 3/1";
  } else if (stream_content == 0x06 and component_type == 0x58) {
    description = "AAC MPEG4 áudio, modo 3/2";
  } else if (stream_content == 0x06 and component_type == 0x59) {
    description = "AAC MPEG4 áudio, modo 3/2 + LFE";
  } else if (stream_content == 0x06 and component_type >= 0x60 and component_type <= 0x9e) {
    description = "reserved";
  } else if (stream_content == 0x06 and component_type == 0x9f) {
    description = "AAC MPEG4 descrição de pure audio para deficientes visuais";
  } else if (stream_content == 0x06 and component_type == 0xa0) {
    description = "AAC MPEG4 áudio com áudio elevado para deficientes auditivos";
  } else if (stream_content == 0x06 and component_type == 0xa1) {
    description = "AAC MPEG4 descrição de mixed audio para deficientes visuais";
  } else if (stream_content == 0x06 and component_type >= 0xa2 and component_type <= 0xaf) {
    description = "reserved";
  } else if (stream_content == 0x06 and component_type >= 0xb0 and component_type <= 0xfe) {
    description = "user defined";
  } else if (stream_content == 0x06 and component_type == 0xff) {
    description = "reserved";
  } else if (stream_content == 0x06 and component_type >= 0x00 and component_type <= 0xff) {
    description = "reserved";
  } else if (stream_content == 0x07 and component_type >= 0x00 and component_type <= 0xff) {
    description = "reserved";
  } else if (stream_content == 0x08 and component_type >= 0x00 and component_type <= 0xff) {
    description = "reserved";
  } else if (stream_content == 0x09 and component_type >= 0x00 and component_type <= 0xff) {
    description = "reserved";
  } else if (stream_content == 0x0a and component_type >= 0x00 and component_type <= 0xff) {
    description = "reserved";
  } else if (stream_content == 0x0b and component_type >= 0x00 and component_type <= 0xff) {
    description = "reserved";
  } else if (stream_content == 0x0c and component_type >= 0x00 and component_type <= 0xff) {
    description = "reserved";
  } else if (stream_content == 0x0d and component_type >= 0x00 and component_type <= 0xff) {
    description = "reserved";
  } else if (stream_content == 0x0e and component_type >= 0x00 and component_type <= 0xff) {
    description = "reserved";
  } else if (stream_content == 0x0f and component_type >= 0x00 and component_type <= 0xff) {
    description = "reserved";
  }

  return description;
}

std::string GetServiceDescription(int service_type)
{
  std::string description;

  if (service_type == 0x00) {
    description = "Reservado para uso futuro";
  } else if (service_type == 0x01) {
    description = "Serviço de televisão digital HD";
    // description = "Serviço de televisão digital";
  } else if (service_type == 0x02) {
    description = "Serviço de áudio digital";
  } else if (service_type == 0x03) {
    description = "Serviço de teletexto";
  } else if (service_type == 0x04) {
    description = "Serviço de referência NVOD";
  } else if (service_type == 0x05) {
    description = "Serviço time-shifted NVOD";
  } else if (service_type == 0x06) {
    description = "Serviço de mosaico";
  } else if (service_type >= 0x07 and service_type <= 0x09) {
    description = "Reservado para uso futuro";
  } else if (service_type == 0x0a) {
    description = "Codificação avançada para serviço de rádio digital";
  } else if (service_type == 0x0b) {
    description = "Codificação avançada para serviço de mosaico";
  } else if (service_type == 0x0c) {
    description = "Serviço de transmissão de dados";
  } else if (service_type == 0x0d) {
    description = "Reservado para interface de uso comum (ver EN 50221)";
  } else if (service_type == 0x0e) {
    description = "RCS Map (ver EN 301 790)";
  } else if (service_type == 0x0f) {
    description = "RCS FLS (ver EN 301 790)";
  } else if (service_type == 0x10) {
    description = "Serviço DVB MHP";
  } else if (service_type == 0x11) {
    description = "Serviço de televisão digital MPEG-2 HD";
  } else if (service_type >= 0x12 and service_type <= 0x15) {
    description = "Reservado para uso futuro";
  } else if (service_type == 0x16) {
    description = "Codificação avançada de serviço de televisão digital SD";
  } else if (service_type == 0x17) {
    description = "Codificação avançada de serviço de NVOD SD time-shifted";
  } else if (service_type == 0x18) {
    description = "Codificação avançada de serviço de referência NVOD SD";
  } else if (service_type == 0x19) {
    description = "Codificação avançada de serviço de televisão digital HD";
  } else if (service_type == 0x1a) {
    description = "Codificação avançada de serviço de NVOD HD time-shifted";
  } else if (service_type == 0x1b) {
    description = "Codificação avançada de serviço de referência NVOD HD";
  } else if (service_type >= 0x1c and service_type < 0x7f) {
    description = "Reservado para uso futuro";
  } else if (service_type >= 0x80 and service_type <= 0xa0) {
    description = "Definido pelo provedor de serviço";
  } else if (service_type == 0xa1) {
    description = "Serviço especial de vídeo";
  } else if (service_type == 0xa2) {
    description = "Serviço especial de áudio";
  } else if (service_type == 0xa3) {
    description = "Serviço especial de dados";
  } else if (service_type == 0xa4) {
    description = "Serviço de engenharia";
  } else if (service_type == 0xa5) {
    description = "Serviço promocional de vídeo";
  } else if (service_type == 0xa6) {
    description = "Serviço promocional de áudio";
  } else if (service_type == 0xa7) {
    description = "Serviço promocional de dados";
  } else if (service_type == 0xa8) {
    description = "Serviço de dados para armazenamento antecipado";
  } else if (service_type == 0xa9) {
    description = "Serviço de dados exclusivo para armazenamento";
  } else if (service_type == 0xaa) {
    description = "Lista de serviços de bookmark";
  } else if (service_type == 0xab) {
    description = "Serviço simultâneo do tipo servidor";
  } else if (service_type == 0xac) {
    description = "Serviço independente de arquivos";
  } else if (service_type >= 0xad and service_type <= 0xbf) {
    description = "Não definido (área definida pela organização de regulamentação)";
  } else if (service_type == 0xc0) {
    description = "Serviço de televisão digital LD";
    // description = "Serviço de dados";
  } else if (service_type >= 0xc1 and service_type <= 0xff) {
    description = "Não definido";
  }

  return description;
}

std::string GetDescriptorName(int descriptor_tag)
{
	/*
	switch (descriptor_tag) {
		// INFO:: T-REC-H.222.0-200605-PDF-E.pdf
		case 0x00: return "reserved";
		case 0x01: return "reserved";
		case 0x02: return "video_stream_descriptor";
		case 0x03: return "audio_stream_descriptor";
		case 0x04: return "hierarchy_descriptor";
		case 0x05: return "registration_descriptor";
		case 0x06: return "data_stream_alignment_descriptor";
		case 0x07: return "target_background_grid_descriptor";
		case 0x08: return "video_window_descriptor";
		case 0x09: return "ca_descriptor";
		case 0x0a: return "iso_639_language_descriptor";
		case 0x0b: return "system_clock_descriptor";
		case 0x0c: return "multiplex_buffer_utilization_descriptor";
		case 0x0d: return "copyright_descriptor";
		case 0x0e: return "maximum_bitrate_descriptor";
		case 0x0f: return "private_data_indicator_descriptor";
		case 0x10: return "smoothing_buffer_descriptor";
		case 0x11: return "std_descriptor";
		case 0x12: return "ibp_descriptor";
		case 0x13: 
		case 0x14: 
		case 0x15: 
		case 0x16: 
		case 0x17: 
		case 0x18: 
		case 0x19: 
		case 0x1a: return "Defined in ISO/IEC 13818-6";
		case 0x1b: return "mpeg-4_video_descriptor";
		case 0x1c: return "mpeg-4_audio_descriptor";
		case 0x1d: return "iod_descriptor";
		case 0x1e: return "sl_descriptor";
		case 0x1f: return "fmc_descriptor";
		case 0x20: return "external_es_id_descriptor";
		case 0x21: return "muxcode_descriptor";
		case 0x22: return "fmxbuffersize_descriptor";
		case 0x23: return "multiplex_buffer_descriptor";
		case 0x24: return "content_labeling_descriptor";
		case 0x25: return "metadata_pointer_descriptor";
		case 0x26: return "metadata_descriptor";
		case 0x27: return "metadata_std_descriptor";
		case 0x28: return "avc_video_descriptor";
		case 0x29: return "ipmp_descriptor";
		case 0x2a: return "avc_timing_descriptor";
		case 0x2b: return "mpeg-2_aac_audio_descriptor";
		case 0x2c: return "flexmuxtiming_descriptor";
		default:
							 break;
	}
	*/

	switch (descriptor_tag) {
		// INFO:: ABNTNBR15606-3_2007Vc_2008.pdf
		// INFO:: ABNTNBR15603-1_2007Vc_2008.pdf
		case 0x06: return "location_descriptor";
		case 0x09: return "conditional_access_descriptor";
		case 0x0a: return "iso_639_language_descriptor";
		case 0x0d: return "copyright_descriptor";
		case 0x11: return "ip_signalling_descriptor";
		case 0x13: return "carousel_identifier_descriptor";
		case 0x14: return "association_tag_descriptor";
		case 0x15: return "extension_tag_descriptor"; // deferred_association_tags_descriptor
		case 0x28: return "avc_video_descriptor";
		case 0x2a: return "avc_timing_and_hrd_descriptor";
		case 0xc0: return "hierarchical_transmission_descriptor";
		case 0xc1: return "digital_copy_control_descriptor";
		case 0xc2: return "network_identifier_descriptor";
		case 0xc3: return "partial_transport_stream_time_descriptor";
		case 0xc4: return "audio_component_descriptor";
		case 0xc5: return "hyperlink_descriptor";
		case 0xc6: return "target_area_descriptor";
		case 0xc7: return "data_content_descriptor";
		case 0xc8: return "video_decode_control_descriptor";
		case 0xc9: return "download_content_descriptor";
		case 0xca: return "ca_emm_ts_descriptor";
		case 0xcb: return "ca_contract_information_descriptor";
		case 0xcc: return "ca_service_descriptor";
		case 0xcd: return "ts_information_descriptor";
		case 0xce: return "extender_broadcaster_descriptor";
		case 0xcf: return "logo_transmission_descriptor";
		case 0xd0: return "basic_local_event_descriptor";
		case 0xd1: return "reference_descriptor";
		case 0xd2: return "node_relation_descriptor";
		case 0xd3: return "short_node_information_descriptor";
		case 0xd4: return "stc_reference_descriptor";
		case 0xd5: return "series_descriptor";
		case 0xd6: return "event_group_descriptor";
		case 0xd7: return "si_parameter_descriptor";
		case 0xd8: return "broadcaster_name_descriptor";
		case 0xd9: return "component_group_descriptor";
		case 0xda: return "si_prime_ts_descriptor";
		case 0xdb: return "board_information_descriptor";
		case 0xdc: return "ldt_linkage_descriptor";
		case 0xdd: return "connected_transmission_descriptor";
		case 0xde: return "content_availability_descriptor";
		case 0xe0: return "service_group_descriptor";
		case 0xf7: return "carousel_compatible_composited_descriptor";
		case 0xf8: return "conditional_playback_descriptor";
		case 0xfa: return "terrestrial_delivery_system_descriptor";
		case 0xfb: return "partial_reception_descriptor";
		case 0xfc: return "emergency_information_descriptor";
		case 0xfd: return "data_component_descriptor";
		case 0xfe: return "system_management_descriptor";
		default:
							 break;
	}

	switch (descriptor_tag) {
		// INFO:: DVB (en_300468v011101p.pdf)
		case 0x40: return "network_name_descriptor";
		case 0x41: return "service_list_descriptor";
		case 0x42: return "stuffing_descriptor";
		case 0x43: return "satellite_delivery_system_descriptor";
		case 0x44: return "cable_delivery_system_descriptor";
		case 0x45: return "VBI_data_descriptor";
		case 0x46: return "VBI_teletext_descriptor";
		case 0x47: return "bouquet_name_descriptor";
		case 0x48: return "service_descriptor";
		case 0x49: return "country_availability_descriptor";
		case 0x4a: return "linkage_descriptor";
		case 0x4b: return "NVOD_reference_descriptor";
		case 0x4c: return "time_shifted_service_descriptor";
		case 0x4d: return "short_event_descriptor";
		case 0x4e: return "extended_event_descriptor";
		case 0x4f: return "time_shifted_event_descriptor";
		case 0x50: return "component_descriptor";
		case 0x51: return "mosaic_descriptor";
		case 0x52: return "stream_identifier_descriptor";
		case 0x53: return "CA_identifier_descriptor";
		case 0x54: return "content_descriptor";
		case 0x55: return "parental_rating_descriptor";
		case 0x56: return "teletext_descriptor";
		case 0x57: return "telephone_descriptor";
		case 0x58: return "local_time_offset_descriptor";
		case 0x59: return "subtitling_descriptor";
		case 0x5a: return "terrestrial_delivery_system_descriptor";
		case 0x5b: return "multilingual_network_name_descriptor";
		case 0x5c: return "multilingual_bouquet_name_descriptor";
		case 0x5d: return "multilingual_service_name_descriptor";
		case 0x5e: return "multilingual_component_descriptor";
		case 0x5f: return "private_data_specifier_descriptor";
		case 0x60: return "service_move_descriptor";
		case 0x61: return "short_smoothing_buffer_descriptor";
		case 0x62: return "frequency_list_descriptor";
		case 0x63: return "partial_transport_stream_descriptor";
		case 0x64: return "data_broadcast_descriptor";
		case 0x65: return "scrambling_descriptor";
		case 0x66: return "data_broadcast_id_descriptor";
		case 0x67: return "transport_stream_descriptor";
		case 0x68: return "DSNG_descriptor";
		case 0x69: return "PDC_descriptor";
		case 0x6a: return "AC-3_descriptor";
		case 0x6b: return "ancillary_data_descriptor";
		case 0x6c: return "cell_list_descriptor";
		case 0x6d: return "cell_frequency_link_descriptor";
		case 0x6e: return "announcement_support_descriptor";
		case 0x6f: return "application_signalling_descriptor";
		case 0x70: return "adaptation_field_data_descriptor";
		case 0x71: return "service_identifier_descriptor";
		case 0x72: return "service_availability_descriptor";
		case 0x73: return "default_authority_descriptor";
		case 0x74: return "related_content_descriptor";
		case 0x75: return "TVA_id_descriptor";
		case 0x76: return "content_identifier_descriptor";
		case 0x77: return "time_slice_fec_identifier_descriptor";
		case 0x78: return "ECM_repetition_rate_descriptor";
		case 0x79: return "S2_satellite_delivery_system_descriptor";
		case 0x7a: return "enhanced_AC-3_descriptor";
		case 0x7b: return "DTS_descriptor";
		case 0x7c: return "AAC)descriptor";
		case 0x7d: return "XAIT)location_descriptor";
		case 0x7e: return "FTA_content_management_descriptor";
		case 0x7f: return "extension_descriptor";
		case 0xff: return "forbidden";
		default: 
							 break;
	}

	if (descriptor_tag >= 0x37 && descriptor_tag <= 0x3f) {
		return "reserved";
	} else if (descriptor_tag >= 0x80 && descriptor_tag <= 0xcf) {
    if (descriptor_tag == 0xa3) {
			return "component name descriptor - ATSC A/65A, ATSC Working Draft";
    }

		return "atsc descriptor";
	} else if (descriptor_tag >= 0xd0 && descriptor_tag <= 0xdf) {
		return "isdb descriptor";
	} else if (descriptor_tag >= 0xe0 && descriptor_tag <= 0xe9) {
		return "cablelabs descriptor";
	}

	return "unknown descriptor";
}

void DumpBytes(std::string id, const char *data, int length, int columns)
{
  if (length == 0) {
    return;
  }

	printf("----:: Dump (%s) <%d bytes> :: BEGIN ::----\n", id.c_str(), length);

	const char *ptr = data;

	if (columns < 8) {
		columns = 8;
	}

	while (length > 0) {
		for (int i=0; i<columns && i<length; i++) {
			printf("%02x ", (uint8_t)ptr[i]);
		}

		if (length < columns) {
			for (int i=0; i<(columns-length); i++) {
				printf("   ");
			}
		}

		for (int i=0; i<columns && i<length; i++) {
			if (isprint(ptr[i])) {
				printf("%c", ptr[i]);
			} else {
				printf(".");
			}
		}
		
		length = length - columns;
		ptr = ptr + columns;
		
		if (length > 0) {
			printf("\n");
		}
	}
	
	printf("\n----:: Dump (%s) :: END ::----\n", id.c_str());
}

int main_mpeg(int argc, char *argv[])
{
	return EXIT_SUCCESS;
}

