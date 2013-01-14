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
#ifndef J_PARAMMAPPER_H
#define J_PARAMMAPPER_H

#include "jobject.h"

#include <string>
#include <map>

#include <stdint.h>

namespace jcommon {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ParamMapper : public virtual jcommon::Object{

	private:
		std::map<std::string, std::string> _params;

	public:
		/**
		 * \Constructor.
		 *
		 */
		ParamMapper();

		/**
		 * \brief Destructor.
		 *
		 */
		virtual ~ParamMapper();

		/**
		 * \brief
		 *
		 */
		virtual std::map<std::string, std::string> & GetParameters();

		/**
		 * \brief 
		 *
		 */
		virtual std::string GetTextParam(std::string key, std::string value = "");

		/**
		 * \brief 
		 *
		 */
		virtual bool GetBooleanParam(std::string key, bool value = false);

		/**
		 * \brief 
		 *
		 */
		virtual int64_t GetIntegerParam(std::string key, int64_t value = 0LL);

		/**
		 * \brief 
		 *
		 */
		virtual double GetDecimalParam(std::string key, double value = 0.0);

		/**
		 * \brief 
		 *
		 */
		virtual void SetTextParam(std::string key, std::string value);

		/**
		 * \brief 
		 *
		 */
		virtual void SetBooleanParam(std::string key, bool value);

		/**
		 * \brief 
		 *
		 */
		virtual void SetIntegerParam(std::string key, int64_t value);

		/**
		 * \brief 
		 *
		 */
		virtual void SetDecimalParam(std::string key, double value);

		/**
		 * \brief
		 *
		 */
		virtual void RemoveParamByID(std::string id);

		/**
		 * \brief
		 *
		 */
		virtual void RemoveParamByIndex(int index);

		/**
		 * \brief
		 *
		 */
		virtual Object * Clone();
		
		/**
		 * \brief
		 *
		 */
		virtual bool Equals(Object *);
		
		/**
		 * \brief
		 *
		 */
		virtual void Copy(Object *o);

};

}

#endif
