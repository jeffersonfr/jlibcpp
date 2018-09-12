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
#include "jdevice/jcpu.h"
#include "jcommon/jstringtokenizer.h"
#include "jio/jfileinputstream.h"
#include "jio/jbufferedreader.h"
#include "jexception/jruntimeexception.h"

namespace jphysic {

CPU::CPU():
  jdevice::Device()
{
	Builder();
}

CPU::~CPU()
{
}

void CPU::Builder() 
{
	jio::FileInputStream *file = nullptr;
	jio::BufferedReader *reader = nullptr;

	try {
		file = new jio::FileInputStream("/proc/cpuinfo");
		reader = new jio::BufferedReader(file);
	} catch (...) {
		if (file != nullptr) {
			delete file;
		}

		throw jcommon::RuntimeException("Cannot retrive cpu info");
	}
	
	std::string line;
	std::string::size_type pos;

	while (reader->IsEOF() == false) {
		line = reader->ReadLine();
	
		pos = line.find("vendor_id");
		if (pos != std::string::npos) {
			jcommon::StringTokenizer t(line, ": ", jcommon::SPLIT_FLAG, false);
			if (t.GetSize() == 2) {
				_vendor = t.GetToken(1);
			}
		}
		
		pos = line.find("model");
		if (pos != std::string::npos) {
			jcommon::StringTokenizer t(line, ": ", jcommon::SPLIT_FLAG, false);
			if (t.GetSize() == 2) {
				pos = t.GetToken(0).find("model name");
				if (pos != std::string::npos) {
					_name = t.GetToken(1);
				} else {
					_model = t.GetToken(1);
				}
			}
		}
		
		pos = line.find("cpu MHz");
		if (pos != std::string::npos) {
			jcommon::StringTokenizer t(line, ": ", jcommon::SPLIT_FLAG, false);
			if (t.GetSize() == 2) {
				_mhz = t.GetToken(1);
			}
		}
		
		pos = line.find("cache size");
		if (pos != std::string::npos) {
			jcommon::StringTokenizer t(line, ": ", jcommon::SPLIT_FLAG, false);
			if (t.GetSize() == 2) {
				_cache = t.GetToken(1);
			}
		}
		
		pos = line.find("bogomips");
		if (pos != std::string::npos) {
			jcommon::StringTokenizer t(line, ":", jcommon::SPLIT_FLAG, false);
			if (t.GetSize() == 2) {
				_bogomips = t.GetToken(1);
			}
		}
		
		pos = line.find("flags");
		if (pos != std::string::npos) {
			jcommon::StringTokenizer t(line, ":", jcommon::SPLIT_FLAG, false);
			if (t.GetSize() == 2) {
				_flags = t.GetToken(1);
			}
		}
	}
}

std::string CPU::GetVendor()
{
	return _vendor;
}

std::string CPU::GetModel()
{
	return _model;
}

std::string CPU::GetName()
{
	return _name;
}

std::string CPU::GetMHz()
{
	return _mhz;
}

std::string CPU::GetCache()
{
	return _cache;
}

std::string CPU::GetBogomips()
{
	return _bogomips;
}

std::string CPU::GetFlags()
{
	return _flags;
}

std::string CPU::What()
{
	std::ostringstream o;
	
	o << "Vendor:: " << GetVendor() << std::endl;
	o << "Model:: " << GetModel() << std::endl;
	o << "Name: " << GetName() << std::endl;
	o << "MHz:: " << GetMHz() << std::endl;
	o << "Cache size:: " << GetCache() << std::endl;
	o << "Bogomips:: " << GetBogomips() << std::endl;
	o << "Flags:: " << GetFlags() << std::endl;

	return o.str();
}
		
}
