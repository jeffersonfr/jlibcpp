#include "jobject.h"
#include "jproperties.h"
#include "jdynamiclink.h"
#include "jsystem.h"
#include "jurl.h"
#include "jstringtokenizer.h"
#include "jstringutils.h"
#include "joptions.h"
#include "jdate.h"
#include "jsystem.h"
#include "jxmlparser.h"
#include "jhtmlparser.h"

#include <iostream>
#include <cstdlib>

#include <stdio.h>

using namespace std;

namespace teste {
	
class Teste : public jcommon::Object{

	public:
		Teste():jcommon::Object()
		{
			jcommon::Object::SetClassName("teste::Teste");
			
			printf("Hello, world !\n");
		}
};

}

void testObject()
{
	std::cout << "\n\nTesting Object ...\n" << std::endl;
	
	teste::Teste t;

	std::cout << t.what() << std::endl;

	if (t.InstanceOf("jcommon::Object") == true) {
		std::cout << "Teste is instance of Object" << std::endl;
	} else {
		std::cout << "Teste is not instance of Object" << std::endl;
	}
}

void testLink()
{
	/*
	jcommon::DynamicLink link;
	double (*cossine)(double);

	link.Load("libm.so", RTLD_LAZY);
	*(void **)(&cossine) = link.FindSymbol("cos");
	printf("Cos(0.0) = %.2f\n", (*cossine)(0.0));
	link.Unload("");
	*/
}

void testProperties()
{
	jcommon::Properties p;

	p.Load("/var/dlive/config/reflector.ini");
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

	jcommon::StringTokenizer t(s, "\n", jcommon::SPLIT_FLAG, false);
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
		jcommon::StringTokenizer w(super_lines[i], " ", jcommon::SPLIT_FLAG, true);
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
		puts(texts[i].c_str());
	}
}

void testURL()
{
	jcommon::URL url_01("http://www.google.com");
	jcommon::URL url_02("http://charges.com/a?a=1");
	jcommon::URL url_03("ftp://www.lavid.ufpb.br/cvs#ref");
	jcommon::URL url_04("http://phrack.com:80/?a=1&b=2");
	jcommon::URL url_05("file:///test/file.mpg");
	jcommon::URL url_06("./file.mpg");
	jcommon::URL url_07("/file.mpg");
	jcommon::URL url_08("file.mpg");
	jcommon::URL url_09("file://file.mpg");

	std::cout << "URL 01:: " << url_01.what() << std::endl;
	std::cout << "URL 02:: " << url_02.what() << std::endl;
	std::cout << "URL 03:: " << url_03.what() << std::endl;
	std::cout << "URL 04:: " << url_04.what() << std::endl;
	std::cout << "URL 05:: " << url_05.what() << std::endl;
	std::cout << "URL 06:: " << url_06.what() << std::endl;
	std::cout << "URL 07:: " << url_07.what() << std::endl;
	std::cout << "URL 08:: " << url_08.what() << std::endl;
	std::cout << "URL 09:: " << url_09.what() << std::endl;
}

void testOptions(int argc, char **argv)
{
	jcommon::Options o(argc, argv);

	o.SetOptions("a:");

	printf("Options:: %d, %d, %s, %s\n", o.ExistsOption("a"), o.ExistsOption("b"), o.GetArgument("a").c_str(), o.GetArgument("b").c_str());
}

void testDate()
{
	long long t = jcommon::Date::CurrentTimeMillis();

	t = t/1000LL;

	jcommon::Date d((time_t)t);

	std::cout << "Date:: " << d.what() << std::endl;
}

int justify(std::string full_text, int max)
{
	std::vector<std::string> words, 
		texts;
	int i = 0, 
		k = 0;
	std::string temp, 
		previous;

	// filtra o texto, procurando pelas palavras
	for (i=0; i<(int)full_text.size(); i++) {
		if (isspace((int)full_text[i]) == 0) {
			std::string temp = "";

			do {
				temp += full_text[i];
			} while (i++ < (int)full_text.size() && isspace((int)full_text[i]) == 0);

			words.push_back(temp);

			while (i++ < (int)full_text.size() && isspace((int)full_text[i]) != 0);

			i--;

		}
	}

	// filtra cada linha com o texto maximo permitido
	i = 0;
	while (i < (int)words.size() - 1) {
		temp = words[i] + " ";
		k = 0;

		do {
			previous = temp;
			temp += words[i+1] + " ";
		} while (++i < (int)(words.size()-1) && (int)(temp.size()) < (max + 1));

		if (i != ((int)words.size()-1)) {
			texts.push_back(previous);
			printf("%s\n", previous.c_str());
		}
	}

	temp = previous;
	if (i < (int)words.size()) {
		for (; i<(int)words.size(); i++) {
			temp += words[i] + " ";
		}

		texts.push_back(temp);
		printf("%s\n\n", temp.c_str());
	}

	// justifica o text
	int size;
	for (i=0; i<(int)texts.size()-1; i++) {
		jcommon::StringTokenizer token(texts[i], " ");

		size = (max - (texts[i].size() - token.GetSize()));

		for (int j=0; j<token.GetSize(); j++) {
			printf("%s", token.GetToken(j).c_str());
			
			for (int s=0; s<size/(token.GetSize()-1); s++) {
				printf(" ");
			}
		}

		printf("\n");
	}

	jcommon::StringTokenizer token(texts[i], " ");

	size = (max - (texts[i].size() - token.GetSize()));

	for (int j=0; j<token.GetSize(); j++) {
		printf("%s ", token.GetToken(j).c_str());
	}

	printf("\n");

	return 0;
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
	// testObject();
	// testLink();
	// testProperties();
	// testSystem();
	// testStringToken();
	testURL();
	// testOptions(argc, argv);
	// testDate();
	// justify("This is a simple test with a justify text in your screen. This example show all lines in a text with the same width.", 50);
	// create_xml_file();
	// html_parser_test();

    return EXIT_SUCCESS;
}

































