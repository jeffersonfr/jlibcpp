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
#include "jgui/jtextcomponent.h"
#include "jcommon/jstringutils.h"
#include "jexception/joutofboundsexception.h"

#include <algorithm>

namespace jgui {

TextComponent::TextComponent(int x, int y, int width, int height):
  jgui::Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::TextComponent");

	_caret_color = Color(0xff, 0x00, 0x00, 0xff);

	_halign = JHA_LEFT;
	_valign = JVA_CENTER;
	
	_caret_position = 0;
	_echo_char = '\0';
	_is_editable = true;
	_caret_visible = true;
	_caret_type = JCT_UNDERSCORE;
	_selection_start = 0;
	_selection_end = 0;
	_max_text_length = -1;
}
		
TextComponent::~TextComponent()
{
}

Color & TextComponent::GetCaretColor()
{
	return _caret_color;
}

void TextComponent::SetCaretColor(int red, int green, int blue, int alpha)
{
	SetCaretColor(Color(red, green, blue, alpha));
}

void TextComponent::SetCaretColor(const Color &color)
{
	_caret_color = color;

	Repaint();
}

void TextComponent::SetTextSize(int max)
{
	_max_text_length = max;
}

int TextComponent::GetTextSize()
{
	return _max_text_length;
}

void TextComponent::SetEchoChar(char echo_char)
{
	_echo_char = echo_char;

	Repaint();
}

char TextComponent::GetEchoChar()
{
	return _echo_char;
}

bool TextComponent::EchoCharIsSet()
{
	return (_echo_char != '\0');
}

void TextComponent::SetCaretType(jcaret_type_t t)
{
	_caret_type = t;
}

void TextComponent::SetCaretVisible(bool visible)
{
	_caret_visible = visible;
}

std::string TextComponent::GetSelectedText()
{
	return _text.substr(_selection_start, _selection_end);
}

bool TextComponent::IsEditable()
{
	return _is_editable;
}

void TextComponent::SetEditable(bool b)
{
	_is_editable = b;

	Repaint();
}

int TextComponent::GetSelectionStart()
{
	return _selection_start;
}

void TextComponent::SetSelectionStart(int position)
{
	_selection_start = position;

	Repaint();
}

int TextComponent::GetSelectionEnd()
{
	return _selection_end;
}

void TextComponent::SetSelectionEnd(int position)
{
	_selection_end = position;

	Repaint();
}

void TextComponent::Select(int start, int end)
{
	// WARNNING:: no caso de nao haver texto esse metodo irah lancar excecao
	if ((start < 0 || start >= (int)_text.size()) || (end <= start || end > (int)_text.size())) {
		throw jexception::OutOfBoundsException("Index out of range");
	}

	_selection_start = start;
	_selection_end = end;

	Repaint();
}

void TextComponent::SelectAll()
{
	_selection_start = 0;
	_selection_end = _text.size();

	Repaint();
}

void TextComponent::SetCaretPosition(int position)
{
	_caret_position = position;
			
	if (_caret_position > (int)_text.size()) {
		_caret_position = _text.size();
	} else {
		if (_text[_caret_position] == -61) {
			_caret_position++;
		}
	}

	Repaint();
}

int TextComponent::GetCaretPosition()
{
	return _caret_position;
}

void TextComponent::SetText(std::string text)
{
	_text = jcommon::StringUtils::ReplaceString(text, "\t", "    ");

	_caret_position = 0;
	_selection_start = 0;
	_selection_end = 0;
	
	Repaint();

	DispatchTextEvent(new jevent::TextEvent(this, _text));
}

std::string TextComponent::GetText()
{
	return _text;
}

void TextComponent::IncrementCaretPosition(int size)
{
	_caret_position += size;

	if (_caret_position > (int)_text.size()) {
		_caret_position = _text.size();
	} else {
		if (_text[_caret_position] == -61) {
			_caret_position++;
		}
	}

	Repaint();
}

void TextComponent::DecrementCaretPosition(int size)
{
	_caret_position -= size;

	if (_caret_position < 0) {
		_caret_position = 0;
	} else {
		if (_text[_caret_position] == -89) {
			_caret_position--;
		}
	}

	Repaint();
}

void TextComponent::Insert(std::string text)
{
	if (text == "") {
		return;
	}

	text = jcommon::StringUtils::ReplaceString(text, "\t", "    ");

	_selection_start = 0;
	_selection_end = 0;

	if ((int)_text.size() < _max_text_length || _max_text_length == -1) {
		_text = _text.substr(0, _caret_position) + text + _text.substr(_caret_position, _text.size());
			
		IncrementCaretPosition(text.size());
	}

	DispatchTextEvent(new jevent::TextEvent(this, _text));
}

void TextComponent::Append(std::string text)
{
	_caret_position = _text.size();

	Insert(text);
}

void TextComponent::Backspace()
{
	if (_caret_position > 0) {
		_caret_position--;

		Delete();
	}
}

void TextComponent::Delete()
{
	if (_selection_start != _selection_end) {
		_text = _text.substr(0, _selection_start) + _text.substr(_selection_end);

		_caret_position = _selection_start;
		_selection_start = 0;
		_selection_end = 0;
	} else {
		if (_caret_position >= (int)_text.size()) {
			if (_text.size() > 0) {
				if (_text[_caret_position-1] == -89) {
					_text = _text.substr(0, _text.size()-2);

					_caret_position--;
				} else {
					_text = _text.substr(0, _text.size()-1);
				}

				_caret_position--;
			} else {
				return;
			}
		} else {
			// _text = _text.substr(0, _caret_position) + _text.substr(_caret_position+1, _text.size());
			if (_text[_caret_position] == -61) {
				_text = _text.erase(_caret_position, 2);
			} else if (_text[_caret_position] == -89) {
				_text = _text.erase(_caret_position-1, 2);
			} else {
				_text = _text.erase(_caret_position, 1);
			}
		}
	}

	Repaint();

	DispatchTextEvent(new jevent::TextEvent(this, _text));
}

void TextComponent::SetVerticalAlign(jvertical_align_t align)
{
	_valign = align;
	
	Repaint();
}

void TextComponent::SetHorizontalAlign(jhorizontal_align_t align)
{
	_halign = align;
	
	Repaint();
}

jvertical_align_t TextComponent::GetVerticalAlign()
{
	return _valign;
}

jhorizontal_align_t TextComponent::GetHorizontalAlign()
{
	return _halign;
}

void TextComponent::RegisterTextListener(jevent::TextListener *listener)
{
	if (listener == NULL) {
		return;
	}

 	std::lock_guard<std::mutex> guard(_text_listener_mutex);

	if (std::find(_text_listeners.begin(), _text_listeners.end(), listener) == _text_listeners.end()) {
		_text_listeners.push_back(listener);
	}
}

void TextComponent::RemoveTextListener(jevent::TextListener *listener)
{
	if (listener == NULL) {
		return;
	}

 	std::lock_guard<std::mutex> guard(_text_listener_mutex);

  _text_listeners.erase(std::remove(_text_listeners.begin(), _text_listeners.end(), listener), _text_listeners.end());
}

void TextComponent::DispatchTextEvent(jevent::TextEvent *event)
{
	if (event == NULL) {
		return;
	}

	_text_listener_mutex.lock();

	std::vector<jevent::TextListener *> listeners = _text_listeners;

	_text_listener_mutex.unlock();

	for (std::vector<jevent::TextListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
		jevent::TextListener *listener = (*i);

		listener->TextChanged(event);
	}

	delete event;
}

std::vector<jevent::TextListener *> & TextComponent::GetTextListeners()
{
	return _text_listeners;
}

}
