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

#include <iostream>
#include <cstdlib>

#include <stdio.h>

using namespace std;

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
