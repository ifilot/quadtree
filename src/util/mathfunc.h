/**************************************************************************
 *   mathfunc.h  --  This file is part of Afelirin.                       *
 *                                                                        *
 *   Copyright (C) 2016, Ivo Filot                                        *
 *                                                                        *
 *   Netris is free software: you can redistribute it and/or modify       *
 *   it under the terms of the GNU General Public License as published    *
 *   by the Free Software Foundation, either version 3 of the License,    *
 *   or (at your option) any later version.                               *
 *                                                                        *
 *   Netris is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty          *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
 *   See the GNU General Public License for more details.                 *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program.  If not, see http://www.gnu.org/licenses/.  *
 *                                                                        *
 **************************************************************************/

#ifndef _MATHFUNC_H
#define _MATHFUNC_H

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Sparse>

#include <boost/serialization/split_free.hpp>
#include <boost/serialization/vector.hpp>

/**
 * @fn square
 * @brief Convenience function that returns the square of a double
 *
 * @param c  Number to be squared
 *
 * @return square
 */
inline double square(double c) {
    return c * c;
}

/**
 * @fn cube
 * @brief Convenience function that returns the cube of a double
 *
 * @param c  Number to be cubed
 *
 * @return cube
 */
inline double cube(double c) {
    return c * c * c;
}

inline double deg2rad(double deg) {
    return deg / 360.0f * 2.0f * M_PI;
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

template <typename T>
T clamp(const T& n, const T& lower, const T& upper) {
  return std::max(lower, std::min(n, upper));
}

#endif //_MATHFUNC_H
