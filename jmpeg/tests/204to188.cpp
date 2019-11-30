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
#include "jexception/joverflowexception.h"
#include "jexception/joutofboundsexception.h"

#include <iostream>

#include <string.h>
#include <stdlib.h>

class ISDBTFileInputStream : public jio::InputStream {

  private:
    jio::InputStream *_stream;
    int _lgap;
    int _rgap;

  public:
    ISDBTFileInputStream(std::string file, int lgap, int rgap):
      jio::InputStream()
    {
      _stream = new jio::FileInputStream(file);

      _lgap = lgap;
      _rgap = rgap;

      // INFO:: try to search the sync byte of transport stream
      int count = 0;
      int64_t c;

      while ((c = _stream->Read()) != 0x47) { // sync byte
        count = count + 1;
      }

      _stream->Reset();
      _stream->Skip(count);
    }
    
    virtual ~ISDBTFileInputStream() 
    {
      if (_stream != nullptr) {
        delete _stream;
        _stream = nullptr;
      }
    }
    
    virtual int64_t Read(char *data, int64_t size)
    {
      _stream->Skip(_lgap);
      
      int64_t 
        r = _stream->Read(data, size);

      _stream->Skip(_rgap);

      if (r <= 0) {
        return -1LL;
      }

      return size;
    }
};

int main(int argc, char **argv)
{
  if (argc < 3) {
    std::cout << "usage:: " << argv[0] << " <in-204> <out-188>" << std::endl;

    return -1;
  }

  ISDBTFileInputStream in(argv[1], 0, 16);
  jio::FileOutputStream out(argv[2]);
  char buffer[188];
  int64_t r;

  while ((r = in.Read(buffer, 188)) == 188) {
    out.Write(buffer, 188);
  }

  out.Close();
  in.Close();

  return 0;
}
