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
#include "jcalendar.h"
#include "jillegalargumentexception.h"

namespace jcommon {
	
Calendar::Calendar():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::Calendar");

	_day = 1;
	_month = 1;
	_year = 1970;
}

Calendar::Calendar(int day, int month, int year):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::Calendar");

	_day = day;
	_month = month;
	_year = year;
}

Calendar::~Calendar()
{
}

bool Calendar::is_bissexto(int ano)
{
	if (((ano%4) == 0 && (ano%100) != 0) || (ano%400) == 0) {
		return true;
	}

	return false;
}

void Calendar::SetTime(int day, int month, int year)
{
	// WARNNING:: 
}

int Calendar::GetDay() 
{
	return _day;
}

int Calendar::GetMonth()
{
	return _month;
}

int Calendar::GetYear()
{
	return _year;
}

bool Calendar::IsBissexto()
{
	return is_bissexto(_year);
}

void Calendar::AddDays(int n)
{
	int vmes[12] = {
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	};
	
	int dia = GetDay();
	int mes = GetMonth();
	int ano = GetYear();
	int count = n;
	
	while (--count >= 0) {
		vmes[1] = (is_bissexto(ano))?29:28;
		
		if (++dia > vmes[mes-1]) {
			dia=1;
			
			if (++mes > 12) {
				mes=1, ano++;
			}
		}
	}

	SetTime(dia, mes, ano);
}


void Calendar::SubDays(int n)
{
	int vmes[12] = {
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	};
	
	int dia = GetDay();
	int mes = GetMonth();
	int ano = GetYear();
	int count = n;
	
	while (--count >= 0) {
		vmes[1] = (is_bissexto(ano))?29:28;
		
		if (--dia < 1) {
			dia=vmes[mes-2];
			
			if (--mes < 1) {
				mes=12, ano--;
			}
		}
	}
	
	SetTime(dia, mes, ano);
}

void Calendar::IncrementDay()
{
	int vmes[12] = {
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	};
	
	if (is_bissexto(GetYear())) {
		vmes[1] = 29;
	}
	
	int dia = GetDay();
	int mes = GetMonth();
	int ano = GetYear();
	
	if (++dia > vmes[mes-1]) {
		dia=1;
		
		if (++mes>12) {
			dia=1;
			mes=1;
			ano++;
		}
	}
	
	SetTime(dia, mes, ano);
}

void Calendar::DecrementDay()
{
	int vmes[12] = {
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	};
	
	if (is_bissexto(GetYear())) {
		vmes[1] = 29;
	}
	
	int dia = GetDay();
	int mes = GetMonth();
	int ano = GetYear();
	
	if (--dia < 1) {
		if (--mes < 1) {
			ano--;
			mes=12;
		}

		dia=vmes[mes-1];
	}
	
	SetTime(dia, mes, ano);
}

bool Calendar::IsValidated()
{
	return true;
}

int Calendar::CountDays(Calendar *date2)
{
	Calendar c;

	int i = 0;
	
	if (Compare(date2) < 0) {
		c.SetTime(GetDay(), GetMonth(), GetYear());
		
		while (c.Equals(date2) == false) {
			c.IncrementDay();
			i++;
		}
	} else if (Compare(date2) > 0) {
		c.SetTime(date2->GetDay(), date2->GetMonth(), date2->GetYear());
		
		while (c.Equals(this) == false) {
			c.IncrementDay();
			i++;
		}
	} else {
		i = 0;
	}
	
	return i;
}

bool Calendar::Equals(Object *o)
{
	if (o->InstanceOf("jcommon::Calendar") == false) {
		return false;
	}

	Calendar *date = (Calendar *)o;
	
	return ((GetDay() == date->GetDay()) && (GetMonth() == date->GetMonth()) && (GetYear() == date->GetYear()))?true:false;
}

int Calendar::Compare(Object *o)
{
	if (o->InstanceOf("jcommon::Calendar") == false) {
		throw IllegalArgumentException("Compare with Incompatible types");
	}

	Calendar *date = (Calendar *)o;
	
	if (GetYear() < date->GetYear()) {
		return -1;
	} else if (GetYear() > date->GetYear()) {
		return 1;
	} else {
		if (GetMonth() < date->GetMonth()) {
		   return -1;
		} else if (GetMonth() > date->GetMonth()) {
			return 1;
		} else {
			if (GetDay() < date->GetDay()) {
				return -1;
			} else if (GetDay() > date->GetDay()) {
				return 1;
			} else {
				return 0;
			}
		}
	}
}

}

