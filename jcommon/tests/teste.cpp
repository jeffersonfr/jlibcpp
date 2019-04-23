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
#include "jcommon/jsystem.h"
#include "jcommon/jproperties.h"
#include "jcommon/jstringutils.h"
#include "jcommon/jstringtokenizer.h"
#include "jcommon/jdynamiclink.h"
#include "jcommon/joptions.h"
#include "jnetwork/jhtmlparser.h"
#include "jio/jfile.h"

#include <iostream>

#include <gnu/lib-names.h>

namespace teste {
	
class Teste : public jcommon::Object {

	public:
		Teste():
      jcommon::Object()
		{
			jcommon::Object::SetClassName("teste::Teste");
			
			std::cout << "Hello, world !" << std::endl;
		}
};

}

void testObject()
{
	std::cout << "\n\nTesting Object ...\n" << std::endl;
	
	teste::Teste t;

	std::cout << t.What() << std::endl;

	if (t.InstanceOf("jcommon::Object") == true) {
		std::cout << "Teste is instance of Object" << std::endl;
	} else {
		std::cout << "Teste is not instance of Object" << std::endl;
	}
}

void testTrim()
{
	std::string text = " \t \tTestando a chamada de funcao Trim\t\t\t\t  ";

	std::cout << ":: LTrim :: [" << jcommon::StringUtils::LeftTrim(text) << "]" << std::endl;
	std::cout << ":: RTrim :: [" << jcommon::StringUtils::RightTrim(text) << "]" << std::endl;
	std::cout << ":: Trim :: [" << jcommon::StringUtils::Trim(text) << "]" << std::endl;
}

void testLink()
{
	jcommon::DynamicLink link;
	double (*cosine)(double) = nullptr;

	link.Load(LIBM_SO, jcommon::JLF_LAZY);

	*(void **)(&cosine) = link.FindSymbol("cos");
	// cosine = (double (*)(double))link.FindSymbol("cos");

	std::cout << "Cos(0.0) = " << (*cosine)(0.0) << std::endl;
	
	link.Unload();
}

void testProperties()
{
	jcommon::Properties p("properties.ini");

	p.Load();
	p.Save();
}

void testSystem()
{
	jcommon::System::EnableKeyboardBuffer(false);
	jcommon::System::Getch();
	jcommon::System::EnableKeyboardBuffer(true);
	jcommon::System::Getch();
}

void testStringToken()
{
	std::string s = "O Playstation 3 eh o melhor video-game da atualidade.\nNele podemos encontrar os recursos mais avancados de memoria e processamento suficientes para executar os melhores e mais avancados jogos.\n\t\tMesmo assim, eu gostaria de ter um Wii aqui para a nossa empresa, desta maneira todos nos poderiamos dispersar a tensao apenas com alguns minutos de diversao interativa de qualidade. O       XBox       naum presta, eu soh estou citando ele para encher linguica e conseguir observar a justificacao do texto";

	jcommon::StringTokenizer t(s, "\n", jcommon::JTT_STRING, false);
	std::vector<std::string> super_lines, 
		lines,
		texts;
	std::string temp, 
		previous;
	int max = 100,
		word_size;

	for (int i=0; i<t.GetSize(); i++) {
		temp = jcommon::StringUtils::ReplaceString(t.GetToken(i) + "\n", "\t", " ");

		super_lines.push_back(temp);
	}

	for (int i=0; i<(int)super_lines.size(); i++) {
		jcommon::StringTokenizer w(super_lines[i], " ", jcommon::JTT_STRING, true);
		std::vector<std::string> words;

		for (int i=0; i<w.GetSize(); i++) {
			words.push_back(w.GetToken(i));
		}

		temp = words[0];

		for (int j=1; j<(int)words.size(); j++) {
			previous = temp;
			temp += words[j];

			// font->GetStringWidth(font, temp.c_str(), -1, &word_size);
			word_size = temp.size();

			if (word_size > (max + 1)) {
				temp = words[j];
				texts.push_back(previous);
			}
		}

		texts.push_back(temp);
	}

	for (int i=0; i<(int)texts.size(); i++) {
		std::cout << texts[i] << std::endl;
	}
}

void testOptions(int argc, char **argv)
{
	jcommon::Options o(argc, argv);

	o.SetOptions("a:");

	std::cout << "Options:: " << o.ExistsOption("a") << ", " << o.ExistsOption("b") << ", " << o.GetArgument("a") << ", " << o.GetArgument("b") << std::endl;
}

int main(int argc, char *argv[])
{
	//testObject();
	//testTrim();
	//testLink();
	testProperties();
	//testSystem();
	//testStringToken();
	//testOptions(argc, argv);

	return EXIT_SUCCESS;
}

