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

namespace jgui {

/**
 * \brief
 *
 */
struct jrational_t {
  int num;
  int den;
    
  bool operator==(jrational_t<T> param)
  {
    return (num == param.num and den == param.den);
  }

  bool operator!=(jrational_t<T> param)
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

    jpoint_t<T> operator+(jpoint_t<T> param)
    {
      return {x + param.x, y + param.y};
    }
    
    jpoint_t<T> operator-(jpoint_t<T> param)
    {
      return {x - param.x, y - param.y};
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

    float Distance(jpoint_t<T> param)
    {
      T
        px = x - param.x,
        py = y - param.y;

      return sqrt(px*px + py*py);
    }

    jpoint_t<float> Normalize()
    {
      return jpoint_t<float>{(float)x, (float)y}/sqrtf(x*x + y*y);
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
    
    operator jgui::jpoint_t<T>()
    {
      return {
        .x = x,
        .y = y
      };
    }

    jpoint3d_t operator+(jpoint3d_t param)
    {
      return {x + param.x, y + param.y, x + param.z};
    }
    
    jpoint3d_t operator-(jpoint3d_t param)
    {
      return {x - param.x, y - param.y, z - param.z};
    }
    
    jpoint3d_t operator*(float param)
    {
      return {x*param, y*param, z*param};
    }
    
    jpoint3d_t operator/(float param)
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

    float Distance(jpoint3d_t param)
    {
      T
        px = x - param.x,
        py = y - param.y,
        pz = z - param.z;

      return sqrt(px*px + py*py + pz*pz);
    }

    jpoint3d_t<float> Normalize()
    {
      return jpoint3d_t<float>{(float)x, (float)y, (float)z}/sqrtf(x*x + y*y + z*z);
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
    
    jsize_t<T> operator+(jsize_t<T> param)
    {
      return {width + param.width, width + param.width};
    }
    
    jsize_t<T> operator-(jsize_t<T> param)
    {
      return {height - param.height, height - param.height};
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

  };

/**
 * \brief
 *
 */
template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  struct jrect_t {
    jpoint_t<T> point;
    jsize_t<T> size;
    
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

    jrect_t<T> operator+(jrect_t<T> param)
    {
      return {point + param.point, size + param.size};
    }
    
    jrect_t<T> operator-(jrect_t<T> param)
    {
      return {point - param.point, size - param.size};
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

    bool Contains(jrect_t<T> param)
    {
      return (param.point.x >= point.x) and (param.point.y >= point.y) and ((param.point.x + param.size.width) <= size.width) and ((param.point.y + param.size.height) <= size.height);
    }

    bool Intersects(jpoint_t<T> param)
    {
      if (param.x > point.x and param.x < (point.x + size.width) and param.y > point.y and param.y < (point.y + size.height)) {
        return true;
      }

      return false;
    }

    bool Intersects(jrect_t<T> param)
    {
      return (((point.x > (param.point.x + param.size.width)) or ((point.x + size.width) < param.point.x) or (point.y > (param.point.y + param.size.height)) or ((point.y + size.height) < param.point.y)) == 0);
    }

    jrect_t<T> Intersection(jrect_t<T> param)
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

