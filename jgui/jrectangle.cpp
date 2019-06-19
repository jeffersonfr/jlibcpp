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
#include <type_traits>

namespace jgui {

/**
 * \brief
 *
 */
struct jrational_t {
  int num;
  int den;
};

/**
 * \brief
 *
 */
template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  struct jinsets_t {
    T left;
    T top;
    T right;
    T bottom;
  };

/**
 * \brief
 *
 */
template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  struct jpoint_t {
    T x;
    T y;

    struct jpoint_t operator+(struct jpoint_t &&param)
    {
      return {x + param.x, y + param.y};
    }
  };

/**
 * \brief
 *
 */
template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  struct jpoint3d_t {
    T x;
    T y;
    T z;
    
    operator jgui::jpoint_t<T>()
    {
      return {
        .x = x,
        .y = y
      };
    }

  };

/**
 * \brief
 *
 */
template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  struct jsize_t {
    T width;
    T height;
  };

/**
 * \brief
 *
 */
template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  struct jline_t {
    struct jpoint_t<T> p0;
    struct jpoint_t<T> p1;
};

/**
 * \brief
 *
 */
template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  struct jrect_t {
    struct jpoint_t<T> point;
    struct jsize_t<T> size;
    
    operator jgui::jpoint_t<T>()
    {
      return {
        .x = point.x,
        .y = point.y
      };
    }

    operator jgui::jsize_t<T>()
    {
      return {
        .width = size.width,
        .height = size.height
      };
    }

    operator jgui::jline_t<T>()
    {
      return {
        .p0 = {
          .x = point.x,
          .y = point.y
        },
        .p1 = {
          .x = point.x + size.width,
          .y = point.y + size.height
        }
      };
    }

};

/**
 * \brief
 *
 */
template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  struct jregion_t {
    T x;
    T y;
    T width;
    T height;

    operator jgui::jpoint_t<T>()
    {
      return {
        .x = x,
        .y = y
      };
    }

    operator jgui::jsize_t<T>()
    {
      return {
        .width = width,
        .height = height
      };
    }

    operator jgui::jline_t<T>()
    {
      return {
        .p0 = {
          .x = x,
          .y = y
        },
        .p1 = {
          .x = x + width,
          .y = y + height
        }
      };
    }

    operator jgui::jrect_t<T>()
    {
      return {
        .point = {
          .x = x,
          .y = y
        },
        .size = {
          .width = width,
          .height = height
        }
      };
    }
  };

}

