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
#include <cmath>
#include <optional>
#include <type_traits>

#include <stdio.h>

namespace jgui {

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
 
    template<typename U> operator jinsets_t<U>()
    {
      return {
        .left = (U)left,
        .top = (U)top,
        .right = (U)right,
        .bottom = (U)bottom
      };
    }

    jinsets_t<T> & operator=(const T &param)
    {
      left = param;
      top = param;
      right = param;
      bottom = param;

      return *this;
    }

    bool operator==(const jinsets_t<T> &param)
    {
      return (left == param.left and top == param.top and right == param.right and bottom == param.bottom);
    }

    bool operator!=(const jinsets_t<T> &param)
    {
      return (left != param.left or top != param.top or right != param.right or bottom != param.bottom);
    }

    friend std::ostream & operator<<(std::ostream& out, const jinsets_t<T> &param)
    {
      out << param.left << ", " << param.top << ", " << param.right << ", " << param.bottom;

      return out;
    }

    friend std::istream & operator>>(std::istream& is, jinsets_t<T> &param) 
    {
      is >> param.left >> param.top >> param.right >> param.bottom;

      return is;
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

    template<typename U> operator jpoint_t<U>()
    {
      return {
        .x = (U)x,
        .y = (U)y
      };
    }

    bool operator==(const jpoint_t<T> &param)
    {
      return (x == param.x and y == param.y);
    }

    bool operator!=(const jpoint_t<T> &param)
    {
      return (x != param.x or y != param.y);
    }

    jpoint_t<T> & operator-()
    {
      x = -x;
      y = -y;

      return *this;
    }
    
    template<typename U> jpoint_t<T> & operator=(const U &param)
    {
      x = (T)param;
      y = (T)param;

      return *this;
    }

    template<typename U> jpoint_t<typename std::common_type<T, U>::type> operator+(const U &param)
    {
      return {(x + param), (y + param)};
    }
    
    template<typename U> jpoint_t<typename std::common_type<T, U>::type> operator-(const U &param)
    {
      return {(x - param), (y - param)};
    }
    
    template<typename U> jpoint_t<typename std::common_type<T, U>::type> operator*(const U &param)
    {
      return {(x*param), (y*param)};
    }
    
    template<typename U> jpoint_t<typename std::common_type<T, U>::type> operator/(const U &param)
    {
      return {(x/param), (y/param)};
    }

    template<typename U> jpoint_t<T> & operator+=(const U &param)
    {
      return (*this = *this + param);
    }
    
    template<typename U> jpoint_t<T> & operator-=(const U param)
    {
      return (*this = *this - param);
    }
    
    template<typename U> jpoint_t<T> & operator*=(const U &param)
    {
      return (*this = *this*param);
    }
    
    template<typename U> jpoint_t<T> & operator/=(const U &param)
    {
      return (*this = *this/param);
    }
    
    template<typename U> jpoint_t<T> & operator=(const jpoint_t<U> &param)
    {
      x = (T)param.x;
      y = (T)param.y;

      return *this;
    }

    template<typename U> jpoint_t<typename std::common_type<T, U>::type> operator+(const jpoint_t<U> &param)
    {
      return {(x + param.x), (y + param.y)};
    }
    
    template<typename U> jpoint_t<typename std::common_type<T, U>::type> operator-(const jpoint_t<U> &param)
    {
      return {(x - param.x), (y - param.y)};
    }
    
    template<typename U> jpoint_t<typename std::common_type<T, U>::type> operator*(const jpoint_t<U> &param)
    {
      return {(x*param.x), (y*param.y)};
    }
    
    template<typename U> jpoint_t<typename std::common_type<T, U>::type> operator/(const jpoint_t<U> &param)
    {
      return {(x/param.x), (y/param.y)};
    }
    
    template<typename U> jpoint_t<T> & operator+=(const jpoint_t<U> &param)
    {
      return (*this = *this + param);
    }
    
    template<typename U> jpoint_t<T> & operator-=(const jpoint_t<U> &param)
    {
      return (*this = *this - param);
    }
    
    template<typename U> jpoint_t<T> & operator*=(const jpoint_t<U> &param)
    {
      return (*this = *this*param);
    }
    
    template<typename U> jpoint_t<T> & operator/=(const jpoint_t<U> &param)
    {
      return (*this = *this/param);
    }
    
    template<typename U> float Distance(const jpoint_t<U> &param)
    {
      typename std::common_type<T, U>::type
        px = (x - param.x),
        py = (y - param.y);

      return std::sqrt(px*px + py*py);
    }

    template<typename U> typename std::common_type<T, U>::type Scalar(const jpoint_t<U> &param)
    {
      return (x*param.x) + (y*param.y);
    }

    float Norm()
    {
      return x*x + y*y;
    }

    float EuclidianNorm()
    {
      return std::sqrt(Norm());
    }

    template<typename U, typename = typename std::enable_if<std::is_floating_point<U>::value, T>::type> jpoint_t<U> Normalize()
    {
      return jpoint_t<U>(*this)/EuclidianNorm();
    }
 
    jpoint_t<T> Rotate(float angle)
    {
      float
        c = cosf(angle),
        s = sinf(angle);

      return {x*c - y*s, x*s + y*c};
    }

    float Angle()
    {
      float
        angle = fabs(atanf(y/(float)x));

      if (x > 0) {
        if (y > 0) {
        } else {
          angle = 2*M_PI - angle;
        }
      } else {
        if (y > 0) {
          angle = M_PI - angle;
        } else {
          angle = M_PI + angle;
        }
      }

      return angle;
    }

    T Min()
    {
      return std::min<T>(x, y);
    }

    T Max()
    {
      return std::max<T>(x, y);
    }

    template<typename U> friend jpoint_t<typename std::common_type<T, U>::type> operator+(const U &param, const jpoint_t<T> &thiz)
    {
      return {(param + thiz.x), (param + thiz.y)};
    }
    
    template<typename U> friend jpoint_t<typename std::common_type<T, U>::type> operator-(const U &param, const jpoint_t<T> &thiz)
    {
      return {(param - thiz.x), (param - thiz.y)};
    }
    
    template<typename U> friend jpoint_t<typename std::common_type<T, U>::type> operator*(const U &param, const jpoint_t<T> &thiz)
    {
      return {(param*thiz.x), (param*thiz.y)};
    }
    
    template<typename U> friend jpoint_t<typename std::common_type<T, U>::type> operator/(const U &param, const jpoint_t<T> &thiz)
    {
      return {(param/thiz.x), (param/thiz.y)};
    }

    friend std::ostream & operator<<(std::ostream& out, const jpoint_t<T> &param)
    {
      out << param.x << ", " << param.y;

      return out;
    }

    friend std::istream & operator>>(std::istream& is, jpoint_t<T> &param) 
    {
      is >> param.x >> param.y;

      return is;
    }

  };

/**
 * brief
 *
 */
template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  struct jline_t {
    jpoint_t<T> p0;
    jpoint_t<T> p1;
    
    template<typename U> operator jline_t<U>()
    {
      return {
        .p0 = p0,
        .p1 = p1,
      };
    }

    bool operator==(const jline_t<T> &param)
    {
      return (p0 == param.p0 and p1 == param.p1);
    }

    bool operator!=(const jline_t<T> &param)
    {
      return (p0 != param.p0 or p1 != param.p1);
    }

    jline_t<T> & operator-()
    {
      p0 = -p0;
      p1 = -p1;

      return *this;
    }

    template<typename U> jline_t<T> & operator=(const U &param)
    {
      p0 = param;
      p1 = param;

      return *this;
    }

    template<typename U> jline_t<typename std::common_type<T, U>::type> operator+(const U &param)
    {
      return {p0 + param, p1 + param};
    }
    
    template<typename U> jline_t<typename std::common_type<T, U>::type> operator-(const U &param)
    {
      return {p0 - param, p1 - param};
    }
    
    template<typename U> jline_t<typename std::common_type<T, U>::type> operator*(const U &param)
    {
      return {p0*param, p1*param};
    }
    
    template<typename U> jline_t<typename std::common_type<T, U>::type> operator/(const U &param)
    {
      return {p0/param, p1/param};
    }

    template<typename U> jline_t<T> & operator+=(const U &param)
    {
      return (*this = *this + param);
    }
    
    template<typename U> jline_t<T> & operator-=(const U &param)
    {
      return (*this = *this - param);
    }
    
    template<typename U> jline_t<T> & operator*=(const U &param)
    {
      return (*this = *this*param);
    }
    
    template<typename U> jline_t<T> & operator/=(const U &param)
    {
      return (*this = *this/param);
    }
    
    template<typename U> jline_t<T> & operator=(const jpoint_t<U> &param)
    {
      p0 = param;
      p1 = param;

      return *this;
    }

    template<typename U> jline_t<typename std::common_type<T, U>::type> operator+(const jpoint_t<U> &param)
    {
      return {p0 + param, p1 + param};
    }
    
    template<typename U> jline_t<typename std::common_type<T, U>::type> operator-(const jpoint_t<U> &param)
    {
      return {p0 - param, p1 - param};
    }
    
    template<typename U> jline_t<typename std::common_type<T, U>::type> & operator+=(const jpoint_t<U> &param)
    {
      return (*this = *this + param);
    }
    
    template<typename U> jline_t<typename std::common_type<T, U>::type> & operator-=(const jpoint_t<U> &param)
    {
      return (*this = *this - param);
    }
    
    template<typename U> jline_t<T> & operator=(const jline_t<U> &param)
    {
      p0 = param.p0;
      p1 = param.p1;

      return *this;
    }

    template<typename U> jline_t<typename std::common_type<T, U>::type> operator+(const jline_t<U> &param)
    {
      return {p0 + param.p0, p1 + param.p1};
    }
    
    template<typename U> jline_t<typename std::common_type<T, U>::type> operator-(const jline_t<U> &param)
    {
      return {p0 - param.p0, p1 - param.p1};
    }

    template<typename U> jline_t<T> & operator+=(const jline_t<U> &param)
    {
      return (*this = *this + param);
    }
    
    template<typename U> jline_t<T> & operator-=(const jline_t<U> &param)
    {
      return (*this = *this - param);
    }
    
    T Size()
    {
        return p0.Distance(p1);
    }

    template<typename U> bool Exists(const jgui::jpoint_t<U> &point)
    {
      return Sign(point) == 0;
    }

    jpoint_t<T> Point(float t)
    {
      return jpoint_t<T>{(T)(p0.x + t*(p1.x - p0.x)), (T)(p0.y + t*(p1.y - p0.y))};
    }

    template<typename U> int Sign(const jpoint_t<U> &point)
    {
      typename std::common_type<T, U>::type
        sign = (p1.x - p0.x)*(point.y - p0.y) - (p1.y - p0.y)*(point.x - p0.x);

      if (sign < 0) {
        return (T)-1;
      }

      if (sign > 0) {
        return (T)+1;
      }

      return 0;
    }

    /**
     * \brief Returns the perpendicular intersection in line u=[0..1]. To known 
     * the point in line calculate the intersection point as follows:
     *
     * x = u*(line.p1.x - line.p0.x)
     * y = u*(line.p1.y - line.p0.y)
     *
     */
    template<typename U> std::optional<float> PerpendicularIntersection(const jpoint_t<U> &point)
    {
      float
        px = p1.x - p0.x,
        py = p1.y - p0.y;
      float
        den = (px*px + py*py);

      if (den == 0.0f) {
				return std::nullopt;
      }

      return ((point.x - p0.x)*px + (point.y - p0.y)*py)/den;
    }

    /**
     * \brief Returns the intersection point u and v in range [0..1]. To known 
     * the point in line calculate the intersection point as follows:
     *
     * x0 = t*(line0.p1.x - line0.p0.x)
     * y0 = t*(line0.p1.y - line0.p0.y)
     *
     * x1 = u*(line1.p1.x - line1.p0.x)
     * y1 = u*(line1.p1.y - line1.p0.y)
     *
     */
    template<typename U> std::optional<std::pair<float, float>> Intersection(const jline_t<U> &line)
    {
      const float x1 = line.p0.x;
      const float y1 = line.p0.y;
      const float x2 = line.p1.x;
      const float y2 = line.p1.y;

      const float x3 = p0.x;
      const float y3 = p0.y;
      const float x4 = p1.x;
      const float y4 = p1.y;

      const float den = (x1 - x2)*(y3 - y4) - (y1 - y2)*(x3 - x4);

      if (den == 0) {
				return std::nullopt;
      }

      const float t1 = -((x1 - x2)*(y1 - y3) - (y1 - y2)*(x1 - x3))/den;
      const float t0 = ((x1 - x3)*(y3 - y4) - (y1 - y3)*(x3 - x4))/den;

      return std::make_pair(t1, t0);
    }

    template<typename U> float Angle(const jline_t<U> &line)
    {
      float
        m0 = (p1.y - p0.y)/(p1.x - p0.x),
        m1 = (line.p1.y - line.p0.y)/(line.p1.x - line.p0.x);
      float
        den = 1 + m0*m1;

      if (den == 0.0f) {
        return M_PI/2.0f;
      }

      return atanf((m0 - m1)/den);
    }

    template<typename U> friend jline_t<typename std::common_type<T, U>::type> operator+(const U &param, const jline_t<T> &thiz)
    {
      return {param + thiz.p0, param + thiz.p1};
    }
    
    template<typename U> friend jline_t<typename std::common_type<T, U>::type> operator-(const U &param, const jline_t<T> &thiz)
    {
      return {param - thiz.p0, param - thiz.p1};
    }
    
    template<typename U> friend jline_t<typename std::common_type<T, U>::type> operator*(const U &param, const jline_t<T> &thiz)
    {
      return {param*thiz.p0, param*thiz.p1};
    }
    
    template<typename U> friend jline_t<typename std::common_type<T, U>::type> operator/(const U &param, const jline_t<T> &thiz)
    {
      return {param/thiz.p0, param/thiz.p1};
    }
    
    friend std::ostream & operator<<(std::ostream& out, const jline_t<T> &param)
    {
      out << std::string("(") << param.p0 << "), (" << param.p1 << ")";

      return out;
    }

    friend std::istream & operator>>(std::istream& is, jline_t<T> &param) 
    {
      is >> param.p0.x >> param.p0.y >> param.p1.x >> param.p1.y;

      return is;
    }

  };

/**
 * \brief
 *
 */
template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
  struct jcircle_t {
    jpoint_t<T> center;
    T radius;
    
    template<typename U> operator jcircle_t<U>()
    {
      return {
        .center = center,
        .radius = radius
      };
    }

    jcircle_t<T> & operator-()
    {
      center = -center;

      return *this;
    }

    bool operator==(const jcircle_t<T> &param)
    {
      return (center == param.center and radius == param.radius);
    }

    bool operator!=(const jline_t<T> &param)
    {
      return (center != param.center or radius != param.radius);
    }

    template<typename U> jcircle_t<T> & operator=(const U &param)
    {
      radius = (T)param;

      return *this;
    }

    template<typename U> jline_t<typename std::common_type<T, U>::type> operator+(const U &param)
    {
      return {center, (radius + param)};
    }
    
    template<typename U> jline_t<typename std::common_type<T, U>::type> operator-(const U &param)
    {
      return {center, (radius - param)};
    }

    template<typename U> jline_t<typename std::common_type<T, U>::type> operator*(const U &param)
    {
      return {center, (radius*param)};
    }
    
    template<typename U> jline_t<typename std::common_type<T, U>::type> operator/(const U &param)
    {
      return {center, (radius/param)};
    }

    template<typename U> jline_t<T> & operator+=(const U &param)
    {
      return (*this = *this + param);
    }

    template<typename U> jline_t<T> & operator-=(const U &param)
    {
      return (*this = *this - param);
    }

    template<typename U> jline_t<T> & operator*=(const U &param)
    {
      return (*this = *this*param);
    }

    template<typename U> jline_t<T> & operator/=(const U &param)
    {
      return (*this = *this/param);
    }

    template<typename U> jcircle_t<T> & operator=(const jpoint_t<U> &param)
    {
      center = param;

      return *this;
    }

    template<typename U> jcircle_t<typename std::common_type<T, U>::type> operator+(const jpoint_t<U> &param)
    {
      return {center + param, radius};
    }
    
    template<typename U> jcircle_t<typename std::common_type<T, U>::type> operator-(const jpoint_t<U> &param)
    {
      return {center - param, radius};
    }
    
    template<typename U> jcircle_t<typename std::common_type<T, U>::type> & operator+=(const jpoint_t<U> &param)
    {
      return (*this = *this + param);
    }
    
    template<typename U> jcircle_t<typename std::common_type<T, U>::type> & operator-=(const jpoint_t<U> &param)
    {
      return (*this = *this - param);
    }
    
    template<typename U> jcircle_t<T> & operator=(const jcircle_t<U> &param)
    {
      center = param.center;
			radius = param.radius;

      return *this;
    }

    T Size()
    {
        return 2*M_PI*radius;
    }

    template<typename U> bool Inside(const jgui::jpoint_t<U> &point)
    {
      return point.Distance(center) <= radius;
    }

    float Angle(const jpoint_t<T> &param)
    {
      return (param - center).Angle();
    }

    /**
     * \brief Returns the intersection circle and line.
     *
     */
    template<typename U> std::optional<std::pair<jgui::jpoint_t<float>, jgui::jpoint_t<float>>> Intersection(const jline_t<U> &line)
    {
      float 
        dx = line.p1.x - line.p0.x, 
        dy = line.p1.y - line.p0.y, 
        A = dx*dx + dy*dy,
        B = 2*(dx*(line.p0.x - center.x) + dy*(line.p0.y - center.y)),
        C = (line.p0.x - center.x)*(line.p0.x - center.x) + (line.p0.y - center.y)*(line.p0.y - center.y) - radius*radius,
        det = B*B - 4*A*C;

      if ((A <= 0.0000001) || (det < 0)) { // no real solutions
				return std::nullopt;
      }

      float
        t0 = (float)((-B + sqrtf(det))/(2*A)),
        t1 = (float)((-B - sqrtf(det))/(2*A));

      return std::make_pair(jpoint_t<float>{line.p0.x + t0*dx, line.p0.y + t0*dy}, jpoint_t<float>{line.p0.x + t1*dx, line.p0.y + t1*dy});
    }

    template<typename U> friend jcircle_t<typename std::common_type<T, U>::type> operator+(const T &param, const jcircle_t<T> &thiz)
    {
      return {thiz.center, (param + thiz.radius)};
    }
    
    template<typename U> friend jcircle_t<typename std::common_type<T, U>::type> operator-(const T &param, const jcircle_t<T> &thiz)
    {
      return {thiz.center, (param - thiz.radius)};
    }

    template<typename U> friend jcircle_t<typename std::common_type<T, U>::type> operator*(const T &param, const jcircle_t<T> &thiz)
    {
      return {thiz.center, (param*thiz.radius)};
    }
    
    template<typename U> friend jcircle_t<typename std::common_type<T, U>::type> operator/(const T &param, const jcircle_t<T> &thiz)
    {
      return {thiz.center, (param/thiz.radius)};
    }

    template<typename U> friend jcircle_t<typename std::common_type<T, U>::type> operator+(const jpoint_t<T> &param, const jcircle_t<T> &thiz)
    {
      return {thiz.center + param, thiz.radius};
    }
    
    template<typename U> friend jcircle_t<typename std::common_type<T, U>::type> operator-(const jpoint_t<T> &param, const jcircle_t<T> &thiz)
    {
      return {thiz.center - param, thiz.radius};
    }
    
    friend std::ostream & operator<<(std::ostream& out, const jcircle_t<T> &param)
    {
      out << std::string("(") << param.center << "), " << param.radius;

      return out;
    }

    friend std::istream & operator>>(std::istream& is, jcircle_t<T> &param) 
    {
      is >> param.center.x >> param.center.y >> param.radius;

      return is;
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
    
    template<typename U> operator jsize_t<U>()
    {
      return {
        .width = (U)width,
        .height = (U)height
      };
    }

    jsize_t<T> & operator-()
    {
      width = -width;
      height = -height;

      return *this;
    }
    
    bool operator==(const jsize_t<T> &param)
    {
      return (width == param.width and height == param.height);
    }

    bool operator!=(const jsize_t<T> &param)
    {
      return (width != param.width or height != param.height);
    }

    template<typename U> jsize_t<T> & operator=(const U &param)
    {
      width = (T)param;
      height = (T)param;

      return *this;
    }

    template<typename U> jsize_t<typename std::common_type<T, U>::type> operator+(const U &param)
    {
      return {(width + param), (height + param)};
    }
    
    template<typename U> jsize_t<typename std::common_type<T, U>::type> operator-(const U &param)
    {
      return {(width - param), (height - param)};
    }
    
    template<typename U> jsize_t<typename std::common_type<T, U>::type> operator*(const U &param)
    {
      return {(width*param), (height*param)};
    }
    
    template<typename U> jsize_t<typename std::common_type<T, U>::type> operator/(const U &param)
    {
      return {(width/param), (height/param)};
    }

    template<typename U> jsize_t<T> & operator+=(const U &param)
    {
      return (*this = *this + param);
    }

    template<typename U> jsize_t<T> & operator-=(const U &param)
    {
      return (*this = *this - param);
    }

    template<typename U> jsize_t<T> & operator*=(const U &param)
    {
      return (*this = *this*param);
    }

    template<typename U> jsize_t<T> & operator/=(const U &param)
    {
      return (*this = *this/param);
    }

    template<typename U> jsize_t<T> & operator=(const jsize_t<U> &param)
    {
      width = (T)param.width;
      height = (T)param.height;

      return *this;
    }

    template<typename U> jsize_t<typename std::common_type<T, U>::type> operator+(const jsize_t<U> &param)
    {
      return {(width + param.width), (height + param.height)};
    }
    
    template<typename U> jsize_t<typename std::common_type<T, U>::type> operator-(const jsize_t<U> &param)
    {
      return {(width - param.width), (height - param.height)};
    }
    
    template<typename U> jsize_t<T> & operator+=(const jsize_t<U> &param)
    {
      return (*this = *this + param);
    }
    
    template<typename U> jsize_t<T> & operator-=(const jsize_t<U> &param)
    {
      return (*this = *this - param);
    }
    
    T Area()
    {
      return width*height;
    }

    T Min()
    {
      return std::min<T>(width, height);
    }

    T Max()
    {
      return std::max<T>(width, height);
    }

    template<typename U> friend jsize_t<typename std::common_type<T, U>::type> operator+(const U &param, const jsize_t<T> &thiz)
    {
      return {(param + thiz.width), (param + thiz.height)};
    }
    
    template<typename U> friend jsize_t<typename std::common_type<T, U>::type> operator-(const U &param, const jsize_t<T> &thiz)
    {
      return {(param - thiz.width), (param - thiz.height)};
    }
    
    template<typename U> friend jsize_t<typename std::common_type<T, U>::type> operator*(const U &param, const jsize_t<T> &thiz)
    {
      return {(param*thiz.width), (param*thiz.height)};
    }
    
    template<typename U> friend jsize_t<typename std::common_type<T, U>::type> operator/(const U &param, const jsize_t<T> &thiz)
    {
      return {(param/thiz.width), (param/thiz.height)};
    }
    
    friend std::ostream & operator<<(std::ostream& out, const jsize_t<T> &param)
    {
      out << param.width << ", " << param.height;

      return out;
    }

    friend std::istream & operator>>(std::istream& is, jsize_t<T> &param) 
    {
      is >> param.width >> param.height;

      return is;
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
    
    template<typename U> operator jrect_t<U>()
    {
      return {
        .point = point,
        .size = size
      };
    }

    bool operator==(const jrect_t<T> &param)
    {
      return (point == param.point and size == param.size);
    }

    bool operator!=(const jrect_t<T> &param)
    {
      return (point != param.point or size != param.size);
    }

    jrect_t<T> & operator-()
    {
      point = -point;
      size = -size;

      return *this;
    }
    
    jrect_t<T> & operator=(T param)
    {
      size = param;

      return *this;
    }

    template<typename U> jrect_t<typename std::common_type<T, U>::type> operator+(const U &param)
    {
      return {point + param, size + param};
    }
    
    template<typename U> jrect_t<typename std::common_type<T, U>::type> operator-(const U &param)
    {
      return {point - param, size - param};
    }
    
    template<typename U> jrect_t<typename std::common_type<T, U>::type> operator*(const U &param)
    {
      return {point*param, size*param};
    }
    
    template<typename U> jrect_t<typename std::common_type<T, U>::type> operator/(const U &param)
    {
      return {point/param, size/param};
    }

    template<typename U> jrect_t<T> & operator+=(const U &param)
    {
      return (*this = *this + param);
    }
    
    template<typename U> jrect_t<T> & operator-=(const U &param)
    {
      return (*this = *this - param);
    }
    
    template<typename U> jrect_t<T> & operator*=(const U &param)
    {
      return (*this = *this*param);
    }
    
    template<typename U> jrect_t<T> & operator/=(const U &param)
    {
      return (*this = *this/param);
    }

    template<typename U> jrect_t<T> & operator=(const jpoint_t<U> &param)
    {
      point = param;

      return *this;
    }

    template<typename U> jrect_t<typename std::common_type<T, U>::type> operator+(const jpoint_t<U> &param)
    {
      return {point + param, size};
    }
    
    template<typename U> jrect_t<typename std::common_type<T, U>::type> operator-(const jpoint_t<U> &param)
    {
      return {point - param, size};
    }
    
    template<typename U> jrect_t<T> & operator+=(const jpoint_t<U> &param)
    {
      return (*this = *this + param);
    }
    
    template<typename U> jrect_t<T> & operator-=(const jpoint_t<U> &param)
    {
      return (*this = *this - param);
    }
    
    template<typename U> jrect_t<T> & operator=(const jsize_t<U> &param)
    {
      size = param;

      return *this;
    }

    template<typename U> jrect_t<typename std::common_type<T, U>::type> operator+(const jsize_t<U> &param)
    {
      return {point, size + param};
    }
    
    template<typename U> jrect_t<typename std::common_type<T, U>::type> operator-(const jsize_t<U> &param)
    {
      return {point, size + param};
    }
    
    template<typename U> jrect_t<T> & operator+=(const jsize_t<U> &param)
    {
      return (*this = *this + param);
    }
    
    template<typename U> jrect_t<T> & operator-=(const jsize_t<U> &param)
    {
      return (*this = *this - param);
    }

    template<typename U> jrect_t<T> & operator=(const jrect_t<U> &param)
    {
			point = param.point;
      size = param.size;

      return *this;
    }

    template<typename U> jrect_t<typename std::common_type<T, U>::type> operator+(const jrect_t<U> &param)
    {
      return {point + param.point, size + param.size};
    }
    
    template<typename U> jrect_t<typename std::common_type<T, U>::type> operator-(const jrect_t<U> &param)
    {
      return {point - param.point, size - param.size};
    }
    
    template<typename U> jrect_t<T> & operator+=(const jrect_t<U> &param)
    {
      return (*this = *this + param);
    }
    
    template<typename U> jrect_t<T> & operator-=(const jrect_t<U> &param)
    {
      return (*this = *this + param);
    }
    
    bool operator>(const jrect_t<T> &param)
    {
      return size > param.size;
    }

    bool operator>=(const jrect_t<T> &param)
    {
      return size >= param.size;
    }

    bool operator<(const jrect_t<T> &param)
    {
      return size < param.size;
    }

    bool operator<=(const jrect_t<T> &param)
    {
      return size <= param.size;
    }

    template<typename U> bool Intersects(const jpoint_t<U> &param)
    {
      if (param.x > point.x and param.x < (point.x + size.width) and param.y > point.y and param.y < (point.y + size.height)) {
        return true;
      }

      return false;
    }

    template<typename U> bool Intersects(const jline_t<U> &param)
    {
      std::pair<float, float>
        i0 = param.Intersection({{point.x, point.y}, {point.x + size.width, point.y}}),
        i1 = param.Intersection({{point.x + size.width, point.y}, {point.x + size.width, point.y + size.height}}),
        i2 = param.Intersection({{point.x + size.width, point.y + size.height}, {point.x, point.y + size.height}}),
        i3 = param.Intersection({{point.x, point.y + size.height}, {point.x, point.y}});

      if (i0.first >= 0.0f and i0.first <= 1.0f or
          i1.first >= 0.0f and i1.first <= 1.0f or
          i2.first >= 0.0f and i2.first <= 1.0f or
          i3.first >= 0.0f and i3.first <= 1.0f) {
        return true;
      }

      return false;
    }

    template<typename U> bool Intersects(const jcircle_t<U> &param)
    {
      std::optional<std::pair<jgui::jpoint_t<float>, jgui::jpoint_t<float>>>
        i0 = param.Intersection({{point.x, point.y}, {point.x + size.width, point.y}}),
        i1 = param.Intersection({{point.x + size.width, point.y}, {point.x + size.width, point.y + size.height}}),
        i2 = param.Intersection({{point.x + size.width, point.y + size.height}, {point.x, point.y + size.height}}),
        i3 = param.Intersection({{point.x, point.y + size.height}, {point.x, point.y}});

      if (i0 != std::nullopt and i1 != std::nullopt and i2 != std::nullopt and i3 != std::nullopt) {
        return true;
      }

      return false;
    }

    template<typename U> bool Contains(const jrect_t<U> &param)
    {
      return (param.point.x >= point.x) and (param.point.y >= point.y) and ((param.point.x + param.size.width) <= size.width) and ((param.point.y + param.size.height) <= size.height);
    }

    template<typename U> bool Intersects(const jrect_t<U> &param)
    {
      return (((point.x > (param.point.x + param.size.width)) or ((point.x + size.width) < param.point.x) or (point.y > (param.point.y + param.size.height)) or ((point.y + size.height) < param.point.y)) == 0);
    }

    template<typename U> jrect_t<T> Intersection(const jrect_t<U> &param)
    {
      int 
        left = std::max(point.x, param.point.x),
        top = std::max(point.y, param.point.y),
        right = std::min(point.x + size.width, param.point.x + param.size.width),
        bottom = std::min(point.y + size.height, param.point.y + param.size.height);

      if (right > left and bottom > top) {
        return {{left, top}, {right - left, bottom - top}};
      }

      return {0, 0, 0, 0};
    }

    template<typename U> friend jrect_t<typename std::common_type<T, U>::type> operator+(const U &param, const jrect_t<T> &thiz)
    {
      return {param + thiz.point, param + thiz.size};
    }
    
    template<typename U> friend jrect_t<typename std::common_type<T, U>::type> operator-(const U &param, const jrect_t<T> &thiz)
    {
      return {param - thiz.point, param - thiz.size};
    }
    
    template<typename U> friend jrect_t<typename std::common_type<T, U>::type> operator*(const U &param, const jrect_t<T> &thiz)
    {
      return {param*thiz.point, param*thiz.size};
    }
    
    template<typename U> friend jrect_t<typename std::common_type<T, U>::type> operator/(const U &param, const jrect_t<T> &thiz)
    {
      return {param/thiz.point, param/thiz.size};
    }
    
    template<typename U> friend jline_t<typename std::common_type<T, U>::type> operator+(const jpoint_t<U> &param, const jrect_t<T> &thiz)
    {
      return {param + thiz.point, thiz.size};
    }
    
    template<typename U> friend jline_t<typename std::common_type<T, U>::type> operator-(const jpoint_t<U> &param, const jrect_t<T> &thiz)
    {
      return {param - thiz.point, thiz.size};
    }
    
    template<typename U> friend jline_t<typename std::common_type<T, U>::type> operator+(const jsize_t<U> &param, const jrect_t<T> &thiz)
    {
      return {thiz.point, param + thiz.size};
    }
    
    template<typename U> friend jline_t<typename std::common_type<T, U>::type> operator-(const jsize_t<U> &param, const jrect_t<T> &thiz)
    {
      return {thiz.point, param - thiz.size};
    }
    
    friend std::ostream & operator<<(std::ostream& out, const jrect_t<T> &param)
    {
      out << param.point << ", " << param.size;

      return out;
    }

    friend std::istream & operator>>(std::istream& is, jrect_t<T> &param) 
    {
      is >> param.point.x >> param.point.y >> param.size.width >> param.height;

      return is;
    }

};

}

#endif

