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
#ifndef J_CHECKButton_H
#define J_CHECKButton_H

#include "jgui/jcomponent.h"
#include "jevent/jtogglelistener.h"

#include <mutex>

namespace jgui {

enum jcheckbox_type_t {
  JCBT_CHECK,
  JCBT_RADIO
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class CheckButton : public Component {

  private:
    /** \brief */
    std::vector<jevent::ToggleListener *> _check_listeners;
    /** \brief */
    std::mutex _check_listener_mutex;
    /** \brief */
    std::string _text;
    /** \brief */
    jcheckbox_type_t _type;
    /** \brief */
    jhorizontal_align_t _halign;
    /** \brief */
    jvertical_align_t _valign;
    /** \brief */
    bool _checked;
    /** \brief */
    bool _just_check;
    /** \brief */
    bool _is_wrap;

  public:
    /**
     * \brief
     *
     */
    CheckButton(jcheckbox_type_t type, std::string text, jgui::jrect_t<int> bounds = {0, 0, DEFAULT_COMPONENT_WIDTH, DEFAULT_COMPONENT_HEIGHT});
  
    /**
     * \brief
     *
     */
    virtual ~CheckButton();

    /**
     * \brief
     *
     */
    virtual void SetWrap(bool b);
    
    /**
     * \brief
     *
     */
    virtual bool IsWrap();
    
    /**
     * \brief
     *
     */
    virtual void SetText(std::string text);

    /**
     * \brief
     *
     */
    virtual std::string GetText();

    /**
     * \brief
     *
     */
    virtual void SetType(jcheckbox_type_t type);
    
    /**
     * \brief
     *
     */
    virtual jcheckbox_type_t GetType();
    
    /**
     * \brief
     *
     */
    virtual bool IsSelected();
    
    /**
     * \brief
     *
     */
    virtual void SetSelected(bool b);

    /**
     * \brief
     *
     */
    virtual void SetHorizontalAlign(jhorizontal_align_t align);
    
    /**
     * \brief
     *
     */
    virtual jhorizontal_align_t GetHorizontalAlign();
    
    /**
     * \brief
     *
     */
    virtual void SetVerticalAlign(jvertical_align_t align);
    
    /**
     * \brief
     *
     */
    virtual jvertical_align_t GetVerticalAlign();
    
    /**
     * \brief
     *
     */
    virtual void Paint(Graphics *g);
    
    /**
     * \brief
     *
     */
    virtual bool KeyPressed(jevent::KeyEvent *event);

    /**
     * \brief
     *
     */
    virtual bool MousePressed(jevent::MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual bool MouseReleased(jevent::MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual bool MouseMoved(jevent::MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual bool MouseWheel(jevent::MouseEvent *event);

    /**
     * \brief
     *
     */
    virtual void RegisterToggleListener(jevent::ToggleListener *listener);
    
    /**
     * \brief
     *
     */
    virtual void RemoveToggleListener(jevent::ToggleListener *listener);
    
    /**
     * \brief
     *
     */
    virtual void DispatchToggleEvent(jevent::ToggleEvent *event);
    
    /**
     * \brief
     *
     */
    virtual const std::vector<jevent::ToggleListener *> & GetToggleListeners();

};

}

#endif

