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
#include "jcommon/jdate.h"
#include "jcommon/jproperties.h"
#include "jcommon/jxmlparser.h"
#include "jcommon/jstringutils.h"
#include "jcommon/jstringtokenizer.h"
#include "jcommon/jdynamiclink.h"
#include "jcommon/joptions.h"
#include "jnetwork/jurl.h"
#include "jnetwork/jhtmlparser.h"
#include "jio/jfile.h"

#include <iostream>

#include <gnu/lib-names.h>

namespace teste {
	
class Teste : public jcommon::Object{

	public:
		Teste():jcommon::Object()
		{
			jcommon::Object::SetClassName("teste::Teste");
			
			std::cout << "Hello, world !" << std::cout;
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
	double (*cosine)(double) = NULL;

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

void testURL()
{
	jcommon::URL *url[] = {
		new jcommon::URL("http://www.google.com/url/http://redirect.org"),
		new jcommon::URL("http://www.google.com/?url=http://redirect.org"),
		new jcommon::URL("http://charges.com/a?a=1"),
		new jcommon::URL("ftp://www.lavid.ufpb.br/cvs#ref"),
		new jcommon::URL("http://phrack.com:80/?a=1&b=2"),
		new jcommon::URL("http://phrack.com:80"),
		new jcommon::URL("file:///test/file.mpg"),
		new jcommon::URL("file://test/file.mpg"),
		new jcommon::URL("./file.mpg"),
		new jcommon::URL("/file.mpg"),
		new jcommon::URL("file.mpg"),
		new jcommon::URL("file:/file.mpg")
	};

	for (int i=0; i<12; i++) {
		std::cout << "-------------------------------" << std::endl;
		std::cout << "URL: [" << url[i]->What() << "]" << std::endl;
		std::cout << "GetProtocol(): " << url[i]->GetProtocol() << std::endl;

		if (url[i]->GetProtocol() == "file") {
			std::cout << "GetPath(): " << jio::File::NormalizePath(url[i]->GetPath()) << std::endl;
		} else {
			std::cout << "GetPath(): " << url[i]->GetPath() << std::endl;
		}

		std::cout << "GetQuery(): " << url[i]->GetQuery() << std::endl;
		std::cout << "GetHost(): " << url[i]->GetHost() << std::endl;
		std::cout << "GetPort(): " << url[i]->GetPort() << std::endl;
		std::cout << "GetFile(): " << url[i]->GetFile() << std::endl;
		std::cout << "GetParameters(): " << url[i]->GetParameters() << std::endl;
		std::cout << "GetReference(): " << url[i]->GetReference() << std::endl;

		delete url[i];
	}
}

void testOptions(int argc, char **argv)
{
	jcommon::Options o(argc, argv);

	o.SetOptions("a:");

	std::cout << "Options:: " << o.ExistsOption("a") << ", " << o.ExistsOption("b") << ", " << o.GetArgument("a") << ", " << o.GetArgument("b") << std::endl;
}

void testDate()
{
	jcommon::Date d(std::time(nullptr));

	std::cout << "Date:: " << d.What() << std::endl;
}

void create_xml_file()
{
	jcommon::XmlDocument doc;

	jcommon::XmlElement *component = new jcommon::XmlElement("component"); 
	component->SetAttribute("alias", "1");
   	component->SetAttribute("guid", "2"); 

	doc.LinkEndChild(component);
	doc.SaveFile("j.xml");
}

void html_parser_test()
{
	jcommon::HTMLParser parser;

	parser.Parse("./index.html");
	parser.Dump();
}

int main(int argc, char *argv[])
{
	//testObject();
	//testTrim();
	//testLink();
	testProperties();
	//testSystem();
	//testStringToken();
	//testURL();
	//testOptions(argc, argv);
	//testDate();
	//create_xml_file();
	//html_parser_test();

	return EXIT_SUCCESS;
}

