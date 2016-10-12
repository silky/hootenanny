#include "FrechetDistance.h"

// geos
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/LineString.h>

//  Hoot
#include <hoot/core/algorithms/DirectionFinder.h>
#include <hoot/core/algorithms/WayHeading.h>
#include <hoot/core/algorithms/linearreference/LocationOfPoint.h>
#include <hoot/core/ops/CopySubsetOp.h>
#include <hoot/core/schema/OsmSchema.h>
#include <hoot/core/util/ElementConverter.h>

namespace hoot
{

FrechetDistance::FrechetDistance(const ConstOsmMapPtr &map, const ConstWayPtr &way1, const ConstWayPtr &way2, Radians maxAngle)
  : _matrix(boost::extents[way1->getNodeCount()][way2->getNodeCount()]), _maxAngle(maxAngle)
{
  _map.reset(new OsmMap());
  CopySubsetOp(map,
               way1->getElementId(),
               way2->getElementId()).apply(_map);
  _w1 = _map->getWay(way1->getId());
  _w2 = _map->getWay(way2->getId());

  _ls1 = ElementConverter(_map).convertToLineString(_w1);
  _ls2 = ElementConverter(_map).convertToLineString(_w2);

  //  Precalculate the locations and distances for way1
  LocationOfPoint locatorWay2(_map, _w2);
  for (size_t i = 0; i < _w1->getNodeCount(); i++)
  {
    Coordinate pointOnWay1 = _map->getNode(_w1->getNodeId(i))->toCoordinate();
    WayLocation nearestPointOnWay2 = locatorWay2.locate(pointOnWay1);
    _locations_w1.push_back(nearestPointOnWay2);
    _distances_w1.push_back(pointOnWay1.distance(nearestPointOnWay2.getCoordinate()));
  }

  //  Precalculate the locations and distances for way2
  LocationOfPoint locatorWay1(_map, _w1);
  for (size_t i = 0; i < _w2->getNodeCount(); i++)
  {
    Coordinate pointOnWay2 = _map->getNode(_w2->getNodeId(i))->toCoordinate();
    WayLocation nearestPointOnWay1 = locatorWay1.locate(pointOnWay2);
    _locations_w2.push_back(nearestPointOnWay1);
    _distances_w2.push_back(pointOnWay2.distance(nearestPointOnWay1.getCoordinate()));
  }

  //  Precalculate the discreet matrix
  _matrix = calculateMatrix();
}

frechet_matrix FrechetDistance::calculateMatrix()
{
  int rows = _ls1->getNumPoints();
  int cols = _ls2->getNumPoints();
  if (rows < 1 || cols < 1)
    throw HootException("FrechetDistance::calculateMatrix - ways not valid sizes");
  frechet_matrix frechet(boost::extents[rows][cols]);
  for (int r = 0; r < rows; r++)
  {
    for (int c = 0; c < cols; c++)
    {
      frechet[r][c] = _ls1->getCoordinateN(r).distance(_ls2->getCoordinateN(c));
//      LOG_VAR(frechet[r][c]);
    }
//    LOG_VAR('\n');
  }

  return frechet;
}

void FrechetDistance::advanceAndCheckRow(const int rows, const int cols, int& r, int& c, Meters& max_frechet)
{
  advanceAndCheck(rows, cols, r, c, max_frechet, true, false);
}

void FrechetDistance::advanceAndCheckColumn(const int rows, const int cols, int& r, int& c, Meters& max_frechet)
{
  advanceAndCheck(rows, cols, r, c, max_frechet, false, true);
}

void FrechetDistance::advanceAndCheckBoth(const int rows, const int cols, int& r, int& c, Meters& max_frechet)
{
  advanceAndCheck(rows, cols, r, c, max_frechet, true, true);
}

void FrechetDistance::advanceAndCheck(const int rows, const int cols, int& r, int& c, Meters& max_frechet, bool advance_row, bool advance_col)
{
//  _angleDiff = WayHeading::deltaMagnitude(heading1, heading2);

  if (!advance_row && ! advance_col)
    return;
  //  Advance the row
  if (advance_row)
  {
    max_frechet = max(max_frechet, _distances_w1[r]);
    r++;
    if (r < rows)
      max_frechet = max(max_frechet, _distances_w1[r]);
  }
  //  Advance the column
  if (advance_col)
  {
    max_frechet = max(max_frechet, _distances_w2[c]);
    c++;
    if (c < cols)
      max_frechet = max(max_frechet, _distances_w2[c]);
  }
}

Radians FrechetDistance::getHeadingWay1(int index)
{
  return getHeading(_w1, index);
}

Radians FrechetDistance::getHeadingWay2(int index)
{
  return getHeading(_w2, index);
}

Radians FrechetDistance::getHeading(WayPtr way, int index)
{
  int index1 = index;
  int index2 = index + 1;

  if (index == (long)(way->getNodeCount() - 1))
  {
    index1 = index - 1;
    index2 = index;
  }

  Coordinate c11 = _map->getNode(way->getNodeId(index1))->toCoordinate();
  Coordinate c12 = _map->getNode(way->getNodeId(index2))->toCoordinate();

  LineSegment ls(c11, c12);

  while (ls.p0 == ls.p1)
  {
    if (index2 == (long)(way->getNodeCount() - 1))
      index1--;
    else if (index1 > 0)
      index2++;
    else
      return 0;
    c11 = _map->getNode(way->getNodeId(index1))->toCoordinate();
    c12 = _map->getNode(way->getNodeId(index2))->toCoordinate();

    ls = LineSegment(c11, c12);
  }

  if (ls.p0 != ls.p1)
    return ls.angle();
  else
    return WayHeading::calculateHeading(WayLocation(_map, way, index, 0), 0.5);
}

Meters FrechetDistance::distance()
{
#ifdef OLD_VERSION
  int rows = _ls1->getNumPoints();
  int cols = _ls2->getNumPoints();
  if (rows < 1 || cols < 1)
    throw HootException("FrechetDistance::_calculate - Invalid matrix size");
  int r = 0, c = 0;
  Meters frechet_distance = _matrix[0][0];
  while (r != rows && c != cols)
  {
    Meters max_frechet = 0.0;
    //  Check for the next move
    if (r == rows - 1 && c == cols -1)
      break;
    else if (r == rows - 1)
      advanceAndCheckColumn(rows, cols, r, c, max_frechet);
    else if (c == cols - 1)
      advanceAndCheckRow(rows, cols, r, c, max_frechet);
    else if (_matrix[r + 1][c + 1] <= _matrix[r + 1][c] && _matrix[r + 1][c + 1] <= _matrix[r][c + 1])
      advanceAndCheckBoth(rows, cols, r, c, max_frechet);
    else if (_matrix[r][c + 1] <= _matrix[r + 1][c] && _matrix[r][c + 1] <= _matrix[r + 1][c + 1])
      advanceAndCheckColumn(rows, cols, r, c, max_frechet);
    else if (_matrix[r + 1][c] <= _matrix[r][c + 1] && _matrix[r + 1][c] <= _matrix[r + 1][c + 1])
      advanceAndCheckRow(rows, cols, r, c, max_frechet);
    double value = (max_frechet > 0.0 ? min(_matrix[r][c], max_frechet) : _matrix[r][c]);
    frechet_distance = max(frechet_distance, value);
  }
  return frechet_distance;
#else
  //  Since at least one endpoint in each way points to an endpoint of another a simple
  //  maximum value of all the closest distances is the non-discreet Frechet distance
  return max(*max_element(_distances_w1.begin(), _distances_w1.end()),
             *max_element(_distances_w2.begin(), _distances_w2.end()));
#endif
}

frechet_subline FrechetDistance::maxSubline(Meters maxDistance)
{
  frechet_subline best_subline;

  int rows = _ls1->getNumPoints();
  int cols = _ls2->getNumPoints();
  if (rows < 1 || cols < 1)
    throw HootException("FrechetDistance::_calculate - Invalid matrix size");
  //  Don't create a subline for non-similar ways
  if (DirectionFinder::isSimilarDirection(_map, _w1, _w2) == false)
    return best_subline;

  Meters max_frechet = distance();

  frechet_subline starts;
  //  Check if the max subline is the combination of both lines
  if (max_frechet >= maxDistance)
  {
    //  Need some kind of algorithm here to get a few different starting locations for max sublines
    //  Let's start with an exhaustive search for starting points less than max distance
    for (int r = 0; r < rows; r++)
    {
      for (int c = 0; c < cols; c++)
      {
        //  Only start where the distance is less than max distance
        if (_matrix[r][c] < maxDistance)
        {
          //  Check above and left to eliminate starting points that lie along a path already started
          if (r > 0 && _matrix[r - 1][c] < maxDistance)
            continue;
          if (c > 0 && _matrix[r][c - 1] < maxDistance)
            continue;
          if (r > 0 && c > 0 && _matrix[r][c] < maxDistance)
            continue;
          starts.push_back(vertex_match(r, c));
        }
      }
    }
  }
  else
  {
    //  Max frechet fits inside the max distance
    starts.push_back(vertex_match(0, 0));
  }

  Meters best_frechet = maxDistance;

  for (frechet_subline::size_type i = 0; i < starts.size(); i++)
  {
    frechet_subline subline;

    int r = starts[i].first;
    int c = starts[i].second;

    //  Use the starting position and modify it if the ways are reversed
    subline.push_back(vertex_match(r, c));

    Meters frechet = _matrix[r][c];

    while (r != rows && c != cols)
    {
      Meters max_frechet = 0.0;
      //  Check for the next move
      if (r == rows - 1 && c == cols -1)
        break;
      else if (r == rows - 1)
        advanceAndCheckColumn(rows, cols, r, c, max_frechet);
      else if (c == cols - 1)
        advanceAndCheckRow(rows, cols, r, c, max_frechet);
      else if (_matrix[r + 1][c + 1] <= _matrix[r + 1][c] && _matrix[r + 1][c + 1] <= _matrix[r][c + 1])
        advanceAndCheckBoth(rows, cols, r, c, max_frechet);
      else if (_matrix[r][c + 1] <= _matrix[r + 1][c] && _matrix[r][c + 1] <= _matrix[r + 1][c + 1])
        advanceAndCheckColumn(rows, cols, r, c, max_frechet);
      else if (_matrix[r + 1][c] <= _matrix[r][c + 1] && _matrix[r + 1][c] <= _matrix[r + 1][c + 1])
        advanceAndCheckRow(rows, cols, r, c, max_frechet);
      double value = (max_frechet > 0.0 ? min(_matrix[r][c], max_frechet) : _matrix[r][c]);
      //  Check that the distance is less than the max distance in order to include this node
      if (value < maxDistance)
      {
        //  Check to make sure that the headings are correct before adding the subline entry
        Radians h1 = getHeadingWay1(r);
        Radians h2 = getHeadingWay2(c);
        Radians angleDiff = WayHeading::deltaMagnitude(h1, h2);
        if (angleDiff <= _maxAngle)
        {
          subline.push_back(vertex_match(r, c));
          frechet = max(frechet, _matrix[r][c]);
        }
        else
        {
          Radians h1 = getHeadingWay1(r);
          Radians h2 = getHeadingWay2(c);
          Radians angleDiff = WayHeading::deltaMagnitude(h1, h2);
          LOG_VAR(_w1->getNodeId(r));
          LOG_VAR(_w2->getNodeId(c));
          LOG_VAR(h1);
          LOG_VAR(h2);
          LOG_VAR(angleDiff);
          LOG_VAR(_maxAngle);
          break;
        }
      }
      else
      {
        break;
      }
    }
    //  Do some backtracking if the two endpoints are too far away of each other
    while (frechet > maxDistance)
    {
      subline.pop_back();
      vertex_match m = subline[subline.size() - 1];
      frechet = _matrix[m.first][m.second];
    }
    //  Is this subline "better" than the current best?
    if ((best_subline.size() == subline.size() && frechet < best_frechet) ||
        (best_subline.size() < subline.size()))
    {
      best_frechet = frechet;
      best_subline = subline;
    }
  }

  return best_subline;
}

/*******************************************************/

Meters FrechetDistanceOld::calculate(const ConstOsmMapPtr& map, const ConstWayPtr& way1, const ConstWayPtr& way2)
{
  WayPtr w1(new Way(*way1));
  WayPtr w2(new Way(*way2));
  //  Check the direction of the two ways
  if (DirectionFinder::isSimilarDirection(map, w1, w2) == false)
  {
    if (OsmSchema::getInstance().isOneWay(*w1) == true)
      w2->reverseOrder();
    else
      w1->reverseOrder();
  }

  shared_ptr<const LineString> ls1 = ElementConverter(map).convertToLineString(w1);
  shared_ptr<const LineString> ls2 = ElementConverter(map).convertToLineString(w2);

  return calculate(ls1, ls2);
}

Meters FrechetDistanceOld::calculate(frechet_matrix &matrix)
{
  return _calculate(matrix, NULL, NULL);
}

Meters FrechetDistanceOld::calculate(shared_ptr<const LineString> ls1, shared_ptr<const LineString> ls2)
{
  if (!ls1 || !ls2)
    throw HootException("FrechetDistance::calculate - LineString arguments invalid");
  int rows = ls1->getNumPoints();
  int cols = ls2->getNumPoints();
  if (rows < 1 || cols < 1)
    throw HootException("FrechetDistance::calculate - LineString arguments not valid sizes");
  frechet_matrix frechet(boost::extents[rows][cols]);

  return _calculate(frechet, ls1.get(), ls2.get());
}

Meters FrechetDistanceOld::_calculate(frechet_matrix &matrix, const LineString* ls1, const LineString* ls2)
{
  if (matrix.num_dimensions() != 2)
    throw HootException("FrechetDistance::_calculate - Invalid matrix dimensions");
  int rows = matrix.shape()[0],
      cols = matrix.shape()[1];
  if (rows < 1 || cols < 1)
    throw HootException("FrechetDistance::_calculate - Invalid matrix size");
  if ((ls1 && (int)ls1->getNumPoints() != rows) || (ls2 && (int)ls2->getNumPoints() != cols))
    throw HootException("FrechetDistance::_calculate - LineString and matrix size miss-match");
  bool empty = false;
  if (ls1 && ls2)
    empty = true;
  int r = 0, c = 0;
  if (empty)
    matrix[0][0] = ls1->getCoordinateN(0).distance(ls2->getCoordinateN(0));
  Meters frechet_distance = matrix[0][0];
  while (r != rows && c != cols)
  {
    //  Populate the sparse matrix if the matrix is empty
    if (empty)
    {
      if (r + 1 < rows)
        matrix[r + 1][c] = ls1->getCoordinateN(r + 1).distance(ls2->getCoordinateN(c));
      if (c + 1 < rows)
        matrix[r][c + 1] = ls1->getCoordinateN(r).distance(ls2->getCoordinateN(c + 1));
      if (r + 1 < rows && c + 1 < rows)
        matrix[r + 1][c + 1] = ls1->getCoordinateN(r + 1).distance(ls2->getCoordinateN(c + 1));
    }
    //  Check for the next move
    if (r == rows - 1 && c == cols -1)
      break;
    else if (r == rows - 1)
      c++;
    else if (c == cols - 1)
      r++;
    else if (matrix[r + 1][c + 1] <= matrix[r + 1][c] && matrix[r + 1][c + 1] <= matrix[r][c + 1])
    {
      r++;
      c++;
    }
    else if (matrix[r][c + 1] <= matrix[r + 1][c] && matrix[r][c + 1] <= matrix[r + 1][c + 1])
      c++;
    else if (matrix[r + 1][c] <= matrix[r][c + 1] && matrix[r + 1][c] <= matrix[r + 1][c + 1])
      r++;
    frechet_distance = max(frechet_distance, matrix[r][c]);
  }
  return frechet_distance;
}

Meters FrechetDistanceOld::calculatePoly(const ConstOsmMapPtr& map, const ConstWayPtr& way1, const ConstWayPtr& way2)
{
  WayPtr w1(new Way(*way1));
  WayPtr w2(new Way(*way2));
  //  Check the direction of the two ways
  if (DirectionFinder::isSimilarDirection(map, w1, w2) == false)
  {
    if (OsmSchema::getInstance().isOneWay(*w1) == true)
      w2->reverseOrder();
    else
      w1->reverseOrder();
  }

  shared_ptr<const LineString> ls1 = ElementConverter(map).convertToLineString(w1);
  shared_ptr<const LineString> ls2 = ElementConverter(map).convertToLineString(w2);

  return calculatePoly(ls1, ls2);
}

Meters FrechetDistanceOld::calculatePoly(shared_ptr<const LineString> ls1, shared_ptr<const LineString> ls2)
{
  if (!ls1 || !ls2)
    throw HootException("FrechetDistance::calculatePoly - LineString arguments invalid");
  //  Three points (plus the first point again) is the smallest polygon
  if (ls1->getNumPoints() < 4 || ls2->getNumPoints() < 4)
    throw HootException("FrechetDistance::calculatePoly - LineString arguments not valid sizes");
  //  Make sure the line string is a closed polygon
  if (!ls1->isClosed() || !ls2->isClosed())
    throw HootException("FrechetDistance::calculatePoly - LineString arguments not closed polygons");

  //  Remove the last coordinate before processing
  CoordinateSequence* cs1 = GeometryFactory::getDefaultInstance()->getCoordinateSequenceFactory()->create(ls1->getNumPoints() - 1, 2);
  for (size_t i = 0; i < ls1->getNumPoints() - 1; i++)
    cs1->setAt(ls1->getCoordinateN(i), i);
  shared_ptr<LineString> ls_mod1(GeometryFactory::getDefaultInstance()->createLineString(cs1));

  CoordinateSequence* cs2 = GeometryFactory::getDefaultInstance()->getCoordinateSequenceFactory()->create(ls2->getNumPoints() - 1, 2);
  for (size_t i = 0; i < ls2->getNumPoints() - 1; i++)
    cs2->setAt(ls2->getCoordinateN(i), i);
  shared_ptr<LineString> ls_mod2(GeometryFactory::getDefaultInstance()->createLineString(cs2));

  frechet_matrix matrix = calculateMatrix(ls_mod1, ls_mod2);

  return calculatePoly(matrix);
}

Meters FrechetDistanceOld::calculatePoly(frechet_matrix& matrix)
{
  if (matrix.num_dimensions() != 2)
    throw HootException("FrechetDistance::calculatePoly - Invalid matrix dimensions");
  int rows = matrix.shape()[0],
      cols = matrix.shape()[1];
  if (rows < 3 || cols < 3)
    throw HootException("FrechetDistance::calculatePoly - Invalid matrix size");

  Meters best_frechet = std::numeric_limits<double>::max();

  //  Iterate all columns in the matrix
  for (int start_c = 0; start_c < cols; start_c++)
  {
    int r = 0;
    int c = start_c;
    Meters frechet = matrix[r][c];

    while (r != rows || c != start_c)
    {
      //  Next column can roll back to 0
      int next_c = (c == cols - 1) ? 0 : c + 1;
      int next_r = r + 1;
      //  Check for the next move
      if (r == rows - 1 && c == start_c)
        break;
      else if (r == rows - 1)
      {
        c = next_c;
        if (r == rows - 1 && c == start_c)
          break;
      }
      else if (next_c == start_c)
      {
        r = next_r;
      }
      else if (matrix[next_r][next_c] <= matrix[next_r][c] && matrix[next_r][next_c] <= matrix[r][next_c])
      {
        r = next_r;
        c = next_c;
      }
      else if (matrix[r][next_c] <= matrix[next_r][c] && matrix[r][next_c] <= matrix[next_r][next_c])
        c = next_c;
      else if (matrix[next_r][c] <= matrix[r][next_c] && matrix[next_r][c] <= matrix[next_r][next_c])
        r = next_r;
      frechet = max(frechet, matrix[r][c]);
    }
    //  Is this polygon "better" than the current best?
    if (frechet < best_frechet)
      best_frechet = frechet;
  }
  return best_frechet;
}

frechet_matrix FrechetDistanceOld::calculateMatrix(shared_ptr<const LineString> ls1, shared_ptr<const LineString> ls2)
{
  if (!ls1 || !ls2)
    throw HootException("FrechetDistance::calculateMatrix - LineString arguments invalid");
  int rows = ls1->getNumPoints();
  int cols = ls2->getNumPoints();
  if (rows < 1 || cols < 1)
    throw HootException("FrechetDistance::calculateMatrix - LineString arguments not valid sizes");
  frechet_matrix frechet(boost::extents[rows][cols]);
  for (int r = 0; r < rows; r++)
  {
    for (int c = 0; c < cols; c++)
      frechet[r][c] = ls1->getCoordinateN(r).distance(ls2->getCoordinateN(c));
  }
  return frechet;
}

frechet_subline FrechetDistanceOld::calculateSubline(frechet_matrix &matrix, Meters maxDistance)
{
  return _calculateSubline(matrix, maxDistance);
}

frechet_subline FrechetDistanceOld::_calculateSubline(frechet_matrix& matrix, Meters maxDistance, bool rev1, bool rev2)
{
  int rows = matrix.shape()[0],
      cols = matrix.shape()[1];

  Meters max_frechet = calculate(matrix);

  frechet_subline starts;
  //  Check if the max subline is the combination of both lines
  if (max_frechet > maxDistance)
  {
    //  Need some kind of algorithm here to get a few different starting locations for max sublines
    //  Let's start with an exhaustive search for starting points less than max distance
    for (int r = 0; r < rows; r++)
    {
      for (int c = 0; c < cols; c++)
      {
        //  Only start where the distance is less than max distance
        if (matrix[r][c] < maxDistance)
        {
          //  Check above and left to eliminate starting points that lie along a path already started
          if (r > 0 && matrix[r - 1][c] < maxDistance)
            continue;
          if (c > 0 && matrix[r][c - 1] < maxDistance)
            continue;
          if (r > 0 && c > 0 && matrix[r][c] < maxDistance)
            continue;
          starts.push_back(vertex_match(r, c));
        }
      }
    }
  }
  else
  {
    //  Max frechet fits inside the max distance
    starts.push_back(vertex_match(0, 0));
  }

  frechet_subline best_subline;
  Meters best_frechet = maxDistance;

  for (frechet_subline::size_type i = 0; i < starts.size(); i++)
  {
    frechet_subline subline;

    int r = starts[i].first;
    int c = starts[i].second;

    //  Use the starting position and modify it if the ways are reversed
    subline.push_back(vertex_match((rev1 ? rows - 1 - r : r), (rev2 ? cols - 1 - c : c)));

    Meters frechet = matrix[r][c];

    while (r != rows && c != cols)
    {
      //  Check for the next move
      if (r == rows - 1 && c == cols -1)
        break;
      else if (r == rows - 1)
        c++;
      else if (c == cols - 1)
        r++;
      else if (matrix[r + 1][c + 1] <= matrix[r + 1][c] && matrix[r + 1][c + 1] <= matrix[r][c + 1])
      {
        r++;
        c++;
      }
      else if (matrix[r][c + 1] <= matrix[r + 1][c] && matrix[r][c + 1] <= matrix[r + 1][c + 1])
        c++;
      else if (matrix[r + 1][c] <= matrix[r][c + 1] && matrix[r + 1][c] <= matrix[r + 1][c + 1])
        r++;
      //  Check that the distance is less than the max distance in order to include this node
      if (matrix[r][c] < maxDistance)
      {
        //  Reverse the row or column index if needed
        subline.push_back(vertex_match((rev1 ? rows - 1 - r : r), (rev2 ? cols - 1 - c : c)));
        frechet = max(frechet, matrix[r][c]);
      }
      else
        break;
    }
    //  Is this subline "better" than the current best?
    if ((best_subline.size() == subline.size() && frechet < best_frechet) ||
        (best_subline.size() < subline.size()))
    {
      best_frechet = frechet;
      best_subline = subline;
    }
  }

  return best_subline;
}

frechet_subline FrechetDistanceOld::calculateSubline(const ConstOsmMapPtr& map, const ConstWayPtr& way1, const ConstWayPtr& way2, Meters maxDistance)
{
  WayPtr w1(new Way(*way1));
  WayPtr w2(new Way(*way2));
  //  Check the direction of the two ways
  bool rev1 = false;
  bool rev2 = false;
  if (DirectionFinder::isSimilarDirection(map, w1, w2) == false)
  {
    if (OsmSchema::getInstance().isOneWay(*w1) == true)
    {
      w2->reverseOrder();
      rev2 = true;
    }
    else
    {
      w1->reverseOrder();
      rev1 = true;
    }
  }

  shared_ptr<const LineString> ls1 = ElementConverter(map).convertToLineString(w1);
  shared_ptr<const LineString> ls2 = ElementConverter(map).convertToLineString(w2);

  frechet_matrix frechet = calculateMatrix(ls1, ls2);

  return _calculateSubline(frechet, maxDistance, rev1, rev2);
}

frechet_subline FrechetDistanceOld::calculateSublineExpanded(const ConstOsmMapPtr& map, const ConstWayPtr& way1, const ConstWayPtr& way2, Meters maxDistance)
{
  OsmMapPtr mapCopy;
  WayPtr w1_p, w2_p;

  createCopies(map, way1, way2, mapCopy, w1_p, w2_p);

  frechet_subline sub1 = calculateSubline(map, way1, w2_p, maxDistance);
  frechet_subline sub2 = calculateSubline(map, way2, w1_p, maxDistance);

  //  Now do something with the two sublines to combine them into one
  if (sub1.size() <= sub2.size())
  {
    return sub1;
  }
  else
  {
    return sub2;
  }
}

void FrechetDistanceOld::createCopies(const ConstOsmMapPtr& map, const ConstWayPtr& way1, const ConstWayPtr& way2,
                                   OsmMapPtr& mapCopy, WayPtr& way1copy, WayPtr& way2Copy, Meters maxDistance)
{
  Status status = Status::Unknown1;
  //  Create copies to change while keeping the originals unchanged
  mapCopy.reset(new OsmMap());
  CopySubsetOp(map,
               way1->getElementId(),
               way2->getElementId()).apply(mapCopy);
  WayPtr w1 = mapCopy->getWay(way1->getId());
  WayPtr w2 = mapCopy->getWay(way2->getId());
  //  Iterate the first way and create a new way of closest points on the second way
  LocationOfPoint locatorWay2(mapCopy, w2);
  way2Copy.reset(new Way(status, mapCopy->createNextWayId(), maxDistance));
  bool reachleft = true;
  for (size_t i = 0; i < w1->getNodeCount(); i++)
  {
    WayLocation nearestPointOnWay2 = locatorWay2.locate(mapCopy->getNode(w1->getNodeId(i))->toCoordinate());
    if (nearestPointOnWay2.isValid())
    {
      int segment = nearestPointOnWay2.getSegmentIndex();
      NodePtr node1 = mapCopy->getNode(w2->getNodeId(segment));
      NodePtr node2 = mapCopy->getNode(w2->getNodeId(segment + 1));
      Coordinate c = nearestPointOnWay2.getCoordinate();
      Meters dist1 = node1->toCoordinate().distance(c);
      Meters dist2 = node2->toCoordinate().distance(c);
      //  At the beginning of the
      if (reachleft)
      {
        //  Use the nearest node(s) that exists if possible
        if (dist1 <= maxDistance && dist2 <= maxDistance)
        {
          if (!way2Copy->hasNode(node1->getId()))
            way2Copy->addNode(node1->getId());
          if (!way2Copy->hasNode(node2->getId()))
            way2Copy->addNode(node2->getId());
        }
        else if (dist1 <= maxDistance)
        {
          if (!way2Copy->hasNode(node1->getId()))
            way2Copy->addNode(node1->getId());
        }
        else if (dist2 <= maxDistance)
        {
          if (!way2Copy->hasNode(node2->getId()))
            way2Copy->addNode(node2->getId());
        }
        else
        {
          NodePtr n(new Node(status, mapCopy->createNextNodeId(), nearestPointOnWay2.getCoordinate(), maxDistance));
          mapCopy->addNode(n);
          way2Copy->addNode(n->getId());
        }
        reachleft = false;
      }
      else
      {
        //  Use the nearest node that exists if possible
        if (dist1 <= maxDistance && dist2 <= maxDistance)
        {
          if (!way2Copy->hasNode(node1->getId()))
            way2Copy->addNode(node1->getId());
          if (!way2Copy->hasNode(node2->getId()))
            way2Copy->addNode(node2->getId());
        }
        else if (dist2 < maxDistance)
        {
          if (!way2Copy->hasNode(node2->getId()))
            way2Copy->addNode(node2->getId());
        }
        else if (dist1 < maxDistance)
        {
          if (!way2Copy->hasNode(node1->getId()))
            way2Copy->addNode(node1->getId());
        }
        else
        {
          NodePtr n(new Node(status, mapCopy->createNextNodeId(), nearestPointOnWay2.getCoordinate(), maxDistance));
          mapCopy->addNode(n);
          way2Copy->addNode(n->getId());
        }
      }
    }
  }
  mapCopy->addWay(way2Copy);

  LocationOfPoint locatorWay1(mapCopy, w1);
  way1copy.reset(new Way(status, mapCopy->createNextWayId(), maxDistance));
  for (size_t i = 0; i < w2->getNodeCount(); i++)
  {
    WayLocation nearestPointOnWay1 = locatorWay1.locate((mapCopy->getNode(w2->getNodeId(i))->toCoordinate()));
    if (nearestPointOnWay1.isValid())
    {
      NodePtr n(new Node(status, mapCopy->createNextNodeId(), nearestPointOnWay1.getCoordinate(), maxDistance));
      mapCopy->addNode(n);
      way1copy->addNode(n->getId());
    }
  }
  mapCopy->addWay(way1copy);
}

Meters FrechetDistanceOld::calculateExpanded(const ConstOsmMapPtr& map, const ConstWayPtr& way1, const ConstWayPtr& way2)
{
  OsmMapPtr mapCopy;
  WayPtr w1_p, w2_p;

  createCopies(map, way1, way2, mapCopy, w1_p, w2_p);

  return max(calculate(mapCopy, way1, w2_p), calculate(mapCopy, w1_p, way2));
}

}
