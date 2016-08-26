/*
 * This file is part of Hootenanny.
 *
 * Hootenanny is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * --------------------------------------------------------------------
 *
 * The following copyright notices are generated automatically. If you
 * have a new notice to add, please use the format:
 * " * @copyright Copyright ..."
 * This will properly maintain the copyright information. DigitalGlobe
 * copyrights will be updated automatically.
 *
 * @copyright Copyright (C) 2016 DigitalGlobe (http://www.digitalglobe.com/)
 */
#ifndef FRECHETDISTANCE_H
#define FRECHETDISTANCE_H

//  Boost
#include <boost/multi_array.hpp>

// Hoot
#include <hoot/core/OsmMap.h>
#include <hoot/core/Units.h>
#include <hoot/core/elements/Way.h>

// TGS
#include <tgs/SharedPtr.h>

namespace hoot
{

typedef boost::multi_array<Meters, 2> frechet_matrix;
typedef frechet_matrix::index frechet_index;
typedef std::pair<int, int> vertex_match;
typedef std::vector<vertex_match> frechet_subline;

class FrechetDistance
{
public:
  static Meters calculate(OsmMapPtr map, shared_ptr<Way> w1, shared_ptr<Way> w2);
  static Meters calculate(frechet_matrix& matrix);
  static Meters calculate(shared_ptr<const LineString> ls1, shared_ptr<const LineString> ls2);

  static Meters calculatePoly(OsmMapPtr map, shared_ptr<Way> w1, shared_ptr<Way> w2);
  static Meters calculatePoly(frechet_matrix& matrix);
  static Meters calculatePoly(shared_ptr<const LineString> ls1, shared_ptr<const LineString> ls2);

  static frechet_matrix calculateMatrix(shared_ptr<const LineString> ls1, shared_ptr<const LineString> ls2);

  static frechet_subline calculateSubline(frechet_matrix& matrix, Meters maxDistance = 15);
  static frechet_subline calculateSubline(OsmMapPtr map, shared_ptr<Way> w1, shared_ptr<Way> w2, Meters maxDistance = 15);

protected:
  FrechetDistance() {}

  static Meters _calculate(frechet_matrix &matrix, const LineString* ls1, const LineString* ls2);

};

}
#endif // FRECHETDISTANCE_H
