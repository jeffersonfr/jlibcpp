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
  } else { // INFO:: considering only table id information
    if (tid == TS_PAT_TABLE_ID) {
			return "Program Association Table";
    } else if (tid == TS_CAT_TABLE_ID) {
			return "Condicional Access Table";
    } else if (tid == TS_PMT_TABLE_ID) {
			return "Program Map Table";
    } else if (tid == TS_TSDT_TABLE_ID) {
			return "Transport Stream Description";
    } else if (tid == TS_METADATA_TABLE_ID) {
			return "Metadata Table";
    } else if (tid == TS_NIT_TABLE_ID) {
			return "Network Information Table";
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
			case 0x05: return "H.222.0/13818-1  private sections";
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
		case 0x09: return "conditional_access_descriptor";
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
		case 0xc7: return "data_contents_descriptor";
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
		case 0x7b: return "DTS descriptor";
		case 0x7c: return "AAC descriptor";
		case 0x7d: return "XAIT location descriptor";
		case 0x7e: return "FTA_content_management_descriptor";
		case 0x7f: return "extension descriptor";
		case 0xff: return "forbidden";
		default: 
							 break;
	}

	if (descriptor_tag >= 0x37 && descriptor_tag <= 0x3f) {
		return "reserved";
	} else if (descriptor_tag >= 0x80 && descriptor_tag <= 0xcf) {
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
	printf("----:: Dump (%s) :: BEGIN ::----\n", id.c_str());

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

