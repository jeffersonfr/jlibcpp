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
#include "jguilib.h"
#include "jkeyevent.h"
#include "jgraphics.h"
#include "jmutex.h"
#include "jautolock.h"
#include "jcomponentlistener.h"
#include "jfont.h"
#include "jgfxhandler.h"
#include "jkeyevent.h"
#include "jmouseevent.h"

#include <string>
#include <vector>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define DEFAULT_COMPONENT_WIDTH		100
#define DEFAULT_COMPONENT_HEIGHT	40

#define CENTER_VERTICAL_TEXT \
	(_font==NULL)?0:(_font->GetHeight()>_size.height)?0:((_size.height-_font->GetHeight())/2)

/*
#define CENTER_VERTICAL_ERROR \
	(int)(_font->GetHeight()/(2.25*(double)GFXHandler::GetInstance()->GetScreenHeight()/(double)DEFAULT_SCALE_HEIGHT))

#define CENTER_VERTICAL_TEXT \
	(_font==NULL)?0:(_font->GetHeight()>_height)?0:((_height-_font->GetHeight())/2-CENTER_VERTICAL_ERROR)<0?0:((_height-_font->GetHeight())/2-CENTER_VERTICAL_ERROR)
*/

namespace jgui {

enum jcomponent_alignment_t {
	TOP_ALIGNMENT = 0,
	CENTER_ALIGNMENT = 1,
	BOTTOM_ALIGNMENT = 2,
	LEFT_ALIGNMENT = 0,
	RIGHT_ALIGNMENT = 2
};

enum jcomponent_orientation_t {
	LEFT_TO_RIGHT_ORIENTATION,
	RIGHT_TO_LEFT_ORIENTATION,
	TOP_TO_BOTTOM_ORIENTATION,
	BOTTOM_TO_TOP_ORIENTATION,
};

enum jcomponent_border_t {
	NONE_BORDER,
	FLAT_BORDER,
	LINE_BORDER,
	GRADIENT_BORDER,
	ROUND_BORDER,
	BEVEL_BORDER,
	DOWN_BEVEL_BORDER,
	ETCHED_BORDER
};

// component baseline resize behavior
enum jcomponent_behavior_t {
	// Indicates the baseline remains fixed relative to the y-origin.  That is, <code>getBaseline</code> returns
	// the same value regardless of the height or width.  For example, a <code>JLabel</code> containing non-empty 
	// text with a vertical alignment of <code>TOP</code> should have a baseline type of <code>CONSTANT_ASCENT</code>.
	CB_CONSTANT_ASCENT,
	// Indicates the baseline remains fixed relative to the height and does not change as the width is varied.  That is, 
	// for any height H the difference between H and <code>getBaseline(w, H)</code> is the same.  For example, a <code>
	// JLabel</code> containing non-empty text with a vertical alignment of <code>BOTTOM</code> should have a baseline 
	// type of <code>CONSTANT_DESCENT</code>.
	CB_CONSTANT_DESCENT,
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
	CB_CENTER_OFFSET,
	// Indicates the baseline resize behavior can not be expressed using any of the other constants.  
	// This may also indicate the baseline varies with the width of the component.  This is also returned
	// by components that do not have a baseline.
	CB_OTHER
};

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
class Component : public virtual jcommon::Object{

	friend class Container;
	friend class Frame;

	protected:
		jthread::Mutex _component_mutex;

		std::vector<FocusListener *> _focus_listeners;
		std::vector<ComponentListener *> _component_listeners;
		Component *_left, 
				  *_right,
				  *_up,
				  *_down;
		jgui::Container *_parent;
		jgui::Font *_font;
		jpoint_t _location;
		jsize_t _size,
						_minimum_size,
						_maximum_size,
						_preferred_size;
		jcolor_t _bg_color,
						 _fg_color,
						 _bgfocus_color,
						 _fgfocus_color,
						 _border_color,
						 _borderfocus_color;
		jcomponent_alignment_t _alignment_x,
													 _alignment_y;
		jcomponent_orientation_t _orientation;
		jcomponent_border_t _border;
		int _gradient_level,
				_vertical_gap,
				_horizontal_gap,
				_border_size;
		bool _has_focus,
			 _is_visible,
			 _ignore_repaint,
			 _truncate_string,
			 _background_visible,
			 _is_focusable,
			 _enabled,
			 _theme_enabled,
			 _is_opaque,
			 _is_valid;

		/**
		 * \brief
		 *
		 */
		std::string TruncateString(std::string text, int width);

	protected:
		/**
		 * \brief render surface using component_border_t parameter
		 *
		 */
		void FillRectangle(Graphics *g, int x, int y, int width, int height);

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
		static int CountLines(std::string text, int width, Font *font);

		/**
		 * \brief
		 *
		 */
		virtual Container * GetParent();
		
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
		virtual void SetIgnoreRepaint(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetOpaque(bool opaque);
		
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
		virtual void SetParent(Container *parent);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetBackgroundVisible(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetTruncated(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetEnabled(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetNavigation(Component *left, Component *right, Component *up, Component *down);
		
		/**
		 * \brief
		 *
		 */
		virtual Component * GetLeftComponent();
		
		/**
		 * \brief
		 *
		 */
		virtual Component * GetRightComponent();
		
		/**
		 * \brief
		 *
		 */
		virtual Component * GetUpComponent();
		
		/**
		 * \brief
		 *
		 */
		virtual Component * GetDownComponent();
		
		/**
		 * \brief
		 *
		 */
		virtual bool ProcessEvent(KeyEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool ProcessEvent(MouseEvent *event);
		
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
		virtual void SetBackgroundColor(jcolor_t color);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetForegroundColor(jcolor_t color);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetBackgroundFocusColor(jcolor_t color);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetForegroundFocusColor(jcolor_t color);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetBorderColor(jcolor_t color);

		/**
		 * \brief
		 *
		 */
		virtual void SetBorderFocusColor(jcolor_t color);

		/**
		 * \brief
		 *
		 */
		virtual jcolor_t GetBackgroundColor();
		
		/**
		 * \brief
		 *
		 */
		virtual jcolor_t GetForegroundColor();
		
		/**
		 * \brief
		 *
		 */
		virtual jcolor_t GetBackgroundFocusColor();
		
		/**
		 * \brief
		 *
		 */
		virtual jcolor_t GetForegroundFocusColor();
		
		/**
		 * \brief
		 *
		 */
		virtual jcolor_t GetBorderColor();

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
		virtual void PaintBorder(Graphics *g);
		
		/**
		 * \brief
		 *
		 */
		virtual void Repaint(bool all = false);

		/**
		 * \brief
		 *
		 */
		void RegisterFocusListener(FocusListener *listener);
		
		/**
		 * \brief
		 *
		 */
		void RemoveFocusListener(FocusListener *listener);
		
		/**
		 * \brief
		 *
		 */
		void DispatchEvent(FocusEvent *event);
		
		/**
		 * \brief
		 *
		 */
		std::vector<FocusListener *> & GetFocusListeners();

		/**
		 * \brief
		 *
		 */
		void RegisterComponentListener(ComponentListener *listener);
		
		/**
		 * \brief
		 *
		 */
		void RemoveComponentListener(ComponentListener *listener);
		
		/**
		 * \brief
		 *
		 */
		void DispatchEvent(ComponentEvent *event);
		
		/**
		 * \brief
		 *
		 */
		std::vector<ComponentListener *> & GetComponentListeners();

};

}

#endif

