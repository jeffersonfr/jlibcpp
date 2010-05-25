#include "jcommonlib.h"

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

	std::cout << t.what() << std::endl;

	if (t.InstanceOf("jcommon::Object") == true) {
		std::cout << "Teste is instance of Object" << std::endl;
	} else {
		std::cout << "Teste is not instance of Object" << std::endl;
	}
}

void testTrim()
{
	std::string text = " Testando a chamada de funcao Trim\t\t\t\t  ";

	text = jcommon::StringUtils::Trim(text);

	std::cout << ":: Trim :: [" << text << "]" << std::endl << std::endl;
}

void testLink()
{
	jcommon::DynamicLink link;

	link.Load("libm.so", jcommon::LAZY_LINK);
	
	double (*cossine)(double);
	*(void **)(&cossine) = link.FindSymbol("cos");

	std::cout << "Cos(0.0) = " << (*cossine)(0.0) << std::endl;
	
	link.Unload();
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
		std::cout << texts[i] << std::endl;
	}
}

void testURL()
{
	jcommon::URL url_01("http://www.google.com/url/http://redirect.org");
	jcommon::URL url_02("http://charges.com/a?a=1");
	jcommon::URL url_03("ftp://www.lavid.ufpb.br/cvs#ref");
	jcommon::URL url_04("http://phrack.com:80/?a=1&b=2");
	jcommon::URL url_05("file:///test/file.mpg");
	jcommon::URL url_06("./file.mpg");
	jcommon::URL url_07("/file.mpg");
	jcommon::URL url_08("file.mpg");
	jcommon::URL url_09("file:/file.mpg");

	std::cout << "URL 01:: " << url_01.what() << std::endl;
	std::cout << "URL 02:: " << url_02.what() << std::endl;
	std::cout << "URL 03:: " << url_03.what() << std::endl;
	std::cout << "URL 04:: " << url_04.what() << std::endl;
	std::cout << "URL 05:: " << url_05.what() << std::endl;
	std::cout << "URL 06:: " << url_06.what() << std::endl;
	std::cout << "URL 07:: " << url_07.what() << std::endl;
	std::cout << "URL 08:: " << url_08.what() << std::endl;
	std::cout << "URL 09:: " << url_09.what() << std::endl;

	std::cout << "URL 01:: " << url_01.GetPath() << std::endl;
	std::cout << "URL 02:: " << url_02.GetPath() << std::endl;
	std::cout << "URL 03:: " << url_03.GetPath() << std::endl;
	std::cout << "URL 04:: " << url_04.GetPath() << std::endl;
	std::cout << "URL 05:: " << url_05.GetPath() << std::endl;
	std::cout << "URL 06:: " << url_06.GetPath() << std::endl;
	std::cout << "URL 07:: " << url_07.GetPath() << std::endl;
	std::cout << "URL 08:: " << url_08.GetPath() << std::endl;
	std::cout << "URL 09:: " << url_09.GetPath() << std::endl;
}

void testOptions(int argc, char **argv)
{
	jcommon::Options o(argc, argv);

	o.SetOptions("a:");

	std::cout << "Options:: " << o.ExistsOption("a") << ", " << o.ExistsOption("b") << ", " << o.GetArgument("a") << ", " << o.GetArgument("b") << std::endl;
}

void testDate()
{
	long long t = jcommon::Date::CurrentTimeMillis();

	t = t/1000LL;

	jcommon::Date d((time_t)t);

	std::cout << "Date:: " << d.what() << std::endl;
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
	testTrim();
	//testLink();
	//testProperties();
	//testSystem();
	//testStringToken();
	testURL();
	//testOptions(argc, argv);
	//testDate();
	//create_xml_file();
	//html_parser_test();

	return EXIT_SUCCESS;
}

































