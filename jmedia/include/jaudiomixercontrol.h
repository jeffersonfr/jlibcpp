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
#include "jio/jinputstream.h"

namespace jmedia {

enum jaudio_format_t {
  // 8-bit support
  JAF_AUDIO_S8, // signed 8-bit samples
  JAF_AUDIO_U8, // unsigned 8-bit samples

  // 16-bit support
  JAF_AUDIO_S16LSB, // signed 16-bit samples in little-endian byte order
  JAF_AUDIO_S16MSB, // signed 16-bit samples in big-endian byte order
  JAF_AUDIO_S16SYS, // signed 16-bit samples in native byte order
  JAF_AUDIO_S16, // JAF_AUDIO_S16LSB
  JAF_AUDIO_U16LSB, // unsigned 16-bit samples in little-endian byte order
  JAF_AUDIO_U16MSB, // unsigned 16-bit samples in big-endian byte order
  JAF_AUDIO_U16SYS, // unsigned 16-bit samples in native byte order
  JAF_AUDIO_U16, // JAF_AUDIO_U16LSB

  // 32-bit support
  JAF_AUDIO_S32LSB, // 32-bit integer samples in little-endian byte order
  JAF_AUDIO_S32MSB, // 32-bit integer samples in big-endian byte order
  JAF_AUDIO_S32SYS, // 32-bit integer samples in native byte order
  JAF_AUDIO_S32, // JAF_AUDIO_S32LSB
  
  // float support
  JAF_AUDIO_F32LSB, // 32-bit floating point samples in little-endian byte order
  JAF_AUDIO_F32MSB, // 32-bit floating point samples in big-endian byte order
  JAF_AUDIO_F32SYS, // 32-bit floating point samples in native byte order
  JAF_AUDIO_F32 // JAF_AUDIO_F32LSB
};

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
    virtual Audio * CreateAudio(jio::InputStream *stream, jaudio_format_t format, int frequency, int channels);

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
