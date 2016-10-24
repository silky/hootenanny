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
 * @copyright Copyright (C) 2015 DigitalGlobe (http://www.digitalglobe.com/)
 */
#ifndef POIPOLYGONEVIDENCESCORER_H
#define POIPOLYGONEVIDENCESCORER_H

// hoot
#include <hoot/core/OsmMap.h>

// geos
#include <geos/geom/Geometry.h>

namespace hoot
{

/**
 *
 */
class PoiPolygonEvidenceScorer
{
public:

  PoiPolygonEvidenceScorer(double matchDistance, double _reviewDistance, double distance,
                           double typeScoreThreshold, double nameScoreThreshold,
                           unsigned int matchEvidenceThreshold, shared_ptr<Geometry> poiGeom,
                           long surroundingPolyCount, ConstOsmMapPtr map, QString testUuid);

  unsigned int calculateEvidence(ConstElementPtr poi, ConstElementPtr poly);

  QString getType1BestKvp() const { return _t1BestKvp; }
  QString getType2BestKvp() const { return _t2BestKvp; }

  double getTypeScore() const { return _typeScore; }
  bool getTypeMatch() const { return _typeMatch; }

  double getNameScore() const { return _nameScore; }
  bool getNameMatch() const { return _nameMatch; }
  bool getExactNameMatch() const { return _exactNameMatch; }

private:

  double _matchDistance;
  double _reviewDistance;
  double _distance;
  double _typeScoreThreshold;
  double _nameScoreThreshold;
  unsigned int _matchEvidenceThreshold;

  double _typeScore;
  bool _typeMatch;
  double _nameScore;
  bool _nameMatch;
  bool _exactNameMatch;

  QString _t1BestKvp;
  QString _t2BestKvp;

  shared_ptr<Geometry> _poiGeom;

  long _surroundingPolyCount;

  ConstOsmMapPtr _map;

  bool _closeMatch;

  QString _testUuid;
  bool _testFeatureFound;

  unsigned int _getDistanceEvidence(ConstElementPtr poi, ConstElementPtr poly);
  unsigned int _getConvexPolyDistanceEvidence(ConstElementPtr poly);
  unsigned int _getTypeEvidence(ConstElementPtr poi, ConstElementPtr poly);
  unsigned int _getNameEvidence(ConstElementPtr poi, ConstElementPtr poly);
  unsigned int _getAddressEvidence(ConstElementPtr poi, ConstElementPtr poly);

};

}

#endif // POIPOLYGONEVIDENCESCORER_H