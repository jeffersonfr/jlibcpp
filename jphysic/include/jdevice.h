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
#ifndef J_DEVICE_H
#define J_DEVICE_H

#include <string>
#include <map>

#include <stdint.h>

namespace jsystem{

enum jdevice_type_t {
	HOST_BRIDGE,
	PCI_BRIDGE,
	ISA_BRIDGE,
	IDE_INTERFACE,
	MULTIMEDIA_AUDIO_CONTROLLER,
	USB_CONTROLLER,
	ETHERNET_CONTROLLER,
	RAID_BUS_CONTROLLER,
	INPUT_DEVICE_CONTROLLER,
	FIREWIRE,
	DISPLAY_CONTROLLER
};

enum jdevice_status_t {
	DEVICE_ACTIVE,
	DEVICE_INATIVE
};
	
/**
 * \brief
 *
 * \author Jeff Ferr
*/
class Device{

	friend class DeviceBuilder;

	private:
		/** \brief */
		std::map<int, std::string> _name;
		/** \brief */
		std::map<int, jdevice_type_t> _type;
		/** \brief */
		std::map<int, jdevice_status_t> _status;
		/** \brief */
		int _number_of_devices;
		
	public:
		/**
		 * \brief
		 *
		 */
		Device();

		/**
		 * \brief
		 * 
		 */
		virtual ~Device();

		/**
		 * \brief
		 * 
		 */
		virtual void Builder() = 0;

		/**
		 * \brief
		 *
		 */
		std::string GetName(int index);

		/**
		 * \brief
		 *
		 */
		jdevice_status_t GetStatus(int index);
		
		/**
		 * \brief
		 *
		 */
		jdevice_type_t GetType(int index);

		/**
		 * \brief
		 *
		 */
		int GetNumberOfDevices();
		
		
};

}

#endif
