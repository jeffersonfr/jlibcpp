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
#ifndef J_COMPONENT_H
#define J_COMPONENT_H

#include "jobject.h"
#include "jkeyevent.h"
#include "jgraphics.h"
#include "jmutex.h"
#include "jautolock.h"
#include "jcomponentlistener.h"
#include "jfont.h"
#include "jkeyevent.h"
#include "jmouseevent.h"
#include "jkeymap.h"
#include "jkeylistener.h"
#include "jmouselistener.h"
#include "jdatalistener.h"
#include "jtheme.h"
#include "jdataevent.h"

#include <string>
#include <vector>

#define DEFAULT_COMPONENT_WIDTH		128
#define DEFAULT_COMPONENT_HEIGHT	32

namespace jgui {

class FocusListener;
class FocusEvent;
class InteractionListener;
class InteractionEvent;
class Container;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Component : public KeyListener, public MouseListener{

	friend class Container;
	friend class Frame;

	protected:
		/** \brief */
		std::vector<FocusListener *> _focus_listeners;
		/** \brief */
		std::vector<ComponentListener *> _component_listeners;
		/** \brief */
		std::vector<jcommon::DataListener *> _data_listeners;
		/** \brief */
		jthread::Mutex _focus_listener_mutex;
		/** \brief */
		jthread::Mutex _component_listener_mutex;
		/** \brief */
		jthread::Mutex _data_listener_mutex;
		/** \brief */
		jcommon::ParamMapper _params;
		/** \brief */
		jgui::Container *_parent;
		/** \brief */
		Component *_left;
		/** \brief */
		Component *_right;
		/** \brief */
		Component *_up;
		/** \brief */
		Component *_down;
		/** \brief */
		jgui::Theme *_theme;
		/** \brief */
		jgui::KeyMap *_keymap;
		/** \brief */
		std::string _name;
		/** \brief */
		jpoint_t _location;
		/** \brief */
		jpoint_t _scroll_location;
		/** \brief */
		jsize_t _size;
		/** \brief */
		jsize_t _preferred_size;
		/** \brief */
		jsize_t _minimum_size;
		/** \brief */
		jsize_t _maximum_size;
		/** \brief */
		jcomponent_alignment_t _alignment_x;
		/** \brief */
		jcomponent_alignment_t _alignment_y;
		/** \brief */
		jcomponent_orientation_t _orientation;
		/** \brief */
		int _gradient_level;
		/** \brief */
		int _vertical_gap;
		/** \brief */
		int _horizontal_gap;
		/** \brief */
		int _scroll_size;
		/** \brief */
		int _scroll_gap;
		/** \brief */
		int _scroll_minor_increment;
		/** \brief */
		int _scroll_major_increment;
		/** \brief */
		int _relative_mouse_x;
		/** \brief */
		int _relative_mouse_y;
		/** \brief */
		int _relative_mouse_w;
		/** \brief */
		int _relative_mouse_h;
		/** \brief */
		int _component_state;
		/** \brief */
		bool _has_focus;
		/** \brief */
		bool _is_visible;
		/** \brief */
		bool _is_cyclic_focus;
		/** \brief */
		bool _is_navigation_enabled;
		/** \brief */
		bool _is_ignore_repaint;
		/** \brief */
		bool _is_background_visible;
		/** \brief */
		bool _is_focusable;
		/** \brief */
		bool _is_enabled;
		/** \brief */
		bool _is_focus_cycle_root;
		/** \brief */
		bool _is_valid;
		/** \brief */
		bool _is_scrollable_x;
		/** \brief */
		bool _is_scrollable_y;
		/** \brief */
		bool _is_scroll_visible;
		/** \brief */
		bool _is_smooth_scroll;

	protected:
		/**
		 * \brief
		 *
		 */
		virtual void GetInternalComponents(Container *parent, std::vector<Component *> *components);

		/**
		 * \brief
		 *
		 */
		virtual bool ProcessNavigation(KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual void FindNextComponentFocus(jregion_t rect, Component **left, Component **right, Component **up, Component **down);

		/**
		 * \brief Makes sure the component is visible in the scroll if this container is scrollable.
		 * 
		 * @param x
		 * @param y
		 * @param width
		 * @param height
		 * @param coordinateSpace the component according to whose coordinates rect is defined. Rect's 
		 * 		x/y are relative to that component (they are not absolute).
		 *
		 */
		virtual void ScrollToVisibleArea(int x, int y, int width, int height, Component *coordinateSpace);

	public:
		/**
		 * \brief
		 *
		 */
		Component(int x = 0, int y = 0, int width = 0, int height = 0);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Component();

		/**
		 * \brief
		 *
		 */
		virtual void SetName(std::string name);
		
		/**
		 * \brief
		 *
		 */
		virtual std::string GetName();

		/**
		 * \brief
		 *
		 */
		virtual Container * GetParent();
		
		/**
		 * \brief
		 *
		 */
		virtual Container * GetTopLevelAncestor();

		/**
		 * \brief
		 *
		 */
		virtual int GetBaseline(int width, int height);
		
		/**
		 * \brief
		 *
		 */
		virtual jcomponent_behavior_t GetBaselineResizeBehavior();

		/**
		 * \brief
		 *
		 */
		virtual jcomponent_alignment_t GetAlignmentX();
		
		/**
		 * \brief
		 *
		 */
		virtual jcomponent_alignment_t GetAlignmentY();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetAlignmentX(jcomponent_alignment_t align);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetAlignmentY(jcomponent_alignment_t align);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetComponentOrientation(jcomponent_orientation_t orientation);
		
		/**
		 * \brief
		 *
		 */
		virtual jcomponent_orientation_t GetComponentOrientation();

		/**
		 * \brief
		 *
		 */
		virtual bool IsScrollableX();
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsScrollableY();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetScrollableX(bool scrollable);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetScrollableY(bool scrollable);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetScrollable(bool scrollable);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetSmoothScrolling(bool smooth);

		/**
		 * \brief
		 *
		 */
		virtual bool IsCyclicFocus();

		/**
		 * \brief
		 *
		 */
		virtual void SetCyclicFocus(bool b);

		/**
		 * \brief
		 *
		 */
		virtual bool IsSmoothScrolling();

		/**
		 * \brief
		 *
		 */
		virtual bool IsScrollable();
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsScrollVisible();
		
		/**
		 * \brief
		 *
		 */
		virtual jpoint_t GetAbsoluteLocation();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetScrollX();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetScrollY();
		
		/**
		 * \brief
		 *
		 */
		virtual jpoint_t GetScrollLocation();
		
		/**
		 * \brief
		 *
		 */
		virtual jsize_t GetScrollDimension();
		
		/**
		 * \brief
		 *
		 */
		virtual jregion_t GetVisibleBounds();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetScrollX(int x);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetScrollY(int y);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetScrollLocation(jpoint_t t);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetScrollSize();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetScrollSize(int size);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetScrollGap();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetScrollGap(int gap);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetMinorScrollIncrement();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetMajorScrollIncrement();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetMinorScrollIncrement(int increment);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetMajorScrollIncrement(int increment);
		
		/**
		 * \brief
		 *
		 */
		virtual Theme * GetTheme();

		/**
		 * \brief
		 *
		 */
		virtual void SetTheme(Theme *theme);

		/**
		 * \brief
		 *
		 */
		virtual void SetKeyMap(KeyMap *keymap);

		/**
		 * \brief
		 *
		 */
		virtual KeyMap * GetKeyMap();

		/**
		 * \brief
		 *
		 */
		virtual bool GetIgnoreRepaint();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetIgnoreRepaint(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsOpaque();
		
		/**
		 * \brief
		 *
		 */
		virtual void Invalidate();
		
		/**
		 * \brief
		 *
		 */
		virtual void Revalidate();
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsValid();

		/**
		 * \brief
		 *
		 */
		virtual void SetGap(int hgap, int vgap);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetHorizontalGap(int hgap);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetVerticalGap(int vgap);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetHorizontalGap();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetVerticalGap();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetParent(Container *parent);
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsBackgroundVisible();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetBackgroundVisible(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsEnabled();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetEnabled(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetNavigationEnabled(bool b);

		/**
		 * \brief
		 *
		 */
		virtual bool IsNavigationEnabled();

		/**
		 * \brief
		 *
		 */
		virtual Component * GetNextFocusLeft();
		
		/**
		 * \brief
		 *
		 */
		virtual Component * GetNextFocusRight();
		
		/**
		 * \brief
		 *
		 */
		virtual Component * GetNextFocusUp();
		
		/**
		 * \brief
		 *
		 */
		virtual Component * GetNextFocusDown();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetNextFocusLeft(Component *cmp);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetNextFocusRight(Component *cmp);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetNextFocusUp(Component *cmp);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetNextFocusDown(Component *cmp);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetNextComponentFocus(Component *left, Component *right, Component *up, Component *down);

		/**
		 * \brief
		 *
		 */
		virtual bool KeyPressed(KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool KeyReleased(KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool KeyTyped(KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool MousePressed(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseReleased(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseMoved(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseWheel(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual void RequestFocus();
		
		/**
		 * \brief
		 *
		 */
		virtual void ReleaseFocus();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetVisible(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsVisible();
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsHidden();
		
		/**
		 * \brief
		 *
		 */
		virtual bool HasFocus();

		/**
		 * \brief Verify if the second component contains the first one;
		 *
		 */
		virtual bool Contains(Component *c1, Component *c2);
		
		/**
		 * \brief
		 *
		 */
		virtual bool Contains(Component *c, int x, int y, int width, int height);

		/**
		 * \brief
		 *
		 */
		virtual bool Contains(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

		/**
		 * \brief Verify if the second component intersects the first one;
		 *
		 */
		virtual bool Intersects(Component *c1, Component *c2);
		
		/**
		 * \brief
		 *
		 */
		virtual bool Intersects(Component *c, int x, int y, int width, int height);

		/**
		 * \brief
		 *
		 */
		virtual bool Intersects(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

		/**
		 * \brief
		 *
		 */
		virtual void SetMinimumSize(jsize_t size);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetMaximumSize(jsize_t size);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetPreferredSize(jsize_t size);
		
		/**
		 * \brief
		 *
		 */
		virtual jsize_t GetMinimumSize();
		
		/**
		 * \brief
		 *
		 */
		virtual jsize_t GetMaximumSize();
		
		/**
		 * \brief
		 *
		 */
		virtual jsize_t GetPreferredSize();

		/**
		 * \brief
		 *
		 */
		virtual void Move(int x, int y);
		
		/**
		 * \brief
		 *
		 */
		virtual void Move(jpoint_t location);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetBounds(int x, int y, int width, int height);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetBounds(jpoint_t location, jsize_t size);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetBounds(jregion_t region);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetLocation(int x, int y);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetLocation(jpoint_t point);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetSize(int width, int height);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetSize(jsize_t size);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetX();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetY();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetWidth();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetHeight();
		
		/**
		 * \brief
		 *
		 */
		virtual jpoint_t GetLocation();
		
		/**
		 * \brief
		 *
		 */
		virtual jsize_t GetSize();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetGradientLevel(int level);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetGradientLevel();

		/**
		 * \brief
		 *
		 */
		virtual void RaiseToTop();
		
		/**
		 * \brief
		 *
		 */
		virtual void LowerToBottom();
		
		/**
		 * \brief
		 *
		 */
		virtual void PutAtop(Component *c);
		
		/**
		 * \brief
		 *
		 */
		virtual void PutBelow(Component *c);

		/**
		 * \brief
		 *
		 */
		virtual void SetFocusable(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsFocusable();

		/**
		 * \brief
		 *
		 */
		virtual bool IsFocusCycleRoot();

		/**
		 * \brief
		 *
		 */
		virtual void SetFocusCycleRoot(bool b);

		/**
		 * \brief
		 *
		 */
		virtual Container * GetFocusCycleRootAncestor();

		/**
		 * \brief
		 *
		 */
		virtual bool Intersect(int x, int y);

		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);
		
		/**
		 * \brief
		 *
		 */
		virtual void PaintBackground(Graphics *g);
		
		/**
		 * \brief
		 *
		 */
		virtual void PaintBorders(Graphics *g);
		
		/**
		 * \brief
		 *
		 */
		virtual void PaintScrollbars(Graphics *g);

		/**
		 * \brief
		 *
		 */
		virtual void Repaint(Component *cmp = NULL);

		/**
		 * \brief
		 *
		 */
		virtual void RegisterFocusListener(FocusListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveFocusListener(FocusListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchFocusEvent(FocusEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual std::vector<FocusListener *> & GetFocusListeners();

		/**
		 * \brief
		 *
		 */
		virtual void RegisterComponentListener(ComponentListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveComponentListener(ComponentListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchComponentEvent(ComponentEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual std::vector<ComponentListener *> & GetComponentListeners();

		/**
		 * \brief
		 *
		 */
		virtual jcommon::ParamMapper * GetParams();

		/**
		 * \brief
		 *
		 */
		virtual void RegisterDataListener(jcommon::DataListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveDataListener(jcommon::DataListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchDataEvent(jcommon::DataEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual std::vector<jcommon::DataListener *> & GetDataListeners();

};

}

#endif

