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
#include "jmpeg/jtransportstreampacket.h"
#include "jexception/jinvalidargumentexception.h"

#include <algorithm>

namespace jmpeg {

AdaptationField::AdaptationField(std::shared_ptr<std::string> data):
  jcommon::Object()
{
	jcommon::Object::SetClassName("jmpeg::AdaptationField");

  _data = data;
}

AdaptationField::~AdaptationField()
{
}

std::shared_ptr<std::string> AdaptationField::GetData()
{
  return _data;
}

int AdaptationField::GetDiscontinuityIndicator()
{
  if (_data->size() == 0) {
    return -1;
  }

  return TS_GM8(_data->c_str() + 0, 0, 1);
}

int AdaptationField::GetRandomAccessIndicator()
{
  if (_data->size() == 0) {
    return -1;
  }

  return TS_GM8(_data->c_str() + 0, 1, 1);
}

int AdaptationField::GetElementaryStreamPriorityIndicator()
{
  if (_data->size() == 0) {
    return -1;
  }

  return TS_GM8(_data->c_str() + 0, 2, 1);
}

int AdaptationField::GetPCRFlag()
{
  if (_data->size() == 0) {
    return -1;
  }

  return TS_GM8(_data->c_str() + 0, 3, 1);
}

int AdaptationField::GetOPCRFlag()
{
  if (_data->size() == 0) {
    return -1;
  }

  return TS_GM8(_data->c_str() + 0, 4, 1);
}

int AdaptationField::GetSplicingPointFlag()
{
  if (_data->size() == 0) {
    return -1;
  }

  return TS_GM8(_data->c_str() + 0, 5, 1);
}

int AdaptationField::GetTransportPrivateDataFlag()
{
  if (_data->size() == 0) {
    return -1;
  }

  return TS_GM8(_data->c_str() + 0, 6, 1);
}

int AdaptationField::GetAdaptationFieldExtensionFlag()
{
  if (_data->size() == 0) {
    return -1;
  }

  return TS_GM8(_data->c_str() + 0, 7, 1);
}

TransportStreamPacket::TransportStreamPacket(std::shared_ptr<std::string> data):
  jcommon::Object()
{
	jcommon::Object::SetClassName("jmpeg::TransportStreamPacket");
  
  if (data->size() != TS_PACKET_LENGTH) {
    throw jexception::InvalidArgumentException("Packets must have 188 bytes");
  }

  _data = data;
}

TransportStreamPacket::~TransportStreamPacket()
{
}

std::shared_ptr<std::string> TransportStreamPacket::GetData()
{
  return _data;
}

int TransportStreamPacket::GetSyncByte()
{
  return TS_G8(_data->c_str() + 0);
}

int TransportStreamPacket::GetTransportErrorIndicator()
{
  return TS_GM8(_data->c_str() + 1, 0, 1);
}

int TransportStreamPacket::GetPayloadUnitStartIndicator()
{
  return TS_GM8(_data->c_str() + 1, 1, 1);
}

int TransportStreamPacket::GetTransportPriority()
{
  return TS_GM8(_data->c_str() + 1, 2, 1);
}

int TransportStreamPacket::GetProgramIdentifier()
{
  return TS_GM16(_data->c_str() + 1, 3, 13);
}

int TransportStreamPacket::GetTransportScramblingControl()
{
  return TS_GM8(_data->c_str() + 3, 0, 2);
}

int TransportStreamPacket::GetAdaptationFieldControl()
{
  return TS_GM8(_data->c_str() + 3, 2, 2);
}

int TransportStreamPacket::GetContinuityCounter()
{
  return TS_GM8(_data->c_str() + 3, 4, 4);
}

std::shared_ptr<AdaptationField> TransportStreamPacket::GetAdaptationField()
{
  int adaptation_field_exists = GetAdaptationFieldControl() & 0x02;

  if (adaptation_field_exists == 0) {
    return nullptr;
  }

  int adaptation_field_length = TS_G8(_data->c_str() + TS_HEADER_LENGTH);

  return std::make_shared<AdaptationField>(std::make_shared<std::string>(_data->c_str(), adaptation_field_length + 1));
}

std::shared_ptr<std::string> TransportStreamPacket::GetPayload()
{
  int contains_payload = GetAdaptationFieldControl() & 0x01;

  if (contains_payload == 0) {
    return nullptr;
  }

  std::shared_ptr<AdaptationField> field = GetAdaptationField();

  int offset = TS_HEADER_LENGTH;

  if (field != nullptr) {
    offset = offset + field->GetData()->size();
  }

  return std::make_shared<std::string>(_data->c_str() + offset, TS_PACKET_LENGTH - offset);
}

}
