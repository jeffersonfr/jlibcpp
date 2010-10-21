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
#include "jfileinputstream.h"
#include "jfileoutputstream.h"
#include "jobjectinputstream.h"
#include "jobjectoutputstream.h"
#include "jserializable.h"
#include "jjson.h"

#include <iostream>
#include <sstream>

using namespace std;
using namespace jio;

class Complex : public Serializable {

	private:
		int real,
			imaginary;

	public:
		Complex():
			jio::Serializable()
		{
			Serializable::SetClassName("Complex");

			real = 0;
			imaginary = 0;
		}

		Complex(int r, int i):
			jio::Serializable()
		{
			Serializable::SetClassName("Complex");

			real = r;
			imaginary = i;
		}

		virtual ~Complex()
		{
		}

		virtual std::string SerializeObject()
		{
			std::ostringstream o;

			o << "{\"real\":" << real << ", \"imaginary\":" << imaginary << "}";

			return o.str();
		}

		virtual void AssemblyObject(std::string object)
		{
			jcommon::JSONDomParser parser;

			parser.append(object.c_str(), object.size());

			if (NULL != parser.getError()) {
				return;
			}

			jcommon::JSONIterator iterator(parser.getValue());

			for (const jcommon::JSONNode *node=iterator.getNext(); node!=NULL; node=iterator.getNext()) {
				if (node->getType() == jcommon::JSONNode::ePair) {
					jcommon::JSONPairNode *pnode = ((jcommon::JSONPairNode*)node);
					jcommon::JSONIntNode *inode = ((jcommon::JSONIntNode*)pnode->getValue());

					if (strcmp(pnode->getName(), "real") == 0) {
						real = inode->getValue();
					} else if (strcmp(pnode->getName(), "imaginary") == 0) {
						imaginary = inode->getValue();
					}
				}
			}
		}

		std::string what()
		{
			std::ostringstream o;

			o << real;
		   
			if (imaginary < 0) {
				o << " - ";
			} else {
				o << " + ";
			}

			o << imaginary << "i";

			return o.str();
		}
};

class ObjectInputStreamImpl : public ObjectInputStream {

	public:
		ObjectInputStreamImpl(InputStream *is):
			ObjectInputStream(is)
		{
		}

		virtual jcommon::Object * CreateObject(std::string id)
		{
			if (id == "Complex") {
				return new Complex();
			}

			return NULL;
		}
};

int main() 
{
	Complex	*a,
			s(2, 3);
	
	FileOutputStream *fos = new FileOutputStream("/tmp/obj");
	FileInputStream *fis = new FileInputStream("/tmp/obj");
	ObjectOutputStream *oos = new ObjectOutputStream(fos);
	ObjectInputStream *ois = new ObjectInputStreamImpl(fis);

	oos->Write(&s);
	oos->Flush();

	a = (Complex *)ois->Read();

	if (a != NULL) {
		std::cout << "Complex [" << a->what() << "]" << std::endl;
	}

	delete a;

	delete oos;
	delete ois;
	delete fos;
	delete fis;

	return EXIT_SUCCESS;
}

