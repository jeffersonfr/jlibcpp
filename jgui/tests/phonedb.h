#ifndef PHONEDB_H
#define PHONEDB_H

#include "jxmlparser.h"

#include <string>
#include <iostream>
#include <iostream>
#include <sstream>

using namespace jcommon;

namespace mtel {

class PhoneDB{

	public:
		struct phone_t{
				std::string name;
				std::string phone1;
				std::string phone2;
		};

	private:
		std::vector<phone_t> events;
		std::string _file;

	public:
		PhoneDB(std::string file);
		virtual ~PhoneDB();

		bool Load();
		bool Save();
		int GetCapacity();
		int GetSize();
		struct PhoneDB::phone_t * Get(int i);
		bool IsFull();
		bool IsEmpty();
		bool Add(std::string name, std::string phone1, std::string phone2);
		int Find(std::string name, std::string phone1, std::string phone2);
		int Search(std::string name);
		bool Update(int i, std::string name, std::string phone1, std::string phone2);
		void Remove(int i);
		void RemoveAll();

};

}

#endif

