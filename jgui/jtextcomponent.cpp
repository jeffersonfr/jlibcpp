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
#include "Stdafx.h"
#include "jguilib.h"

namespace jgui {

TextComponent::TextComponent(int x, int y, int width, int height):
  jgui::Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::TextComponent");

	_halign = LEFT_HALIGN;
	_valign = CENTER_VALIGN;
	
	_caret_position = 0;
	_echo_char = '\0';
	_is_editable = true;
	_caret_visible = true;
	_caret_type = UNDERSCORE_CURSOR;
	_selection_start = 0;
	_selection_end = 0;
	_max_text_length = -1;
}
		
TextComponent::~TextComponent()
{
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
}

char TextComponent::GetEchoChar()
{
	return _echo_char;
}

bool TextComponent::EchoCharIsSet()
{
	return (_echo_char != '\0');
}

void TextComponent::SetCaretType(jcursor_type_t t)
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
}

int TextComponent::GetSelectionStart()
{
	return _selection_start;
}

void TextComponent::SetSelectionStart(int position)
{
	_selection_start = position;
}

int TextComponent::GetSelectionEnd()
{
	return _selection_end;
}

void TextComponent::SetSelectionEnd(int position)
{
	_selection_end = position;
}

void TextComponent::Select(int start, int end)
{
	// WARNNING:: no caso de nao haver texto esse metodo irah lancar excecao
	if ((start < 0 || start >= (int)_text.size()) || (end <= start || end > (int)_text.size())) {
		throw jcommon::RuntimeException("Index out of range");
	}

	_selection_start = start;
	_selection_end = end;
}

void TextComponent::SelectAll()
{
	_selection_start = 0;
	_selection_end = _text.size();
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
	
	DispatchTextEvent(new TextEvent(this, _text));
}

std::string TextComponent::GetText()
{
	return _text;
}

void TextComponent::IncrementCaretPosition(int size)
{
	{
		jthread::AutoLock lock(&_component_mutex);

		_caret_position += size;

		if (_caret_position > (int)_text.size()) {
			_caret_position = _text.size();
		} else {
			if (_text[_caret_position] == -61) {
				_caret_position++;
			}
		}
	}

	Repaint();
}

void TextComponent::DecrementCaretPosition(int size)
{
	{
		jthread::AutoLock lock(&_component_mutex);

		_caret_position -= size;

		if (_caret_position < 0) {
			_caret_position = 0;
		} else {
			if (_text[_caret_position] == -89) {
				_caret_position--;
			}
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
		Repaint();
	}

	DispatchTextEvent(new TextEvent(this, _text));
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

	DispatchTextEvent(new TextEvent(this, _text));
}

void TextComponent::SetVerticalAlign(jvertical_align_t align)
{
	_valign = align;
}

void TextComponent::SetHorizontalAlign(jhorizontal_align_t align)
{
	_halign = align;
}

jvertical_align_t TextComponent::GetVerticalAlign()
{
	return _valign;
}

jhorizontal_align_t TextComponent::GetHorizontalAlign()
{
	return _halign;
}

void TextComponent::RegisterTextListener(TextListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_text_listeners.begin(), _text_listeners.end(), listener) == _text_listeners.end()) {
		_text_listeners.push_back(listener);
	}
}

void TextComponent::RemoveTextListener(TextListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<TextListener *>::iterator i = std::find(_text_listeners.begin(), _text_listeners.end(), listener);

	if (i != _text_listeners.end()) {
		_text_listeners.erase(i);
	}
}

void TextComponent::DispatchTextEvent(TextEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k=0;

	while (k++ < (int)_text_listeners.size()) {
		_text_listeners[k-1]->TextChanged(event);
	}

	/*
	for (std::vector<TextListener *>::iterator i=_text_listeners.begin(); i!=_text_listeners.end(); i++) {
		(*i)->TextChanged(event);
	}
	*/

	delete event;
}

std::vector<TextListener *> & TextComponent::GetTextListeners()
{
	return _text_listeners;
}

}
