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
#ifndef J_GEOMETRY_H
#define J_GEOMETRY_H

#include <algorithm>
#include <type_traits>
#include <cmath>

#include <stdio.h>

namespace jgui {

/**
 * \brief
 *
 */
struct jrational_t {
  int num;
  int den;
    
  bool operator==(jrational_t param)
  {
    return (num == param.num and den == param.den);
  }

  bool operator!=(jrational_t param)
  {
    return (num != param.num or den != param.den);
  }

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
 
    jinsets_t<T> & operator=(T param)
    {
      left = param;
      top = param;
      right = param;
      bottom = param;

      return *this;
    }

    bool operator==(jinsets_t<T> param)
    {
      return (left == param.left and top == param.top and right == param.right and bottom == param.bottom);
    }

    bool operator!=(jinsets_t<T> param)
    {
      return (left != param.left or top != param.top or right != param.right or bottom != param.bottom);
    }

  };

/**
 * \brief
 *
 */
template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  struct jpoint_t {
    T x;
    T y;

    template<typename U> operator jgui::jpoint_t<U>()
    {
      return {
        .x = (U)x,
        .y = (U)y
      };
    }

    jpoint_t<T> & operator=(T param)
    {
      x = param;
      y = param;

      return *this;
    }

    template<typename U> jpoint_t<T> operator+(jpoint_t<U> param)
    {
      return {x + param.x, y + param.y};
    }
    
    template<typename U> jpoint_t<T> operator-(jpoint_t<U> param)
    {
      return {x - param.x, y - param.y};
    }
    
    template<typename U> jpoint_t<T> & operator+=(jpoint_t<U> param)
    {
      x = x + param.x;
      y = y + param.y;

      return *this;
    }
    
    template<typename U> jpoint_t<T> & operator-=(jpoint_t<U> param)
    {
      x = x - param.x;
      y = y - param.y;

      return *this;
    }
    
    jpoint_t<T> operator*(float param)
    {
      return {x*param, y*param};
    }
    
    jpoint_t<T> operator/(float param)
    {
      return {x/param, y/param};
    }

    bool operator==(jpoint_t<T> param)
    {
      return (x == param.x and y == param.y);
    }

    bool operator!=(jpoint_t<T> param)
    {
      return (x != param.x or y != param.y);
    }

    template<typename U> float Distance(jpoint_t<U> param)
    {
      T
        px = x - param.x,
        py = y - param.y;

      return std::sqrt(px*px + py*py);
    }

    jpoint_t<float> Normalize()
    {
      return jpoint_t<float>{(float)x, (float)y}/std::sqrt(x*x + y*y);
    }
 
    T Min()
    {
      return std::min<T>(x, y);
    }

    T Max()
    {
      return std::max<T>(x, y);
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
    
    template<typename U> operator jgui::jpoint3d_t<U>()
    {
      return {
        .x = (U)x,
        .y = (U)y,
        .z = (U)z
      };
    }

    jpoint3d_t<T> & operator=(T param)
    {
      x = param;
      y = param;
      z = param;

      return *this;
    }

    template<typename U> jpoint3d_t<T> & operator=(jgui::jpoint_t<U> &param)
    {
      x = param.x;
      y = param.y;
      z = 0;

      return *this;
    }

    template<typename U> jpoint3d_t<T> operator+(jpoint3d_t<U> param)
    {
      return {x + param.x, y + param.y, x + param.z};
    }
    
    template<typename U> jpoint3d_t<T> operator-(jpoint3d_t<U> param)
    {
      return {x - param.x, y - param.y, z - param.z};
    }
    
    template<typename U> jpoint3d_t<T> & operator+=(jpoint3d_t<U> param)
    {
      x = x + param.x;
      y = y + param.y;
      z = z + param.z;

      return *this;
    }
    
    template<typename U> jpoint3d_t<T> & operator-=(jpoint3d_t<U> param)
    {
      x = x - param.x;
      y = y - param.y;
      z = z - param.z;

      return *this;
    }
    
    jpoint3d_t<T> operator*(float param)
    {
      return {x*param, y*param, z*param};
    }
    
    jpoint3d_t<T> operator/(float param)
    {
      return {x/param, y/param, z/param};
    }

    bool operator==(jpoint3d_t<T> param)
    {
      return (x == param.x and y == param.y and z == param.z);
    }

    bool operator!=(jpoint3d_t<T> param)
    {
      return (x != param.x or y != param.y or z != param.z);
    }

    template<typename U> float Distance(jpoint3d_t<U> param)
    {
      T
        px = x - param.x,
        py = y - param.y,
        pz = z - param.z;

      return std::sqrt(px*px + py*py + pz*pz);
    }

    jpoint3d_t<float> Normalize()
    {
      return jpoint3d_t<float>{(float)x, (float)y, (float)z}/std::sqrt(x*x + y*y + z*z);
    }
 
    T Min()
    {
      return std::min<T>(x, y, z);
    }

    T Max()
    {
      return std::max<T>(x, y, z);
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
    
    template<typename U> operator jgui::jsize_t<U>()
    {
      return {
        .width = (U)width,
        .height = (U)height
      };
    }

    jsize_t<T> & operator=(T param)
    {
      width = param;
      height = param;

      return *this;
    }

    template<typename U> jsize_t<T> operator+(jsize_t<U> param)
    {
      return {width + param.width, height + param.height};
    }
    
    template<typename U> jsize_t<T> operator-(jsize_t<U> param)
    {
      return {width - param.width, height - param.height};
    }
    
    template<typename U> jsize_t<T> & operator+=(jsize_t<U> param)
    {
      width = width + param.width;
      height = height + param.height;

      return *this;
    }
    
    template<typename U> jsize_t<T> & operator-=(jsize_t<U> param)
    {
      width = width - param.width;
      height = height - param.height;

      return *this;
    }
    
    jsize_t<T> operator*(float param)
    {
      return {width*param, height*param};
    }
    
    jsize_t<T> operator/(float param)
    {
      return {width/param, height/param};
    }

    bool operator>(jsize_t<T> param)
    {
      return (width*height) > (param.width*param.height);
    }

    bool operator>=(jsize_t<T> param)
    {
      return (width*height) >= (param.width*param.height);
    }

    bool operator<(jsize_t<T> param)
    {
      return (width*height) < (param.width*param.height);
    }

    bool operator<=(jsize_t<T> param)
    {
      return (width*height) <= (param.width*param.height);
    }

    bool operator==(jsize_t<T> param)
    {
      return (width == param.width and height == param.height);
    }

    bool operator!=(jsize_t<T> param)
    {
      return (width != param.width or height != param.height);
    }

    T Area()
    {
      return width*height;
    }

  };

/**
 * \brief
 *
 */
template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  struct jrect_t {
    jpoint_t<T> point;
    jsize_t<T> size;
    
    template<typename U> operator jgui::jrect_t<U>()
    {
      return {
        .point = point,
        .size = size
      };
    }

    jrect_t<T> & operator=(T param)
    {
      point = param;
      size = param;

      return *this;
    }

    template<typename U> jrect_t<T> & operator=(jpoint_t<U> &param)
    {
      point = param;

      return *this;
    }

    template<typename U> jrect_t<T> & operator=(jsize_t<U> &param)
    {
      size = param;

      return *this;
    }

    template<typename U> jrect_t<T> operator+(jrect_t<U> param)
    {
      return {point + param.point, size + param.size};
    }
    
    template<typename U> jrect_t<T> operator-(jrect_t<U> param)
    {
      return {point - param.point, size - param.size};
    }
    
    template<typename U> jrect_t<T> & operator+=(jrect_t<U> param)
    {
      point = point + param.point;
      size = size + param.size;

      return *this;
    }
    
    template<typename U> jrect_t<T> & operator-=(jrect_t<U> param)
    {
      point = point - param.point;
      size = size - param.size;

      return *this;
    }
    
    jrect_t<T> operator*(float param)
    {
      return {point*param, size*param};
    }
    
    jrect_t<T> operator/(float param)
    {
      return {point/param, size/param};
    }

    bool operator>(jrect_t<T> param)
    {
      return size > param.size;
    }

    bool operator>=(jrect_t<T> param)
    {
      return size >= param.size;
    }

    bool operator<(jrect_t<T> param)
    {
      return size < param.size;
    }

    bool operator<=(jrect_t<T> param)
    {
      return size <= param.size;
    }

    bool operator==(jrect_t<T> param)
    {
      return (point == param.point and size == param.size);
    }

    bool operator!=(jsize_t<T> param)
    {
      return (point != param.point or size != param.size);
    }

    template<typename U> bool Contains(jrect_t<U> param)
    {
      return (param.point.x >= point.x) and (param.point.y >= point.y) and ((param.point.x + param.size.width) <= size.width) and ((param.point.y + param.size.height) <= size.height);
    }

    template<typename U> bool Intersects(jpoint_t<U> param)
    {
      if (param.x > point.x and param.x < (point.x + size.width) and param.y > point.y and param.y < (point.y + size.height)) {
        return true;
      }

      return false;
    }

    template<typename U> bool Intersects(jrect_t<U> param)
    {
      return (((point.x > (param.point.x + param.size.width)) or ((point.x + size.width) < param.point.x) or (point.y > (param.point.y + param.size.height)) or ((point.y + size.height) < param.point.y)) == 0);
    }

    template<typename U> jrect_t<T> Intersection(jrect_t<U> param)
    {
      int 
        left = (std::max)(point.x, param.point.x),
        top = (std::max)(point.y, param.point.y),
        right = (std::min)(point.x + size.width, param.point.x + param.size.width),
        bottom = (std::min)(point.y + size.height, param.point.y + param.size.height);

      if (right > left and bottom > top) {
        return {{left, top}, {right - left, bottom - top}};
      }

      return {0, 0, 0, 0};
    }

};

}

#endif

