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

// CPP Unit
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestFixture.h>

// GEOS
#include <geos/geom/LineString.h>
// Hoot
#include <hoot/core/MapProjector.h>
#include <hoot/core/OsmMap.h>
#include <hoot/core/algorithms/FrechetSublineMatcher.h>
#include <hoot/core/io/OsmMapReaderFactory.h>
#include <hoot/core/io/OsmReader.h>
#include <hoot/core/io/OsmWriter.h>
#include <hoot/core/util/GeometryUtils.h>
#include <hoot/core/visitors/FindWaysVisitor.h>

#include "../TestUtils.h"

using namespace hoot;


namespace hoot
{

class FrechetSublineMatcherTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(FrechetSublineMatcherTest);
  CPPUNIT_TEST(sublineTest);
  CPPUNIT_TEST(runVTest);
  CPPUNIT_TEST_SUITE_END();

public:

  OsmMapPtr createMap()
  {
    shared_ptr<OsmMap> map(new OsmMap());
    OsmMap::resetCounters();
    auto_ptr<OGREnvelope> env(GeometryUtils::toOGREnvelope(Envelope(0, 1, 0, 1)));
    MapProjector::projectToPlanar(map, *env);

    return map;
  }


  void sublineTest()
  {

  }

  void runVTest()
  {
    return;

    OsmMapPtr map = createMap();

    /*
     * Create ways like this:
     * w1 \    / w2
     *     \  /
     *      \/
     */
    Coordinate c1[] = { Coordinate(0.0, 0.0), Coordinate(-15.0, 50.0), Coordinate::getNull() };
    WayPtr w1 = TestUtils::createWay(map, Status::Unknown1, c1, 10.0, "w1");

    Coordinate c2[] = { Coordinate(0.0, 0.0), Coordinate(15.0, 50.0), Coordinate::getNull() };
    WayPtr w2 = TestUtils::createWay(map, Status::Unknown2, c2, 10.0, "w2");

    FrechetSublineMatcher uut;
//    MaximalSubline uut(new MaximalSubline::ThresholdMatchCriteria(40.0, M_PI / 1.0), 40.0);

    double score;
    WaySublineMatchString m = uut.findMatch(map, w1, w2, score);
//    vector<WaySublineMatch> m = uut.findAllMatches(map, w1, w2, score);

    HOOT_STR_EQUALS(1, m.getMatches().size());
    HOOT_STR_EQUALS(
      "subline 1: start: way: -1 index: 0 fraction: 0 end: way: -1 index: 1 fraction: 0\n"
      "subline 2: start: way: -2 index: 0 fraction: 0 end: way: -2 index: 1 fraction: 0",
      m.getMatches().at(0).toString());
  }
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FrechetSublineMatcherTest, "current");

}
