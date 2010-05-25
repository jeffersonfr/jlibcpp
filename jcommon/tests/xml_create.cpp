#include "jcommonlib.h"

void create_xml_file()
{
	jcommon::XmlDocument doc;

	jcommon::XmlElement *component = new jcommon::XmlElement("component"); 
	component->SetAttribute("alias", "1");
	component->SetAttribute("guid", "2"); 

	doc.LinkEndChild(component);
	doc.SaveFile("j.xml");
}

int main(int argc, char *argv[])
{
	create_xml_file();

	return EXIT_SUCCESS;
}
