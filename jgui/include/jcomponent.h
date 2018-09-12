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

#include "jgui/jgraphics.h"
#include "jgui/jfont.h"
#include "jgui/jkeymap.h"
#include "jgui/jtheme.h"
#include "jevent/jdatalistener.h"
#include "jevent/jkeylistener.h"
#include "jevent/jmouselistener.h"
#include "jevent/jcomponentlistener.h"
#include "jevent/jfocuslistener.h"

#include <vector>
#include <mutex>

#define DEFAULT_COMPONENT_WIDTH		128
#define DEFAULT_COMPONENT_HEIGHT	32

namespace jgui {

enum jcomponent_alignment_t {
	JCA_TOP,
	JCA_CENTER,
	JCA_BOTTOM,
	JCA_LEFT,
	JCA_RIGHT
};

enum jcomponent_orientation_t {
	JCO_LEFT_TO_RIGHT,
	JCO_RIGHT_TO_LEFT,
	JCO_UP_TO_BOTTOM,
	JCO_BOTTOM_TO_UP,
};

enum jcomponent_border_t {
	JCB_EMPTY,
	JCB_LINE,
	JCB_BEVEL,
	JCB_ROUND,
	JCB_RAISED_GRADIENT,
	JCB_LOWERED_GRADIENT,
	JCB_RAISED_BEVEL,
	JCB_LOWERED_BEVEL,
	JCB_RAISED_ETCHED,
	JCB_LOWERED_ETCHED
};

// component baseline resize behavior
enum jcomponent_behavior_t {
	// Indicates the baseline remains fixed relative to the y-origin.  That is, <code>getBaseline</code> returns
	// the same value regardless of the height or width.  For example, a <code>JLabel</code> containing non-empty 
	// text with a vertical alignment of <code>TOP</code> should have a baseline type of <code>CONSTANT_ASCENT</code>.
	JCB_CONSTANT_ASCENT,
	// Indicates the baseline remains fixed relative to the height and does not change as the width is varied.  That is, 
	// for any height H the difference between H and <code>getBaseline(w, H)</code> is the same.  For example, a <code>
	// JLabel</code> containing non-empty text with a vertical alignment of <code>BOTTOM</code> should have a baseline 
	// type of <code>CONSTANT_DESCENT</code>.
	JCB_CONSTANT_DESCENT,
	// Indicates the baseline remains a fixed distance from the center of the component.  That is, for any height H the
	// difference between <code>getBaseline(w, H)</code> and <code>H / 2</code> is the same (plus or minus one depending 
	// upon rounding error). <p> Because of possible rounding errors it is recommended you ask for the baseline with two 
	// consecutive heights and use the return value to determine if you need to pad calculations by 1.  The following shows 
	// how to calculate the baseline for any height:
	// <pre>
	//    jregion_t preferredSize = component->GetPreferredSize();
	//    int baseline = GetBaseline(preferredSize.width, preferredSize.height);
	//    int nextBaseline = GetBaseline(preferredSize.width, preferredSize.height + 1);
	// 		// Amount to add to height when calculating where baseline lands for a particular height:
	// 		int padding = 0;
	// 		// Where the baseline is relative to the mid point
	// 		int baselineOffset = baseline - height / 2;
	// 		if (preferredSize.height % 2 == 0 && baseline != nextBaseline) {
	// 			padding = 1;
	// 		} else if (preferredSize.height % 2 == 1 && baseline == nextBaseline) {
	// 		  baselineOffset--;
	// 		  padding = 1;
	// 		}
	// 		// The following calculates where the baseline lands for the height z:
	// 		int calculatedBaseline = (z + padding) / 2 + baselineOffset;
	// 	</pre>
	JCB_CENTER_OFFSET,
	// Indicates the baseline resize behavior can not be expressed using any of the other constants.  
	// This may also indicate the baseline varies with the width of the component.  This is also returned
	// by components that do not have a baseline.
	JCB_OTHER
};

class Container;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Component : public jevent::KeyListener, public jevent::MouseListener {

	friend class Container;
	friend class Frame;

	private:
		/** \brief */
		std::vector<jevent::FocusListener *> _focus_listeners;
		/** \brief */
		std::vector<jevent::ComponentListener *> _component_listeners;
		/** \brief */
		std::vector<jevent::DataListener *> _data_listeners;
		/** \brief */
		std::mutex _focus_listener_mutex;
		/** \brief */
		std::mutex _component_listener_mutex;
		/** \brief */
		std::mutex _data_listener_mutex;
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
		jgui::KeyMap *_keymap;
		/** \brief */
		jgui::Theme *_theme;
		/** \brief */
		std::string _name;
		/** \brief */
		jpoint_t _location;
		/** \brief */
		jpoint_t _scroll_location;
		/** \brief */
    int _scroll_minor_increment;
		/** \brief */
    int _scroll_major_increment;
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
		virtual bool ProcessNavigation(jevent::KeyEvent *event);

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
		virtual jgui::jpoint_t GetScrollLocation();
		
		/**
		 * \brief
		 *
		 */
		virtual jgui::jsize_t GetScrollDimension();
		
		/**
		 * \brief
		 *
		 */
		virtual jgui::jregion_t GetVisibleBounds();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetScrollLocation(int x, int yt);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetScrollLocation(jpoint_t t);
		
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
		virtual bool IsIgnoreRepaint();
		
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
		virtual bool KeyPressed(jevent::KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool KeyReleased(jevent::KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool KeyTyped(jevent::KeyEvent *event);

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
		virtual void SetVisible(bool visible);
		
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
		virtual void Repaint(Component *cmp = nullptr);

		/**
		 * \brief
		 *
		 */
		virtual void RegisterFocusListener(jevent::FocusListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveFocusListener(jevent::FocusListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchFocusEvent(jevent::FocusEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual const std::vector<jevent::FocusListener *> & GetFocusListeners();

		/**
		 * \brief
		 *
		 */
		virtual void RegisterComponentListener(jevent::ComponentListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveComponentListener(jevent::ComponentListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchComponentEvent(jevent::ComponentEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual const std::vector<jevent::ComponentListener *> & GetComponentListeners();

		/**
		 * \brief
		 *
		 */
		virtual jcommon::ParamMapper * GetParams();

		/**
		 * \brief
		 *
		 */
		virtual void RegisterDataListener(jevent::DataListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveDataListener(jevent::DataListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchDataEvent(jevent::DataEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual const std::vector<jevent::DataListener *> & GetDataListeners();

};

}

#endif

