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
#include <hoot/core/algorithms/linearreference/WayLocation.h>
#include <hoot/core/elements/Way.h>

// TGS
#include <tgs/SharedPtr.h>

namespace hoot
{

typedef boost::multi_array<Meters, 2> frechet_matrix;
typedef frechet_matrix::index frechet_index;
typedef std::pair<int, int> vertex_match;
typedef std::vector<vertex_match> frechet_subline;

typedef shared_ptr<LineString> LineStringPtr;

/** Class for calculating Frechet Distance between two ways and calculating maximal subline matches.
 *  Algorithm developed from "A new merging process for data integration base on the descrete Frechet distance"
 *  (http://thomas.devogele.free.fr/articles/avant_2003_selection/DevogeleSDH2002.pdf)
 */
class FrechetDistance
{
public:
  /**
   * @brief FrechetDistance class constructor
   * @param map - pointer to the OSM map containing the two ways to be compared
   * @param way1 - pointer to the first way to be compared
   * @param way2 - pointer to the second way to be compared
   */
  FrechetDistance(const ConstOsmMapPtr& map, const ConstWayPtr& way1, const ConstWayPtr& way2, Radians maxAngle = M_PI);

  /**
   * @brief distance - calculate the Frechet distance between the two ways, _w1 and _w2
   * @return Frechet distance in meters
   */
  Meters distance();

  /**
   * @brief maxSubline - find the maximum length, in nodes, subline for the two ways
   * @param maxDistance - max distance between the two ways to consider as a match
   * @return set of matching node indices that represent the subline
   */
  frechet_subline maxSubline(Meters maxDistance = 15.0);

protected:
  /**
   * @brief calculateMatrix
   * @return distinct Frechet matrix of distances calculated
   */
  frechet_matrix calculateMatrix();
  /**
   * @brief advanceAndCheck
   * @param rows - constant number of rows in the Frechet matrix
   * @param cols - constant number of columns in the Frechet matrix
   * @param r - current row number, could be modified in the advance operation
   * @param c - current column number, could be modified in the advance operation
   * @param max_frechet - maximum distance
   * @param advance_row - boolean flag to advance the row
   * @param advance_col - boolean flag to advance the column
   */
  void advanceAndCheck(const int rows, const int cols, int& r, int& c, Meters& max_frechet, bool advance_row, bool advance_col);
  /**
   * @brief advanceAndCheckRow - helper function that calls advanceAndCheck that advances
   *  a row
   * See advanceAndCheck for parameter information
   */
  void advanceAndCheckRow(const int rows, const int cols, int& r, int& c, Meters& max_frechet);
  /**
   * @brief advanceAndCheckColumn - helper function that calls advanceAndCheck that advances
   *  a column
   * See advanceAndCheck for parameter information
   */
  void advanceAndCheckColumn(const int rows, const int cols, int& r, int& c, Meters& max_frechet);
  /**
   * @brief advanceAndCheckBoth - helper function that calls advanceAndCheck that advances
   *  both a row and a column
   * See advanceAndCheck for parameter information
   */
  void advanceAndCheckBoth(const int rows, const int cols, int& r, int& c, Meters& max_frechet);

  Radians getHeadingWay1(int index);
  Radians getHeadingWay2(int index);
  Radians getHeading(WayPtr way, int index);


protected:
  //  Copies of the map and both ways
  OsmMapPtr _map;
  WayPtr _w1;
  WayPtr _w2;
  //  Ways converted to LineString objects
  LineStringPtr _ls1;
  LineStringPtr _ls2;
  //  Distinct Frechet matrix
  frechet_matrix _matrix;
  //  Locations along _w2 for each node in _w1
  vector<WayLocation> _locations_w1;
  //  Locations along _w1 for each node in _w2
  vector<WayLocation> _locations_w2;
  //  Distances between each node in _w1 and the corresponding WayLocation on _w2
  vector<Meters> _distances_w1;
  //  Distances between each node in _w2 and the corresponding WayLocation on _w1
  vector<Meters> _distances_w2;
  //  Max difference in angles to calculate Frechet distance for
  Radians _angleDiff;
  //  Max angle to compare _angleDiff against
  Radians _maxAngle;
};

class FrechetDistanceOld
{
public:
  /**
   * @brief calculate function calculates the Frechet distance between two ways
   * @param map - OsmMap pointer used to check direction of ways and in way conversion
   * @param way1 - Pointer to first way
   * @param way2 - Pointer to second way (order doesn't matter)
   * @return the Frechet distance in meters
   */
  static Meters calculate(const ConstOsmMapPtr& map, const ConstWayPtr& way1, const ConstWayPtr& way2);

  /**
   * @brief calculate function calculates the Frechet distance between two ways already loaded into a Frechet matrix created by calculateMatrix
   * @param matrix - Frechet matrix
   * @return the Frechet distance in meters
   */
  static Meters calculate(frechet_matrix& matrix);

  /**
   * @brief calculate function calculates the Frechet distance between two ways already converted to LineStrings
   * @param ls1 - Pointer to first LineString
   * @param ls2 - Pointer to second LineString (order doesn't matter)
   * @return the Frechet distance in meters
   */
  static Meters calculate(shared_ptr<const LineString> ls1, shared_ptr<const LineString> ls2);

  /**
   * @brief calculatePoly function calculates the Frechet distance between two polygons represented as ways
   *  NOTE: w1 begins and ends with the same node (as does w2) to make it a closed polygon
   * @param map - OsmMap pointer used to check direction of ways and in way conversion
   * @param way1 - Pointer to first polygon as a way
   * @param way2 - Pointer to second polygon as a way (order doesn't matter)
   * @return the Frechet distance in meters
   */
  static Meters calculatePoly(const ConstOsmMapPtr& map, const ConstWayPtr& way1, const ConstWayPtr& way2);

  /**
   * @brief calculatePoly function calculates the Frechet distance between two polygons already loaded into a Frechet matrix created by calculateMatrix
   * @param matrix - Frechet matrix for both polygons (matrix doesn't contain the last node for both ways/LineStrings
   * @return the Frechet distance in meters
   */
  static Meters calculatePoly(frechet_matrix& matrix);

  /**
   * @brief calculatePoly function calculates the Frechet distance between two polygons already converted to LineStrings
   *  NOTE: ls1 begins and ends with the same node (as does ls2) to make it a closed polygon
   * @param ls1 - Pointer to first LineString
   * @param ls2 - Pointer to second LineString (order doesn't matter)
   * @return the Frechet distance in meters
   */
  static Meters calculatePoly(shared_ptr<const LineString> ls1, shared_ptr<const LineString> ls2);

  /**
   * @brief calculateMatrix function creates a Frechet matrix with distances between all points
   * @param ls1 - Pointer to first LineString
   * @param ls2 - Pointer to second LineString (order doesn't matter)
   * @return the Frechet matrix
   */
  static frechet_matrix calculateMatrix(shared_ptr<const LineString> ls1, shared_ptr<const LineString> ls2);

  /**
   * @brief calculateSubline function calculates the subline match between to ways that are no further than maxDistance apart
   * @param matrix - input Frechet matrix
   * @param maxDistance - maximum allowable distance between points to be considered in subline
   * @return index pairs for nodes that represent the subline
   */
  static frechet_subline calculateSubline(frechet_matrix& matrix, Meters maxDistance = 15);

  /**
   * @brief calculateSubline function calculates the subline match between to ways that are no further than maxDistance apart
   * @param map - OsmMap pointer used to check direction of ways and in way conversion
   * @param way1 - Pointer to first way
   * @param way2 - Pointer to second way (order doesn't matter)
   * @return index pairs for nodes that represent the subline
   * @return
   */
  static frechet_subline calculateSubline(const ConstOsmMapPtr& map, const ConstWayPtr& way1, const ConstWayPtr& way2, Meters maxDistance = 15);


  static Meters calculateExpanded(const ConstOsmMapPtr& map, const ConstWayPtr& way1, const ConstWayPtr& way2);
  static frechet_subline calculateSublineExpanded(const ConstOsmMapPtr& map, const ConstWayPtr& way1, const ConstWayPtr& way2, Meters maxDistance = 15);

protected:
  /**
   * @brief FrechetDistance constructor is protected
   */
  FrechetDistanceOld() {}

  static void createCopies(const ConstOsmMapPtr& map, const ConstWayPtr& way1,
                           const ConstWayPtr& way2, OsmMapPtr& mapCopy, WayPtr& way1copy, WayPtr& way2Copy, Meters maxDistance = 15);

  /**
   * @brief _calculate function calculates the Frechet distance between either two ways represented as a matrix or LineStrings
   * @param matrix - Frechet matrix of correct size, can be optionally empty and the function will use the two LineStrings REQUIRED
   * @param ls1 - Pointer to first LineString OPTIONAL
   * @param ls2 - Pointer to second LineString (order doesn't matter) OPTIONAL
   * @return the Frechet distance in meters
   */
  static Meters _calculate(frechet_matrix &matrix, const LineString* ls1 = NULL, const LineString* ls2 = NULL);

  /**
   * @brief _calculateSubline function calculates the subline match between to ways that are no further than maxDistance apart
   * @param matrix - input Frechet matrix
   * @param maxDistance - maximum allowable distance between points to be considered in subline
   * @param rev1 - boolean flag to reverse the order of the nodes in matrix row index
   * @param rev2 - boolean flag to reverse the order of the nodes in matrix column index
   * @return index pairs for nodes that represent the subline
   */
  static frechet_subline _calculateSubline(frechet_matrix& matrix, Meters maxDistance = 15, bool rev1 = false, bool rev2 = false);
};

}
#endif // FRECHETDISTANCE_H
