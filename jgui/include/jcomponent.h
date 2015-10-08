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
#include "jgfxhandler.h"
#include "jkeyevent.h"
#include "jmouseevent.h"
#include "jkeymap.h"
#include "jthemelistener.h"
#include "jkeylistener.h"
#include "jmouselistener.h"

#include <string>
#include <vector>

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

class FocusListener;
class FocusEvent;
class InteractionListener;
class InteractionEvent;
class Container;
class ThemeManager;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Component : public KeyListener, public MouseListener, public jgui::ThemeListener{

	friend class Container;
	friend class Frame;

	protected:
		jthread::Mutex _component_mutex;

		std::vector<FocusListener *> _focus_listeners;
		std::vector<ComponentListener *> _component_listeners;
		jgui::Container *_parent;
		Component *_left;
		Component *_right;
		Component *_up;
		Component *_down;
		jgui::Font *_font;
		jgui::KeyMap *_keymap;
		std::string _name;
		jpoint_t _location;
		jpoint_t _scroll_location;
		jsize_t _size;
		jsize_t _preferred_size;
		jsize_t _minimum_size;
		jsize_t _maximum_size;
		Color _bgcolor;
		Color _fgcolor;
		Color _focus_bgcolor;
		Color _focus_fgcolor;
		Color _border_color;
		Color _focus_border_color;
		Color _scrollbar_color;
		Color _disabled_bgcolor;
		Color _disabled_fgcolor;
		Color _disabled_border_color;
		jcomponent_alignment_t _alignment_x;
		jcomponent_alignment_t _alignment_y;
		jcomponent_orientation_t _orientation;
		jcomponent_border_t _border;
		int _gradient_level;
		int _vertical_gap;
		int _horizontal_gap;
		int _border_size;
		int _scroll_size;
		int _scroll_gap;
		int _scroll_minor_increment;
		int _scroll_major_increment;
		int _relative_mouse_x;
		int _relative_mouse_y;
		int _relative_mouse_w;
		int _relative_mouse_h;
		int _component_state;
		bool _has_focus;
		bool _is_visible;
		bool _is_cyclic_focus;
		bool _is_navigation_enabled;
		bool _is_ignore_repaint;
		bool _is_background_visible;
		bool _is_focusable;
		bool _is_enabled;
		bool _is_focus_cycle_root;
		bool _is_theme_enabled;
		bool _is_valid;
		bool _is_scrollable_x;
		bool _is_scrollable_y;
		bool _is_scroll_visible;
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
		virtual void SetThemeEnabled(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsThemeEnabled();

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
		virtual bool HasFocus();

		/**
		 * \brief
		 *
		 */
		virtual void SetBorder(jcomponent_border_t t);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetBorderSize(int size);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetBorderSize();
		
		/**
		 * \brief Verify if the second component contains the first one;
		 *
		 */
		virtual bool Contains(Component *c1, Component *c2);
		
		/**
		 * \brief
		 *
		 */
		virtual bool Contains(Component *c, int x, int y, int w, int h);

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
		virtual bool Intersects(Component *c, int x, int y, int w, int h);

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
		virtual void SetBounds(int x, int y, int w, int h);
		
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
		virtual void SetFont(Font *font);
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsFontSet();
		
		/**
		 * \brief
		 *
		 */
		virtual Font * GetFont();
		
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
		virtual void SetBackgroundColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetForegroundColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetBackgroundFocusColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetForegroundFocusColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetBorderColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetBorderFocusColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetScrollbarColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetDisabledBackgroundColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetDisabledForegroundColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetDisabledBorderColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetBackgroundColor(const Color &color);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetForegroundColor(const Color &color);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetBackgroundFocusColor(const Color &color);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetForegroundFocusColor(const Color &color);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetBorderColor(const Color &color);

		/**
		 * \brief
		 *
		 */
		virtual void SetBorderFocusColor(const Color &color);

		/**
		 * \brief
		 *
		 */
		virtual void SetScrollbarColor(const Color &color);

		/**
		 * \brief
		 *
		 */
		virtual void SetDisabledBackgroundColor(const Color &color);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetDisabledForegroundColor(const Color &color);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetDisabledBorderColor(const Color &color);

		/**
		 * \brief
		 *
		 */
		virtual Color & GetBackgroundColor();
		
		/**
		 * \brief
		 *
		 */
		virtual Color & GetForegroundColor();
		
		/**
		 * \brief
		 *
		 */
		virtual Color & GetBackgroundFocusColor();
		
		/**
		 * \brief
		 *
		 */
		virtual Color & GetForegroundFocusColor();
		
		/**
		 * \brief
		 *
		 */
		virtual Color & GetBorderColor();

		/**
		 * \brief
		 *
		 */
		virtual Color & GetBorderFocusColor();

		/**
		 * \brief
		 *
		 */
		virtual Color & GetScrollbarColor();

		/**
		 * \brief
		 *
		 */
		virtual Color & GetDisabledBackgroundColor();
		
		/**
		 * \brief
		 *
		 */
		virtual Color & GetDisabledForegroundColor();
		
		/**
		 * \brief
		 *
		 */
		virtual Color & GetDisabledBorderColor();

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
		virtual void ThemeChanged(ThemeEvent *event);
};

}

#endif

