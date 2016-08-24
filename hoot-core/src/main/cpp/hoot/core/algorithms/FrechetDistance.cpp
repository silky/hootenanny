#include "FrechetDistance.h"

// geos
#include <geos/geom/LineString.h>

//  Hoot
#include <hoot/core/algorithms/DirectionFinder.h>
#include <hoot/core/schema/OsmSchema.h>
#include <hoot/core/util/ElementConverter.h>

namespace hoot
{

FrechetDistance::FrechetDistance()
{
}

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

  frechet_matrix frechet = calculateMatrix(ls1, ls2);

  return calculate(frechet);
}

Meters FrechetDistance::calculate(frechet_matrix &matrix)
{
  return calculate(matrix, NULL, NULL);
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

  return calculate(frechet, ls1.get(), ls2.get());
}

Meters FrechetDistance::calculate(frechet_matrix &matrix, const LineString* ls1, const LineString* ls2)
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
  if (max_frechet > maxDistance)
  {

  }
  else
  {
    //  Max frechet fits inside the max distance
    starts.push_back(vertex_match(0, 0));
  }

  frechet_subline best_subline;

  for (frechet_subline::size_type i = 0; i < starts.size(); i++)
  {
    frechet_subline subline;
    subline.push_back(starts[i]);

    int s_row = starts[i].first;
    int s_col = starts[i].second;

    int r = s_row;
    int c = s_col;

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
      if (matrix[r][c] < maxDistance)
      {
        subline.push_back(vertex_match(r, c));
      }
      frechet_distance = max(frechet_distance, matrix[r][c]);
    }
  }

  return subline;
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
