#include "FrechetDistance.h"

// geos
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/LineString.h>

//  Hoot
#include <hoot/core/algorithms/DirectionFinder.h>
#include <hoot/core/schema/OsmSchema.h>
#include <hoot/core/util/ElementConverter.h>

namespace hoot
{

Meters FrechetDistance::calculate(OsmMapPtr map, shared_ptr<Way> w1, shared_ptr<Way> w2)
{
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

Meters FrechetDistance::calculate(frechet_matrix &matrix)
{
  return _calculate(matrix, NULL, NULL);
}

Meters FrechetDistance::calculate(shared_ptr<const LineString> ls1, shared_ptr<const LineString> ls2)
{
  if (!ls1 || !ls2)
    throw 1;  //  FIXME: Throw something meaningful
  int rows = ls1->getNumPoints();
  int cols = ls2->getNumPoints();
  if (rows < 1 || cols < 1)
    throw 1;  //  FIXME: Throw something meaningful
  frechet_matrix frechet(boost::extents[rows][cols]);

  return _calculate(frechet, ls1.get(), ls2.get());
}

Meters FrechetDistance::_calculate(frechet_matrix &matrix, const LineString* ls1, const LineString* ls2)
{
  if (matrix.num_dimensions() != 2)
    throw 1;  //  FIXME: Throw something meaningful
  int rows = matrix.shape()[0],
      cols = matrix.shape()[1];
  if (rows < 1 || cols < 1)
    throw 1;  //  FIXME: Throw something meaningful
  if ((ls1 && (int)ls1->getNumPoints() != rows) || (ls2 && (int)ls2->getNumPoints() != cols))
    throw 1;  //  FIXME: Throw something meaningful
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

Meters FrechetDistance::calculatePoly(OsmMapPtr map, shared_ptr<Way> w1, shared_ptr<Way> w2)
{
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

Meters FrechetDistance::calculatePoly(shared_ptr<const LineString> ls1, shared_ptr<const LineString> ls2)
{
  if (!ls1 || !ls2)
    throw 1;  //  FIXME: Throw something meaningful
  //  Three points (plus the first point again) is the smallest polygon
  if (ls1->getNumPoints() < 4 || ls2->getNumPoints() < 4)
    throw 1;  //  FIXME: Throw something meaningful
  //  Make sure the line string is a closed polygon
  if (!ls1->isClosed() || !ls2->isClosed())
    throw 1;  //  FIXME: Throw something meaningful

  //  Remove the last coordinate before processing
  CoordinateSequence* cs1 = GeometryFactory::getDefaultInstance()->getCoordinateSequenceFactory()->create(ls1->getNumPoints() - 1, 2);
  for (size_t i = 0; i < ls1->getNumPoints() - 1; i++)
    cs1->setAt(ls1->getCoordinateN(i), i);
  shared_ptr<LineString> ls_mod1(GeometryFactory::getDefaultInstance()->createLineString(cs1));

  CoordinateSequence* cs2 = GeometryFactory::getDefaultInstance()->getCoordinateSequenceFactory()->create(ls2->getNumPoints() - 1, 2);
  for (size_t i = 0; i < ls2->getNumPoints() - 1; i++)
    cs2->setAt(ls2->getCoordinateN(i), i);
  shared_ptr<LineString> ls_mod2(GeometryFactory::getDefaultInstance()->createLineString(cs2));

  frechet_matrix matrix = calculateMatrix(ls1, ls2);

  return calculatePoly(matrix);
}

Meters FrechetDistance::calculatePoly(frechet_matrix& matrix)
{
  if (matrix.num_dimensions() != 2)
    throw 1;  //  FIXME: Throw something meaningful
  int rows = matrix.shape()[0],
      cols = matrix.shape()[1];
  if (rows < 3 || cols < 3)
    throw 1;  //  FIXME: Throw something meaningful

  Meters best_frechet = 100000000;  //  FIXME: Use max double

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

frechet_matrix FrechetDistance::calculateMatrix(shared_ptr<const LineString> ls1, shared_ptr<const LineString> ls2)
{
  if (!ls1 || !ls2)
    throw 1;  //  FIXME: Throw something meaningful
  int rows = ls1->getNumPoints();
  int cols = ls2->getNumPoints();
  if (rows < 1 || cols < 1)
    throw 1;  //  FIXME: Throw something meaningful
  frechet_matrix frechet(boost::extents[rows][cols]);
  for (int r = 0; r < rows; r++)
  {
    for (int c = 0; c < cols; c++)
      frechet[r][c] = ls1->getCoordinateN(r).distance(ls2->getCoordinateN(c));
  }
  return frechet;
}

frechet_subline FrechetDistance::calculateSubline(frechet_matrix& matrix, Meters maxDistance)
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
    subline.push_back(starts[i]);

    int r = starts[i].first;
    int c = starts[i].second;

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
        subline.push_back(vertex_match(r, c));
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

frechet_subline FrechetDistance::calculateSubline(OsmMapPtr map, shared_ptr<Way> w1, shared_ptr<Way> w2, Meters maxDistance)
{
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

  frechet_matrix frechet = calculateMatrix(ls1, ls2);

  return calculateSubline(frechet, maxDistance);
}

}
