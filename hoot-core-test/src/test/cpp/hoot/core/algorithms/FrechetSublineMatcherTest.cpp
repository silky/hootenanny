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
#include <hoot/core/algorithms/FrechetDistance.h>
#include <hoot/core/visitors/FindWaysVisitor.h>

#include "../TestUtils.h"

using namespace hoot;


namespace hoot
{

class FrechetSublineMatcherTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(FrechetSublineMatcherTest);
  CPPUNIT_TEST(sublineTest);
  CPPUNIT_TEST_SUITE_END();

public:
  void sublineTest()
  {

  }
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FrechetSublineMatcherTest, "current");

}
