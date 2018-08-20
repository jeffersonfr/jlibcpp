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
#ifndef J_AUDIOMIXERCONTROL_H
#define J_AUDIOMIXERCONTROL_H

#include "jmedia/jcontrol.h"

namespace jmedia {

class Audio : public virtual jcommon::Object {
  
  public:

  public:
    /**
     * \brief 
     *
     */
    Audio();

    /**
     * \brief 
     *
     */
    virtual ~Audio();

    /**
     * \brief 
     *
     */
    virtual bool IsLoopEnabled();

    /**
     * \brief 
     *
     */
    virtual float GetVolume();

    /**
     * \brief 
     *
     */
    virtual void SetLoopEnabled(bool enabled);

    /**
     * \brief 
     *
     */
    virtual void SetVolume(float volume);

};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class AudioMixerControl : public Control {

	public:
		/**
		 * \brief 
		 *
		 */
		AudioMixerControl();

		/**
		 * \brief
		 *
		 */
		virtual ~AudioMixerControl();

		/**
		 * \brief
		 *
		 */
    virtual Audio * CreateAudio(std::string filename);

		/**
		 * \brief
		 *
		 */
    virtual void StartSound(Audio *audio);

		/**
		 * \brief
		 *
		 */
    virtual void StopSound(Audio *audio);

};

}

#endif 
