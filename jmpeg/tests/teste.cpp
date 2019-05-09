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
#include "jmpeg/jdemuxmanager.h"
#include "jmpeg/jpsidemux.h"
#include "jmpeg/jpesdemux.h"
#include "jmpeg/jrawdemux.h"
#include "jmpeg/jprivatedemux.h"
#include "jmpeg/jmpeglib.h"
#include "jnetwork/jdatagramsocket.h"
#include "jnetwork/jurl.h"
#include "jio/jfileinputstream.h"
#include "jevent/jdemuxlistener.h"
#include "jlogger/jloggerlib.h"
#include "jmath/jcrc.h"
#include "jcommon/jparammapper.h"
#include "jcommon/jstringtokenizer.h"
#include "jexception/joverflowexception.h"
#include "jexception/joutofboundsexception.h"

#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <tuple>
#include <utility>
#include <sstream>
#include <iomanip>

#include <string.h>
#include <math.h>
#include <fcntl.h>

#define TS_AIT_TABLE_ID 0x74

#define TS_PAT_TIMEOUT  2000
#define TS_BAT_TIMEOUT  4000
#define TS_CAT_TIMEOUT  4000
#define TS_TSDT_TIMEOUT 4000
#define TS_PMT_TIMEOUT  4000
#define TS_NIT_TIMEOUT  4000
#define TS_SDT_TIMEOUT  2000
#define TS_TDT_TIMEOUT  6000
#define TS_RST_TIMEOUT  4000
#define TS_EIT_TIMEOUT  4000
#define TS_PRIVATE_TIMEOUT  10000
#define TS_PCR_TIMEOUT  1000

#define DHEX2DEC(value) ((((value & 0xf0) >> 4) * 10) + (value & 0x0f))

class Utils {

  public:
    static std::string ISO8859_1_TO_UTF8(std::string str)
    {
      const char *str_c_str = str.c_str();
      char *utf8 = new char[2*str.size()];
      int k = 0;

      for (int i=0; i<(int)str.size(); i++) {
        uint8_t c = (uint8_t)str_c_str[i];

        if (c >= 0x80 && c <= 0xbf) {
          utf8[k++] = 0xc2;
          utf8[k++] = c;
        } else if (c >= 0xc0 && c <= 0xff) {
          utf8[k++] = 0xc3;
          utf8[k++] = c - 0x40;
        } else {
          if (c >= 0x20) { // INFO:: accept only valid characters
            utf8[k++] = c;
          }
        }
      }

      utf8[k++] = 0;

      std::string s = std::string(utf8);

      delete [] utf8;

      return s;
    }

    static bool IsInvalidChar(char c)
    {
      switch (c) {
        case 0x0e:
          return true;
        default:
          break;
      }

      return false;
    }

    static std::string RemoveInvalidChars(std::string str)
    {
      str.erase(std::remove_if(str.begin(), str.end(), &IsInvalidChar), str.end());

      return str;
    }
    
    static void ParseJulianDate(int MJD, int &Y, int &M, int &D, int &WD)
    {
      int K = 0;

      Y = (int)(((double)MJD - 15078.2)/365.25);
      M = (int)(((double)MJD - 14956.1 - (int)((double)Y * 365.25))/30.6001);
      D = (int)(MJD - 14956 - int((double)Y * 365.25) - int((double)M * 30.6001));

      if (M == 14 || M == 15) {
        K = 1;
      }

      Y = Y + K + 1900;
      M = M - 1 - K * 12;

      WD = ((MJD + 2) % 7) + 1;
    }
    
    static std::string GetParentalAgeDescription(int age)
    {
      std::string info = "not defined";

      if (age == 0x01) {
        info = "L";
      } else if (age == 0x02) {
        info = "10";
      } else if (age == 0x03) {
        info = "12";
      } else if (age == 0x04) {
        info = "14";
      } else if (age == 0x05) {
        info = "16";
      } else if (age == 0x06) {
        info = "18";
      }

      return info;
    }

    static std::string GetParentalContentDescription(int content)
    {
      std::string info;

      if (content == 0x01) {
        info = "drugs";
      } else if (content == 0x02) {
        info = "violence";
      } else if (content == 0x03) {
        info = "violence and drugs";
      } else if (content == 0x04) {
        info = "sex";
      } else if (content == 0x05) {
        info = "sex and drugs";
      } else if (content == 0x06) {
        info = "violence and sex";
      } else if (content == 0x07) {
        info = "violence, sex and drugs";
      }

      return info;
    }

    static std::string GetLanguageCompressionDescription(int compression)
    {
      std::string info = "no compression";

      if (compression == 0x00) {
        info = "no compression";
      } else if (compression == 0x01) {
        info = "huffman coding";
      } else if (compression == 0x02) {
        info = "huffman coding";
      } else if (compression >= 0x03 and compression <= 0xaf) {
        info = "reserved";
      } else if (compression >= 0xb0 and compression <= 0xff) {
        info = "used in other systems";
      }

      return info;
    }

    static std::string GetLanguageUnicodeModeDescription(int mode)
    {
      std::string info = "Undefined";

      if (mode == 0x00) {
        info = "select unicode code range 0x0000 - 0x00FF";
      } else if (mode == 0x01) {
        info = "select unicode code range 0x0100 - 0x01FF";
      } else if (mode == 0x02) {
        info = "select unicode code range 0x0200 - 0x02FF";
      } else if (mode == 0x03) {
        info = "select unicode code range 0x0300 - 0x03FF";
      } else if (mode == 0x04) {
        info = "select unicode code range 0x0400 - 0x04FF";
      } else if (mode == 0x05) {
        info = "select unicode code range 0x0500 - 0x05FF";
      } else if (mode == 0x06) {
        info = "select unicode code range 0x0600 - 0x06FF";
      } else if (mode >= 0x07 and mode <= 0x08) {
        info = "reserved";
      } else if (mode == 0x09) {
        info = "select unicode code range 0x0900 - 0x09FF";
      } else if (mode == 0x0a) {
        info = "select unicode code range 0x0A00 - 0x0AFF";
      } else if (mode == 0x0b) {
        info = "select unicode code range 0x0B00 - 0x0BFF";
      } else if (mode == 0x0c) {
        info = "select unicode code range 0x0C00 - 0x0CFF";
      } else if (mode == 0x0d) {
        info = "select unicode code range 0x0D00 - 0x0DFF";
      } else if (mode == 0x0e) {
        info = "select unicode code range 0x0E00 - 0x0EFF";
      } else if (mode == 0x0f) {
        info = "select unicode code range 0x0F00 - 0x0FFF";
      } else if (mode == 0x10) {
        info = "select unicode code range 0x1000 - 0x10FF";
      } else if (mode >= 0x11 and mode <= 0x1f) {
        info = "reserved";
      } else if (mode == 0x20) {
        info = "select unicode code range 0x2000 - 0x20FF";
      } else if (mode == 0x21) {
        info = "select unicode code range 0x2100 - 0x21FF";
      } else if (mode == 0x22) {
        info = "select unicode code range 0x2200 - 0x22FF";
      } else if (mode == 0x23) {
        info = "select unicode code range 0x2300 - 0x23FF";
      } else if (mode == 0x24) {
        info = "select unicode code range 0x2400 - 0x24FF";
      } else if (mode == 0x25) {
        info = "select unicode code range 0x2500 - 0x25FF";
      } else if (mode == 0x26) {
        info = "select unicode code range 0x2600 - 0x26FF";
      } else if (mode == 0x27) {
        info = "select unicode code range 0x2700 - 0x27FF";
      } else if (mode >= 0x28 and mode <= 0x2f) {
        info = "reserved";
      } else if (mode == 0x30) {
        info = "select unicode code range 0x3000 - 0x30FF";
      } else if (mode == 0x31) {
        info = "select unicode code range 0x3100 - 0x31FF";
      } else if (mode == 0x32) {
        info = "select unicode code range 0x3200 - 0x32FF";
      } else if (mode == 0x33) {
        info = "select unicode code range 0x3300 - 0x33FF";
      } else if (mode >= 0x34 and mode <= 0x3d) {
        info = "reserved";
      } else if (mode == 0x3e) {
        info = "select Standard Compression Scheme for Unicode (SCSU)";
      } else if (mode == 0x3f) {
        info = "select Unicode, UTF-16 form";
      } else if (mode >= 0x40 and mode <= 0x41) {
        info = "assigned to ATSC standard for Taiwan";
      } else if (mode >= 0x42 and mode <= 0x47) {
        info = "reserved for future ATSC use";
      } else if (mode == 0x48) {
        info = "assigned to ATSC standard for South Korea";
      } else if (mode >= 0x49 and mode <= 0xdf) {
        info = "reserved for future ATSC use";
      } else if (mode >= 0xe0 and mode <= 0xfe) {
        info = "used in other systems";
      } else if (mode == 0xff) {
        info = "not applicable";
      }

      return info;
    }

    static std::string GetTransportProtocolDescription(int protocol)
    {
      std::string info = "UNKNOWN";

      if (protocol == 0x01) {
        info = "Object Carousel";
      } else if (protocol == 0x02) {
        info = "IP";
      } else if (protocol == 0x03) {
        info = "IChannel";
      } else if (protocol == 0x04) {
        info = "Data Carousel";
      } else if (protocol == 0x05) {
        info = "Persistence Carousel";
      }

      return info;
    }

    static std::string GetBIOPTypeInfo(std::string type) 
    {
      std::string info("unknown");

      if (type.find("fil") != std::string::npos) {
        info = "DSM::File";
      } else if (type.find("srg") != std::string::npos) {
        info = "DSM::ServiceGateway";
      } else if (type.find("dir") != std::string::npos) {
        info = "DSM::Directory";
      } else if (type.find("str") != std::string::npos) {
        info = "DSM::Stream";
      } else if (type.find("ste") != std::string::npos) {
        info = "BIOP::StreamEvent";
      }

      return info;
    }

    static std::string GetTapUseDescription(int use)
    {
      std::string info = "UNKNOWN";

      if (use == 0x00) {
        info = "UKNOWN_USE";
      } else if (use == 0x01) {
        info = "MPEG_TS_UP_USE";
      } else if (use == 0x02) {
        info = "MPEG_TS_DOWN_USE";
      } else if (use == 0x03) {
        info = "MPEG_ES_UP_USE";
      } else if (use == 0x04) {
        info = "MPEG_ES_DOWN_USE";
      } else if (use == 0x05) {
        info = "DOWNLOAD_CTRL_USE";
      } else if (use == 0x06) {
        info = "DOWNLOAD_CTRL_UP_USE";
      } else if (use == 0x07) {
        info = "DOWNLOAD_CTRL_DOWN_USE";
      } else if (use == 0x08) {
        info = "DOWNLOAD_DATA_USE";
      } else if (use == 0x09) {
        info = "DOWNLOAD_DATA_UP_USE";
      } else if (use == 0x0a) {
        info = "DOWNLOAD_DATA_DOWN_USE";
      } else if (use == 0x0b) {
        info = "STREAM_NPT_USE";
      } else if (use == 0x0c) {
        info = "STREAM_STATUS_AND_EVENT_USE";
      } else if (use == 0x0d) {
        info = "STREAM_EVENT_USE";
      } else if (use == 0x0e) {
        info = "STREAM_STATUS_USE";
      } else if (use == 0x0f) {
        info = "RPC_USE";
      } else if (use == 0x10) {
        info = "IP_USE";
      } else if (use == 0x11) {
        info = "SDB_CTRL_USE";
      } else if (use == 0x12) {
        info = "T120_TAP1";
      } else if (use == 0x13) {
        info = "T120_TAP2";
      } else if (use == 0x14) {
        info = "T120_TAP3";
      } else if (use == 0x15) {
        info = "T120_TAP4";
      } else if (use == 0x16) {
        info = "BIOP_DELIVERY_PARA_USE";
      } else if (use == 0x17) {
        info = "BIOP_OBJECT_USE";
      } else if (use == 0x18) {
        info = "BIOP_ES_USE";
      } else if (use == 0x19) {
        info = "BIOP_PROGRAM_USE";
      } else if (use == 0x1a) {
        info = "BIOP_DNL_CTRL_USE";
      }

      return info;
    }

    static std::string GetRunningStatusDescription(int status) 
    {
      if (status == 0x00) {
        return "undefined";
      } else if (status == 0x01) {
        return "not running";
      } else if (status == 0x02) {
        return "starts in a few seconds";
      } else if (status == 0x03) {
        return "pausing";
      } else if (status == 0x04) {
        return "running";
      } else if (status == 0x05) {
        return "reserved";
      } else if (status == 0x06) {
        return "reserved";
      } else if (status == 0x07) {
        return "reserved";
      }

      return "undefined";
    }

    static std::string GetAITDescriptorName(int descriptor_tag)
    {
      switch (descriptor_tag) {
        // INFO:: ABNTNBR15606-3 (2012).pdf
        case 0x00: return "application_descriptor";
        case 0x01: return "application_name_descriptor";
        case 0x02: return "transport_protocol_descriptor";
        case 0x03: return "gingaj_application_descriptor";
        case 0x04: return "gingaj_application_location_descriptor";
        case 0x05: return "external_application_authorization_descriptor";
        case 0x06: return "gingancl_application_descriptor";
        case 0x07: return "gingancl_application_location_descriptor";
        case 0x08: 
        case 0x09: 
        case 0x0a: return "(NCL) reserved to the future";
        case 0x0b: return "application_icons_descriptor";
        case 0x0c: return "prefetch_descriptor";
        case 0x0d: return "dii_location_descriptor";
        case 0x11: return "ip_signalling_descriptor";
        case 0x5f: return "private_data_specifier_descriptor";
        case 0xfd: return "data_coding_descriptor";
        default:
                   break;
      }

      return "unknown descriptor";
    }

    static std::string GetAACProfileAndLevelDescription(int profile_and_level)
    {
      std::string info;

      if (profile_and_level >= 0x00 and profile_and_level <= 0x27) {
        info = "reserved";
      } else if (profile_and_level == 0x28) {
        info = "AAC Profile";
      } else if (profile_and_level == 0x29) {
        info = "AAC Profile";
      } else if (profile_and_level == 0x2a) {
        info = "AAC Profile";
      } else if (profile_and_level == 0x2b) {
        info = "AAC Profile";
      } else if (profile_and_level == 0x2c) {
        info = "High Efficiency AAC Profile";
      } else if (profile_and_level == 0x2d) {
        info = "High Efficiency AAC Profile";
      } else if (profile_and_level == 0x2e) {
        info = "High Efficiency AAC Profile";
      } else if (profile_and_level == 0x2f) {
        info = "High Efficiency AAC Profile";
      } else if (profile_and_level >= 0x30 and profile_and_level <= 0x7f) {
        info = "Reservado para uso da ISO";
      } else if (profile_and_level >= 0x80 and profile_and_level <= 0xfd) {
        info = "Private";
      } else if (profile_and_level == 0xfe) {
        info = "No specified";
      } else if (profile_and_level == 0xff) {
        info = "None information about audio";
      }

      return info;
    }

    static std::string GetGenreDescription(int genre, int description)
    {
      std::string info = "Geral";

      if (genre == 0x00) {
        if (description == 0x00) {
          info = "Notícias";
        } else if (description == 0x01) {
          info = "Reportagem";
        } else if (description == 0x02) {
          info = "Documentário";
        } else if (description == 0x03) {
          info = "Biografia";
        }
      } else if (genre == 0x01) {
        if (description == 0x01) {
          info = "Esportes";
        } else {
          info = "Esportes";
        }
      } else if (genre == 0x02) {
        if (description == 0x00) {
          info = "Educativo";
        } else {
          info = "Educativo";
        }
      } else if (genre == 0x03) {
        if (description == 0x00) {
          info = "Novela";
        } else {
          info = "Novela";
        }
      } else if (genre == 0x04) {
        if (description == 0x00) {
          info = "Mini Serie";
        } else {
          info = "Mini Serie";
        }
      } else if (genre == 0x05) {
        if (description == 0x00) {
          info = "Series";
        } else {
          info = "Series";
        }
      } else if (genre == 0x06) {
        if (description == 0x00) {
          info = "Auditório";
        } else if (description == 0x01) {
          info = "Show";
        } else if (description == 0x02) {
          info = "Musical";
        } else if (description == 0x03) {
          info = "Making Of";
        } else if (description == 0x04) {
          info = "Feminino";
        } else if (description == 0x05) {
          info = "Entreterimento";
        } else {
          info = "Variedades";
        }
      } else if (genre == 0x07) {
        if (description == 0x00) {
          info = "Reality Show";
        } else {
          info = "Reality Show";
        }
      } else if (genre == 0x08) {
        if (description == 0x00) {
          info = "Cozinha";
        } else if (description == 0x01) {
          info = "Fashion";
        } else if (description == 0x02) {
          info = "Regional";
        } else if (description == 0x03) {
          info = "Saúde";
        } else if (description == 0x04) {
          info = "Viagem";
        } else {
          info = "Informação";
        }
      } else if (genre == 0x09) {
        if (description == 0x00) {
          info = "Comédia";
        } else {
          info = "Comédia";
        }
      } else if (genre == 0x0a) {
        if (description == 0x00) {
          info = "Infantil";
        } else {
          info = "Infantil";
        }
      } else if (genre == 0x0b) {
        if (description == 0x00) {
          info = "Erótico";
        } else {
          info = "Erótico";
        }
      } else if (genre == 0x0c) {
        if (description == 0x00) {
          info = "Filme";
        } else {
          info = "Filme";
        }
      } else if (genre == 0x0d) {
        if (description == 0x00) {
          info = "Sorteio";
        } else if (description == 0x01) {
          info = "Vendas";
        } else if (description == 0x02) {
          info = "Premiação";
        } else {
          info = "Sorteio, Vendas e Premiação";
        }
      } else if (genre == 0x0e) {
        if (description == 0x00) {
          info = "Debate";
        } else if (description == 0x01) {
          info = "Entrevista";
        } else {
          info = "Debate e Entrevista";
        }
      } else if (genre == 0x0f) {
        if (description == 0x00) {
          info = "Desenho Adulto";
        } else if (description == 0x01) {
          info = "Interatividade";
        } else if (description == 0x02) {
          info = "Policial";
        } else if (description == 0x03) {
          info = "Religião";
        } else {
          info = "Outros";
        }
      }

      return info;
    }

    static std::string GetMarkerDescription(int marker)
    {
      std::string info;

      if (marker == 0x0a) {
        info = "Aiko";
      } else if (marker == 0x28) {
        info = "AOC";
      } else if (marker == 0xa0) {
        info = "CCE";
      } else if (marker == 0x84) {
        info = "Cineral";
      } else if (marker == 0xe5) {
        info = "Digibras";
      } else if (marker == 0x39) {
        info = "Elsys";
      } else if (marker == 0x16) {
        info = "Envision";
      } else if (marker == 0x13) {
        info = "Evadin";
      } else if (marker == 0x93) {
        info = "Gradiente";
      } else if (marker == 0x1f) {
        info = "Intelbras";
      } else if (marker == 0xe0) {
        info = "LGE";
      } else if (marker == 0x36) {
        info = "Mitsubishi";
      } else if (marker == 0xfa) {
        info = "Philco";
      } else if (marker == 0xfe) {
        info = "Philips";
      } else if (marker == 0xc4) {
        info = "Panasonic";
      } else if (marker == 0xc9) {
        info = "Pionner";
      } else if (marker == 0xe8) {
        info = "Positivo";
      } else if (marker == 0xd9) {
        info = "Proview";
      } else if (marker == 0x0e) {
        info = "Qualcomm";
      } else if (marker == 0x8d) {
        info = "Sat Bras";
      } else if (marker == 0x81) {
        info = "Sharp (MBK)";
      } else if (marker == 0x83) {
        info = "Semp";
      } else if (marker == 0xa5) {
        info = "Sony";
      } else if (marker == 0x82) {
        info = "Samsung";
      } else if (marker == 0x96) {
        info = "Sanyo";
      } else if (marker == 0xb4) {
        info = "STB";
      } else if (marker == 0x90) {
        info = "SVA";
      } else if (marker == 0xcb) {
        info = "Tectoy";
      } else if (marker == 0xf8) {
        info = "Thomson";
      } else if (marker == 0xed) {
        info = "Toshiba";
      }

      return info;
    }

    static std::string GetClosedCaptionFormatDescription(int format) 
    {
      std::string info;

      if (format == 0x00) {
        info = "horizontal writing in standard density";
      } else if (format == 0x01) {
        info = "vertical writing in standard density";
      } else if (format == 0x02) {
        info = "horizontal writing in high density";
      } else if (format == 0x03) {
        info = "vertical writing in high density";
      } else if (format == 0x04) {
        info = "horizontal of western language";
      } else if (format == 0x05) {
        info = "reserved";
      } else if (format == 0x06) {
        info = "horizontal writing in 1920 x 1080";
      } else if (format == 0x07) {
        info = "vertical writing in 1920 x 1080";
      } else if (format == 0x08) {
        info = "horizontal writing in 960 x 540";
      } else if (format == 0x09) {
        info = "vertical writing in 960 x 540";
      } else if (format == 0x0a) {
        info = "horizontal writing in 1280 x 720";
      } else if (format == 0x0b) {
        info = "vertical writing in 1280 x 720";
      } else if (format == 0x0b) {
        info = "horizontal writing in 720 x 480";
      } else if (format == 0x0b) {
        info = "vertical writing in 720 x 480";
      } else {
        info = "reserved";
      }

      return info;
    }

    static std::string GetDownloadCancelReasonDescription(int reason) 
    {
      std::string info;

      if (reason == 0x00) {
        info = "unknown";
      } else if (reason == 0x01) {
        info = "rsnScenarioTimeout";
      } else if (reason == 0x02) {
        info = "rsnInsufMem";
      } else if (reason == 0x03) {
        info = "rsnAuthDenied";
      } else if (reason == 0x04) {
        info = "rsnFatal";
      } else if (reason == 0x05) {
        info = "rsnInfoRequestError";
      } else if (reason == 0x06) {
        info = "rsnCompatError";
      } else if (reason == 0x07) {
        info = "rsnUnreliableNetwork";
      } else if (reason == 0x08) {
        info = "rsnInvalidData";
      } else if (reason == 0x09) {
        info = "rsnInvalidBlock";
      } else if (reason == 0x0a) {
        info = "rsnInvalidVersion";
      } else if (reason == 0x0b) {
        info = "rsnAbort";
      } else if (reason == 0x0c) {
        info = "rsnRetrans";
      } else if (reason == 0x0d) {
        info = "rsnBadBlockSize";
      } else if (reason == 0x0e) {
        info = "rsnBadWindow";
      } else if (reason == 0x0f) {
        info = "rsnBadAckPeriod";
      } else if (reason == 0x10) {
        info = "rsnBadWindowTimer";
      } else if (reason == 0x11) {
        info = "rsnBadScenarioTimer";
      } else if (reason == 0x12) {
        info = "rsnBadCapabilities";
      } else if (reason == 0x13) {
        info = "rsnBadModuleTable";
      } else if (reason >= 0x14 and reason <= 0xef) {
        info = "unknown";
      } else if (reason >= 0xf0 and reason <= 0xff) {
        info = "unknown";
      }

      return info;
    }

    static std::string GetDownloadReasonDescription(int reason)
    {
      std::string info;

      if (reason == 0x00) {
        info = "unknown";
      } else if (reason == 0x01) {
        info = "rsnStart";
      } else if (reason == 0x02) {
        info = "rsnAckCont";
      } else if (reason == 0x03) {
        info = "rsnNakRetransBlock";
      } else if (reason == 0x04) {
        info = "rsnNakRetransWindow";
      } else if (reason == 0x05) {
        info = "rsnEnd";
      } else if (reason >= 0x06 and reason <= 0xef) {
        info = "unknown";
      } else if (reason >= 0xf0 and reason <= 0xff) {
        info = "unknown";
      }

      return info;
    }

};

class SI {

  private:

  protected:
    SI()
    {
    }

  public:
    virtual ~SI()
    {
    }

};

class SIService : public SI {

  public:
    enum class service_type_t {
      LD,
      SD,
      HD
    };

    enum class stream_type_t {
      UNKNOWN,
      AUDIO,
      VIDEO,
      SUBTITLE,
      PRIVATE,
      DSMCC_SECTION,
      DSMCC_MESSAGE,
      DSMCC_DESCRIPTOR,
      LIBRAS_MESSAGE,
      LIBRAS_STREAM,
      RESERVED,
    };

    struct elementary_stream_t {
      std::map<int, std::shared_ptr<std::string>> descriptors;
      int program_identifier;
      stream_type_t type;
    };

  private:
    std::vector<std::shared_ptr<struct elementary_stream_t>>
      _elementary_streams;
    std::string
      _service_provider, 
      _service_name;
    int
      _original_network_id, 
      _transport_stream_id, 
      _service_id;
    service_type_t
      _service_type;

  public:
    bool operator==(const std::shared_ptr<SIService> &param)
    {
      return (ServiceProvider() == param->ServiceProvider() and ServiceName() == param->ServiceName() and OriginalNetworkID() == param->OriginalNetworkID() and TransportStreamID() == param->TransportStreamID() and ServiceID() == param->ServiceID() and ServiceType() == param->ServiceType());
    }

  public:
    SIService():
      SI()
    {
    }

    virtual ~SIService()
    {
    }

    void ServiceProvider(std::string param)
    {
      _service_provider = param;
    }

    void ServiceName(std::string param)
    {
      _service_name = param;
    }

    void OriginalNetworkID(int param)
    {
      _original_network_id = param;
    }

    void TransportStreamID(int param)
    {
      _transport_stream_id = param;
    }

    void ServiceID(int param)
    {
      _service_id = param;
    }

    void ServiceType(service_type_t param)
    {
      _service_type = param;
    }

    void AddElementaryStream(std::shared_ptr<struct elementary_stream_t> param)
    {
      for (std::vector<std::shared_ptr<struct elementary_stream_t>>::iterator i=_elementary_streams.begin(); i!=_elementary_streams.end(); i++) {
        if (param->program_identifier == (*i)->program_identifier) {
          return;
        }
      }

      _elementary_streams.push_back(param);
    }

    std::string ServiceProvider()
    {
      return _service_provider;
    }

    std::string ServiceName()
    {
      return _service_name;
    }

    int OriginalNetworkID()
    {
      return _original_network_id;
    }

    int TransportStreamID()
    {
      return _transport_stream_id;
    }

    int ServiceID()
    {
      return _service_id;
    }

    service_type_t ServiceType()
    {
      return _service_type;
    }

    const std::vector<std::shared_ptr<struct elementary_stream_t>> ElementaryStreams()
    {
      return _elementary_streams;
    }

    void Print()
    {
      std::string service_type;

      if (_service_type == service_type_t::HD) {
        service_type = "HD";
      } else if (_service_type == service_type_t::LD) {
        service_type = "LD";
      } else {
        service_type = "SD";
      }

      printf("Service:: provider:[%s], name:[%s], original network id:[0x%04x], transport stream id:[0x%04x], service id:[0x%04x], service type:[%s]\n",
          _service_provider.c_str(), _service_name.c_str(), _original_network_id, _transport_stream_id, _service_id, service_type.c_str()); 
    
      for (std::vector<std::shared_ptr<struct elementary_stream_t>>::iterator i=_elementary_streams.begin(); i!=_elementary_streams.end(); i++) {
        std::shared_ptr<struct elementary_stream_t> stream = (*i);

        std::string 
          stream_type = "unknown";

        if (stream->type == stream_type_t::AUDIO) {
          stream_type = "audio";
        } else if (stream->type == stream_type_t::VIDEO) {
          stream_type = "video";
        } else if (stream->type == stream_type_t::SUBTITLE) {
          stream_type = "subtitle";
        } else if (stream->type == stream_type_t::PRIVATE) {
          stream_type = "private";
        } else if (stream->type == stream_type_t::DSMCC_SECTION) {
          stream_type = "dsmcc section";
        } else if (stream->type == stream_type_t::DSMCC_MESSAGE) {
          stream_type = "dsmcc message";
        } else if (stream->type == stream_type_t::DSMCC_DESCRIPTOR) {
          stream_type = "dsmcc descriptor";
        } else if (stream->type == stream_type_t::LIBRAS_MESSAGE) {
          stream_type = "libras message";
        } else if (stream->type == stream_type_t::LIBRAS_STREAM) {
          stream_type = "libras stream";
        } else if (stream->type == stream_type_t::RESERVED) {
          stream_type = "reserved";
        }

        if (stream->descriptors.find(0x52) != stream->descriptors.end()) { // INFO:: stream identifier descriptor
          std::shared_ptr<std::string> 
            component_tag = stream->descriptors[0x52];
        
          printf("\t%s: program identifer:[0x%04x], component tag:[0x%02x]\n", stream_type.c_str(), stream->program_identifier, (uint8_t)component_tag->data()[0]);
        } else {
          printf("\t%s: program identifer:[0x%04x]\n", stream_type.c_str(), stream->program_identifier);
        }
      }
    }

};

class SITime : public SI {

  public:
    enum class week_day_t {
      SUNDAY,
      MONDAY,
      TUESDAY,
      WEDNESDAY,
      THRUSDAY,
      FRIDAY,
      SATURDAY
    };

  private:
    std::string
      _country;
    int
      _year,
      _month,
      _day,
      _hour,
      _minute,
      _second;
    week_day_t
      _week_day;

  public:
    bool operator==(SITime param)
    {
      return (Year() == param.Year() and Month() == param.Month() and Day() == param.Day() and Hour() == param.Hour() and Minute() == param.Minute() and Second() == param.Second());
    }

  public:
    SITime():
      SI()
    {
      _country = "BRA";
      _year = 0;
      _month = 0;
      _day = 0;
      _hour = 0;
      _minute = 0;
      _second = 0;
      _week_day = week_day_t::SUNDAY;
    }

    virtual ~SITime()
    {
    }

    void Country(std::string param)
    {
      _country = param;
    }

    void Year(int param)
    {
      _year = param;
    }

    void Month(int param)
    {
      _month = param;
    }

    void Day(int param)
    {
      _day = param;
    }

    void Hour(int param)
    {
      _hour = param;
    }

    void Minute(int param)
    {
      _minute = param;
    }

    void Second(int param)
    {
      _second = param;
    }

    void WeekDay(week_day_t param)
    {
      _week_day = param;
    }

    std::string Country()
    {
      return _country;
    }

    int Year()
    {
      return _year;
    }

    int Month()
    {
      return _month;
    }

    int Day()
    {
      return _day;
    }

    int Hour()
    {
      return _hour;
    }

    int Minute()
    {
      return _minute;
    }

    int Second()
    {
      return _second;
    }

    week_day_t WeekDay()
    {
      return _week_day;
    }

    void Print()
    {
      printf("Time:: country:[%s], year:[%02d], month:[%02d], day:[%02d], hour:[%02d], minute:[%02d], second:[%02d]\n",
          _country.c_str(), _year, _month, _day, _hour, _minute, _second); 
    }

};

class SINetwork : public SI {

  private:
    std::string
      _transport_stream_name;
    int
      _network_id, 
      _original_network_id, 
      _transport_stream_id,
      _channel_number;

  public:
    bool operator==(std::shared_ptr<SINetwork> param)
    {
      return (TransportStreamName() == param->TransportStreamName() and NetworkID() == param->NetworkID() and OriginalNetworkID() == param->OriginalNetworkID() and TransportStreamID() == param->TransportStreamID());
    }

  public:
    SINetwork():
      SI()
    {
    }

    virtual ~SINetwork()
    {
    }

    void TransportStreamName(std::string param)
    {
      _transport_stream_name = param;
    }

    void NetworkID(int param)
    {
      _network_id = param;
    }

    void OriginalNetworkID(int param)
    {
      _original_network_id = param;
    }

    void TransportStreamID(int param)
    {
      _transport_stream_id = param;
    }

    void ChannelNumber(int param)
    {
      _channel_number = param;
    }

    std::string TransportStreamName()
    {
      return _transport_stream_name;
    }

    int NetworkID()
    {
      return _network_id;
    }

    int OriginalNetworkID()
    {
      return _original_network_id;
    }

    int TransportStreamID()
    {
      return _transport_stream_id;
    }

    int ChannelNumber()
    {
      return _channel_number;
    }

    void Print()
    {
      printf("Network:: transport stream name:[%s], network id:[0x%04x], original network id:[0x%04x], transport stream id:[0x%04x], channel number:[%02d]\n",
          _transport_stream_name.c_str(), _network_id, _original_network_id, _transport_stream_id, _channel_number); 
    }

};

class SIEvent : public SI {

  private:
    std::string
      _event_name,
      _description;
    int
      _original_network_id, 
      _transport_stream_id,
      _service_id,
      _event_id;

  public:
    bool operator==(std::shared_ptr<SIEvent> &param)
    {
      return (EventName() == param->EventName() and Description() == param->Description() and OriginalNetworkID() == param->OriginalNetworkID() and TransportStreamID() == param->TransportStreamID() and ServiceID() == param->ServiceID() and EventID() == param->EventID());
    }

  public:
    SIEvent():
      SI()
    {
    }

    virtual ~SIEvent()
    {
    }

    void EventName(std::string param)
    {
      _event_name = param;
    }

    void Description(std::string param)
    {
      _description = param;
    }

    void OriginalNetworkID(int param)
    {
      _original_network_id = param;
    }

    void TransportStreamID(int param)
    {
      _transport_stream_id = param;
    }

    void ServiceID(int param)
    {
      _service_id = param;
    }

    void EventID(int param)
    {
      _event_id = param;
    }

    std::string EventName()
    {
      return _event_name;
    }

    std::string Description()
    {
      return _description;
    }

    int OriginalNetworkID()
    {
      return _original_network_id;
    }

    int TransportStreamID()
    {
      return _transport_stream_id;
    }

    int ServiceID()
    {
      return _service_id;
    }

    int EventID()
    {
      return _event_id;
    }

    void Print()
    {
      printf("Event:: event name:[%s], original network id:[0x%04x], transport stream id:[0x%04x], service id:[0x%04x], event id:[0x%04x]\n",
          _event_name.c_str(), _original_network_id, _transport_stream_id, _service_id, _event_id); 
    }

};

class SIData : public SI {

  public:
    struct module_info_t {
      std::map<int, bool> blocks;
      std::shared_ptr<std::string> data;
      int id;
      int carousel;
      int size;
      int version;
      int block_size;
      bool complete;
    };

    struct object_info_t {
      std::shared_ptr<struct module_info_t> module;
      std::string object_key;
      std::string kind;
      int offset;
    };

    struct ior_info_t {
      std::string object_key;
      const char *ptr;
    };
    
  private:
    std::vector<std::shared_ptr<struct module_info_t>>
      _modules;
    std::vector<std::shared_ptr<struct object_info_t>>
      _objects;
    std::map<std::string, std::vector<std::string>> // INFO:: object_key => files or directories's object_key
      _nodes;
    std::map<std::string, std::string> // INFO:: object_key => object_name
      _names;
    std::mutex
      _mutex;
    uint32_t
      _download_id;

  public:
    bool operator==(std::shared_ptr<SIData> param)
    {
      return (DownloadID() == param->DownloadID());
    }

    std::shared_ptr<struct object_info_t> GetObjectByKey(std::string key)
    {
      for (std::vector<std::shared_ptr<struct object_info_t>>::iterator i=_objects.begin(); i!=_objects.end(); i++) {
        std::shared_ptr<struct object_info_t> object = (*i);

        if (object->object_key == key) {
          return object;
        }
      }

      return nullptr;
    }

    void ProcessFilesystem(std::string base_directory, std::string current_path, std::string object_key)
    {
      std::string 
        path = base_directory + "/" + current_path;

      std::ostringstream o;
        
      o << "mkdir -p \"" << path << "\"";

      if (system(o.str().c_str()) != 0) {
        printf("SIData::ProcessFilesystem:: unable to create directory [%s]\n", base_directory.c_str());
      }

      for (std::map<std::string, std::vector<std::string>>::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        if (i->first == object_key) {
          std::string module = object_key.substr(0, object_key.find("."));

          for (std::vector<std::string>::iterator j=i->second.begin(); j!=i->second.end(); j++) {
            std::shared_ptr<struct object_info_t> object = GetObjectByKey((*j));

            if (object != nullptr) {
              std::string id = object->object_key;

              if (object->kind == "fil") {
                std::ostringstream o;

                o << "mv \"" << base_directory << "/" << id << "\" \"" << path << "/" << _names[id].c_str() << "\"";

                if (system(o.str().c_str()) != 0) {
                  break;
                }
              } else if (object->kind == "dir") {
                ProcessFilesystem(base_directory, current_path + "/" + _names[id].c_str(), id);
              }
            }
          }
        }
      }
    }

  public:
    SIData():
      SI()
    {
      _download_id = -1;
    }

    virtual ~SIData()
    {
    }

    static std::string GetObjectKeyAsText(int carousel, int module, std::string key)
    {
      std::ostringstream o;

      o << std::hex << std::setw(16) << std::setfill('0') << carousel << ".";
      o << std::hex << std::setw(16) << std::setfill('0') << module << ".";

      for (int i=0; i<(int)key.size(); i++) {
        o << std::hex << std::setw(2) << std::setfill('0') << (int)(key[i] & 0xff);
      }

      return o.str();
    }

    static std::shared_ptr<struct ior_info_t> ProcessIOR(const char *ptr)
    {
      uint32_t type_id_length = TS_G32(ptr + 0);
      std::string type_id = std::string(ptr + 4, type_id_length);

      ptr = ptr + 4 + type_id_length;

      // INFO:: alignment gap
      int offset = type_id_length%4;

      if (offset != 0) {
        ptr = ptr + (4 - (type_id_length%4));
      }

      uint32_t tagged_profiles_count = TS_G32(ptr + 0);

      ptr = ptr + 4;

      std::shared_ptr<struct ior_info_t> ior = std::make_shared<struct ior_info_t>();

      for (uint32_t i=0; i<tagged_profiles_count; i++) {
        uint32_t profile_id_tag = TS_G32(ptr + 0);
        // uint32_t profile_data_length = TS_G32(ptr + 4);
        std::string profile_info;

        if (profile_id_tag == 0x49534f06) {
          profile_info = "TAG_BIOP";
        } else if (profile_id_tag == 0x49534f05) {
          profile_info = "TAG_LITE_OPTIONS";
        }

        printf("DSMCC:IOR: profile tag:[%s]\n", profile_info.c_str());

        ptr = ptr + 8;

        if (profile_info == "TAG_BIOP") { // TR 101 202: Table 4.5
          // int profile_data_byte_order = TS_G8(ptr + 0);
          int lite_components_count = TS_G8(ptr + 1);

          // INFO:: BIOP::ObjectLocation
          uint32_t component_id_tag = TS_G32(ptr + 2);

          if (component_id_tag != 0x49534F50) {
            printf("DSMCC:IOR/TAG_BIOP/BIOP::ObjectLocation: invalid component id tag");

            return nullptr;
          }

          // int component_data_length = TS_G8(ptr + 6);
          uint32_t carousel_id = TS_G32(ptr + 7);
          int module_id = TS_G16(ptr + 11);
          int version_major = TS_G8(ptr + 13); // BIOP protocol major version 1
          int version_minor = TS_G8(ptr + 14); // BIOP protocol minor version 0
          int object_key_length = TS_G8(ptr + 15);
          std::string object_key(ptr + 16, object_key_length);

          printf("DSMCC:IOR/TAG_BIOP/BIOP:: carousel id:[0x%08x], module id:[0x%04x], version:[%02d.%02d]\n", carousel_id, module_id, version_major, version_minor);

          DumpBytes("DSMCC:IOR/TAG_BIOP: object key data byte", ptr + 16, object_key_length);

          ptr = ptr + 16 + object_key_length;

          // INFO:: DSM::ConnBinder
          uint32_t component_id_tag2 = TS_G32(ptr + 0);

          if (component_id_tag2 != 0x49534F40) {
            printf("DSMCC:DownloadDataMessage::biop<dir>/TAG_BIOP/DSM::ConnBinder: invalid component id tag\n");

            return nullptr;
          }

          // int component_data_length2 = TS_G8(ptr + 4);
          int taps_count = TS_G8(ptr + 5);

          ptr = ptr + 6;

          // INFO:: BIOP::Tap
          int id = TS_G16(ptr + 0); // 0x0000
          int use = TS_G16(ptr + 2); // 0x0016
          int association_tag = TS_G16(ptr + 4);
          // int selector_length = TS_G8(ptr + 6); // 0x0A
          int selector_type = TS_G16(ptr + 7); // 0x01
          uint32_t transaction_id = TS_G32(ptr + 9);
          // uint32_t timeout = TS_G32(ptr + 13);

          printf("DSMCC:IOR/TAG_BIOP/BIOP::Tap1: id:[0x%04x], use:[%s], association tag:[0x%04x], selector type:[%04x], transaction id:[%08x]\n", id, Utils::GetTapUseDescription(use).c_str(), association_tag, selector_type, transaction_id);

          ptr = ptr + 17;

          for (int i=0; i<taps_count - 1; i++) {
            int id = TS_G16(ptr + 0); // 0x0000
            int use = TS_G16(ptr + 2); // 0x0016 (BIOP_DELIVERY_PARA_USE<0x16>, BIOP_OBJECT_USE<0x17>)
            int association_tag = TS_G16(ptr + 4);
            int selector_length = TS_G8(ptr + 6);

            printf("DSMCC:IOR/TAG_BIOP/BIOP::Tap2: id:[0x%04x], use:[%s], association tag:[0x%04x]\n", id, Utils::GetTapUseDescription(use).c_str(), association_tag);

            DumpBytes("DSMCC:IOR/TAG_BIOP/BIOP::Tap2: selector data byte", ptr + 7, selector_length);

            ptr = ptr + 7 + selector_length;
          }

          for (int i=0; i<lite_components_count - 2; i++) {
            // INFO:: BIOP::LiteComponent
            uint32_t component_id_tag = TS_G32(ptr + 0);
            int component_data_length = TS_G8(ptr + 4);

            printf("DSMCC:IOR/TAG_BIOP/BIOP::LiteComponent: component id tag:[0x%08x]\n", component_id_tag);

            DumpBytes("DSMCC:IOR/TAG_BIOP/BIOP::LiteComponent: component data byte", ptr + 5, component_data_length);

            ptr = ptr + 5 + component_data_length;
          }

          ior->object_key = SIData::GetObjectKeyAsText(carousel_id, module_id, object_key);
        } else if (profile_info == "TAG_LITE_OPTIONS") { // TR 101 202: Table 4.7
          // int profile_data_byte_order = TS_G8(ptr + 0);
          int component_count = TS_G8(ptr + 1);

          // INFO:: DSM::ServiceLocation
          uint32_t component_id_tag = TS_G32(ptr + 2);

          if (component_id_tag != 0x49534F46) {
            printf("DSMCC:IOR/TAG_LITE_OPTIONS/DSM::ServiceLocation: invalid component id tag\n");

            return nullptr;
          }

          // uint32_t component_data_length = TS_G32(ptr + 6);
          // int service_domain_length = TS_G8(ptr + 10);

          ptr = ptr + 11;

          // INFO:: serviceDomain_Data() -> DVBcarouselNSAPaddress()
          // int afi = TS_G8(ptr + 0); // 0x00
          // int type = TS_G8(ptr + 1); // 0x00
          // uint32_t carousel_id = TS_G32(ptr + 2);
          // int specifier_type = TS_G8(ptr + 6); // 0x01
          // uint32_t specifier_data = TS_GM32(ptr + 7, 0, 24);
          // int transport_stream_id = TS_G16(ptr + 10);
          // int original_network_id = TS_G16(ptr + 12);
          // int service_id = TS_G16(ptr + 14);
          // uint32_t reserved = TS_G32(ptr + 16);

          ptr = ptr + 20;

          // INFO:: CosNaming::Name
          uint32_t names_component_count = TS_G32(ptr + 0);

          ptr = ptr + 4;

          for (uint32_t i=0; i<names_component_count; i++) {
            uint32_t id_length = TS_G32(ptr + 0);
            std::string id(ptr + 4, id_length);

            ptr = ptr + 4 + id_length;

            uint32_t kind_length = TS_G32(ptr + 0);
            std::string kind(ptr + 4, kind_length);

            ptr = ptr + 4 + kind_length;

            printf("DSMCC:IOR/TAG_LITE_OPTIONS/CosNaming::Name: id:[%s], kind:[%s]", id.c_str(), Utils::GetBIOPTypeInfo(kind).c_str());
          }

          uint32_t initial_context_length = TS_G32(ptr + 0);

          DumpBytes("DSMCC:IOR/TAG_LITE_OPTIONS/CosNaming::Name: initial context data byte", ptr + 4, initial_context_length);

          ptr = ptr + 4 + initial_context_length;

          for (int i=0; i<component_count - 1; i++) {
            // uint32_t component_id_tag = TS_G32(ptr + 0);
            int component_data_length = TS_G8(ptr + 4);

            DumpBytes("DSMCC:IOR/TAG_LITE_OPTIONS: component data byte", ptr + 5, component_data_length);

            ptr = ptr + 5 + component_data_length;
          }
        } else {
          printf("DSMCC:IOR: invalid profile id tag\n");

          return nullptr;
        }
      }

      ior->ptr = ptr;

      return ior;
    }

    void DownloadID(uint32_t param)
    {
      std::lock_guard<std::mutex> lock(_mutex);

      _download_id = param;
    }

    void Module(int carousel_id, int module_id, int module_size, int module_version, int block_size)
    {
      std::lock_guard<std::mutex> lock(_mutex);

      for (std::vector<std::shared_ptr<struct module_info_t>>::iterator i=_modules.begin(); i!=_modules.end(); i++) {
        std::shared_ptr<struct module_info_t> module = (*i);

        if (module->id == module_id) {
          if (module->version != module_version) {
            // _modules.erase(i);

            // break;
          }

          return;
        }
      }

      std::shared_ptr<struct module_info_t> info = std::make_shared<struct module_info_t>();

      info->id = module_id;
      info->carousel = carousel_id;
      info->size = module_size;
      info->version = info->version;
      info->block_size = block_size;

      int number_of_blocks = ceil(module_size/block_size + 1);

      std::string data;

      data.resize(module_size);

      info->data = std::make_shared<std::string>(std::move(data));

      for (int i=0; i<number_of_blocks; i++) {
        info->blocks[i] = false;
      }

      _modules.push_back(info);
    }

    void ModuleBlock(int carousel_id, int module_id, int module_version, int block_number, std::shared_ptr<std::string> data)
    {
      std::lock_guard<std::mutex> lock(_mutex);

      for (std::vector<std::shared_ptr<struct module_info_t>>::iterator i=_modules.begin(); i!=_modules.end(); i++) {
        std::shared_ptr<struct module_info_t> module = (*i);

        if (module->carousel == carousel_id) {
          if (module->id == module_id) {
            std::map<int, bool>::iterator j = module->blocks.find(block_number);

            if (j != module->blocks.end() and j->second == false) { // and module->version == module_version) { // TODO:: update module->size when module version was different
              int offset = module->block_size*block_number;

              try {
                module->data->replace(
                    module->data->begin() + offset, module->data->begin() + offset + data->size(), data->begin(), data->end());
              } catch (std::out_of_range &e) {
                printf("SIData::ModuleBlock<error>:: replace out of range\n");
              }

              module->blocks[block_number] = true;
              module->complete = true;

              for (int j=0; j<(int)module->blocks.size(); j++) {
                if (module->blocks[j] == false) {
                  module->complete = false;

                  break;
                }
              }
            }

            break;
          }
        }
      }
    }

    uint32_t DownloadID()
    {
      std::lock_guard<std::mutex> lock(_mutex);

      return _download_id;
    }

    std::vector<std::shared_ptr<struct module_info_t>> Modules()
    {
      std::lock_guard<std::mutex> lock(_mutex);

      std::sort(_modules.begin(), _modules.end(), 
          [](const std::shared_ptr<struct module_info_t> &a, const std::shared_ptr<struct module_info_t> &b) {
            return (a->id < b->id);
          });

      return _modules;
    }

    bool IsComplete()
    {
      std::lock_guard<std::mutex> lock(_mutex);

      for (std::vector<std::shared_ptr<struct module_info_t>>::iterator i=_modules.begin(); i!=_modules.end(); i++) {
        std::shared_ptr<struct module_info_t> module = (*i);

        if (module->complete == false) {
          return false;
        }
      }
      
      return true;
    }

    void Save(std::string path)
    {
      if (IsComplete() == false) {
        printf("SIData::Save: some modules are not complete\n");

        return;
      }

      std::lock_guard<std::mutex> lock(_mutex);

      std::string
        cmd = std::string("rm -rf \"" + path + "\"; mkdir \"" + path + "\"");

      if (system(cmd.c_str()) != 0) {
        return;
      }

      std::string
        service_gateway_object_key;

      for (std::vector<std::shared_ptr<struct object_info_t>>::iterator i=_objects.begin(); i!=_objects.end(); i++) {
        std::shared_ptr<struct object_info_t> object = (*i);

        if (object->module->complete == false) {
          continue;
        }

        if (object->kind == "srg") {
          service_gateway_object_key = object->object_key;
        }

        if (object->kind != "fil") {
          continue;
        }

        const char 
          *ptr = object->module->data->c_str() + object->offset;

        // INFO:: BIOP header
        uint32_t magic = TS_G32(ptr + 0);

        if (magic != 0x42494f50) { // 'B','I','O','P'
          break;
        }

        int biop_version_major = TS_G8(ptr + 4);
        int biop_version_minor = TS_G8(ptr + 5);
        int byte_order = TS_G8(ptr + 6);
        int message_type = TS_G8(ptr + 7);
        // uint32_t message_size = TS_G32(ptr + 8);
        int object_key_length = TS_G8(ptr + 12); 
        std::string object_key = std::string(ptr + 13, object_key_length);
        // uint32_t object_kind_length = TS_G32(ptr + 13 + object_key_length);
        uint32_t object_kind = TS_G32(ptr + 17 + object_key_length);

        if (biop_version_major != 0x01 || 
            biop_version_minor != 0x00 || 
            byte_order != 0x00 || 
            message_type != 0x00) { // 0x00: Indicates that the message is being sent from the User to the Network to begin a scenario
          break;
        }

        std::string biop(ptr + 17 + object_key_length);

        ptr = ptr + 21 + object_key_length;

        if (object_kind == 0x66696c00) { // 'f', 'i', 'l', '\0' (file)
          int object_info_length = TS_G16(ptr + 0); 
          uint64_t dsm_file_content_size = TS_G64(ptr + 2); 

          ptr = ptr + 10 + object_info_length - 8;

          int service_context_list_count = TS_G8(ptr + 0); 

          ptr = ptr + 1;

          for (int i=0; i<service_context_list_count; i++) {
            // uint32_t context_id = TS_G32(ptr + 0); 
            int context_data_length = TS_G16(ptr + 4); 

            ptr = ptr + 6 + context_data_length;
          }

          // uint32_t message_body_length = TS_G32(ptr + 0);
          // uint32_t content_length = TS_G32(ptr + 4);

          ptr = ptr + 8;

          // INFO:: save files to disk
          std::string path = std::string("/tmp/data/") + object->object_key;

          int fd = open(path.c_str(), O_CREAT | O_TRUNC | O_LARGEFILE | O_WRONLY, 0666);

          if (fd > 0) {
            if (write(fd, ptr, dsm_file_content_size) == (int)dsm_file_content_size) {
              close(fd);
              fsync(fd);
            }
          }

          ptr = ptr + dsm_file_content_size;
        }
      }

      ProcessFilesystem(path, "", service_gateway_object_key);
    }

    void Reset()
    {
      std::lock_guard<std::mutex> lock(_mutex);

      _download_id = -1;

      _modules.clear();
      _objects.clear();
      _nodes.clear();
      _names.clear();
    }

    void Parse()
    {
      std::lock_guard<std::mutex> lock(_mutex);

      char tmp[255];

      for (std::vector<std::shared_ptr<struct module_info_t>>::iterator i=_modules.begin(); i!=_modules.end(); i++) {
        std::shared_ptr<struct module_info_t> module = (*i);

        if (module->complete == false) {
          continue;
        }

        // INFO:: module data
        const char 
          *ptr = module->data->c_str(),
          *end = ptr + module->data->size();
        
        while (ptr < end) {
          // INFO:: BIOP header
          uint32_t magic = TS_G32(ptr + 0);

          if (magic != 0x42494f50) { // 'B','I','O','P'
            break;
          }

          int biop_version_major = TS_G8(ptr + 4);
          int biop_version_minor = TS_G8(ptr + 5);
          int byte_order = TS_G8(ptr + 6);
          int message_type = TS_G8(ptr + 7);
          // uint32_t message_size = TS_G32(ptr + 8);
          int object_key_length = TS_G8(ptr + 12); 
          std::string object_key = std::string(ptr + 13, object_key_length);
          // uint32_t object_kind_length = TS_G32(ptr + 13 + object_key_length);
          uint32_t object_kind = TS_G32(ptr + 17 + object_key_length);

          if (biop_version_major != 0x01 || 
              biop_version_minor != 0x00 || 
              byte_order != 0x00 || 
              message_type != 0x00) { // 0x00: Indicates that the message is being sent from the User to the Network to begin a scenario
            break;
          }

          object_key = GetObjectKeyAsText(module->carousel, module->id, object_key);

          // INFO:: creating object reference
          std::shared_ptr<struct object_info_t> object = std::make_shared<struct object_info_t>();

          object->module = module;
          object->object_key = object_key;
          object->kind = std::string(ptr + 17 + object_key_length);
          object->offset = ptr - module->data->c_str();

          _objects.push_back(object);

          ptr = ptr + 21 + object_key_length;

          if (object_kind == 0x66696c00) { // 'f', 'i', 'l', '\0' (file)
            int object_info_length = TS_G16(ptr + 0); 
            uint64_t dsm_file_content_size = TS_G64(ptr + 2); 

            DumpBytes("object info data byte", ptr + 10, object_info_length - 8);

            ptr = ptr + 10 + object_info_length - 8;

            int service_context_list_count = TS_G8(ptr + 0); 

            ptr = ptr + 1;

            for (int i=0; i<service_context_list_count; i++) {
              // uint32_t context_id = TS_G32(ptr + 0); 
              int context_data_length = TS_G16(ptr + 4); 

              DumpBytes("context data byte", ptr + 6, context_data_length);

              ptr = ptr + 6 + context_data_length;
            }

            uint32_t message_body_length = TS_G32(ptr + 0);
            uint32_t content_length = TS_G32(ptr + 4);

            ptr = ptr + 8;

            printf("DSMCC:DownloadDataMessage:biop<fil>: dsm file content size:[%lu], service context list count:[%d], message body length:[%u], content length:[%u]\n", 
                dsm_file_content_size, service_context_list_count, message_body_length, content_length);

            // INFO:: save files to disk
            sprintf(tmp, "/tmp/data/%s", object_key.c_str());

            int fd = open(tmp, O_CREAT | O_TRUNC | O_LARGEFILE | O_WRONLY, 0666);

            if (fd > 0) {
              if (write(fd, ptr, dsm_file_content_size) == (int)dsm_file_content_size) { // content_length);
                close(fd);
                fsync(fd);
              }
            }

            ptr = ptr + dsm_file_content_size;
          } else if (
              object_kind == 0x73726700 or // 's', 'r', 'g', '\0' (gateway)
              object_kind == 0x64697200) { // 'd', 'i', 'r', '\0' (directory)
            std::string kind = "dir";

            if (object_kind == 0x73726700) {
              kind = "srg";
            }

            int object_info_length = TS_G16(ptr + 0); 

            DumpBytes("object info data byte", ptr + 2, object_info_length);

            ptr = ptr + 2 + object_info_length;

            int service_context_list_count = TS_G8(ptr + 0); 

            ptr = ptr + 1;

            for (int i=0; i<service_context_list_count; i++) {
              // uint32_t context_id = TS_G32(ptr + 0); 
              int context_data_length = TS_G16(ptr + 4); 

              DumpBytes("service context data byte", ptr + 6, context_data_length);

              ptr = ptr + 6 + context_data_length;
            }

            // uint32_t message_body_length = TS_G32(ptr + 0);
            int binds_count = TS_G16(ptr + 4);

            ptr = ptr + 6;

            if (object_kind == 0x73726700) {
              printf("JJJJJJJJJJJ::1: %s\n", object_key.c_str());
              _names[object_key] = "/";
            }

            for (int i=0; i<binds_count; i++) {
              int name_component_count = TS_G8(ptr + 0);

              ptr = ptr + 1;

              std::string object_name;

              for (int j=0; j<name_component_count; j++) {
                int id_length = TS_G8(ptr + 0);
                std::string id(ptr + 1, id_length);

                ptr = ptr + 1 + id_length;

                int kind_length = TS_G8(ptr + 0);
                std::string kind(ptr + 1, kind_length);

                ptr = ptr + 1 + kind_length;

                printf("DSMCC:DownloadDataMessage:biop<%s>/binds: id:[%s], kind:[%s]\n", kind.c_str(), id.c_str(), Utils::GetBIOPTypeInfo(kind).c_str());

                object_name = id;
              }

              int binding_type = TS_G8(ptr + 0);

              std::string binding_info;

              if (binding_type == 0x01) {
                binding_info = "nobject";
              } else {
                binding_info = "ncontext";
              }

              printf("DSMC:biop<%s>/IOR: binding info:[%s]\n", kind.c_str(), binding_info.c_str());

              std::shared_ptr<struct ior_info_t> ior = ProcessIOR(ptr + 1);

              if (ior == nullptr) {
                printf("DSMCC:DownloadDataMessage:biop<%s>/binds: <IOR error>\n", kind.c_str());

                break;
              }

              ptr = ior->ptr;

              _nodes[object_key].push_back(ior->object_key);
 
              _names[ior->object_key] = object_name;

              int object_info_length2 = TS_G16(ptr + 0);

              ptr = ptr + 2 + object_info_length2;
            }
          } else if (object_kind == 0x73747200) { // 's', 't', 'r', '\0' (stream event)
            int object_info_length = TS_G16(ptr + 0);
            int aDescription_length = TS_G8(ptr + 2); 

            DumpBytes("DSMCC:DownloadDataMessage:Stream: aDescription bytes", ptr + 3, aDescription_length);

            ptr = ptr + 3 + aDescription_length;

            // AppNPT
            uint32_t aSeconds = TS_G32(ptr + 0); 
            uint32_t aMicroseconds = TS_G32(ptr + 4);
            int audio = TS_G8(ptr + 8);
            int video = TS_G8(ptr + 9);
            int data = TS_G8(ptr + 10);

            printf("DSMCC:DownloadDataMessage:biop<str>: seconds:[%u], microseconds:[%u], audio:[0x%02x], video:[0x%02x], data:[0x%02x]\n", aSeconds, aMicroseconds, audio, video, data);

            DumpBytes("DSMCC:DownloadDataMessage:: object info byte", ptr + 11, object_info_length - (aDescription_length + 10));

            ptr = ptr + 11 + object_info_length - (aDescription_length + 10);

            int service_context_list_count = TS_G8(ptr + 0);

            DumpBytes("DSMCC:DownloadDataMessage:: service context list bytes", ptr + 1, service_context_list_count);

            ptr = ptr + 1 + service_context_list_count;

            // uint32_t message_body_length = TS_G32(ptr + 0);
            int taps_count = TS_G8(ptr + 4);

            ptr = ptr + 5;

            for (int i=0; i<taps_count; i++) {
              int id = TS_G16(ptr + 0);
              int use = TS_G16(ptr + 2);
              int association_tag = TS_G16(ptr + 4);
              int selector_length = TS_G8(ptr + 6);

              printf("DSMCC:DownloadDataMessage:taps: id:[0x%04x], use:[%s], association tag:[0x%04x], selector length:[0x%02x]\n", id, Utils::GetTapUseDescription(use).c_str(), association_tag, selector_length);
              
              ptr = ptr + 7;
            }
          } else if (object_kind == 0x73746500) { // 's', 't', 'e', '\0' (stream event message)
            int object_info_length = TS_G16(ptr + 0); 
            int aDescription_length = TS_G8(ptr + 2); 
            // uint32_t duration_aSeconds = TS_G32(ptr + 3 + aDescription_length); 
            // uint32_t duration_aMicroseconds = TS_G32(ptr + 3 + aDescription_length + 4); 
            // int audio = TS_G8(ptr + 3 + aDescription_length + 8); 
            // int video = TS_G8(ptr + 3 + aDescription_length + 9); 
            // int data = TS_G8(ptr + 3 + aDescription_length + 10); 

            // DSM::Event::EventList_T
            ptr = ptr + 3 + aDescription_length + 11;

            int eventNames_count = TS_G16(ptr + 0);
            int count = 0;

            ptr = ptr + 2;

            for (int i=0; i<eventNames_count; i++) {
              int eventName_length = TS_G8(ptr + 0);
              std::string name(ptr + 1, eventName_length);

              printf("DSMCC:DownloadDataMessage:biop<ste>: event name:[%s]\n", name.c_str());

              count = count + eventName_length;

              ptr = ptr + eventName_length + 1;
            }

            int object_info_byte_length = object_info_length - (aDescription_length + 10) - (2 + eventNames_count + count) - 2;

            ptr = ptr + object_info_byte_length;

            int serviceContextList_count = TS_G8(ptr + 0);

            if (serviceContextList_count != 0x00) {
              break;
            }

            ptr = ptr + 1; // + eventNames_count;

            // uint32_t messageBody_length = TS_G32(ptr + 0);
            int taps_count = TS_G8(ptr + 4);

            ptr = ptr + 4 + 1;

            for (int i=0; i<taps_count; i++) {
              int id = TS_G16(ptr + 0);
              int use = TS_G16(ptr + 2);
              std::string use_str;
              int association_tag = TS_G16(ptr + 4);
              // int selector_length = TS_G8(ptr + 6);

              printf("DSMCC:DownloadDataMessage:biop<str>: id:[0x%04x], use:[%s], association tag:[0x%04x]\n", id, Utils::GetTapUseDescription(use).c_str(), association_tag);

              ptr = ptr + 7;
            }

            int eventIds_count = eventNames_count; // TS_G8(ptr + 0);

            ptr = ptr + 1;

            for (int i=0; i<eventIds_count; i++) {
              int event_id = TS_G16(ptr + 0);

              printf("DSMCC:DownloadDataMessage:biop<str>: event id:[0x%04x]\n", event_id);

              ptr = ptr + 2;
            }
          }
        }
      }
    }

    void Print()
    {
      std::lock_guard<std::mutex> lock(_mutex);

      printf("Data:: download id:[0x%08x], number of modules:[%lu]\n", _download_id, _modules.size());

      for (std::vector<std::shared_ptr<struct module_info_t>>::iterator i=_modules.begin(); i!=_modules.end(); i++) {
        std::shared_ptr<struct module_info_t> module = (*i);

        printf("\tmodule id:[%08d], module size:[%08d], module version:[%d], blocks:[", module->id, module->size, module->version);

        for (int j=0; j<(int)module->blocks.size(); j++) {
          printf((module->blocks[j] == false)?".":"#");
        }

        printf("]:[%s]\n", (module->complete == true)?"complete":"not complete");

        for (std::vector<std::shared_ptr<struct object_info_t>>::iterator j=_objects.begin(); j!=_objects.end(); j++) {
          std::shared_ptr<struct object_info_t> object = (*j);
          std::string id = object->object_key;

          if (object->module->id == module->id) {
            // INFO:: id = <module_id> + "." + <object_key>
            printf("\t\t[%s]: object key:[%s], object name:[%s]\n", object->kind.c_str(), id.substr(id.find(".") + 1).c_str(), _names[id].c_str());
          }
        }
      }
    }

};

class SISubtitle : public SI {

  private:
    std::vector<std::string>
      _lines;

  public:
    bool operator==(std::shared_ptr<SISubtitle> param)
    {
      return true;
    }

  public:
    SISubtitle():
      SI()
    {
    }

    virtual ~SISubtitle()
    {
    }

    void Unit(const char *data, int length)
    {
      std::string
        text;
      uint8_t 
        *code = (uint8_t *)data,
        *end = code + length;

      while (code != end) {
        uint8_t byte = *code++;

        // Table 7-15 C0 Control Set
        if (byte == 0x00) { // null (do nothing)
          printf("Closed Caption:: data unit: <NULL>\n");
        } else if (byte == 0x01) { // 
        } else if (byte == 0x02) { // 
        } else if (byte == 0x03) { // 
        } else if (byte == 0x04) { // 
        } else if (byte == 0x05) { // 
        } else if (byte == 0x06) { // 
        } else if (byte == 0x07) { // BEL (bell)::(used to call attention with alarm or signal)
          printf("Closed Caption:: data unit: <BEL>\n");
        } else if (byte == 0x08) { // APB (active position backward)::(move cursor backward)
          printf("Closed Caption:: data unit: <APB>\n");
        } else if (byte == 0x09) { // APF (active position forward)::(move cursor forward)
          printf("Closed Caption:: data unit: <APF>\n");
        } else if (byte == 0x0a) { // APD (active position control)::(move cursor down)
          printf("Closed Caption:: data unit: <APD>\n");
        } else if (byte == 0x0b) { // APU (active position control)::(move cursor up)
          printf("Closed Caption:: data unit: <APU>\n");
        } else if (byte == 0x0c) { // CS (clear screen)::(clear the screen)
          printf("Closed Caption:: data unit: <CS>\n");
        } else if (byte == 0x0d) { // APR (active position return)::(move to first position of the line)
          printf("Closed Caption:: data unit: <APR>\n");
        } else if (byte == 0x0e) { // LS1
          printf("Closed Caption:: data unit: <LS1>\n");
        } else if (byte == 0x0f) { // LS0
          printf("Closed Caption:: data unit: <LS0>\n");
        } else if (byte == 0x10) { // 
        } else if (byte == 0x11) { // 
        } else if (byte == 0x12) { // 
        } else if (byte == 0x13) { // 
        } else if (byte == 0x14) { // 
        } else if (byte == 0x15) { // 
        } else if (byte == 0x16) { // PAPF (parameterized active position forward::[length])::(set the cursor forward a number of times)
          printf("Closed Caption:: data unit: <PAPF>::[0x%02x]\n", *code++);
        } else if (byte == 0x17) { // 
        } else if (byte == 0x18) { // CAN (cancel)::(cover from the current position to the end of line with background color)
          printf("Closed Caption:: data unit: <CAN>\n");
        } else if (byte == 0x19) { // SS2 (single shift 2)::(invoke character code set)
          printf("Closed Caption:: data unit: <SS2>\n");
        } else if (byte == 0x1a) { // 
        } else if (byte == 0x1b) { // ESC
          printf("Closed Caption:: data unit: <ESC>\n");
        } else if (byte == 0x1c) { // APS (active position set::[row, column])::(set the current position of cursor)
          uint8_t
            param1 = *code++,
                   param2 = *code++;

          printf("Closed Caption:: data unit: <APS>::[0x%02x, 0x%02x]\n", param1, param2);
        } else if (byte == 0x1d) { // SS3 (single shift 3)::(special characters escape [G3])
          printf("Closed Caption:: data unit: <SS3>::[0x%02x]\n", *code++);
        } else if (byte == 0x1e) { // RS (record separator)::(information division code)
          printf("Closed Caption:: data unit: <RS>\n");
        } else if (byte == 0x1f) { // US (unit separator)::(information division code)
          printf("Closed Caption:: data unit: <US>\n");
          // } else if (byte == 0x20) { // SP (space)
          //   printf("Closed Caption:: data unit: <CHAR>::[%c]\n", (char)'\x20');
        } else if (byte == 0x7f) { // DEL (delete)
          printf("Closed Caption:: data unit: <DEL>\n");
        } else if (byte == 0x80) { // BKF (black foreground)::(set foreground color to black)
          printf("Closed Caption:: data unit: <BKF>\n");
        } else if (byte == 0x81) { // RDF (red foreground)::(set foreground color to red)
          printf("Closed Caption:: data unit: <RDF>\n");
        } else if (byte == 0x82) { // GRF (green foreground)::(set foreground color to green)
          printf("Closed Caption:: data unit: <GRF>\n");
        } else if (byte == 0x83) { // YLF (yellow foreground)::(set foreground color to yellow)
          printf("Closed Caption:: data unit: <YLF>\n");
        } else if (byte == 0x84) { // BLF (blue foreground)::(set foreground to blue)
          printf("Closed Caption:: data unit: <BLF>\n");
        } else if (byte == 0x85) { // MGF (magenta foreground)::(set foreground to magenta)
          printf("Closed Caption:: data unit: <MGF>\n");
        } else if (byte == 0x86) { // CNF (cyan foreground)::(set foreground to cyan)
          printf("Closed Caption:: data unit: <CNF>\n");
        } else if (byte == 0x87) { // WHF (white foreground)::(set foreground to white)
          printf("Closed Caption:: data unit: <WHF>\n");
        } else if (byte == 0x88) { // SSZ (small size)::(set character size to small)
          printf("Closed Caption:: data unit: <SSZ>\n");
        } else if (byte == 0x89) { // MSZ (middle size)::(set character size to middle)
          printf("Closed Caption:: data unit: <MSZ>\n");
        } else if (byte == 0x8a) { // NSZ (normal size)::(set character size to normal)
          printf("Closed Caption:: data unit: <NSZ>\n");
        } else if (byte == 0x8b) { // SZX (character size controls::[size])::(set size of character)
          printf("Closed Caption:: data unit: <SZX>::[0x%02x]\n", *code++);
        } else if (byte == 0x8c) { // 
        } else if (byte == 0x8d) { // 
        } else if (byte == 0x8e) { // 
        } else if (byte == 0x8f) { // 
        } else if (byte == 0x90) { // COL (color controls::[color])::(set foreground, background, half foreground, half background and CMLA colors)
          printf("Closed Caption:: data unit: <COL>::[0x%02x]\n", *code++);
        } else if (byte == 0x91) { // FLC (flashing control::[param])::(set beginning and end of flashing)
          printf("Closed Caption:: data unit: <FLC>::[0x%02x]\n", *code++);
        } else if (byte == 0x92) { // CDC (conceal display controls::[param1[, param2]])::(set beginning and end of concealing)
          uint8_t 
            param1 = *code++,
                   param2 = 0;

          if (param1 == 0x20) {
            param2 = *code++;
          }

          printf("Closed Caption:: data unit: <CDC>::[0x%02x, 0x%02x]\n", param1, param2);
        } else if (byte == 0x93) { // POL (pattern polarity controls)::(set pattern and polarity of the character and the mosaic)
          printf("Closed Caption:: data unit: <POL>::[0x%02x]\n", *code++);
        } else if (byte == 0x94) { // WMM (writing mode modification)::(change the writing mode to the memory of display)
          printf("Closed Caption:: data unit: <WMM>::[0x%02x]\n", *code++);
        } else if (byte == 0x95) { // MACRO (macro definition)::(???)
          printf("Closed Caption:: data unit: <MACRO>::[TODO]\n");
        } else if (byte == 0x96) { // 
        } else if (byte == 0x97) { // HLC (highlighting control)::(set start and ending of enclosure)
          printf("Closed Caption:: data unit: <HLC>::[0x%02x]\n", *code++);
        } else if (byte == 0x98) { // RPC (repeat character)::(cause a displayable character or mosaic the immediately follows the code)
          printf("Closed Caption:: data unit: <RPC>::[0x%02x]\n", *code++);
        } else if (byte == 0x99) { // SPL (stop lining)::(underlining and mosaic division process is terminated)
          printf("Closed Caption:: data unit: <SPL>\n");
        } else if (byte == 0x9a) { // STL (start lining)::(starts the lining process)
          printf("Closed Caption:: data unit: <STL>\n");
        } else if (byte == 0x9b) { // CSI
          std::string params;

          do {
            params.append((char *)code++, 1);
          } while (*code != 0x20);

          code++; // 0x20

          uint8_t control = *code++;

          std::string info = "unknown";

          if (control == 0x42) {
            info = "character deformation";
          } else if (control == 0x53) {
            info = "set writing form";
          } else if (control == 0x54) {
            info = "composite character composition";
          } else if (control == 0x56) {
            info = "set display format";
          } else if (control == 0x57) {
            info = "character composite data designation";
          } else if (control == 0x58) {
            info = "set horizontal spacing";
          } else if (control == 0x59) {
            info = "set vertical spacing";
          } else if (control == 0x5d) {
            info = "colouring block";
          } else if (control == 0x5e) {
            info = "raster colour designation";
          } else if (control == 0x5f) {
            info = "set display position";
          } else if (control == 0x61) {
            info = "active coordinate position set";
          } else if (control == 0x62) {
            info = "witch control";
          } else if (control == 0x63) {
            info = "ornament control";
          } else if (control == 0x64) {
            info = "font";
          } else if (control == 0x65) {
            info = "character font set";
          } else if (control == 0x66) {
            info = "external character set";
          } else if (control == 0x68) {
            info = "build-in sound replay";
          } else if (control == 0x69) {
            info = "alternative character set";
          } else if (control == 0x6e) {
            info = "raster colour command";
          } else if (control == 0x6f) {
            info = "skip character set";
          }

          printf("Closed Caption:: data unit: <CSI>::[0x%02x]:[%s]\n", control, info.c_str());
        } else if (byte == 0x9c) { // 
        } else if (byte == 0x9d) { // TIME (time control)
          uint8_t
            param1 = *code++,
                   param2 = *code++;

          printf("Closed Caption:: data unit: <TIME>::[0x%02x, 0x%02x]\n", param1, param2);
        } else if (byte == 0x9e) { // 
        } else if (byte == 0x9f) { // 
        } else if (byte == 0xa0) { // 10/0
        } else if (byte == 0xff) { // 15/15
        } else {
          text = text + (char)byte;

          printf("Closed Caption:: data unit: <CHAR>::[%c]\n", (char)byte);
        }
      }

      if (text.empty() == false) {
        _lines.push_back(text);
      }
    }

    void Print()
    {
      printf("SISubtitle:: number of lines:[%ld]\n", _lines.size());

      for (int i=0; i<(int)_lines.size(); i++) {
        printf("\t:%04d:[%s]\n", i, _lines[i].c_str());
      }
    }
};

class SIFacade {

  private:
    std::vector<std::shared_ptr<SIService>>
      _services;
    std::mutex
      _services_mutex;
    std::vector<std::shared_ptr<SINetwork>>
      _networks;
    std::mutex
      _networks_mutex;
    std::vector<std::shared_ptr<SIEvent>> 
      _events;
    std::mutex
      _events_mutex;
    std::shared_ptr<SIData>
      _data;
    std::mutex
      _data_mutex;
    std::shared_ptr<SISubtitle>
      _subtitle;
    std::mutex
      _subtitle_mutex;
    SITime
      _time;
    std::mutex
      _time_mutex;

  private:
    SIFacade()
    {
      _data = std::make_shared<SIData>();
      _subtitle = std::make_shared<SISubtitle>();
    }

  public:
    virtual ~SIFacade()
    {
    }

    static SIFacade * GetInstance()
    {
      static SIFacade 
        *instance = new SIFacade();

      return instance;
    }

    void Service(std::shared_ptr<SIService> &param)
    {
      std::lock_guard<std::mutex> 
        lock(_services_mutex);

      for (auto service : _services) {
        if (service->ServiceID() == param->ServiceID()) {
          return;
        }
      }
       
      _services.push_back(param);
    }

    std::vector<std::shared_ptr<SIService>> Services()
    {
      std::lock_guard<std::mutex> 
        lock(_services_mutex);

      return _services;
    }

    std::shared_ptr<SIService> Service(int service_id)
    {
      std::lock_guard<std::mutex> 
        lock(_services_mutex);

      for (auto service : _services) {
        if (service->ServiceID() == service_id) {
          return service;
        }
      }

      return nullptr;
    }

    void Network(std::shared_ptr<SINetwork> &param)
    {
      std::lock_guard<std::mutex> 
        lock(_networks_mutex);

      for (auto network : _networks) {
        if (network->TransportStreamName() == param->TransportStreamName()) {
          return;
        }
      }

      _networks.push_back(param);
    }

    std::vector<std::shared_ptr<SINetwork>> Networks()
    {
      std::lock_guard<std::mutex> 
        lock(_networks_mutex);

      return _networks;
    }
    
    void Event(std::shared_ptr<SIEvent> &param)
    {
      std::lock_guard<std::mutex> 
        lock(_events_mutex);

      for (auto event : _events) {
        if (event->ServiceID() == param->ServiceID() and event->EventID() == param->EventID()) {
          return;
        }
      }

      _events.push_back(param);

      std::sort(_events.begin(), _events.end(), 
          [](const std::shared_ptr<SIEvent> &a, const std::shared_ptr<SIEvent> &b) {
            return (a->EventID() < b->EventID());
          });
    }

    std::vector<std::shared_ptr<SIEvent>> Events()
    {
      std::lock_guard<std::mutex> 
        lock(_events_mutex);

      return _events;
    }
    
    std::vector<std::shared_ptr<SIEvent>> Events(int service_id)
    {
      std::vector<std::shared_ptr<SIEvent>>
        events = Events();

      events.erase(
          std::remove_if(events.begin(), events.end(), [service_id=service_id](std::shared_ptr<SIEvent> event) {
            return event->ServiceID() != service_id;
          }), events.end());

      return events;
    }
    
    std::shared_ptr<SIData> Data()
    {
      std::lock_guard<std::mutex> 
        lock(_data_mutex);

      return _data;
    }
    
    std::shared_ptr<SISubtitle> Subtitle()
    {
      std::lock_guard<std::mutex> 
        lock(_subtitle_mutex);

      return _subtitle;
    }
    
    void Time(SITime &param)
    {
      std::lock_guard<std::mutex> 
        lock(_time_mutex);

      _time = param;
    }

    SITime Time()
    {
      std::lock_guard<std::mutex> 
        lock(_time_mutex);

      return _time;
    }
    
};

class PSIParser : public jevent::DemuxListener {

  private:
    std::map<int, SIService::stream_type_t> _stream_types;
    std::vector<jmpeg::Demux *> _demuxes;

  private:
    void StopDemux(std::string id)
    {
      for (std::vector<jmpeg::Demux *>::iterator i=_demuxes.begin(); i!=_demuxes.end(); i++) {
        jmpeg::Demux *demux = (*i);

        if (demux->GetID() == id) {
          printf("Demux [%s] stopped\n", id.c_str());

          demux->Stop();

          _demuxes.erase(i);

          return;
        }
      }
     
      printf("Demux [%s] no exists\n", id.c_str());
    }

  public:
    PSIParser()
    {
      _stream_types[0x00] = SIService::stream_type_t::RESERVED;
      _stream_types[0x01] = SIService::stream_type_t::VIDEO;
      _stream_types[0x02] = SIService::stream_type_t::VIDEO;
      _stream_types[0x03] = SIService::stream_type_t::AUDIO;
      _stream_types[0x04] = SIService::stream_type_t::AUDIO;
      _stream_types[0x05] = SIService::stream_type_t::PRIVATE;
      _stream_types[0x06] = SIService::stream_type_t::SUBTITLE;
      _stream_types[0x0b] = SIService::stream_type_t::DSMCC_MESSAGE;
      _stream_types[0x0c] = SIService::stream_type_t::DSMCC_DESCRIPTOR;
      _stream_types[0x0d] = SIService::stream_type_t::DSMCC_SECTION;
      _stream_types[0x0f] = SIService::stream_type_t::AUDIO;
      _stream_types[0x10] = SIService::stream_type_t::VIDEO;
      _stream_types[0x11] = SIService::stream_type_t::AUDIO;
      _stream_types[0x1b] = SIService::stream_type_t::VIDEO;
      _stream_types[0x24] = SIService::stream_type_t::VIDEO;
      _stream_types[0x42] = SIService::stream_type_t::VIDEO;
      _stream_types[0x80] = SIService::stream_type_t::LIBRAS_MESSAGE; // TODO:: no normative
      _stream_types[0x81] = SIService::stream_type_t::AUDIO;
      _stream_types[0x82] = SIService::stream_type_t::AUDIO;
      _stream_types[0x83] = SIService::stream_type_t::AUDIO;
      _stream_types[0x84] = SIService::stream_type_t::AUDIO;
      _stream_types[0x85] = SIService::stream_type_t::AUDIO;
      _stream_types[0x86] = SIService::stream_type_t::AUDIO;
      _stream_types[0x87] = SIService::stream_type_t::AUDIO;
      _stream_types[0x88] = SIService::stream_type_t::LIBRAS_STREAM;
      _stream_types[0x89] = SIService::stream_type_t::LIBRAS_MESSAGE;
      _stream_types[0x90] = SIService::stream_type_t::SUBTITLE;
      _stream_types[0xd1] = SIService::stream_type_t::VIDEO;
      _stream_types[0xea] = SIService::stream_type_t::VIDEO;

      StartPSIDemux("pat", TS_PAT_PID, TS_PAT_TABLE_ID, TS_PAT_TIMEOUT);
    }

    virtual ~PSIParser()
    {
      for (std::vector<jmpeg::Demux *>::iterator i=_demuxes.begin(); i!=_demuxes.end(); i++) {
        jmpeg::Demux *demux = (*i);

        demux->Stop();
        delete demux;
      }

      _demuxes.clear();
    }

    void StartRawDemux(std::string id, int pid, int timeout)
    {
      for (std::vector<jmpeg::Demux *>::iterator i=_demuxes.begin(); i!=_demuxes.end(); i++) {
        jmpeg::Demux *demux = (*i);

        if (demux->GetID() == id) {
          printf("Demux [%s] already created\n", id.c_str());

          return;
        }
      }

      jmpeg::RawDemux *demux = new jmpeg::RawDemux();

      demux->SetID(id);
      demux->RegisterDemuxListener(this);
      demux->SetPID(pid);
      demux->SetTimeout(std::chrono::milliseconds(timeout));
      demux->Start();

      _demuxes.push_back(demux);
    }

    void StartPSIDemux(std::string id, int pid, int tid, int timeout)
    {
      for (std::vector<jmpeg::Demux *>::iterator i=_demuxes.begin(); i!=_demuxes.end(); i++) {
        jmpeg::Demux *demux = (*i);

        if (demux->GetID() == id) {
          printf("Demux [%s] already created\n", id.c_str());

          return;
        }
      }

      jmpeg::PSIDemux *demux = new jmpeg::PSIDemux();

      demux->SetID(id);
      demux->RegisterDemuxListener(this);
      demux->SetPID(pid);
      demux->SetTID(tid);
      demux->SetTimeout(std::chrono::milliseconds(timeout));
      demux->SetCRCCheckEnabled(false);
      demux->Start();

      _demuxes.push_back(demux);
    }

    void StartPrivateDemux(std::string id, int pid, int tid, int timeout)
    {
      for (std::vector<jmpeg::Demux *>::iterator i=_demuxes.begin(); i!=_demuxes.end(); i++) {
        jmpeg::Demux *demux = (*i);

        if (demux->GetID() == id) {
          printf("Demux [%s] already created\n", id.c_str());

          return;
        }
      }

      jmpeg::PrivateDemux *demux = new jmpeg::PrivateDemux();

      demux->SetID(id);
      demux->RegisterDemuxListener(this);
      demux->SetPID(pid);
      demux->SetTID(tid);
      demux->SetTimeout(std::chrono::milliseconds(timeout));
      demux->SetCRCCheckEnabled(false);
      demux->Start();

      _demuxes.push_back(demux);
    }

    void StartPESDemux(std::string id, int pid, int timeout)
    {
      for (std::vector<jmpeg::Demux *>::iterator i=_demuxes.begin(); i!=_demuxes.end(); i++) {
        jmpeg::Demux *demux = (*i);

        if (demux->GetID() == id) {
          printf("Demux [%s] already created\n", id.c_str());

          return;
        }
      }

      jmpeg::PESDemux *demux = new jmpeg::PESDemux();

      demux->SetID(id);
      demux->RegisterDemuxListener(this);
      demux->SetPID(pid);
      demux->SetTimeout(std::chrono::milliseconds(timeout));
      demux->Start();

      _demuxes.push_back(demux);
    }

    virtual void AITDescriptorDump(const char *data, int length)
    {
      int descriptor_tag = TS_G8(data);
      int descriptor_length = length - 2; // TS_G8(data + 1);
      const char *ptr = data + 2;

      printf("\tDescriptor:: tag:[0x%02x], length:[%d]::[%s]\n", descriptor_tag, descriptor_length, Utils::GetAITDescriptorName(descriptor_tag).c_str());

      if (descriptor_tag == 0x00) { // application descriptor
        int application_profile_length = TS_G8(ptr + 0);

        printf("\t\t:: application profile length:[%d]\n", application_profile_length);

        int profile_current_byte = 0;

        while (profile_current_byte < application_profile_length) {
          int application_profile = TS_G16(ptr + profile_current_byte + 1);
          int version_major = TS_G8(ptr + profile_current_byte + 3);
          int version_minor = TS_G8(ptr + profile_current_byte + 4);
          int version_micro = TS_G8(ptr + profile_current_byte + 5);

          printf("\t\t:: application profile:[0x%04x], version:[%d.%d.%d]\n", application_profile, version_major, version_minor, version_micro);

          profile_current_byte = profile_current_byte + 5;  
        }

        // int bound_visibility = TS_G8(ptr + profile_current_byte + 1);
        // int bound_flag = (bound_visibility & 0x80) >> 7;
        // int visibility = (bound_visibility & 0x60) >> 5;
        // int application_priotiry = TS_G8(ptr + profile_current_byte + 2);
      } else if (descriptor_tag == 0x01) { // application name descriptor
        int count = 0;

        while (count < descriptor_length) {
          std::string language_code = std::string((ptr + count), 3);
          int name_length = TS_G8(ptr + 3 + count);
          std::string name = std::string(ptr + 4 + count, name_length);

          printf("\t\t:: application language:[%s], name:[%s]\n", language_code.c_str(), name.c_str());

          count = count + 5 + name_length;

          break; // we can get more than one app name ='[
        } 
      } else if (descriptor_tag == 0x02) { // transport protocol descriptor
        int protocol_id = TS_G16(ptr + 0);
        int transpor_protocol_label = TS_G8(ptr + 2);

        printf("\t\t:: transport protocol tag:[0x%04x/%s], transport_protocol_label:[0x%02x]\n", protocol_id, Utils::GetTransportProtocolDescription(protocol_id).c_str(), transpor_protocol_label);

        if (protocol_id == 0x01) {
          int remote_connection = TS_GM8(ptr + 3, 0, 1);

          if (remote_connection == 0x01) {
            int original_network_id = TS_G16(ptr + 4);
            int transport_stream_id = TS_G16(ptr + 6);
            int service_id = TS_G16(ptr + 8);
            int component_tag = TS_G8(ptr + 9);

            printf("\t\t:: original network id:[0x%04x], transport_stream_id:[0x%04x], service id:[0x%04x], component tag:[0x%04x]\n", original_network_id, transport_stream_id, service_id, component_tag);
          } else {
            int component_tag = TS_G8(ptr + 4);

            printf("\t\t:: component tag:[0x%04x]\n", component_tag);

            // TODO:: initialize dsmcc here
          }
        } 
      } else if (descriptor_tag == 0x03 || descriptor_tag == 0x06) {  // gingaj application descriptor, gingancl application descriptor
        int count = 0;

        while (count < descriptor_length) {
          int param_length = TS_G8(ptr + count);
          std::string param(ptr + count + 1, param_length);

          printf("\t\t:: param:[%s]\n", param.c_str());

          count = count + 1 + param_length;
        }
      } else if (descriptor_tag == 0x04 || descriptor_tag == 0x07) {  // gingaj location descriptor, gingancl location descriptor
        int base_directory_length = TS_G8(ptr + 0);
        std::string base_directory(ptr + 1, base_directory_length);
        int class_extension_length = TS_G8(ptr + 1 + base_directory_length);
        std::string class_extension = std::string(ptr + 2 + base_directory_length, class_extension_length);
        int main_file_length = descriptor_length - base_directory_length - class_extension_length - 2;
        std::string main_file = std::string(ptr + 2 + base_directory_length + class_extension_length, main_file_length);

        printf("\t\t:: base_directory:[%s], class extension:[%s], main_file:[%s]\n", base_directory.c_str(), class_extension.c_str(), main_file.c_str());
      } else if (descriptor_tag == 0x05) { // external application authorrisation descriptor
        int count = 0;

        while (count < descriptor_length) {
          uint32_t oid = TS_G32(ptr + 0); 
          int aid = TS_G16(ptr + 4);
          int application_priority = TS_G8(ptr + 6);

          printf("\t\t:: oid:[0x%08x], aid:[0x%04x], application priority:[%d]\n", oid, aid, application_priority);

          ptr = ptr + 7;
          count = count + 7;
        }
      } else if (descriptor_tag == 0x0b) { // application icons descriptor
        int icon_locator_length = TS_G8(ptr + 0);
        std::string icon_locator(ptr + 1, icon_locator_length);
        int icon_flags = TS_G8(ptr + 1 + icon_locator_length);
        
        printf("\t\t:: icon locator:[%s], icon flags:[0x%02x]\n", icon_locator.c_str(), icon_flags);
      } else if (descriptor_tag == 0x0c) { // prefetch descriptor
        int transport_protocol_label = TS_G8(ptr + 0);

        printf("\t\t:: transport protocol label:[%d]\n", transport_protocol_label);

        int loop_length = descriptor_length - 1;
        int count = 0;

        ptr = ptr + 1;

        while (count < loop_length) {
          int label_length = TS_G8(ptr + 0);
          std::string label(ptr + 1 + label_length);
          int prefetch_priority = TS_G8(ptr + 1 + label_length);

          printf("\t\t:: label:[%s], prefetch priority:[0x%02x]\n", label.c_str(), prefetch_priority);

          ptr = ptr + 1 + label_length + 1;
          count = count + 1 + label_length + 1;
        }
      } else if (descriptor_tag == 0x0d) { // dii location descriptor
      } else if (descriptor_tag == 0x11) { // ip signalling descriptor
        int platform_id = TS_GM32(ptr, 0, 24);

        printf("\t\t:: ip signaliing:[%d]\n", platform_id);
      // } else if (descriptor_tag == 0xfd) { // data coding descriptor
      } else {
        DumpBytes("Data", ptr, descriptor_length);
      }
    }

    virtual void DescriptorDump(SI *si, const char *data, int length)
    {
      int descriptor_tag = TS_G8(data);
      int descriptor_length = length - 2; // TS_G8(data + 1);
      const char *ptr = data + 2;

      printf("\tDescriptor:: tag:[0x%02x], length:[%d]::[%s]\n", descriptor_tag, descriptor_length, GetDescriptorName(descriptor_tag).c_str());

      if (descriptor_tag == 0x13) { // carousel identifier descriptor
        const char *end = ptr + descriptor_length;

        uint32_t carousel_id = TS_G32(ptr + 0);
        int format_id = TS_G8(ptr + 4);
        std::string format = "No FormatterSpecifier";

        if (format_id == 0x01) {
          format = "This FormatSpecifier is an aggregation of the fields necessary to locate the ServiceGateway";
        } else if (format_id > 0x01 && format_id <= 0x7f) {
          format = "Reserved for future use of DVB";
        } else if (format_id > 0x7f && format_id <= 0xff) {
          format = "Reserved for private use";
        }

        printf("\t\t:: carousel id:[0x%08x], format id:[0x%02x]::[%s]\n", carousel_id, format_id, format.c_str());

        ptr = ptr + 5;

        if (format_id == 0x01) {
          int module_version = TS_G8(ptr + 0);
          int module_id = TS_G16(ptr + 1);
          // int block_size = TS_G16(ptr + 3);
          // uint32_t module_size = TS_G32(ptr + 5);
          // int compression_method = TS_G8(ptr + 9);
          // uint32_t original_size = TS_G32(ptr + 10);
          int timeout = TS_G8(ptr + 14);
          int object_key_length = TS_G8(ptr + 15);

          printf("\t\t:: module version:[0x%02x], module id:[0x%04x], timeout:[%d]\n", module_version, module_id, timeout);
        
          if (object_key_length > 0) {
            DumpBytes("ObjectKey Data", ptr + 16, object_key_length);
          }

          ptr = ptr + 16 + object_key_length;
        }
        
        int private_length = end - ptr;

        if (private_length > 0) {
          DumpBytes("Private Data", ptr, private_length);
        }
      } else if (descriptor_tag == 0x06) { // location descriptor
        int location_tag = TS_G8(ptr + 0);

        printf("\t\t:: location tag:[0x%02x]\n", location_tag);
      } else if (descriptor_tag == 0x0a) { // iso 639 language descriptor
        std::string language = std::string(ptr + 0, 3);

        printf("\t\t:: language:[%s]\n", language.c_str());
      } else if (descriptor_tag == 0x14) { // association tag descriptor
        const char *end = ptr + descriptor_length;

        int association_tag = TS_G16(ptr + 0);
        int use_id = TS_G16(ptr + 2);
        std::string use = "Unknown";

        if (use_id == 0x0000) {
          use = "DSI with IOR of SGW";
        } else if (use_id >= 0x0100 && use_id <= 0x1fff) {
          use = "DVD reserved";
        } else if (use_id >= 0x2000 && use_id <= 0xffff) {
          use = "User private";
        }

        printf("\t\t:: association tag:[0x%04x], use id:[0x%04x]::[%s]\n", association_tag, use_id, use.c_str());

        int selector_length = TS_G8(ptr + 4);

        /*
        if (use_id == 0x0000) {
          // selection_length == 0x08
          uint32_t transation_id = TS_G32(ptr + 5);
          int timeout = TS_G16(ptr + 9);
        } else if (use_id == 0x0001) {
          // selection_length == 0x00
        } else {
          DumpBytes("Private Data", ptr + 5, selector_length);
        }
        */
          
        if (selector_length > 0) {
          DumpBytes("Selector Bytes", ptr + 5, selector_length);
        }

        ptr = ptr + 5 + selector_length;

        int private_length = end - ptr;

        if (private_length > 0) {
          DumpBytes("Private Data", ptr, private_length);
        }
      } else if (descriptor_tag == 0x15) { // extension_tag_descriptor or deferred association tags descriptor
        const char *end = ptr + descriptor_length;

        int association_tags_loop_length = TS_G8(ptr + 0);

        for (int i=0; i<association_tags_loop_length; i+=2) {
          int association_tag = TS_G16(ptr + 1 + i);

          printf("\t\t:: association tag:[%d] = 0x%04x\n", i, association_tag);
        }

        ptr = ptr + association_tags_loop_length;

        int transport_stream_id = TS_G16(ptr + 0);
        int program_number = TS_G16(ptr + 2);
        int original_network_id = TS_G16(ptr + 4);

        printf("\t\t:: transport stream id:[0x%04x], program number:[0x%04x], original network id::[0x%04x]\n", transport_stream_id, program_number, original_network_id);

        ptr = ptr + 6;

        int private_length = end - ptr;

        if (private_length > 0) {
          DumpBytes("Private Data", ptr, private_length);
        }
      } else if (descriptor_tag == 0x40) { // network name descriptor
        std::string name(ptr, descriptor_length);

        name = Utils::ISO8859_1_TO_UTF8(name);

        printf("\t\t:: name:[%s]\n", name.c_str());
      } else if (descriptor_tag == 0x41) { // service list descriptor
        int services_loop_length = descriptor_length;
        int services_loop_count = 0;

        while (services_loop_count < services_loop_length) {
          int service_id = TS_G16(ptr + 0);
          int service_type = TS_G8(ptr + 2);

          printf("\t\t:: service id:[0x%04x], service type:[0x%02x]::[%s]\n", service_id, service_type, GetServiceDescription(service_type).c_str());

          ptr = ptr + 3;

          services_loop_count = services_loop_count + 3;
        }
      } else if (descriptor_tag == 0x48) { // service descriptor [ABNTNBR 15603-2 2009]
        int service_type = TS_G8(ptr + 0); // 0x01: HD, 0xXX: LD
        int service_provider_name_length = TS_G8(ptr + 1);
        std::string service_provider_name(ptr + 2, service_provider_name_length);
        int service_name_length = TS_G8(ptr + 2 + service_provider_name_length);
        std::string service_name(ptr + 3 + service_provider_name_length, service_name_length);

        service_provider_name = Utils::ISO8859_1_TO_UTF8(service_provider_name);
        service_name = Utils::ISO8859_1_TO_UTF8(service_name);

        SIService *param = dynamic_cast<SIService *>(si);
        
        if (param != nullptr) {
          if (service_type == 0x01) {
            param->ServiceType(SIService::service_type_t::HD);
          } else if (service_type == 0xc0) {
            param->ServiceType(SIService::service_type_t::LD);
          } else {
            param->ServiceType(SIService::service_type_t::SD);
          }

          param->ServiceProvider(service_provider_name);
          param->ServiceName(service_name);
        }

        printf("\t\t:: service type:[0x%02x/%s], service provider name:[%s], service name:[%s]\n", service_type, GetServiceDescription(service_type).c_str(), service_provider_name.c_str(), service_name.c_str());
      } else if (descriptor_tag == 0x49) { // country availability descriptor
        int country_availability_flag = TS_G8(ptr + 0);
        std::string country(ptr + 1, 3);
        
        printf("\t\t:: country availability flag:[%d], country:[%s]\n", country_availability_flag, country.c_str());
      } else if (descriptor_tag == 0x4d) { // short event descriptor
        std::string language = std::string(ptr, 3);

        ptr = ptr + 3;

        int event_name_length = TS_G8(ptr + 0);
        std::string event_name(ptr + 1, event_name_length);

        ptr = ptr + 1 + event_name_length;

        int text_length = TS_G8(ptr + 0);
        std::string text(ptr + 1, text_length);

        SIEvent *param = dynamic_cast<SIEvent *>(si);

        if (param != nullptr) {
          param->EventName(event_name);
          param->Description(text);
        }

        printf("\t\t:: language:[%s], event name:[%s], text:[%s]\n", language.c_str(), event_name.c_str(), text.c_str());
      } else if (descriptor_tag == 0x4e) { // extended event descriptor
        int descriptor_number = TS_GM8(ptr + 0, 0, 4);
        int last_descriptor_number = TS_GM8(ptr + 0, 4, 4);
        std::string language(ptr + 1, 3);
        
        printf("\t\t:: descriptor number:[0x%02x], last descriptor number:[0x%02x], language:[%s]\n", descriptor_number, last_descriptor_number, language.c_str());

        int length_of_items = TS_G8(ptr + 4);
        int count = 0;

        ptr = ptr + 5;

        while (count < length_of_items) {
          int item_description_length = TS_G8(ptr + 0);
          std::string item_description(ptr + 1, item_description_length);
          int item_length = TS_G8(ptr + 1 + item_description_length);
          std::string item(ptr + 1 + item_description_length + 1, item_length);

          ptr = ptr + item_description_length + item_length + 2;

          count = count + item_description_length + item_length + 2;  

          printf("\t\t:: item description:[%s], item:[%s]\n", item_description.c_str(), item.c_str());
        }
      } else if (descriptor_tag == 0x50) { // component descriptor 
        const char *end = ptr + descriptor_length;

        // int reserved = TS_GM8(ptr, 0, 4);
        int stream_content = TS_GM8(ptr, 4, 4);
        int component_type = TS_G8(ptr + 1);
        int component_tag = TS_G8(ptr + 2);
        std::string language(ptr + 3, 3);

        printf("\t\t:: stream content:[0x%02x], component type:[0x%02x]:[%s], component tag:[0x%02x], language:[%s]\n", stream_content, component_type, GetComponentDescription(stream_content, component_type).c_str(), component_tag, language.c_str());

        ptr = ptr + 6;

        int private_length = end - ptr;

        if (private_length > 0) {
          DumpBytes("Text Char", ptr, private_length);
        }
      } else if (descriptor_tag == 0x52) { // stream identifier descriptor
        int component_tag = TS_G8(ptr + 0);

        printf("\t\t:: component tag:[0x%02x]\n", component_tag);
      } else if (descriptor_tag == 0x54) { // content descriptor
        int content_nibble = TS_G8(ptr + 0);

        int genre = (content_nibble >> 4) & 0x0f;
        int genre_info = (content_nibble >> 0) & 0x0f;

        printf("\t\t:: description:: %s\n", Utils::GetGenreDescription(genre, genre_info).c_str());
      } else if (descriptor_tag == 0x55) { // parental rating descriptor
        std::string country = std::string(ptr, 3);
        int rate = TS_G8(ptr + 3);

        int rate_age = (rate >> 0) & 0xff; 
        int rate_content = (rate >> 4) & 0x0f;

        printf("\t\t:: country:[%s], age:[%s], content:[%s]\n", country.c_str(), Utils::GetParentalAgeDescription(rate_age).c_str(), Utils::GetParentalContentDescription(rate_content).c_str());
      } else if (descriptor_tag == 0x58) { // local time offset descriptor
        std::string country = std::string(ptr, 3);
        int country_region_id = TS_GM8(ptr + 3, 0, 6);
        int local_time_offset_polarity = TS_GM8(ptr + 3, 7, 1);
        int local_time_offset = TS_G16(ptr + 4);
        // uint64_t time_of_change = TS_GM64(ptr + 6, 0, 40);
        int next_time_offset = TS_G16(ptr + 11);

        // UTC-3
        int julian_date = TS_G16(ptr + 6);

        int Y, M, D, WD, h, m, s;

        Utils::ParseJulianDate(julian_date, Y, M, D, WD);

        h = DHEX2DEC(ptr[8]);
        m = DHEX2DEC(ptr[9]);
        s = DHEX2DEC(ptr[10]);

        SITime *param = dynamic_cast<SITime *>(si);
        
        if (param != nullptr) {
          param->Country(country);
        }

        printf("\t\t:: country:[%s], country id:[%d], local time offset polarity:[%d], local time offset:[%d], time of change:[%02d%02d%02d-%02d%02d%02d], next time offset:[0x%04x]\n", country.c_str(), country_region_id, local_time_offset_polarity, local_time_offset, Y, M, D, h, m, s, next_time_offset);
        // printf(":: country:[%s], country id:[%d], local time offset polarity:[%d], local time offset:[%d], time of change:[%lu], next time offset:[0x%04x]\n", country.c_str(), country_region_id, local_time_offset_polarity, local_time_offset, time_of_change, next_time_offset);
      } else if (descriptor_tag == 0x59) { // subtitling descriptor
        int count = descriptor_length/8;

        for (int i=0; i<count; i++) {
          std::string country = std::string(ptr, 3);
          int subtitling_type = TS_G8(ptr + 3);
          int composition_page_id = TS_G8(ptr + 4);
          int ancillary_page_id = TS_G8(ptr + 6);

          printf("\t\t:: country:[%s], subtitle type:[0x%02x/%s], composition page id:[%d], ancillary page id:[%d]\n", country.c_str(), subtitling_type, GetComponentDescription(0x03, subtitling_type).c_str(), composition_page_id, ancillary_page_id);
        }
      } else if (descriptor_tag == 0x7c) { // aac descriptor
        const char *end = ptr + descriptor_length;

        int profile_and_level = TS_G8(ptr + 0);
        int aac_type_flag = TS_GM8(ptr + 1, 0, 1);
        int aac_type = -1;

        ptr = ptr + 2;

        if (aac_type_flag == 1) {
          aac_type = TS_G8(ptr + 0);

          ptr = ptr + 1;
        }
  
        printf("\t\t:: profile and level:[0x%02x/%s], aac type flag:[%d], aac type:[%d]\n", profile_and_level, Utils::GetAACProfileAndLevelDescription(profile_and_level).c_str(), aac_type_flag, aac_type);

        int private_length = end - ptr;

        if (private_length > 0) {
          DumpBytes("Additional Info", ptr, private_length);
        }
      } else if (descriptor_tag == 0xa3) { // component name descriptor [ATSC A/65A, ATSC Working Draft]
        // int reserved = TS_GM8(ptr + 0, 0, 4);
        int number_strings = TS_G8(ptr + 0);

        printf("\t\t:: number strings :[%d]\n", number_strings);

        for (int i=0; i<number_strings; i++) {
          std::string ISO_639_language_code(ptr + 1, 3);
          int number_segments = TS_G8(ptr + 4);

          ptr = ptr + 5;

          printf("\t\t:: language:[%s], number segments:[%d]\n", ISO_639_language_code.c_str(), number_segments);

          for (int j=0; j<number_segments; j++) {
            int compression_type = TS_G8(ptr + 0);
            int mode = TS_G8(ptr + 1);
            int number_bytes = TS_G8(ptr + 2);

            ptr = ptr + 3;

            printf("\t\t:: compression type:[%s], mode:[%s]\n", Utils::GetLanguageCompressionDescription(compression_type).c_str(), Utils::GetLanguageUnicodeModeDescription(mode).c_str());
              
            DumpBytes("Compressed String Bytes", ptr, number_bytes);
          }
        }
      } else if (descriptor_tag == 0xc4) { // audio component descriptor
        // int reserved = TS_GM8(ptr + 0, 0, 4);
        int stream_content = TS_GM8(ptr + 0, 4, 4);
        int content_type = TS_G8(ptr + 1);
        int component_tag = TS_G8(ptr + 2);
        int stream_type = TS_G8(ptr + 3);
        int group_tag = TS_G8(ptr + 4);
        int multilanguage_flag = TS_GM8(ptr + 5, 0, 1);
        int component_flag = TS_GM8(ptr + 5, 1, 1);
        int quality_indicator = TS_GM8(ptr + 5, 2, 2);
        int sampling_rate = TS_GM8(ptr + 5, 4, 3);
        // int reserved = TS_GM8(ptr + 5, 7, 1);
        std::string language = std::string(ptr + 6, 3);

        printf("\t\t:: stream content:[0x%01x], content type:[0x%01x], component tag::[0x%01x], stream type::[0x%01x], group tag::[0x%01x], multilanguage::[0x%01x], component flag::[0x%01x], quality flag::[0x%01x], sampling rate::[0x%01x], language::[%s]\n", stream_content, content_type, component_tag, stream_type, group_tag, multilanguage_flag, component_flag, quality_indicator, sampling_rate, language.c_str());
      } else if (descriptor_tag == 0xc8) { // video decode control descriptor
        int still_picture_flag = TS_GM8(ptr + 0, 0, 1);
        int sequence_end_code_flag = TS_GM8(ptr + 0, 1, 1);
        int video_encode_format = TS_GM8(ptr + 0, 2, 4);
        
        std::string video_encode = "unknown";

        if (video_encode_format == 0x001) {
          video_encode = "1080i";
        } else if (video_encode_format == 0x010) {
          video_encode = "720i";
        } else if (video_encode_format == 0x011) {
          video_encode = "480p";
        } else if (video_encode_format == 0x100) {
          video_encode = "480i";
        }

        printf("\t\t:: still picture flag:[0x%01x], sequence end code flag:[0x%01x], video encode format::[0x%02x/%s]\n", still_picture_flag, sequence_end_code_flag, video_encode_format, video_encode.c_str());
      } else if (descriptor_tag == 0xc7) { // data content descriptor [ABNTNBR 15608-3/15610-1]
        int data_component_id = TS_G16(ptr + 0);
        int entry_component = TS_G8(ptr + 2);
        int selector_length = TS_G8(ptr + 3);
        int num_languages = TS_G8(ptr + 4);

        printf("\t\t:: data component id:[0x%04x], entry component:[0x%02x], selector length::[0x%02x], number of languages::[0x%02x]\n", data_component_id, entry_component, selector_length, num_languages);

        ptr = ptr + 5;

        for (int i=0; i<num_languages; i++) {
          int language_tag = TS_GM8(ptr + 0, 0, 3);
          // int reserved = TS_GM8(ptr + 0, 3, 1);
          int dmf = TS_GM8(ptr + 0, 4, 4);
          std::string language(ptr + 1, 3);

          printf("\t\t:: language tag:[0x%01x], dmf:[0x%01x], language::[%s]\n", language_tag, dmf, language.c_str());
          
          ptr = ptr + 4;
        }

        int num_of_component_ref = TS_G8(ptr + 0);

        ptr = ptr + 1;

        for (int i=0; i<num_of_component_ref; i++) {
          int component_ref = TS_GM8(ptr + 0, 0, 3);

          printf("\t\t:: component ref:[0x%02x]\n", component_ref);
          
          ptr = ptr + 1;
        }

        std::string language_code(ptr + 0, 3);
        int text_length = TS_G8(ptr + 3);

        if (text_length > 16) { // INFO:: restricted in specification
          text_length = 16;
        }

        std::string text(ptr + 4, text_length);

        printf("\t\t:: language code:[%s], text:[%s]\n", language_code.c_str(), text.c_str());
      } else if (descriptor_tag == 0xc9) { // downlod content descriptor [ABNTNBR 15608-3/15610-1]
        int reboot = TS_GM8(ptr + 0, 0, 1);
        int add_on = TS_GM8(ptr + 0, 1, 1);
        int compatibility_flag = TS_GM8(ptr + 0, 2, 1);
        int module_info_flag = TS_GM8(ptr + 0, 3, 1);
        int text_info_flag = TS_GM8(ptr + 0, 4, 1);
        uint32_t component_size = TS_G32(ptr + 1);
        uint32_t download_id = TS_G32(ptr + 5);
        uint32_t timeout_value_dii = TS_G32(ptr + 9);
        int leak_rate = TS_GM32(ptr + 13, 0, 22);
        int component_tag = TS_G8(ptr + 16);

        printf("\t\t:: reboot:[%d], add on:[%d], compatibiliy flag:[%d], module info flag:[%d], text info flag:[%d], component size:[%u], download id:[0x%08x], timeout value dii:[%d], leak rate:[%u], component tag:[0x%02x]\n", reboot, add_on, compatibility_flag, module_info_flag, text_info_flag, component_size, download_id, timeout_value_dii, leak_rate, component_tag);

        ptr = ptr + 17;

        if (compatibility_flag == 1) {
          // int descriptor_tag = TS_G8(ptr + 0);
          int descriptor_length = TS_G8(ptr + 1);

          DumpBytes("compatibility descriptor", ptr + 2, descriptor_length);

          ptr = ptr + descriptor_length + 2;
        }

        if (module_info_flag == 1) {
          int number_of_modules = TS_G16(ptr + 0);

          ptr = ptr + 2;

          for (int i=0; i<number_of_modules; i++) {
            int module_id = TS_G16(ptr + 0);
            int module_size = TS_G32(ptr + 2);
            int module_info_length = TS_G8(ptr + 6);
        
            printf("\t\t:<module info>: module:[%d], module id:[0x%04x], module size:[0x%08x], module info length:[%d]\n", i, module_id, module_size, module_info_length);

            DumpBytes("module info", ptr + 7, module_info_length);

            ptr = ptr + module_info_length + 7;
          }
        }

        int private_data_length = TS_G8(ptr + 0);

        DumpBytes("private data", ptr + 1, private_data_length);

        ptr = ptr + private_data_length + 1;

        if (text_info_flag == 1) {
          std::string language(ptr + 0, 3);
          int text_length = TS_G8(ptr + 3);

          printf("\t\t:<text info>: language:[%s]\n", language.c_str());

          DumpBytes("text char", ptr + 4, text_length);
        }
      } else if (descriptor_tag == 0xcd) { // ts information descriptor [ABNTNBR 15603-2 2007]
        const char *end = ptr + descriptor_length;

        int remote_control_key_identification = TS_G8(ptr + 0);
        int ts_name_length = TS_GM8(ptr + 1, 0, 6);
        int transmission_type_count = TS_GM8(ptr + 1, 6, 2);
        std::string ts_name(ptr + 2, ts_name_length);

        ts_name = Utils::ISO8859_1_TO_UTF8(ts_name);

        SINetwork *param = dynamic_cast<SINetwork *>(si);

        if (param != nullptr) {
          param->TransportStreamName(ts_name);
          param->ChannelNumber(remote_control_key_identification);
        }

        printf("\t\t:: remote control key identification:[%d], ts name:[%s]\n", remote_control_key_identification, ts_name.c_str());

        ptr = ptr + 2 + ts_name_length;

        if (transmission_type_count > 0) {
          for (int i=0; i<transmission_type_count; i++) {
            // CHANGE:: service_number should come after transmission_type_count2
            int service_number = TS_G8(ptr + 0);
            int transmission_type_count2 = TS_G8(ptr + 1);
          
            for (int j=0; j<transmission_type_count2; j++) {
              int service_identification = TS_G16(ptr + 2+j);

              printf("\t\t:: service number:[0x%02x], service identification:[0x%04x]\n", service_number, service_identification);

              ptr = ptr + 2;
            }

            ptr = ptr + 2;
          }
        }
        
        int private_length = end - ptr;

        if (private_length > 0) {
          DumpBytes("Private Data", ptr, private_length);
        }

        /*
        const char *end = ptr + descriptor_length;

        int remote_control_key_identification = TS_G8(ptr + 0);
        int ts_name_length = TS_GM8(ptr + 1, 0, 6);
        int transmission_type_count = TS_GM8(ptr + 1, 6, 2);
        std::string ts_name(ptr + 2, ts_name_length);

        printf(":: remote control key identification:[0x%02x], ts name:[%s]\n", remote_control_key_identification, ts_name.c_str());

        ptr = ptr + 2 + ts_name_length;

        if (transmission_type_count > 0) {
          for (int i=0; i<transmission_type_count; i++) {
            // CHANGE:: service_number should come after transmission_type_count2
            int service_number = TS_G8(ptr + 0);
            int transmission_type_count2 = TS_G8(ptr + 1);
          
            for (int j=0; j<transmission_type_count2; j++) {
              int service_identification = TS_G16(ptr + 2 + j);

              printf(":: service number:[0x%02x], service identification:[0x%04x]\n", service_number, service_identification);

              ptr = ptr + 2;
            }

            ptr = ptr + 2;
          }
        }
        
        int private_length = end - ptr;

        if (private_length > 0) {
          DumpBytes("Private Data", ptr, private_length);
        }
        */
      } else if (descriptor_tag == 0xcf) { // logo transmission descriptor [ABNTNBR 15608-3 2008]
        int logo_transmission_type = TS_G8(ptr + 0);

        printf("\t\t:: logo transmission type:[0x%02x]\n", logo_transmission_type);

        if (logo_transmission_type == 0x01) {
          // int reserved = TS_GM16(ptr + 1, 0, 7);
          int logo_identifier = TS_GM16(ptr + 1, 7, 9);
          // int reserved = TS_GM16(ptr + 3, 0, 4);
          int logo_version = TS_GM16(ptr + 3, 4, 12);
          int download_data_identifier = TS_G16(ptr + 5);
        
          printf("\t\t\t:: logo identifier:[0x%04x], logo version:[0x%04x], download data identifier:[0x%04x]\n", logo_identifier, logo_version, download_data_identifier);
        } else if (logo_transmission_type == 0x02) {
          // int reserved = TS_GM16(ptr + 1, 0, 7);
          // int logo_identifier = TS_GM16(ptr + 1, 7, 9);
        } else if (logo_transmission_type == 0x03) {
          // std::string logo_character_string(ptr + 1, descriptor_length - 1);
          DumpBytes("logo char", ptr + 1, descriptor_length - 1);
        } else {
          DumpBytes("reserved", ptr + 1, descriptor_length - 1);
        }
      } else if (descriptor_tag == 0xd7) { // si parameter descriptor [ABNTNBR 15603-1 2008]
        int parameter_version = TS_G8(ptr + 0);
        int update_time = TS_G16(ptr + 1);

        printf("\t\t:: parameter version:[0x%02x], update time:[%d]\n", parameter_version, update_time);

        int length = descriptor_length - 3;

        ptr = ptr + 3;

        while (length > 0) {
          int table_id = TS_G8(ptr + 0);
          int table_description_length = TS_G8(ptr + 1);
          std::string table_description_byte(ptr + 2, table_description_length);

          printf("\t\t:: table id:[0x%02x], table description byte:[%s]\n", table_id, table_description_byte.c_str());

          ptr = ptr + 1 + 1 + table_description_length;

          length = length - 1 - 1 - table_description_length;
        }
      } else if (descriptor_tag == 0xfa) { // terrestrial delivery system descriptor
        int area_code = TS_GM16(ptr, 0, 12);
        int guard_interval = TS_GM16(ptr, 12, 2);
        int transmission_mode = TS_GM16(ptr, 14, 2);
        std::string interval;
        std::string mode;

        if (guard_interval == 0x00) {
          interval = "1/32";
        } else if (guard_interval == 0x01) {
          interval = "1/16";
        } else if (guard_interval == 0x02) {
          interval = "1/8";
        } else if (guard_interval == 0x03) {
          interval = "1/4";
        }
        
        if (transmission_mode == 0x00) {
          mode = "Mode 1";
        } else if (transmission_mode == 0x01) {
          mode = "Mode 2";
        } else if (transmission_mode == 0x02) {
          mode = "Mode 3";
        } else if (transmission_mode == 0x03) {
          mode = "Undefined";
        }

        printf("\t\t:: area code:[%d], guard interval:[%d]::[%s], tramission mode:[%d]::[%s]\n", area_code, guard_interval, interval.c_str(), transmission_mode, mode.c_str());

        int count = (descriptor_length - 2)/2;
        
        ptr = ptr + 2;

        for (int i=0; i<count; i++) {
          int frequency = TS_G16(ptr + 0);

          printf("\t\t:: frequency:[%d]\n", frequency);
          // printf(":: frequency:[%.0f + 1/7 MHz]\n", (473.0 + 6.0 * (frequency - 14.0) + 1.0/7.0) * 7.0);

          ptr = ptr + 2;
        }
      } else if (descriptor_tag == 0xfb) { // partial reception descriptor
        int count = descriptor_length/2;

        for (int i=0; i<count; i++) {
          int service_id = TS_G16(ptr + 0);

          printf("\t\t:: service id:[0x%04x]\n", service_id);

          ptr = ptr + 2;
        }
      } else if (descriptor_tag == 0xfc) { // emergency information descriptor (EWBS)
        int service_id = TS_G16(ptr + 0);
        int start_end_flag = TS_GM8(ptr + 2, 0, 1);
        int signal_type = TS_GM8(ptr + 2, 1, 1);
        // int reserved = TS_GM8(ptr + 2, 2, 6);
        int area_code_length = TS_G8(ptr + 3);

        ptr = ptr + 4;

        for (int i=0; i<area_code_length/2; i++) {
          int area_code = TS_GM16(ptr + 0, 0, 12);
          // int reserved = TS_GM16(ptr + 0, 12, 4);

          printf("\t\t:: service id:[0x%04x], start end flag:[%01x], signal type:[%01x], area code:[0x%04x]\n", service_id, start_end_flag, signal_type, area_code);

          ptr = ptr + 2;
        }
      } else if (descriptor_tag == 0xfd) { // data component descriptor
        int data_component_id = TS_G16(ptr + 0);

        printf("\t\t:: data component id:[0x%04x]\n", data_component_id);

        DumpBytes("additional identification info", ptr + 2, descriptor_length - 2);
        
        /*
        // INFO:: ABNT NBR 15608-3, 22.3.3.3
        // INFO:: STD-B24:2008, volume 1, parte 3, 9.6.1 (additional_arib_caption_info)
        int dmf = TS_GM8(ptr + 1, 0, 4);
        // int reserved = TS_GM8(ptr + 1, 4, 2);
        int timing = TS_GM8(ptr + 1, 6, 2);

        printf("\t\t:: dmf:[0x%02x], timing:[0x%02x]\n", dmf, timing);
        */
      } else if (descriptor_tag == 0xfe) { // system management descriptor [ABNTNBR 15608-3-2008]
        int system_management_id = TS_G16(ptr + 0);
        int broadcasting_flag = TS_GM8(ptr, 0, 2);
        int broadcasting_identifier = TS_GM8(ptr, 2, 6);

        std::string flag;
        std::string identifier;

        if (broadcasting_flag == 0x00) {
          flag = "Open television";
        } else if (broadcasting_flag == 0x01) {
          flag = "Not open television";
        } else if (broadcasting_flag == 0x02) {
          flag = "Not open television";
        } else if (broadcasting_flag == 0x03) {
          flag = "Not specified";
        }

        if (broadcasting_identifier == 0x00) {
          identifier = "Not specified";
        } else if (broadcasting_identifier == 0x01) {
          identifier = "Not used";
        } else if (broadcasting_identifier == 0x02) {
          identifier = "Not used";
        } else if (broadcasting_identifier == 0x03) {
          identifier = "ISDB system";
        } else if (broadcasting_identifier >= 0x04 and broadcasting_identifier <= 0x06) {
          identifier = "Not used";
        } else if (broadcasting_identifier >= 0x07 and broadcasting_identifier <= 0x015) {
          identifier = "Not specified";
        }

        printf("\t\t:: system management id:[0x%04x] {broadcasting_id:[%s], broadcasting identified:[%s]}\n", system_management_id, flag.c_str(), identifier.c_str());

        int count = (descriptor_length - 2);

        ptr = ptr + 2;

        if (count > 0) {
          DumpBytes("Additional Identifier Info", ptr, count);
        }
      } else {
        DumpBytes("Data", ptr, descriptor_length);
      }
    }

    virtual void DataArrived(jevent::DemuxEvent *event)
    {
      jmpeg::Demux *demux = reinterpret_cast<jmpeg::Demux *>(event->GetSource());

      const char *ptr = event->GetData();
      int pid = event->GetPID();
      int tid = TS_G8(ptr + 0);
      int length = event->GetLength();

      if (demux->GetType() == jmpeg::JDT_RAW) {
        printf("Raw Packet:: pid:[0x%04x], length:[%d]\n", pid, length);
      } else if (demux->GetType() == jmpeg::JDT_PES) {
        printf("PES Section:: pid:[0x%04x], length:[%d]\n", pid, length);
      } else {
        int version = TS_GM8(ptr + 5, 2, 5);
        int current_next_indicator = TS_GM8(ptr + 5, 7, 1);

        printf("PSI Section:[%s]: pid:[0x%04x], table id:[0x%04x], version:[0x%02x], current next indicator:[%d], length:[%d]\n", GetTableDescription(pid, tid).c_str(), pid, tid, version, current_next_indicator, length);
      
        if (demux->GetType() == jmpeg::JDT_PSI) {
          jmpeg::PSIDemux *d = dynamic_cast<jmpeg::PSIDemux *>(demux);

          if (d->IsCRCFailed() == true) {
            printf("PSI Section:...: <CRC error>\n\n");

            return;
          }
        } else if (demux->GetType() == jmpeg::JDT_PRIVATE) {
          jmpeg::PrivateDemux *d = dynamic_cast<jmpeg::PrivateDemux *>(demux);

          if (d->IsCRCFailed() == true) {
            printf("Private Section:...: <CRC error>\n\n");

            return;
          }
        }
      }

      if (demux->GetID() == "pat") {
        ProcessPAT(event);
      } else if (demux->GetID().find("pmt-") == 0) {
        ProcessPMT(event);
      } else if (demux->GetID() == "cat") {
        ProcessCAT(event);
      } else if (demux->GetID() == "tsdt") {
        ProcessTSDT(event);
      } else if (demux->GetID() == "nit") {
        ProcessNIT(event);
      } else if (demux->GetID() == "sdt") {
        ProcessSDT(event);
      } else if (demux->GetID() == "bat") {
        ProcessBAT(event);
      } else if (demux->GetID() == "tdt") {
        ProcessTDT(event);
      } else if (demux->GetID() == "tot") {
        ProcessTOT(event);
      } else if (demux->GetID() == "rst") {
        ProcessRST(event);
      } else if (demux->GetID() == "pcr") {
        ProcessPCR(event);
      } else if (demux->GetID() == "cdt") {
        ProcessCDT(event);
      } else if (demux->GetID() == "bit") {
        ProcessBIT(event);
      } else if (demux->GetID() == "sdtt") {
        ProcessSDTT(event);
      } else if (demux->GetID() == "eit") {
        if (tid >= 0x4e && tid <= 0x6f) {
          ProcessEIT(event);
        }
      } else if (demux->GetID() == "closed-caption") {
        ProcessPES(event);
      } else if (demux->GetID() == "dsmcc-data") {
        ProcessDSMCC(event);
      } else if (demux->GetID() == "dsmcc-descriptors") {
        ProcessDSMCC(event);
      } else if (demux->GetID() == "libras-data") {
        ProcessLibras(event);
      } else if (demux->GetID() == "private") {
        ProcessPrivate(event);
      } 

      printf("\n");
    }

    virtual void ProcessPAT(jevent::DemuxEvent *event)
    {
      const char *ptr = event->GetData();
      int section_length = TS_PSI_G_SECTION_LENGTH(ptr + 0);

      StopDemux("pat");

      // INFO::
      //  start SDT to get the service name
      //  start TDT/TOT to get the current time
      StartPSIDemux("bat", TS_BAT_PID, TS_BAT_TABLE_ID, TS_BAT_TIMEOUT);
      StartPSIDemux("cat", TS_CAT_PID, TS_CAT_TABLE_ID, TS_CAT_TIMEOUT);
      StartPSIDemux("tsdt", TS_TSDT_PID, TS_TSDT_TABLE_ID, TS_TSDT_TIMEOUT);
      StartPSIDemux("sdt", TS_SDT_PID, TS_SDT_TABLE_ID, TS_SDT_TIMEOUT);
      StartPSIDemux("tdt", TS_TDT_PID, TS_TDT_TABLE_ID, TS_TDT_TIMEOUT);
      StartPSIDemux("tot", TS_TOT_PID, TS_TOT_TABLE_ID, TS_TDT_TIMEOUT);
      StartPSIDemux("rst", TS_RST_PID, TS_RST_TABLE_ID, TS_RST_TIMEOUT);
      StartPSIDemux("eit", TS_EIT_PID, -1, TS_EIT_TIMEOUT);
      // StartPSIDemux("eit-now&next", -1, 0x4e, TS_EIT_TIMEOUT);
      
      // INFO:: extra tables (15608-3)
      StartPSIDemux("sdtt", TS_SDTT_PID, TS_SDTT_TABLE_ID, 5000); // software download trigger table (0x23: fullseg, 0x28: oneseg)
      StartPSIDemux("bit", TS_BIT_PID, TS_BIT_TABLE_ID, 5000); // broadcast identifier table
      StartPSIDemux("cdt", TS_CDT_PID, TS_CDT_TABLE_ID, 5000); // common data table

      int nit_pid = TS_NIT_PID;
      int count = ((section_length - 5)/4 - 1); // last 4 bytes are CRC 

      ptr = ptr + 8;

      for (int i=0; i<count; i++) {
        int program_number = TS_G16(ptr + 0);
        int map_pid = TS_GM16(ptr + 2, 3, 13);

        printf("PAT:: program number:[0x%04x], map pid:[0x%04x]\n", program_number, map_pid);

        if (program_number == 0x00) {
          // CHANGE:: sometimes the headend send invalid values, so I prefer discards
          // nit_pid = map_pid;
        } else {
          char tmp[255];

          sprintf(tmp, "pmt-0x%04x", program_number);

          StartPSIDemux(tmp, map_pid, TS_PMT_TABLE_ID, TS_PMT_TIMEOUT);
        }

        ptr = ptr + 4;
      }

      StartPSIDemux("nit", nit_pid, TS_NIT_TABLE_ID, TS_NIT_TIMEOUT);
    }

    virtual void ProcessCAT(jevent::DemuxEvent *event)
    {
      const char *ptr = event->GetData();
      int section_length = TS_PSI_G_SECTION_LENGTH(ptr + 0);

      printf("CAT::\n");

      ptr = ptr + 8;

      int descriptors_length = section_length - 5 - 4;
      int descriptors_count = 0;

      while (descriptors_count < descriptors_length) {
        // int descriptor_tag = TS_G8(ptr + 0);
        int descriptor_length = TS_G8(ptr + 1);

        DescriptorDump(nullptr, ptr, descriptor_length + 2);

        ptr = ptr + descriptor_length + 2;

        descriptors_count = descriptors_count + descriptor_length + 2;  
      }
    }

    virtual void ProcessTSDT(jevent::DemuxEvent *event)
    {
      const char *ptr = event->GetData();
      int section_length = TS_PSI_G_SECTION_LENGTH(ptr + 0);

      ptr = ptr + 8;

      int descriptors_length = section_length - 5 - 4;
      int descriptors_count = 0;

      printf("TSDT:: descriptors length:[%d]\n", descriptors_length);

      while (descriptors_count < descriptors_length) {
        // int descriptor_tag = TS_G8(ptr + 0);
        int descriptor_length = TS_G8(ptr + 1);

        DescriptorDump(nullptr, ptr, descriptor_length + 2);

        ptr = ptr + descriptor_length + 2;

        descriptors_count = descriptors_count + descriptor_length + 2;  
      }
    }

    virtual void ProcessPMT(jevent::DemuxEvent *event)
    {
      const char *ptr = event->GetData();
      int tid = TS_G8(ptr + 0);
      int section_length = TS_PSI_G_SECTION_LENGTH(ptr + 0);

      int program_number = TS_G16(ptr + 3);
      int pcr_pid = TS_GM16(ptr + 8, 3, 13);
      int vpid = -1;
      int program_info_length = TS_GM16(ptr + 10, 4, 12);

      std::shared_ptr<SIService> param = SIFacade::GetInstance()->Service(program_number);

      if (param == nullptr) {
        param = std::make_shared<SIService>();

        param->ServiceID(program_number);
      }

      printf("PMT:: service number:[0x%04x], program number:[0x%04x], pcr pid:[0x%04x]\n", tid, program_number, pcr_pid);

      ptr = ptr + 12;

      int descriptors_length = program_info_length;
      int descriptors_count = 0;

      while (descriptors_count < descriptors_length) {
        // int descriptor_tag = TS_G8(ptr + 0);
        int descriptor_length = TS_G8(ptr + 1);

        DescriptorDump(nullptr, ptr, descriptor_length + 2);

        ptr = ptr + descriptor_length + 2;

        descriptors_count = descriptors_count + descriptor_length + 2;  
      }

      int services_length = section_length - 14 - descriptors_length; // discards crc
      int services_count = 0;

      while (services_count < services_length) {
        int stream_type = TS_G8(ptr + 0);
        // int reserved_bits_1 = TS_GM8(1, 0, 3); // 0x07
        int elementary_pid = TS_GM16(ptr + 1, 3, 13);
        // int reserved_bits_2 = TS_GM8(ptr + 3, 0, 4); // 0x0f
        // int es_info_length_unsed = TS_GM8(ptr + 4, 4, 2); // 0x00
        int es_info_length = TS_GM16(ptr + 3, 6, 10);

        // TODO:: add elementary stream to param

        printf("PMT:service: elementary stream:[0x%04x], type:[0x%02x]::[%s]\n", elementary_pid, stream_type, GetStreamTypeDescription(stream_type).c_str());

        std::shared_ptr<struct SIService::elementary_stream_t> es = std::make_shared<struct SIService::elementary_stream_t>();

        es->type = _stream_types[stream_type];
        es->program_identifier = elementary_pid;

        if (_stream_types[stream_type] == SIService::stream_type_t::VIDEO) {
          if (vpid < 0) {
            vpid = elementary_pid;
          }
        } else if (_stream_types[stream_type] == SIService::stream_type_t::PRIVATE) {
          StartPrivateDemux("private", elementary_pid, 0x74, TS_PRIVATE_TIMEOUT); // Application Information Section
        } else if (_stream_types[stream_type] == SIService::stream_type_t::SUBTITLE) { // Closed Caption
          StartPESDemux("closed-caption", elementary_pid, 3600000);
        } else if (_stream_types[stream_type] == SIService::stream_type_t::DSMCC_MESSAGE) {
          StartPrivateDemux("dsmcc-data", elementary_pid, -1, TS_PRIVATE_TIMEOUT);
        } else if (_stream_types[stream_type] == SIService::stream_type_t::DSMCC_DESCRIPTOR) {
          StartPrivateDemux("dsmcc-descriptors", elementary_pid, -1, TS_PRIVATE_TIMEOUT);
        } else if (_stream_types[stream_type] == SIService::stream_type_t::LIBRAS_MESSAGE) { // component_tag<0x92>
          StartPrivateDemux("libras-data", elementary_pid, -1, TS_PRIVATE_TIMEOUT);
        } else if (_stream_types[stream_type] == SIService::stream_type_t::LIBRAS_STREAM) { // component_tag<0x93>
        }

        ptr = ptr + 5;

        descriptors_length = es_info_length;
        descriptors_count = 0;

        while (descriptors_count < descriptors_length) {
          int descriptor_tag = TS_G8(ptr + 0);
          int descriptor_length = TS_G8(ptr + 1);

          es->descriptors[descriptor_tag] = std::make_shared<std::string>(std::string(ptr + 2, descriptor_length));

          DescriptorDump(nullptr, ptr, descriptor_length + 2);

          ptr = ptr + descriptor_length + 2;

          descriptors_count = descriptors_count + descriptor_length + 2;  
        }

        param->AddElementaryStream(es);

        services_count = services_count + 5 + descriptors_length;
      }

      SIFacade::GetInstance()->Service(param);

      if (pcr_pid == 0x1fff) { // pmt pcr unsed
        pcr_pid = vpid; // first video pid
      }
      
      StartRawDemux("pcr", pcr_pid, TS_PCR_TIMEOUT);
    }

    virtual void ProcessNIT(jevent::DemuxEvent *event)
    {
      const char *ptr = event->GetData();
      int network_id = TS_G16(ptr + 3);

      printf("NIT:: network_id:[0x%04x]\n", network_id);

      ptr = ptr + 8;

      int descriptors_length = TS_GM16(ptr, 4, 12);
      int descriptors_count = 0;

      ptr = ptr + 2;

      while (descriptors_count < descriptors_length) {
        // int descriptor_tag = TS_G8(ptr + 0);
        int descriptor_length = TS_G8(ptr + 1);

        DescriptorDump(nullptr, ptr, descriptor_length + 2);

        ptr = ptr + descriptor_length + 2;

        descriptors_count = descriptors_count + descriptor_length + 2;  
      }

      // int reserved_future_use = TS_GM8(ptr, 0, 4);
      int transport_stream_loop_length = TS_GM16(ptr, 4, 12);
      int transport_stream_loop_count = 0;

      ptr = ptr + 2;

      while (transport_stream_loop_count < transport_stream_loop_length) {
        int transport_stream_id = TS_G16(ptr + 0);
        int original_network_id = TS_G16(ptr + 2);
        // int reserved_future_use = TS_GM8(ptr + 4, 0, 4);

        printf("NIT:: transport stream: transport stream id:[0x%04x], original network id:[0x%04x]\n", transport_stream_id, original_network_id);

        descriptors_length = TS_GM16(ptr + 4, 4, 12);
        descriptors_count = 0;

        ptr = ptr + 6;

        std::shared_ptr<SINetwork> param = std::make_shared<SINetwork>();

        param->NetworkID(network_id);
        param->OriginalNetworkID(original_network_id);
        param->TransportStreamID(transport_stream_id);

        while (descriptors_count < descriptors_length) {
          // int descriptor_tag = TS_G8(ptr + 0);
          int descriptor_length = TS_G8(ptr + 1);

          DescriptorDump(param.get(), ptr, descriptor_length + 2);

          ptr = ptr + descriptor_length + 2;

          descriptors_count = descriptors_count + descriptor_length + 2;  
        }

        SIFacade::GetInstance()->Network(param);

        transport_stream_loop_count = transport_stream_loop_count + 6 + descriptors_length;
      }
    }

    virtual void ProcessSDT(jevent::DemuxEvent *event)
    {
      const char *ptr = event->GetData();

      int section_length = TS_GM16(ptr + 1, 4, 12);
      int transport_stream_id = TS_G16(ptr + 3);
      int original_network_id = TS_G16(ptr + 8);

      printf("SDT:: transport stream id:[0x%04x], original_network_id:[0x%04x]\n", transport_stream_id, original_network_id);

      int services_length = section_length - 8 - 4;
      int services_count = 0;

      ptr = ptr + 11;

      while (services_count < services_length) {
        int service_id = TS_G16(ptr + 0);
        // int reserved_future_use = TS_GM8(ptr + 2, 0, 6);
        // int EIT_schedule_flag = TS_GM8(ptr + 2, 6, 1);
        // int EIT_present_following_flag = TS_GM8(ptr + 2, 7, 1);
        int running_status = TS_GM8(ptr + 3, 0, 3);
        // int free_CA_mode = TS_GM8(ptr + 3, 3, 1);

        printf("SDT:service: service id:[0x%04x], running status:[0x%02x/%s]\n", service_id, running_status, Utils::GetRunningStatusDescription(running_status).c_str());

        int descriptors_length = TS_GM16(ptr + 3, 4, 12);
        int descriptors_count = 0;

        ptr = ptr + 5;

        std::shared_ptr<SIService> param = SIFacade::GetInstance()->Service(service_id);

        if (param == nullptr) {
          return;
        }

        param->OriginalNetworkID(original_network_id);
        param->TransportStreamID(transport_stream_id);

        while (descriptors_count < descriptors_length) {
          // int descriptor_tag = TS_G8(ptr + 0);
          int descriptor_length = TS_G8(ptr + 1);

          DescriptorDump(param.get(), ptr, descriptor_length + 2);

          ptr = ptr + descriptor_length + 2;

          descriptors_count = descriptors_count + descriptor_length + 2;  
        }

        if (param != nullptr) {
          SIFacade::GetInstance()->Service(param);
        }

        services_count = services_count + 6 + descriptors_length;
      }
    }

    virtual void ProcessBAT(jevent::DemuxEvent *event)
    {
      const char *ptr = event->GetData();

      int section_length = TS_GM16(ptr + 1, 4, 12);
      int bouquet_id = TS_G16(ptr + 3);

      printf("BAT:: bouquet id:[0x%04x]\n", bouquet_id);

      int descriptors_length = TS_GM16(ptr + 9, 4, 12);
      int descriptors_count = 0;

      ptr = ptr + 11;

      while (descriptors_count < descriptors_length) {
        // int descriptor_tag = TS_G8(ptr + 0);
        int descriptor_length = TS_G8(ptr + 1);

        DescriptorDump(nullptr, ptr, descriptor_length + 2);

        ptr = ptr + descriptor_length + 2;

        descriptors_count = descriptors_count + descriptor_length + 2;  
      }

      int events_length = section_length - 7 - descriptors_length - 4;
      int events_count = 0;

      while (events_count < events_length) {
        int transport_stream_id = TS_G16(ptr + 0);
        int original_network_id = TS_G16(ptr + 2);

        printf("BAT:event: transport stream id:[0x%04x], original network id:[0x%04x]\n", transport_stream_id, original_network_id);

        int descriptors_length = TS_GM16(ptr + 4, 4, 12);
        int descriptors_count = 0;

        ptr = ptr + 4;

        while (descriptors_count < descriptors_length) {
          // int descriptor_tag = TS_G8(ptr + 0);
          int descriptor_length = TS_G8(ptr + 1);

          DescriptorDump(nullptr, ptr, descriptor_length + 2);

          ptr = ptr + descriptor_length + 2;

          descriptors_count = descriptors_count + descriptor_length + 2;  
        }

        events_count = events_count + 6 + descriptors_length;
      }
    }

    virtual void ProcessTDT(jevent::DemuxEvent *event)
    {
      const char *ptr = event->GetData();

      int julian_date = TS_G16(ptr + 3);

      int Y, M, D, WD, h, m, s;

      Utils::ParseJulianDate(julian_date, Y, M, D, WD);

      h = DHEX2DEC(ptr[5]);
      m = DHEX2DEC(ptr[6]);
      s = DHEX2DEC(ptr[7]);

      std::string day = "Seg";

      if (WD == 1) {
        day = "Seg";
      } else if (WD == 2) {
        day = "Ter";
      } else if (WD == 3) {
        day = "Qua";
      } else if (WD == 4) {
        day = "Qui";
      } else if (WD == 5) {
        day = "Sex";
      } else if (WD == 6) {
        day = "Sab";
      } else if (WD == 7) {
        day = "Dom";
      }

      printf("TDT:: utc:[%02d%02d%02d-%02d%02d%02d]\n", Y, M, D, h, m, s);
    }

    virtual void ProcessTOT(jevent::DemuxEvent *event)
    {
      const char *ptr = event->GetData();

      int julian_date = TS_G16(ptr + 3);

      int Y, M, D, WD, h, m, s;

      Utils::ParseJulianDate(julian_date, Y, M, D, WD);

      h = DHEX2DEC(ptr[5]);
      m = DHEX2DEC(ptr[6]);
      s = DHEX2DEC(ptr[7]);

      SITime::week_day_t week_day = SITime::week_day_t::MONDAY;

      if (WD == 1) {
        week_day = SITime::week_day_t::MONDAY;
      } else if (WD == 2) {
        week_day = SITime::week_day_t::TUESDAY;
      } else if (WD == 3) {
        week_day = SITime::week_day_t::WEDNESDAY;
      } else if (WD == 4) {
        week_day = SITime::week_day_t::THRUSDAY;
      } else if (WD == 5) {
        week_day = SITime::week_day_t::FRIDAY;
      } else if (WD == 6) {
        week_day = SITime::week_day_t::SATURDAY;
      } else if (WD == 7) {
        week_day = SITime::week_day_t::SUNDAY;
      }

      SITime param;

      param.Year(Y);
      param.Month(M);
      param.Day(D);
      param.Hour(h);
      param.Minute(m);
      param.Second(s);
      param.WeekDay(week_day);

      printf("TOT:: utc:[%02d%02d%02d-%02d%02d%02d]\n", Y, M, D, h, m, s);
        
      int descriptors_length = TS_GM16(ptr + 8, 4, 12);
      int descriptors_count = 0;

      ptr = ptr + 10;

      while (descriptors_count < descriptors_length) {
        // int descriptor_tag = TS_G8(ptr + 0);
        int descriptor_length = TS_G8(ptr + 1);

        DescriptorDump(&param, ptr, descriptor_length + 2);

        ptr = ptr + descriptor_length + 2;

        descriptors_count = descriptors_count + descriptor_length + 2;  
      }
      
      SIFacade::GetInstance()->Time(param);
    }

    virtual void ProcessRST(jevent::DemuxEvent *event)
    {
      const char *ptr = event->GetData();

      int section_length = TS_PSI_G_SECTION_LENGTH(ptr + 0);

      printf("RST:: length:[%d]\n", section_length/9);
        
      int events_length = section_length;
      int events_count = 0;

      ptr = ptr + 3;

      while (events_count < events_length) {
        int transport_stream_id = TS_G16(ptr + 0);
        int original_network_id = TS_G16(ptr + 2);
        int service_id = TS_G16(ptr + 4);
        int event_id = TS_G16(ptr + 6);
        int running_status = TS_GM8(ptr + 8, 5, 3);

        printf("RST:event: transport stream id:[0x%04x], original network id:[0x%04x], service id:[0x%04x], event id:[0x%04x], running status:[0x%02x/%s]\n", transport_stream_id, original_network_id, service_id, event_id, running_status, Utils::GetRunningStatusDescription(running_status).c_str());

        events_count = events_count + 9;
      }
    }

    virtual void ProcessEIT(jevent::DemuxEvent *event)
    {
      const char *ptr = event->GetData();

      int tid = TS_G8(ptr + 0);
      int section_length = TS_PSI_G_SECTION_LENGTH(ptr + 0);
      int service_id = TS_G16(ptr + 3);
      int transport_stream_id = TS_G16(ptr + 8);
      int original_network_id = TS_G16(ptr + 10);

      if (tid == 0x4e) { // present and following (present ts)
      } else if (tid == 0x4f) { // present and following (other ts)
        return;
      } else if (tid >= 0x50 && tid <= 0x5f) { // schedule (present ts)
      } else if (tid >= 0x60 && tid <= 0x6f) { // schedule (other ts)
        return;
      }

      int events_length = section_length - 15;
      int events_count = 0;

      ptr = ptr + 14;

      while (events_count < events_length) {
        int event_id = TS_G16(ptr + 0);
        int julian_date = TS_G16(ptr + 2);

        int Y, M, D, WD, h, m, s, dh, dm, ds;

        Utils::ParseJulianDate(julian_date, Y, M, D, WD);

        h = DHEX2DEC(ptr[4]);
        m = DHEX2DEC(ptr[5]);
        s = DHEX2DEC(ptr[6]);
        dh = DHEX2DEC(ptr[7]);
        dm = DHEX2DEC(ptr[8]);
        ds = DHEX2DEC(ptr[9]);

        std::string day = "Seg";

        if (WD == 1) {
          day = "Seg";
        } else if (WD == 2) {
          day = "Ter";
        } else if (WD == 3) {
          day = "Qua";
        } else if (WD == 4) {
          day = "Qui";
        } else if (WD == 5) {
          day = "Sex";
        } else if (WD == 6) {
          day = "Sab";
        } else if (WD == 7) {
          day = "Dom";
        }

        char tmp[255];
        
        sprintf(tmp, "%02d%02d%02d-%02d%02d%02d (%02d%02d%02d)", Y, M, D, h, m, s, dh, dm, ds);

        int running_status = TS_GM8(ptr + 10, 0, 3);
        // int free_ca_mode = TS_GM8(ptr + 10, 3, 1);

        printf("EIT:: transport stream id:[0x%04x], original network id:[0x%04x], service id:[0x%04x], event id:[0x%04x], date:[%s], running status:[0x%02x/%s]\n", transport_stream_id, original_network_id, service_id, event_id, tmp, running_status, Utils::GetRunningStatusDescription(running_status).c_str());

        int descriptors_length = TS_GM16(ptr + 10, 4, 12);
        int descriptors_count = 0;

        ptr = ptr + 12;

        std::shared_ptr<SIEvent> param = std::make_shared<SIEvent>();

        param->OriginalNetworkID(original_network_id);
        param->TransportStreamID(transport_stream_id);
        param->ServiceID(service_id);
        param->EventID(event_id);

        while (descriptors_count < descriptors_length) {
          // int descriptor_tag = TS_G8(ptr + 0);
          int descriptor_length = TS_G8(ptr + 1);

          descriptors_count = descriptors_count + descriptor_length + 2;  
        
          // INFO:: invalidate and abort the read process if the sizes not maths
          if (descriptors_count > descriptors_length) {
            printf("EIT:: <abort parser>:[descriptors_count > descriptors_length]\n");

            DumpBytes("Invalid bytes", ptr, descriptors_length - (descriptors_count - descriptor_length - 2));

            return;
          }

          DescriptorDump(param.get(), ptr, descriptor_length + 2);

          ptr = ptr + descriptor_length + 2;
        }

        SIFacade::GetInstance()->Event(param);

        events_count = events_count + 12 + descriptors_length;
      }
    }

    virtual void ProcessCDT(jevent::DemuxEvent *event)
    {
      // table_id = 0xc8
      const char *ptr = event->GetData();
      
      int download_data_id = TS_G16(ptr + 3);
      int original_network_id = TS_G16(ptr + 8);
      int data_type = TS_G8(ptr + 10);

      printf("CDT:: download data id:[0x%04x], original network id:[0x%04x], data type:[0x%02x]\n", download_data_id, original_network_id, data_type);

      ptr = ptr + 11;

      int descriptors_length = TS_GM16(ptr, 4, 12);
      int descriptors_count = 0;

      ptr = ptr + 2;

      while (descriptors_count < descriptors_length) {
        // int descriptor_tag = TS_G8(ptr + 0);
        int descriptor_length = TS_G8(ptr + 1);

        DescriptorDump(nullptr, ptr, descriptor_length + 2);

        ptr = ptr + descriptor_length + 2;

        descriptors_count = descriptors_count + descriptor_length + 2;  
      }

      DumpBytes("data module byte", ptr, (event->GetData() + event->GetLength()) - ptr - 4);
    }

    virtual void ProcessBIT(jevent::DemuxEvent *event)
    {
      // table_id = 0xc4
      const char *ptr = event->GetData();
      
      int original_network_id = TS_G16(ptr + 3);
      int descriptors_length = TS_GM16(ptr + 7, 4, 12);
      int descriptors_count = 0;

      printf("BIT:: original network id:[0x%04x]\n", original_network_id);

      ptr = ptr + 9;

      while (descriptors_count < descriptors_length) {
        // int descriptor_tag = TS_G8(ptr + 0);
        int descriptor_length = TS_G8(ptr + 1);

        DescriptorDump(nullptr, ptr, descriptor_length + 2);

        ptr = ptr + descriptor_length + 2;

        descriptors_count = descriptors_count + descriptor_length + 2;  
      }

      int loop_length = (event->GetData() + event->GetLength()) - ptr - 4;
      int loop_count = 0;

      while (loop_count < loop_length) {
        int broadcast_id = TS_G8(ptr + 0);
        int broadcaster_descriptors_length = TS_GM16(ptr + 1, 4, 12);

        printf("BIT:: broadcast id:[0x%04x]\n", broadcast_id);

        ptr = ptr + broadcaster_descriptors_length + 3;
       
        int descriptors_count = 0;

        while (descriptors_count < broadcaster_descriptors_length) {
          // int descriptor_tag = TS_G8(ptr + 0);
          int descriptor_length = TS_G8(ptr + 1);

          DescriptorDump(nullptr, ptr, descriptor_length + 2);

          ptr = ptr + descriptor_length + 2;

          descriptors_count = descriptors_count + descriptor_length + 2;  
        }

        loop_count = loop_count + broadcaster_descriptors_length + 3;
      }
    }

    virtual void ProcessSDTT(jevent::DemuxEvent *event)
    {
      // table_id = 0xc3
      const char *ptr = event->GetData();
      
      // int table_id_ext = TS_G16(ptr + 3);
      int marker_id = TS_GM16(ptr + 3, 0, 8);
      int model_id = TS_GM16(ptr + 3, 8, 8);
      int transport_stream_id = TS_G16(ptr + 8);
      int original_network_id = TS_G16(ptr + 10);
      int service_id = TS_G16(ptr + 12);
      int number_of_contents = TS_G8(ptr + 14);

      printf("SDTT:: marker id:[0x%08x/%s], model id:[0x%02x], transport stream id:[0x%04x], original network id:[0x%04x], service id:[0x%04x], number of contents:[%d]\n", marker_id, Utils::GetMarkerDescription(marker_id).c_str(), model_id, transport_stream_id, original_network_id, service_id, number_of_contents);

      ptr = ptr + 15;

      for (int i=0; i<number_of_contents; i++) {
        int group = TS_GM16(ptr + 0, 0, 4);
        int target_version = TS_GM16(ptr + 0, 4, 12);
        int new_version = TS_GM16(ptr + 2, 0, 12);
        int download_level = TS_GM16(ptr + 2, 12, 2);
        int version_indicator = TS_GM16(ptr + 2, 14, 2);
        int content_description_length = TS_GM16(ptr + 4, 0, 12);
        int schedule_description_length = TS_GM16(ptr + 6, 0, 12);
        // int schedule_time_shift_information = TS_GM16(ptr + 6, 12, 4);

        printf("SDTT:: group:[0x%04x], target version:[0x%04x], new version:[0x%04x], download level:[0x%04x], version indicator:[0x%04x]\n", group, target_version, new_version, download_level, version_indicator);

        ptr = ptr + 8;

        for (int j=0; j<schedule_description_length >> 3; j++) {
          uint64_t start_time = TS_GM64(ptr + 0, 0, 40);
          int duration = TS_GM64(ptr + 0, 40, 24);

          printf("SDTT:: start time:[%lu], duration:[%d]\n", start_time, duration);

          ptr = ptr + 8;
        }

        int descriptors_length = content_description_length - schedule_description_length;
        int descriptors_count = 0;

        while (descriptors_count < descriptors_length) {
          // int descriptor_tag = TS_G8(ptr + 0);
          int descriptor_length = TS_G8(ptr + 1);

          DescriptorDump(nullptr, ptr, descriptor_length + 2);

          ptr = ptr + descriptor_length + 2;

          descriptors_count = descriptors_count + descriptor_length + 2;  
        }
      }
    }

    virtual void ProcessPrivate(jevent::DemuxEvent *event)
    {
      const char *ptr = event->GetData();

      int tid = TS_G8(ptr + 0);
      // int section_length = TS_GM16(ptr + 1, 4, 12);

      if (tid == TS_AIT_TABLE_ID) {
        ProcessAIT(event);
      }
    }

    virtual void ProcessAIT(jevent::DemuxEvent *event)
    {
      const char *ptr = event->GetData();

      // int test_application_flag = TS_GM8(ptr + 3, 0, 1);
      int application_type = TS_GM16(ptr + 3, 1, 15);
      // int reserved = TS_GM8(ptr + 5, 0, 2);
      int version_number = TS_GM8(ptr + 5, 2, 5);
      // int current_next_indicator = TS_GM8(ptr + 5, 7, 1);
      // int section_number = TS_G8(ptr + 6);
      // int last_section_number = TS_G8(ptr + 7);
      // int reserved_future_use = TS_GM8(ptr + 8, 0, 4);
      std::string type = "Unknown";

      if (application_type == 0x01) {
        type = "Ginga-J";
      } else if (application_type == 0x02) {
        type = "DVB-HTML";
      } else if (application_type == 0x06) {
        type = "ACAP-J";
      } else if (application_type == 0x07) {
        type = "ARIB-BML";
      } else if (application_type == 0x09) {
        type = "Ginga-NCL";
      } else if (application_type == 0x10) {
        type = "Resident";
      }

      printf("AIT:: application type:[0x%02x]::[%s], version number:[0x%04x]\n", application_type, type.c_str(), version_number);

      ptr = ptr + 8;

      int descriptors_length = TS_GM16(ptr, 4, 12);
      int descriptors_count = 0;

      ptr = ptr + 2;

      while (descriptors_count < descriptors_length) {
        // int descriptor_tag = TS_G8(ptr + 0);
        int descriptor_length = TS_G8(ptr + 1);

        AITDescriptorDump(ptr, descriptor_length + 2);

        ptr = ptr + descriptor_length + 2;

        descriptors_count = descriptors_count + descriptor_length + 2;  
      }

      int application_loop_length = TS_GM16(ptr, 4, 12);
      int application_loop_count = 0;

      ptr = ptr + 2;

      while (application_loop_count < application_loop_length) {
        uint32_t oid = TS_G32(ptr + 0); 
        int aid = TS_G16(ptr + 4);
        int application_control_code = TS_G8(ptr + 6);
        std::string control_code = "Reserved to the future";

        if (application_control_code == 0x01) {
          control_code = "AUTO_START";
        } else if (application_control_code == 0x02) {
          control_code = "PRESENT";
        } else if (application_control_code == 0x03) {
          control_code = "DESTROY";
        } else if (application_control_code == 0x04) {
          control_code = "KILL";
        } else if (application_control_code == 0x05) {
          control_code = "PREFETCH";
        } else if (application_control_code == 0x06) {
          control_code = "REMOTE";
        } else if (application_control_code == 0x07) {
          control_code = "UNBOUND";
        } else if (application_control_code == 0x08) {
          control_code = "STORE";
        }

        printf("AIT:application: aid:[0x%04x], oid:[0x%04x], application control code:[0x%02x]::[%s]\n", aid, oid, application_control_code, control_code.c_str());

        int descriptors_length = TS_GM16(ptr + 7, 4, 12);
        int descriptors_count = 0;

        ptr = ptr + 9;

        while (descriptors_count < descriptors_length) {
          // int descriptor_tag = TS_G8(ptr + 0);
          int descriptor_length = TS_G8(ptr + 1);

          AITDescriptorDump(ptr, descriptor_length + 2);

          ptr = ptr + descriptor_length + 2;

          descriptors_count = descriptors_count + descriptor_length + 2;  
        }

        application_loop_count = application_loop_count + 9 + descriptors_length;
      }
    }

    virtual void ProcessPCR(jevent::DemuxEvent *event)
    {
      const char *ptr = event->GetData();

      // int transport_error_indicator = TS_GM8(ptr + 1, 0, 1);
      // int payload_unit_start_indicator = TS_GM8(ptr + 1, 1, 1);
      // int transport_priority = TS_GM8(ptr + 1, 2, 1);
      // int pid = TS_GM16(ptr + 1, 3, 13);
      // int scrambling_control = TS_GM8(ptr + 3, 0, 2);
      int adaptation_field_exist = TS_GM8(ptr + 3, 2, 1);
      // int contains_payload = TS_GM8(ptr + 3, 3, 1);
      // int continuity_counter = TS_GM8(ptr + 3, 4, 4);

      ptr = ptr + TS_HEADER_LENGTH;

      // INFO:: discards adaptation field
      if (adaptation_field_exist == 1) {
        int adaptation_field_length = TS_G8(ptr + 0);

        if (adaptation_field_length > 0) {
          int pcr_flag = TS_GM8(ptr + 1, 3, 1);

          if (pcr_flag == 1) {
            uint64_t program_clock_reference_base = (uint64_t)TS_GM32(ptr, 0, 32) << 1 | TS_GM8(ptr + 4, 0, 1);
            // int reserved = TS_GM8(ptr + 4, 1, 6);
            uint64_t program_clock_reference_extension = (uint64_t)TS_GM16(ptr + 4, 7, 9);

            printf("PCR:: base:[%lu], extension:[%lu]\n", program_clock_reference_base, program_clock_reference_extension);
          }
        }
      }
    }

    virtual void ProcessPES(jevent::DemuxEvent *event)
    {
      // PES private data
      const char *ptr = event->GetData();

      int stream_id = TS_G8(ptr + 3);

      // int pes_packet_length = TS_G16(ptr + 4);

      ptr = ptr + 6;

      if (stream_id != 0b10111100 and // program_stream_map
          stream_id != 0b10111110 and // padding_stream
          stream_id != 0b10111111 and // private_stream_2
          stream_id != 0b11110000 and // ECM
          stream_id != 0b11110001 and // EMM
          stream_id != 0b11111111 and // program_stream_directory
          stream_id != 0b11110010 and // DSMCC_stream
          stream_id != 0b11111000) { // ITU-T Rec. H.222.1 type E
        // PES data field (Arib 6-STD B37 v2.4); ABNT NBR 15606-3 (cap. 9); closed caption (synchronous pes; stream_id:[0xbd], data_id:[0x80])
        if (stream_id != 0b10111101) { // private_stream_1 [W3]
          return;
        }

        // Arib 6-STD-B37, table 2-25
        int data_alignment_indicator  = TS_GM8(ptr + 0, 5, 1);
        int pts_dts_flag = TS_GM8(ptr + 1, 0, 2);
        //int escr_flag = TS_GM8(ptr + 1, 2, 1);
        int pes_header_data_length = TS_G8(ptr + 2);
        uint64_t pts = 0LL;

        if (pts_dts_flag == 0b10) {
          pts = (pts << 3) | TS_GM64(ptr + 3, (4 + 0), 3);
          pts = (pts << 15) | TS_GM64(ptr + 3, (4 + 3 + 1), 15);
          pts = (pts << 15) | TS_GM64(ptr + 3, (4 + 3 + 1 + 15 + 1), 15);
        }

        uint32_t ccis_code = TS_G32(ptr + 8);
        int caption_conversion_type = TS_G8(ptr + 12);
        int drcs_conversion_type = TS_GM8(ptr + 13, 0, 2);

        std::string caption_conversion_info = "undefined";
        std::string drcs_conversion_info = "undefined";

        if (caption_conversion_type == 0x01) {
          caption_conversion_info = "HD side panel";
        } else if (caption_conversion_type == 0x02) {
          caption_conversion_info = "SD (4:3)";
        } else if (caption_conversion_type == 0x03) {
          caption_conversion_info = "SD wide side panel";
        } else if (caption_conversion_type == 0x04) {
          caption_conversion_info = "Mobile closed caption";
        }

        if (drcs_conversion_type == 0x00) {
          drcs_conversion_info = "DRCS conversion mode A";
        } else if (drcs_conversion_type == 0x01) {
          drcs_conversion_info = "DRCS conversion mode B";
        } else if (drcs_conversion_type == 0x02) {
          drcs_conversion_info = "Moblie DRCS";
        } else if (drcs_conversion_type == 0x03) {
          drcs_conversion_info = "DRCS conversion not possible";
        }

        printf("Process PES:: pts:[%lu], data alignment indicator:[0x%01x], pes header data length:[%d], ccis code:[0x%08x], caption conversion type:[%s], drcs conversion type:[%s]\n", pts, data_alignment_indicator, pes_header_data_length, ccis_code, caption_conversion_info.c_str(), drcs_conversion_info.c_str());

        ptr = ptr + pes_header_data_length + 3;

        // PES data field (Arib 6-STD B37 v2.4); ABNT NBR 15606-3 (cap. 9); closed caption (synchronous pes; stream_id:[0xbd], data_id:[0x80])
        ProcessClosedCaption(ptr, (event->GetData() + event->GetLength()) - ptr);
      } else if (stream_id == 0b10111100 or // program_stream_map
          stream_id == 0b10111111 or // private_stream_2
          stream_id == 0b11110000 or // ECM
          stream_id == 0b11110001 or // EMM
          stream_id == 0b11111111 or // program_stream_directory
          stream_id == 0b11110010 or // DSMCC_stream
          stream_id == 0b11111000) { // ITU-T Rec. H.222.1 type E stream
        // PES data field (Arib 6-STD B37 v2.4); ABNT NBR 15606-3 (cap. 9); superimposed (asynchronous pes; stream_id:[0xbf], data_id:[0x81])
        if (stream_id != 0b10111111) {
          return;
        }
        
        ProcessClosedCaption(ptr, event->GetLength() - 6); 
      } else if (stream_id == 0b10111110) { // padding_stream
        // do nothing
      }
    }

    virtual void ProcessClosedCaption(const char *ptr, int length)
    {
      int data_identifier = TS_G8(ptr + 0);

      std::string data_info;

      if (data_identifier >= 0x00 and data_identifier <= 0x0f) {
        data_info = "reserved";
      } else if (data_identifier >= 0x10 and data_identifier <= 0x1f) {
        data_info = "EBU teletext only or EBU teletext combined with VPS and/or WSS and/or Closed Captioning and/or VBI sample data";
      } else if (data_identifier >= 0x20 and data_identifier <= 0x7f) {
        data_info = "reserved";
      } else if (data_identifier >= 0x80 and data_identifier <= 0x98) {
        data_info = "user defined";

        if (data_identifier == 0x80) {
          data_info = "closedcaption";
        } else if (data_identifier == 0x81) {
          data_info = "superimposed";
        }
      } else if (data_identifier >= 0x99 and data_identifier <= 0x9b) {
        data_info = "EBU teletext and/or VPS and/or WSS and/or Closed Captioning and/or VBI sample data";
      } else if (data_identifier >= 0x9c and data_identifier <= 0xff) {
        data_info = "user defined";
      }

      printf("Closed Caption:: data identifier:[0x%02x/%s]\n", data_identifier, data_info.c_str());

      // closed caption:[0x80/synchronous pes], superimpose:[0x81/asynchronous pes]
      if (data_identifier != 0x80 and data_identifier != 0x81) {
        return;
      }

      int private_stream_id = TS_G8(ptr + 1);

      if (private_stream_id != 0xff) { // magic number
        return;
      }

      int pes_data_packet_header_length = TS_GM8(ptr + 2, 4, 4); // (W34)

      ptr = ptr + pes_data_packet_header_length + 3;

      // 6-STD B24 (data group)
      int data_group_id = TS_GM8(ptr + 0, 0, 6);
      int data_group_link_number = TS_G8(ptr + 1);
      // int last_data_group_link_number = TS_G8(ptr + 2);
      int data_group_size = TS_G16(ptr + 3);

      std::string data_group_info;
      int caption_data_type_nibble = (data_group_id & 0x0f) >> 0;
      int data_group_id_nibble = (data_group_id & 0xf0) >> 4;

      if (caption_data_type_nibble == 0x00) {
        data_group_info = "Caption management";
      } else if (caption_data_type_nibble == 0x01) {
        data_group_info = "Caption statement (1st language)";
      } else if (caption_data_type_nibble == 0x02) {
        data_group_info = "Caption statement (2nd language)";
      } else if (caption_data_type_nibble == 0x03) {
        data_group_info = "Caption statement (3rd language)";
      } else if (caption_data_type_nibble == 0x04) {
        data_group_info = "Caption statement (4th language)";
      } else if (caption_data_type_nibble == 0x05) {
        data_group_info = "Caption statement (5th language)";
      } else if (caption_data_type_nibble == 0x06) {
        data_group_info = "Caption statement (6th language)";
      } else if (caption_data_type_nibble == 0x07) {
        data_group_info = "Caption statement (7th language)";
      } else if (caption_data_type_nibble == 0x08) {
        data_group_info = "Caption statement (8th language)";
      } else {
        // INFO:: invalid type
        return;
      }

      if (data_group_id_nibble == 0x00) {
        data_group_info = "[Group A] " + data_group_info;
      } else if (data_group_id_nibble == 0x02) {
        data_group_info = "[Group B] " + data_group_info;
      } else {
        // INFO:: invalid type
        return;
      }

      uint16_t 
        sum = jmath::CRC::Calculate16((const uint8_t *)ptr, data_group_size + 7);

      if (sum != 0xffff) {
        return;
      }

      ptr = ptr + 5;

      printf("Closed Caption:: subtitle stream id:[0x%02x], data group id:[0x%02x/%s], data group link number:[0x%02x], data group size:[%d]\n", private_stream_id, data_group_id, data_group_info.c_str(), data_group_link_number, data_group_size);

      if (caption_data_type_nibble == 0x00) { // INFO:: caption management data
        int time_control_mode = TS_GM8(ptr + 0, 0, 2);
        uint64_t offset_time = 0;

        if (time_control_mode == 0x02) {
          offset_time = TS_GM64(ptr + 1, 0, 36);

          ptr = ptr + 5;
        }

        int num_languages = TS_G8(ptr + 1);

        ptr = ptr + 2;

        std::string time_control_info;

        if (time_control_mode == 0x00) {
          time_control_info = "free";
        } else if (time_control_mode == 0x01) {
          time_control_info = "real time";
        } else if (time_control_mode == 0x02) {
          time_control_info = "offset time";
        } else if (time_control_mode == 0x03) {
          time_control_info = "reserved";
        }

        printf("Closed Caption:caption managment: time control mode:[0x%01x/%s], offset time:[0x%01x%08x]\n", time_control_mode, time_control_info.c_str(), uint32_t((offset_time >> 32) & 0x0f), uint32_t(offset_time & 0xffffffff));

        for (int i=0; i<num_languages; i++) {
          int language_tag = TS_GM8(ptr + 0, 0, 3);
          int display_mode = TS_GM8(ptr + 0, 4, 4);
          int display_condition_designation = 0;

          if (display_mode == 0xc0 or display_mode == 0x0d or display_mode == 0x0e) {
            display_condition_designation = TS_G8(ptr + 1);

            ptr = ptr + 1;
          }

          std::string language_code = std::string(ptr + 1, 3);
          int format = TS_GM8(ptr + 4, 0, 4);
          int character_code = TS_GM8(ptr + 4, 4, 2);
          int rollup_mode = TS_GM8(ptr + 4, 6, 2);

          std::string display_condition_info;
          std::string character_info;
          std::string rollup_info;

          if (display_condition_designation == 0x00) {
            display_condition_info = "message display of attenuation due to rain";
          } else {
            display_condition_info = "specified otherwise";
          }

          if (character_code == 0x00) {
            character_info = "8bit code";
          } else if (character_code == 0x01) {
            character_info = "reserved for UCS";
          } else if (character_code == 0x02) {
            character_info = "reserved";
          } else if (character_code == 0x03) {
            character_info = "reserved";
          }

          if (rollup_mode == 0x00) {
            rollup_info = "non roll-up";
          } else if (rollup_mode == 0x01) {
            rollup_info = "roll-up";
          } else if (rollup_mode == 0x02) {
            rollup_info = "reserved";
          } else if (rollup_mode == 0x03) {
            rollup_info = "reserved";
          }

          // INFO:: 6-STD B24 v5.2.1 (Data group data/Caption management data)
          printf("Closed Caption:caption managment: language tag:[0x%01x], display mode:[0x%01x], display condition designation:[0x%02x/%s], language code:[%s], format:[0x%01x/%s], character code:[0x%01x/%s], rollup mode:[0x%01x/%s]\n", language_tag, display_mode, display_condition_designation, display_condition_info.c_str(), language_code.c_str(), format, Utils::GetClosedCaptionFormatDescription(format).c_str(), character_code, character_info.c_str(), rollup_mode, rollup_info.c_str());

          int data_unit_loop_length = TS_GM32(ptr + 5, 0, 24);

          ptr = ptr + 8;

          int count_data_unit = 0;

          while (count_data_unit < data_unit_loop_length) {
            int unit_separator = TS_G8(ptr + 0);

            if (unit_separator != 0x1f) {
              break;
            }

            int data_unit_parameter = TS_G8(ptr + 1);
            int data_unit_size = TS_GM32(ptr + 2, 0, 24);

            std::string data_unit_info = "undefined";

            if (data_unit_parameter == 0x20) {
              data_unit_info = "statement body";
            } else if (data_unit_parameter == 0x28) {
              data_unit_info = "geometric";
            } else if (data_unit_parameter == 0x2c) {
              data_unit_info = "synthesized sound";
            } else if (data_unit_parameter == 0x30) {
              data_unit_info = "1-byte DRCS";
            } else if (data_unit_parameter == 0x31) {
              data_unit_info = "2-byte DRCS";
            } else if (data_unit_parameter == 0x34) {
              data_unit_info = "color map";
            } else if (data_unit_parameter == 0x35) {
              data_unit_info = "bit map";
            }

            printf("Closed Caption:caption managment: data unit parameter:[0x%02x/%s]\n", data_unit_parameter, data_unit_info.c_str());

            // 6-STD-B24v5_1-1p3-E1:: pg. 113
            DumpBytes("data unit byte", ptr, data_unit_loop_length);

            count_data_unit = count_data_unit + data_unit_size + 5;

            ptr = ptr + data_unit_size + 5;
          }
        }
      } else if (caption_data_type_nibble == 0x01) { // INFO:: caption statment data
        int time_control_mode = TS_GM8(ptr + 0, 0, 2);
        uint64_t offset_time = 0;

        if (time_control_mode == 0x01 or time_control_mode == 0x02) {
          offset_time = TS_GM64(ptr + 1, 0, 36);

          ptr = ptr + 5;
        }

        std::string time_control_info;

        if (time_control_mode == 0x00) {
          time_control_info = "free";
        } else if (time_control_mode == 0x01) {
          time_control_info = "real time";
        } else if (time_control_mode == 0x02) {
          time_control_info = "offset time";
        } else if (time_control_mode == 0x03) {
          time_control_info = "reserved";
        }

        printf("Closed Caption:caption statement: time control mode:[0x%01x/%s], offset time:[0x%01x%08x]\n", time_control_mode, time_control_info.c_str(), uint32_t((offset_time >> 32) & 0x0f), uint32_t(offset_time & 0xffffffff));

        int data_unit_loop_length = TS_GM32(ptr + 1, 0, 24);

        ptr = ptr + 4;

        int count_data_unit = 0;

        while (count_data_unit < data_unit_loop_length) {
          int unit_separator = TS_G8(ptr + 0);

          if (unit_separator != 0x1f) {
            break;
          }

          int data_unit_parameter = TS_G8(ptr + 1);
          int data_unit_size = TS_GM32(ptr + 2, 0, 24);

          std::string data_unit_info = "undefined";

          if (data_unit_parameter == 0x20) {
            data_unit_info = "statement body";
          } else if (data_unit_parameter == 0x28) {
            data_unit_info = "geometric";
          } else if (data_unit_parameter == 0x2c) {
            data_unit_info = "synthesized sound";
          } else if (data_unit_parameter == 0x30) {
            data_unit_info = "1-byte DRCS";
          } else if (data_unit_parameter == 0x31) {
            data_unit_info = "2-byte DRCS";
          } else if (data_unit_parameter == 0x34) {
            data_unit_info = "color map";
          } else if (data_unit_parameter == 0x35) {
            data_unit_info = "bit map";
          }

          printf("Closed Caption:caption managment: data unit parameter:[0x%02x/%s]\n", data_unit_parameter, data_unit_info.c_str());

          ptr = ptr + 5;

          // 6-STD-B24v5_1-1p3-E1:: pg. 113
          DumpBytes("data unit byte", ptr, data_unit_size);

          std::shared_ptr<SISubtitle> param = SIFacade::GetInstance()->Subtitle();

          param->Unit(ptr, data_unit_size);

          count_data_unit = count_data_unit + data_unit_size + 5;

          ptr = ptr + data_unit_size;
        }
      }
    }

    virtual void ProcessDSMCC(jevent::DemuxEvent *event)
    {
      // INFO:: ISO IEC 13818-6 - MPEG2 DSMCC - Digital Storage Media Command & Control.pdf
      const char *ptr = event->GetData();

      int tid = TS_G8(ptr + 0);
      int section_length = TS_GM16(ptr + 1, 4, 12);
      std::string type;

      if (tid == 0x3a) {
        type = "MPE reserved";
      } else if (tid == 0x3b) {
        type = "DSI/DII message";
      } else if (tid == 0x3c) {
        type = "DDB message";
      } else if (tid == 0x3d) {
        type = "Stream descriptor";
      } else if (tid == 0x3e) {
        type = "Private data";
      } else if (tid == 0x3f) {
        type = "reserved";
      }

      printf("DSMCC:: table id:[0x%02x], type:[%s], section length:[%d]\n", tid, type.c_str(), section_length);

      if (tid == 0x3a) {
        ProcessDSMCCMultiprotocolEncapsulation(event);
      } else if (tid == 0x3b) {
        ProcessDSMCCInformation(event);
      } else if (tid == 0x3c) {
        ProcessDSMCCMessage(event);
      } else if (tid == 0x3d) {
        ProcessDSMCCDescriptor(event);
      } else if (tid == 0x3e) {
        ProcessDSMCCPrivate(event);
      } else if (tid == 0x3f) {
      }
    }

    virtual void ProcessDSMCCMultiprotocolEncapsulation(jevent::DemuxEvent *event)
    {
      // INFO:: EN 301 192 (compliant with dsmcc private sections)
      const char *ptr = event->GetData();

      int section_length = TS_GM16(ptr + 1, 4, 12);
      int mac_address_6 = TS_G8(ptr + 3);
      int mac_address_5 = TS_G8(ptr + 4);
      int payload_scrambling_control = TS_GM8(ptr + 5, 2, 2);
      int address_scrambling_control = TS_GM8(ptr + 5, 4, 2);
      int llc_snap_flag = TS_GM8(ptr + 5, 6, 1);
      // int section_number = TS_G8(ptr + 6);
      // int last_section_number = TS_G8(ptr + 7);
      int mac_address_4 = TS_G8(ptr + 8);
      int mac_address_3 = TS_G8(ptr + 9);
      int mac_address_2 = TS_G8(ptr + 10);
      int mac_address_1 = TS_G8(ptr + 11);

      ptr = ptr + 12;

      printf("DSMCC:mpe: mac address:[0x%02x:%02x:%02x:%02x:%02x:%02x], payload scrambling:[0x%01x], address scrambling:[0%01x]\n", mac_address_1, mac_address_2, mac_address_3, mac_address_4, mac_address_5, mac_address_6, payload_scrambling_control, address_scrambling_control);

      if (llc_snap_flag == 0x01) {
        // INFO:: LLC_SNAP() ISO/IEC 8802.2 Logical Link Control
        
        DumpBytes("LLC SNAP data byte", ptr + 0, section_length - 9 - 4);
      } else {
        int total_length = TS_G16(ptr + 2); // INFO:: IP Datagram

        DumpBytes("ip datagram data byte", ptr + 0, total_length);
      
        ptr = ptr + total_length;
      }
    }

    virtual void ProcessDSMCCInformation(jevent::DemuxEvent *event)
    {
      const char *ptr = event->GetData();

      ptr = ptr + 8;

      int protocol_discriminator = TS_G8(ptr + 0);
      int dsmcc_type = TS_G8(ptr + 1);
      int message_id = TS_G16(ptr + 2);
      uint32_t transaction_id = TS_G32(ptr + 4);
      int reserved = TS_G8(ptr + 8);
      int adaptation_length = TS_G8(ptr + 9);
      // int message_length = TS_G16(ptr + 10);

      printf("DSMCCInformation:: protocol discriminator:[0x%02x], dsmcc type:[0x%02x], message id:[0x%04x], transaction id:[0x%04x]\n", protocol_discriminator, dsmcc_type, message_id, transaction_id);

      if (protocol_discriminator != 0x11 || // MPEG-2 DSM-CC message
          dsmcc_type != 0x03 || // Download message
          reserved != 0xff) {
        return;
      }

      ptr = ptr + 12 + adaptation_length;

      if (message_id == 0x1001) { // DownloadInfoRequest (DIR)
        uint32_t buffer_size = TS_G32(ptr + 0);
        int maximum_block_size = TS_G16(ptr + 4);
        
        printf("DSMCCInfoRequest:: buffer size:[0x%08x], maximum block size:[0x%04x]\n", buffer_size, maximum_block_size);

        ptr = ptr + 6;

        // INFO:: CompatibilityDescriptor()
        int compatibility_descriptor_length = TS_G16(ptr + 0);

        ptr = ptr + compatibility_descriptor_length + 2;

        int private_data_length = TS_G16(ptr + 0);

        DumpBytes("DSMCC:DownloadInfoRequest: private data byte", ptr + 2, private_data_length);
      } else if (message_id == 0x1002) { // DownloadInfoIndication (DII)
        uint32_t download_id = TS_G32(ptr + 0);
        int block_size = TS_G16(ptr + 4);
        int window_size = TS_G8(ptr + 6);
        // int ack_period = TS_G8(ptr + 7);
        // uint32_t tcdownload_window = TS_G32(ptr + 8);
        // uint32_t tcdownload_scenario = TS_G32(ptr + 12);

        ptr = ptr + 16;

        // INFO:: CompatibilityDescriptor()
        int compatibility_descriptor_length = TS_G16(ptr + 0);

        ptr = ptr + compatibility_descriptor_length + 2;

        int number_of_modules = TS_G16(ptr + 0);

        ptr = ptr + 2;

        printf("DSMCC:DownloadInfoIndication<DII>: download id:[0x%08x], block size:[%d], window size:[%d], number of modules:[%d]\n", download_id, block_size, window_size, number_of_modules);

        // INFO:: creating SIData
        std::shared_ptr<SIData> param = SIFacade::GetInstance()->Data();

        if (param->DownloadID() != (uint32_t)-1 and param->DownloadID() != download_id) {
          // TODO:: SIData->Reset() ???

          return;
        }
            
        param->DownloadID(download_id);

        for (int i=0; i<number_of_modules; i++) {
          int module_id = TS_G16(ptr + 0);
          uint32_t module_size = TS_G32(ptr + 2);
          int module_version = TS_G8(ptr + 6);
          // int module_info_length = TS_G8(ptr + 7);

          param->Module(download_id, module_id, module_size, module_version, block_size);

          printf("DSMCC:DownloadInfoIndication<DII>: module id:[0x%04x], module size:[%d], module version:[0x%02x]\n", module_id, module_size, module_version);

          ptr = ptr + 8;

          // INFO:: BIOP::ModuleInfo
          uint32_t module_timeout = TS_G32(ptr + 0);
          uint32_t block_timeout = TS_G32(ptr + 4);
          uint32_t min_block_time = TS_G32(ptr + 8);
          int taps_count = TS_G8(ptr + 12);

          printf("DSMCC:DownloadInfoIndication<DII>:BIOP/ModuleInfo: module timeout:[%d], block timeout:[%d], min block time:[%d]\n", module_timeout, block_timeout, min_block_time);

          ptr = ptr + 13;

          for (int i=0; i<taps_count; i++) {
            int id = TS_G16(ptr + 0);
            int use = TS_G16(ptr + 2);
            int association_tag = TS_G16(ptr + 4);
            int selector_length = TS_G8(ptr + 6);

            // INFO:: TR 101 202 (constant values)
            if (id != 0x0000 or use != 0x0017 or selector_length != 0x00) {
              printf("DSMCC:DownloadInfoIndication<DII>:BIOP/ModuleInfo/Taps:<error>: id:[0x%04x], use:[%s], selector length:[0x%02x]\n", id, Utils::GetTapUseDescription(use).c_str(), selector_length);

              return;
            }

            printf("DSMCC:DownloadInfoIndication<DII>:BIOP/ModuleInfo/Taps: id:[0x%04x], use:[%s], association tag:[0x%04x]\n", id, Utils::GetTapUseDescription(use).c_str(), association_tag);

            ptr = ptr + 7 + selector_length;
          }

          int user_info_length = TS_G8(ptr + 0);

          DumpBytes("DSMCC:DownloadInfoIndication<DII>:BIOP/ModuleInfo: user info data byte", ptr + 1, user_info_length);

          ptr = ptr + 1 + user_info_length;
        }

        int private_data_length = TS_G16(ptr + 0);

        DumpBytes("DSMCC:DownloadInfoIndication<DII>: private data byte", ptr + 2, private_data_length);

        ptr = ptr + 2 + private_data_length;
      } else if (message_id == 0x1003) { // DownloadDataBlock (DDB)
      } else if (message_id == 0x1004) { // DownloadDataRequest (DDR)
      } else if (message_id == 0x1005) { // DownloadCancel (DC)
        uint32_t download_id = TS_G32(ptr + 0);
        int module_id = TS_G16(ptr + 4);
        int block_number = TS_G16(ptr + 6);
        int download_cancel_reason = TS_G8(ptr + 8);
        // int reserved = TS_G8(ptr + 9);
        int private_data_length = TS_G16(ptr + 10);
 
        printf("DSMCC:DownloadCancel: download id:[0x%08x], module id:[0x%04x], block number:[0x%04x], download cancel reason:[0x%02x/%s]\n", download_id, module_id, block_number, download_cancel_reason, Utils::GetDownloadCancelReasonDescription(download_cancel_reason).c_str());

        DumpBytes("DSMCC:DownloadCancel: private data byte", ptr + 12, private_data_length);
      } else if (message_id == 0x1006) { // DownloadServerInitiate (DSI)
        DumpBytes("DSMCC:DownloadServerInitiate<DSI>: server id", ptr + 0, 20);

        ptr = ptr + 20;

        // INFO:: compatibilitydescriptor()
        int compatibility_descriptor_length = TS_G8(ptr + 0);

        ptr = ptr + compatibility_descriptor_length + 2;
        
        int private_data_length = TS_G16(ptr + 0);

        ptr = ptr + 2;

        printf("DSMCC:DownloadServerInitiate<DSI>: private data length:[%d]\n", private_data_length);

        if (private_data_length == 0x00) {
          return;
        }

        // INFO:: ServiceGatewayInfo()
        std::shared_ptr<struct SIData::ior_info_t> ior = SIData::ProcessIOR(ptr + 0);
          
        if (ptr == nullptr) {
          printf("DSMCC:DownloadServerInitiate<DSI>: <IOR error>\n");

          return;
        }

        ptr = ior->ptr;

        int download_taps_count = TS_G8(ptr + 0);

        ptr = ptr + 1;

        for (int i=0; i<download_taps_count; i++) {
          int id = TS_G16(ptr + 0);
          int use = TS_G16(ptr + 2);
          int association_tag = TS_G16(ptr + 4);
          int selector_length = TS_G8(ptr + 6);

          printf("DSMCC:DownloadServerInitiate<DSI>:Taps: id:[0x%04x], use:[%s], association tag:[0x%04x]\n", id, Utils::GetTapUseDescription(use).c_str(), association_tag);

          ptr = ptr + 7 + selector_length;
        }

        int service_context_list_count = TS_G8(ptr + 0);

        ptr = ptr + 1;

        for (int i=0; i<service_context_list_count; i++) {
          uint32_t context_id = TS_G32(ptr + 0);
          int context_data_length = TS_G16(ptr + 4);
        
          printf("DSMCC:DownloadServerInitiate<DSI>: context id:[0x%08x]\n", context_id);

          DumpBytes("DSMCC:DownloadServerInitiate<DSI>: context data byte", ptr + 6, context_data_length);

          ptr = ptr + 6 + context_data_length;
        }

        /*
        int user_info_length = TS_G16(ptr + 0);

        DumpBytes("DSMCC:DownloadServerInitiate<DSI>: user data byte", ptr + 2, user_info_length);
        */

        int descriptors_length = TS_G16(ptr + 0);
        int descriptors_count = 0;

        while (descriptors_count < descriptors_length) {
          // int descriptor_tag = TS_G8(ptr + 0);
          int descriptor_length = TS_G8(ptr + 1);

          DescriptorDump(nullptr, ptr, descriptor_length + 2);

          ptr = ptr + descriptor_length + 2;

          descriptors_count = descriptors_count + descriptor_length + 2;
        }
      }
    }

    virtual void ProcessDSMCCMessage(jevent::DemuxEvent *event)
    {
      const char *ptr = event->GetData();

      ptr = ptr + 8;

      int protocol_discriminator = TS_G8(ptr + 0);
      int dsmcc_type = TS_G8(ptr + 1);
      int message_id = TS_G16(ptr + 2);
      uint32_t download_id = TS_G32(ptr + 4);
      int reserved = TS_G8(ptr + 8);
      int adaptation_length = TS_G8(ptr + 9);
      int message_length = TS_G16(ptr + 10);

      printf("DSMCC:DownloadDataMessage: protocol discriminator:[0x%02x], dsmcc type:[0x%02x], message id:[0x%04x], download id:[0x%08x]\n", protocol_discriminator, dsmcc_type, message_id, download_id);

      if (protocol_discriminator != 0x11 || // MPEG-2 DSM-CC message
          dsmcc_type != 0x03 || // Download message
          reserved != 0xff) {
        return;
      }

      ptr = ptr + 12 + adaptation_length;

      if (message_id == 0x1001) { // DownloadInfoRequest (DIR)
      } else if (message_id == 0x1002) { // DownloadInfoIndication (DII)
      } else if (message_id == 0x1003) { // DownloadDataBlock (DDB)
        int module_id = TS_G16(ptr + 0);
        int module_version = TS_G8(ptr + 2);
        // int reserved = TS_G8(ptr + 3);
        int block_number = TS_G16(ptr + 4);

        printf("DSMCC:DownloadDataMessage: module id:[0x%04x], module version:[0x%02x], block number:[0x%04x]\n", module_id, module_version, block_number);

        ptr = ptr + 6;

        // INFO:: request SIData
        std::shared_ptr<SIData> param = SIFacade::GetInstance()->Data();

        if (param == nullptr) { // INFO:: we need to find DII message first
          return;
        }

        if (param->DownloadID() != download_id) {
          return;
        }
        
        SIFacade::GetInstance()->Data()->ModuleBlock(download_id, module_id, module_version, block_number, std::make_shared<std::string>(ptr, message_length - adaptation_length - 6));
      } else if (message_id == 0x1004) { // DownloadDataRequest (DDR)
        int module_id = TS_G16(ptr + 0);
        int block_number = TS_G16(ptr + 2);
        int download_reason = TS_G8(ptr + 4);

        printf("DSMCC:DownloadDataRequest: module id:[0x%04x], block number:[0x%04x], download reason:[0x%02x/%s]\n", module_id, block_number, download_reason, Utils::GetDownloadReasonDescription(download_reason).c_str());
      } else if (message_id == 0x1005) { // DownloadCancel (DC)
      } else if (message_id == 0x1006) { // DownloadServerInitiate (DSI)
      }
    }

    virtual void ProcessDSMCCDescriptor(jevent::DemuxEvent *event)
    {
      const char *ptr = event->GetData();

      int section_length = TS_GM16(ptr + 1, 4, 12);

      int descriptors_length = section_length - 5 - 4;
      int descriptors_count = 0;

      ptr = ptr + 8;

      // INFO:: ISO IEC 13818-6 - MPEG2 DSMCC - Digital Storage Media Command & Control.pdf; pg.326
      while (descriptors_count < descriptors_length) {
        int descriptor_tag = TS_G8(ptr + 0);
        int descriptor_length = TS_G8(ptr + 1);

        if (descriptor_tag == 0x01) { // npt reference descriptor
          // int post_discontinuity_indicator = TS_GM8(ptr + 2, 0, 1);
          int content_id = TS_GM8(ptr + 2, 1, 7);
          uint64_t STC_reference = (uint64_t)TS_GM8(ptr + 3, 7, 1) << 32 | TS_G32(ptr + 4);
          uint64_t NPT_reference = (uint64_t)TS_GM8(ptr + 11, 7, 1) << 32 | TS_G32(ptr + 12);
          int scale_numerator = TS_G16(ptr + 16);
          int scale_denominator = TS_G16(ptr + 18);

          printf("DSMCCDescriptor:NPT Reference Descriptor: content id:[0x%04x], STC reference:[%lu], NPT reference:[%lu], scale numerator:[%d], scale denominator:[%d]\n", content_id, STC_reference, NPT_reference, scale_numerator, scale_denominator);
        } else if (descriptor_tag == 0x02) { // npt endpoint descriptor
          uint64_t start_NPT = (uint64_t)TS_GM8(ptr + 3, 7, 1) << 32 | TS_G32(ptr + 4);
          uint64_t stop_NPT = (uint64_t)TS_GM8(ptr + 11, 7, 1) << 32 | TS_G32(ptr + 12);

          printf("DSMCCDescriptor:NPT Endpoint Descriptor: start NPT:[%lu], stop NPT:[%lu]\n", start_NPT, stop_NPT);
        } else if (descriptor_tag == 0x03) { // stream mode descriptor
          int stream_mode = TS_G8(ptr + 2);
          std::string mode = "ISO/IEC 13818-6 reserved";

          if (stream_mode == 0x00) {
            mode = "Open";
          } else if (stream_mode == 0x01) {
            mode = "Pause";
          } else if (stream_mode == 0x02) {
            mode = "Transport";
          } else if (stream_mode == 0x03) {
            mode = "Transport Pause";
          } else if (stream_mode == 0x04) {
            mode = "Search Transport";
          } else if (stream_mode == 0x05) {
            mode = "Search Transport Pause";
          } else if (stream_mode == 0x06) {
            mode = "Pause Search Transport";
          } else if (stream_mode == 0x07) {
            mode = "End of Stream";
          } else if (stream_mode == 0x08) {
            mode = "Pre-search Transport";
          } else if (stream_mode == 0x09) {
            mode = "Pre-search Transport Pause";
          }

          printf("DSMCCDescriptor:Stream Mode Descriptor: mode:[%s]\n", mode.c_str());
        } else if (descriptor_tag == 0x04) { // stream event descriptor
          // ABNTNBR15606_2D2_2007Vc3_2008.pdf
          int event_id = TS_G16(ptr + 2);
          uint64_t event_NPT = (uint64_t)TS_GM8(ptr + 7, 7, 1) << 32 | TS_G32(ptr + 8);

          int private_data_length = descriptor_length - 10;
          std::string private_data_byte(ptr + 12, private_data_length);

          printf("DSMCCDescriptor:Stream Event Descriptor: event id:[0x%04x], event npt:[%lu]\n", event_id, event_NPT);

          DumpBytes("private data", private_data_byte.c_str(), private_data_byte.size());
        }

        ptr = ptr + descriptor_length + 2;

        descriptors_count = descriptors_count + descriptor_length + 2;  
      }
    }

    virtual void ProcessLibras(jevent::DemuxEvent *event)
    {
      const char *ptr = event->GetData();

      int section_length = TS_GM16(ptr + 1, 4, 12);

      int descriptors_length = section_length - 5 - 4;
      int descriptors_count = 0;

      ptr = ptr + 8;

      // ABNTNBR15610-3.pdf
      while (descriptors_count < descriptors_length) {
        int descriptor_tag = TS_G8(ptr + 0);
        int descriptor_length = TS_G8(ptr + 1);

        ptr = ptr + 2;

        if (descriptor_tag == 0x04) { // stream event descriptor
          int event_id = TS_G16(ptr + 0);
          // uint32_t reserved = TS_GM32(ptr + 2, 0, 31);
          // uint64_t event_NPT = TS_GM64(ptr + 5, 7, 33);
          // int private_data_length = TS_G8(ptr + 10);
          int libras_tag = TS_G8(ptr + 11);

          ptr = ptr + 12;

          printf("Libras:: event id:[0x%04x], libras_tag:[0x%02x]\n", event_id, libras_tag);

          if (libras_tag == 0x01) { // LCM
            // int message_id = TS_G8(ptr + 0);
            // int message_length = TS_G16(ptr + 1);
            int resolution = TS_GM8(ptr + 3, 4, 4);
            int dph = TS_G16(ptr + 4);
            int dpv = TS_G16(ptr + 6);
            int dfw = TS_G16(ptr + 8);
            int dfh = TS_G16(ptr + 10);
            int libras_content_type = TS_G8(ptr + 12);
            int reserved_data_length = TS_G8(ptr + 13);

            std::string resolution_info = "reserved";
            std::string libras_content_info = "reserved";

            if (resolution == 0x00) {
              resolution_info = "1920x1080";
            } else if (resolution == 0x01) {
              resolution_info = "1280x720";
            } else if (resolution == 0x02) {
              resolution_info = "960x540";
            } else if (resolution == 0x03) {
              resolution_info = "800x600";
            } else if (resolution == 0x04) {
              resolution_info = "720x576";
            } else if (resolution == 0x05) {
              resolution_info = "720x480";
            } else if (resolution == 0x06) {
              resolution_info = "640x480";
            } else if (resolution == 0x07) {
              resolution_info = "480x540";
            } else if (resolution == 0x08) {
              resolution_info = "320x240";
            } else if (resolution == 0x09) {
              resolution_info = "160x240";
            }

            if (libras_content_type == 0x00) {
              libras_content_info = "transmission pause temporarily";
            } else if (libras_content_type == 0x01) {
              libras_content_info = "secondary stream";
            } else if (libras_content_type == 0x02) {
              libras_content_info = "dictionary data";
            }

            printf("Libras:LCM: resolution:[%s], bounds:[%dx%d+%d+%d], libras content type:[%s]\n", resolution_info.c_str(), dfw, dfh, dph, dpv, libras_content_info.c_str());

            DumpBytes("reserved data length", ptr + 14, reserved_data_length);
            
            ptr = ptr + 14;
          } else if (libras_tag == 0x02) { // LDM
            // int message_id = TS_G8(ptr + 0);
            // int reserved = TS_G8(ptr + 1);
            int sinais_loop_length = TS_G16(ptr + 2);
            int sinais_loop_count = 0;

            ptr = ptr + 4;

            while (sinais_loop_count < sinais_loop_length) {
              int glosa_data_length = TS_G8(ptr + 0);
              std::string glosa(ptr + 1, glosa_data_length);

              printf("Libras:LDM: glosa:[%s]\n", glosa.c_str());

              ptr = ptr + 1 + glosa_data_length;

              sinais_loop_count = sinais_loop_count + 1 + glosa_data_length;
            }
          }
        }

        descriptors_count = descriptors_count + descriptor_length + 2;  
      }
    }

    virtual void ProcessDSMCCPrivate(jevent::DemuxEvent *event)
    {
      const char *ptr = event->GetData();

      int section_length = TS_GM16(ptr + 1, 4, 12);

      DumpBytes("DSMCC Private Section", ptr + 8, section_length - 9);
    }

    virtual void DataNotFound(jevent::DemuxEvent *event)
    {
      printf("Data Not Found:: pid:[0x%04x], length:[%d]\n", event->GetPID(), event->GetLength());
    }

};

class ISDBTFileInputStream : public jio::InputStream {

  private:
    jio::InputStream *_stream;
    int _lgap;
    int _rgap;

  public:
    ISDBTFileInputStream(std::string file, int lgap, int rgap):
      jio::InputStream()
    {
      _stream = new jio::FileInputStream(file);

      _lgap = lgap;
      _rgap = rgap;

      // INFO:: try to search the sync byte of transport stream
      int count = 0;
      int64_t c;

      while ((c = _stream->Read()) != 0x47) { // sync byte
        count = count + 1;
      }

      _stream->Reset();
      _stream->Skip(count);
    }
    
    virtual ~ISDBTFileInputStream() 
    {
      if (_stream != nullptr) {
        delete _stream;
        _stream = nullptr;
      }
    }
    
    virtual int64_t Read(char *data, int64_t size)
    {
      _stream->Skip(_lgap);
      
      int64_t 
        r = _stream->Read(data, size);

      _stream->Skip(_rgap);

      if (r <= 0) {
        return -1LL;
      }

      return size;
    }
};

class ISDBTDatagramInputStream : public jio::InputStream {

  private:
    jnetwork::DatagramSocket *_socket;
    jio::InputStream *_stream;
    int _lgap;
    int _rgap;

  public:
    ISDBTDatagramInputStream(int port, int lgap, int rgap):
      jio::InputStream()
    {
      _socket = new jnetwork::DatagramSocket(port);
      _stream = _socket->GetInputStream();

      _lgap = lgap;
      _rgap = rgap;
    }
    
    virtual ~ISDBTDatagramInputStream() 
    {
      if (_stream != nullptr) {
        delete _stream;
        _stream = nullptr;
      }
    }
    
    virtual int64_t Read(char *data, int64_t size)
    {
      _stream->Skip(_lgap);
      
      int64_t 
        r = _stream->Read(data, size);

      _stream->Skip(_rgap);

      if (r <= 0) {
        return -1LL;
      }

      return size;
    }
};

int main(int argc, char **argv)
{
  if (argc < 4) {
    std::cout << "usage:: " << argv[0] << " <file.ts> <lgap> <rgap> [[<id>:<pid>]...]" << std::endl;
    std::cout << std::endl;
    std::cout << "  lgap: bytes before ts packet (lgap + 188 bytes)" << std::endl;
    std::cout << "  rgap: bytes after ts packet (188 + rgap bytes)" << std::endl;
    std::cout << "   pid: count the occurrencies of specific pid or -1 to consider all pids in stream" << std::endl;
    std::cout << std::endl;
    std::cout << "  examples ..." << std::endl;
    std::cout << "    DVB Packet Size (0 + 188 + 0 = 188 bytes) -> (lgap, rgap) = (0, 0)" << std::endl;
    std::cout << "    ISDBT Packet Size (0 + 188 + 16 = 204 bytes) -> (lgap, rgap) = (0, 16)" << std::endl;
    std::cout << "    MTS Packet Size (4 + 188 + 0 = 192 bytes) -> (lgap, rgap) = (4, 0)" << std::endl;
    std::cout << std::endl;
    std::cout << "  processing closed caption without pat information in dvb ..." << std::endl;
    std::cout << "    ./teste <file.ts> 0 0 closed-caption:0x100" << std::endl;
    std::cout << std::endl;

    return -1;
  }

  jmpeg::DemuxManager 
    *manager = jmpeg::DemuxManager::GetInstance();
  jio::InputStream 
    *stream = nullptr;
  jnetwork::URL
    url(argv[1]);

  if (url.GetProtocol() == "file") {
    stream = new ISDBTFileInputStream(url.GetPath(), atoi(argv[2]), atoi(argv[3]));
  } else if (url.GetProtocol() == "udp") {
    stream = new ISDBTDatagramInputStream(url.GetPort(), atoi(argv[2]), atoi(argv[3]));
  }

  if (stream == nullptr) {
    std::cout << "Invalid url:<" << argv[1] << "]" << std::endl;

    return -1;
  }

  manager->SetInputStream(stream);
  
  PSIParser 
    test;

  for (int i=4; i<argc; i++) {
    jcommon::StringTokenizer token(argv[i], ":");

    if (token.GetSize() != 2) {
      continue;
    }

    std::string
      id1 = token.GetToken(0),
      id2 = token.GetToken(1);

    jcommon::ParamMapper 
      mapper;
    
    mapper.SetTextParam(id1, id2);

    if (
        id1 == "pat" or
        id1 == "pmt-xx" or
        id1 == "cat" or
        id1 == "tsdt" or
        id1 == "nit" or
        id1 == "sdt" or
        id1 == "bat" or
        id1 == "tdt" or
        id1 == "tot" or
        id1 == "rst" or
        id1 == "pcr" or
        id1 == "sdtt" or
        id1 == "eit") {
        test.StartPSIDemux(id1, mapper.GetIntegerParam(id1), -1, 3600000);
    } else if (
        id1 == "ait" or
        id1 == "dsmcc-data" or
        id1 == "dsmcc-descriptors" or
        id1 == "libras-data") {
        test.StartPrivateDemux(id1, mapper.GetIntegerParam(id1), -1, 3600000);
    } else if (
        id1 == "closed-caption") {
        test.StartPESDemux(id1, mapper.GetIntegerParam(id1), 3600000);
    }
  }

  manager->Start();
  manager->WaitSync();
  manager->Stop();

  delete stream;
  stream = nullptr;

  // INFO:: dumping methods
  auto 
    data = SIFacade::GetInstance()->Data();

  data->Parse();
  data->Print();
  data->Save("/tmp/data");

  auto 
    subtitle = SIFacade::GetInstance()->Subtitle();

  subtitle->Print();

  auto 
    networks = SIFacade::GetInstance()->Networks();

  for (auto i : networks) {
    i->Print();
  }

  auto 
    services = SIFacade::GetInstance()->Services();

  for (auto i : services) {
    i->Print();
  }

  auto 
    events = SIFacade::GetInstance()->Events();

  for (auto i : events) {
    i->Print();
  }

  auto 
    time = SIFacade::GetInstance()->Time();

  time.Print();

  // INFO:: pid report
  std::map<int, int> pid_report = manager->GetPidReport();
  int count = 0;

  for (std::map<int, int>::iterator i=pid_report.begin(); i!=pid_report.end(); i++) {
    count = count + i->second;
  }

  printf("\n\nPID Report\n");

  for (std::map<int, int>::iterator i=pid_report.begin(); i!=pid_report.end(); i++) {
    printf("pid:[0x%04x], count:[%d]:[%02.04f%%]\n", i->first, i->second, 100.0*i->second/(double)count);
  }

  return 0;
}

