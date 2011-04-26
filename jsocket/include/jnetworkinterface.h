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
#ifndef J_NETWORKINTERFACE_H
#define J_NETWORKINTERFACE_H

#include "jinetaddress.h"

#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#else
#include <netdb.h>
#include <arpa/inet.h>
#endif

namespace jsocket {

/**
 * \brief InetAddress.
 *
 * \author Jeff Ferr
 */
class NetworkInterface : public virtual jcommon::Object{

	private:
		/** \brief */
		std::vector<NetworkInterface *> _childs;
		/** \brief */
		std::vector <InetAddress *> _addresses;
		/** \brief */
		std::vector <InetAddress *> _broadcast_addresses;
		/** \brief */
		std::vector<uint8_t> _hwaddress;
		/** \brief */
		std::vector<uint8_t> _mask;
		/** \brief */
		NetworkInterface *_parent;
		/** \brief */
		std::string _name;
		/** \brief */
		int _index;
		/** \brief */
		int _mtu;
		/** \brief */
		int _dma;
		/** \brief */
		int _irq;
		/** \brief */
		int _metric;
		/** \brief */
		uint32_t _flags;
		/** \brief */
		bool _is_virtual;

	private:
		/**
		 * \brief Constructor private.
		 *
		 */
		NetworkInterface(NetworkInterface *parent, std::string name, int index, bool is_virtual);

		/**
		 * \brief
		 *
		 */
		void AddInetAddress(InetAddress *addr);

		/**
		 * \brief
		 *
		 */
		void AddBroadcastAddress(InetAddress *addr);

		/**
		 * \brief
		 *
		 */
		void AddSubInterface(NetworkInterface *interface);

	public:
		/**
		 * \brief Destructor virtual.
		 *
		 */
		virtual ~NetworkInterface();

		/**
		 * \brief Convenience method to search for a network interface that has the specified Internet Protocol (IP) address bound to it.
		 *
		 */
		static NetworkInterface * GetByInetAddress(InetAddress *addr);

		/**
		 * \brief Searches for the network interface with the specified name.
		 *
		 */
		static NetworkInterface * GetByName(std::string name);

		/**
		 * \brief Returns all the interfaces on this machine.
		 *
		 */
		static std::vector<NetworkInterface *> GetNetworkInterfaces();

		/**
		 * \brief Get the display name of this network interface.
		 *
		 */
		virtual std::string GetDisplayName();

		/**
		 * \brief Returns the hardware address (usually MAC) of the interface if it has one and if it can be accessed given the current privileges.
		 *
		 */
		virtual std::vector<uint8_t> GetHardwareAddress();

		/**
		 * \brief 
		 *
		 */
		virtual std::vector<uint8_t> GetNetworkMask();

		/**
		 * \brief Convenience method to return an Enumeration with all or a subset of the InetAddresses bound to this network interface.
		 *
		 */
		virtual std::vector<InetAddress *> GetInetAddresses();

		/**
		 * \brief Get a List of all or a subset of the  <address, broadcast> of this network interface.
		 *
		 */
		virtual std::vector<InetAddress *> GetBroadcastAddresses();

		/**
		 * \brief Returns the Maximum Transmission Unit (MTU) of this interface.
		 *
		 */
		virtual int GetDMA();

		/**
		 * \brief Returns the Maximum Transmission Unit (MTU) of this interface.
		 *
		 */
		virtual int GetIRQ();

		/**
		 * \brief Returns the Maximum Transmission Unit (MTU) of this interface.
		 *
		 */
		virtual int GetMetric();

		/**
		 * \brief Returns the Maximum Transmission Unit (MTU) of this interface.
		 *
		 */
		virtual int GetMTU();

		/**
		 * \brief Get the name of this network interface.
		 *
		 */
		virtual std::string GetName();

		/**
		 * \brief
		 *
		 */
		virtual int GetIndex();

		/** 
		 * \brief Returns the parent NetworkInterface of this interface if this is a subinterface, or null if it is a physical (non 
		 * virtual) interface or has no parent. 
		 *
		 */
		virtual NetworkInterface * GetParent();

		/**
		 * \brief Get an Enumeration with all the subinterfaces (also known as virtual interfaces) attached to this network interface.
		 *
		 */
		virtual std::vector<NetworkInterface *> GetSubInterfaces();

		/**
		 * \brief Returns whether a network interface is a loopback interface.
		 *
		 */
		virtual bool IsLoopback();

		/**
		 * \brief Returns whether a network interface is a point to point interface.
		 *
		 */
		virtual bool IsPointToPoint();

		/**
		 * \brief Returns whether a network interface is up and running.
		 *
		 */
		virtual bool IsUp();

		/**
		 * \brief Returns whether this interface is a virtual interface (also called subinterface).
		 *
		 */
		virtual bool IsVirtual();

		/**
		 * \brief Returns whether a network interface supports multicasting or not.
		 *
		 */
		virtual bool SupportsMulticast();

		/**
		 * \brief Returns a string representation of the object.
		 *
		 */
		virtual std::string what();

};

}

#endif
