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
#include "jio/jfileinputstream.h"
#include "jio/jfileoutputstream.h"
#include "jio/jobjectinputstream.h"
#include "jio/jobjectoutputstream.h"
#include "jio/jmemoryinputstream.h"
#include "jio/jserializable.h"
#include "jcommon/jjson.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jparserexception.h"

#include <iostream>
#include <sstream>

#include <strings.h>

class Complex : public jio::Serializable {

	private:
		int real;
    int imag;

	public:
		Complex():
			jio::Serializable()
		{
			Serializable::SetClassName("Complex");

			real = 0;
			imag = 0;
		}

		Complex(int r, int i):
			jio::Serializable()
		{
			Serializable::SetClassName("Complex");

			real = r;
			imag = i;
		}

		virtual ~Complex()
		{
		}

		virtual std::string SerializeObject()
		{
			std::ostringstream o;

			o << "{\"real\":" << real << ", \"imag\":" << imag << "}";

			return o.str();
		}

		virtual void AssemblyObject(std::string object)
		{
			try {
        jio::MemoryInputStream
          stream((uint8_t *)object.c_str(), object.size());
				jcommon::JSONValue 
          *root = jcommon::JSON::Parse(&stream),
					*psd = root->GetFirstChild();

				while (psd != nullptr) {
					if (strcasecmp(psd->GetName().c_str(), "real") == 0) {
						real = atoi(psd->GetValue().c_str());
					}

					if (strcasecmp(psd->GetName().c_str(), "imag") == 0) {
						imag = atoi(psd->GetValue().c_str());
					}

					psd = psd->NextSibling();
				}

				delete root;
			} catch (jexception::ParserException &e) {
				throw jexception::RuntimeException("Object assembly exception");
			}
		}

		virtual std::string What()
		{
			std::ostringstream o;

			o << real;
		   
			if (imag < 0) {
				o << " - ";
			} else {
				o << " + ";
			}

			o << imag << "i";

			return o.str();
		}
};

class ObjectInputStreamImpl : public jio::ObjectInputStream {

	public:
		ObjectInputStreamImpl(jio::InputStream *is):
			jio::ObjectInputStream(is)
		{
		}

		virtual jcommon::Object * CreateObject(std::string id)
		{
			if (id == "Complex") {
				return new Complex();
			}

			return nullptr;
		}
};

int main() 
{
	Complex	*a,
			s(2, 3);
	
  jio::FileOutputStream *fos = new jio::FileOutputStream("/tmp/obj");
	jio::FileInputStream *fis = new jio::FileInputStream("/tmp/obj");
	jio::ObjectOutputStream *oos = new jio::ObjectOutputStream(fos);
	jio::ObjectInputStream *ois = new ObjectInputStreamImpl(fis);

	oos->Write(&s);
	oos->Flush();

	a = (Complex *)ois->Read();

	if (a != nullptr) {
		std::cout << "Complex [" << a->What() << "]" << std::endl;
	}

	delete a;

	delete oos;
	delete ois;
	delete fos;
	delete fis;

	return EXIT_SUCCESS;
}

