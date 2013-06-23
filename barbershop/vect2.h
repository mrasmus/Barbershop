/** \file vect2.h
    \brief Provides a 2d vector template and several classes which use it. */
#ifndef _VECT2_H
#define _VECT2_H

#include <cmath>
#include <iostream>
//#include "list.h"
//#include "misc.h"

/// 2D Vector class.
template <typename T>
class vect2
{ public:
    vect2() { x=y=0; } ///< x and y components 0
    vect2(T x_, T y_) { x=x_, y=y_; }

    /// Dot product.
    T operator*(const vect2<T> &v) const { return x*v.x + y*v.y; }

    /// Vector addition.
    vect2<T> operator+(const vect2<T> &v) const
    { return vect2(x+v.x, y+v.y);
    }
    /// Vector subtraction.
    vect2<T> operator-(const vect2<T> &v) const
    { return vect2(x-v.x, y-v.y);
    }
    /// Vector multiply by scalar.
    vect2<T> operator*(const T &s) const { return vect2<T>(x*s, y*s); }
    /// Vector divide by scalar.
    vect2<T> operator/(const T &s) const { return vect2<T>(x/s, y/s); }
    /// Vector negate.
    vect2<T> operator-() const { return vect2<T>(-x, -y); }

    vect2<T> operator+=(const vect2<T> &v) { x+=v.x, y+=v.y; return *this; }
    vect2<T> operator-=(const vect2<T> &v) { x-=v.x, y-=v.y; return *this; }
    vect2<T> operator*=(const T        &s) { x*=s,   y*=s;   return *this; }
    vect2<T> operator/=(const T        &s) { x/=s,   y/=s;   return *this; }

    bool operator==(const vect2<T> &v) const { return x==v.x && y==v.y; }
    bool operator!=(const vect2<T> &v) const { return x!=v.x || y!=v.y; }
    
    /// Check if above or to the left.
    bool operator< (const vect2<T> &v) const
    { return y<v.y || y==v.y && x<v.x;
    }
    /// Check if below or to the right.
    bool operator> (const vect2<T> &v) const
    { return y>v.y || y==v.y && x>v.x;
    }

    /** \brief Assuming the vectors are points on a 2d plane, the square of
        the distance between them. */
    T distance_squared(const vect2<T> &v) const
    { return (v.x-x)*(v.x-x) + (v.y-y)*(v.y-y);
    }
    /** \brief Assuming the vectors are points on a 2d plane, the distance
        between them. */
    T distance(const vect2<T> &v) const 
    { return sqrt(distance_squared(v));
    }
    /// Length of vector
    double magnitude() const { return distance(vect2<T>(0, 0)); }

    /// Keep direction, but set length to 1.
    vect2<T> normalize() const { return *this/magnitude(); }
    /// One of the vectors perpendicular to this.
    vect2<T> perpendicular() const { return vect2<T>(-y, x); }

    vect2<T> rotate(float angle) const
    {
      return vect2<T>(
        x*cos(angle)-y*sin(angle), y*cos(angle)+x*sin(angle));
    }

    T x, y; ///< Actual data
};

//@{
/// Vect2 of type float
typedef vect2<float> vect2f;
typedef vect2f point;
typedef vect2f delta;
typedef vect2f normal;
typedef vect2f size;
//@}

//@{
/// Vect2 of type int
typedef vect2<int> vect2i;
typedef vect2i pointi;
typedef vect2i sizei;
//@}

/// Convert vect2 to vect2<int>
inline vect2i v2i(vect2f v) { return vect2i(int   (v.x), int   (v.y)); }
/// Convert vect2 to vect2<float>
inline vect2f v2f(vect2f v) { return vect2f(float(v.x), float(v.y)); }

#endif
