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
#ifndef J_TRANSPORTSTREAMPACKET_H
#define J_TRANSPORTSTREAMPACKET_H

#include "jmpeg/jmpeglib.h"
#include "jcommon/jobject.h"

#include <memory>

namespace jmpeg {

class AdaptationField : public jcommon::Object {

  protected:
    /** \brief */
    std::shared_ptr<std::string> _data;

  public:
    /**
     * \brief
     *
     */
    AdaptationField(std::shared_ptr<std::string> data);

    /**
     * \brief
     *
     */
    virtual ~AdaptationField();

    /**
     * \brief
     *
     */
    std::shared_ptr<std::string> GetData();

    /**
     * \brief
     *
     */
    int GetDiscontinuityIndicator();

    /**
     * \brief
     *
     */
    int GetRandomAccessIndicator();

    /**
     * \brief
     *
     */
    int GetElementaryStreamPriorityIndicator();

    /**
     * \brief
     *
     */
    int GetPCRFlag();

    /**
     * \brief
     *
     */
    int GetOPCRFlag();

    /**
     * \brief
     *
     */
    int GetSplicingPointFlag();

    /**
     * \brief
     *
     */
    int GetTransportPrivateDataFlag();

    /**
     * \brief
     *
     */
    int GetAdaptationFieldExtensionFlag();

};

class TransportStreamPacket : public jcommon::Object {

  protected:
    /** \brief */
    std::shared_ptr<std::string> _data;

  public:
    /**
     * \brief
     *
     */
    TransportStreamPacket(std::shared_ptr<std::string> data);

    /**
     * \brief
     *
     */
    virtual ~TransportStreamPacket();
 
    /**
     * \brief
     *
     */
    std::shared_ptr<std::string> GetData();

    /**
     * \brief
     *
     */
    int GetSyncByte();

    /**
     * \brief
     *
     */
    int GetTransportErrorIndicator();

    /**
     * \brief
     *
     */
    int GetPayloadUnitStartIndicator();

    /**
     * \brief
     *
     */
    int GetTransportPriority();

    /**
     * \brief
     *
     */
    int GetProgramIdentifier();

    /**
     * \brief
     *
     */
    int GetTransportScramblingControl();

    /**
     * \brief
     *
     */
    int GetAdaptationFieldControl();

    /**
     * \brief
     *
     */
    int GetContinuityCounter();

    /**
     * \brief
     *
     */
    std::shared_ptr<AdaptationField> GetAdaptationField();

    /**
     * \brief
     *
     */
    std::shared_ptr<std::string> GetPayload();

};

}

#endif

