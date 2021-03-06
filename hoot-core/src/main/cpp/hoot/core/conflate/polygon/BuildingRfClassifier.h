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
#ifndef BUILDINGRFCLASSIFIER_H
#define BUILDINGRFCLASSIFIER_H

// hoot
#include <hoot/core/OsmMap.h>
#include <hoot/core/conflate/MatchClassification.h>
#include <hoot/core/conflate/RfExtractorClassifier.h>

// tgs
#include <tgs/RandomForest/RandomForest.h>

namespace hoot
{
using namespace Tgs;

class FeatureExtractor;
class WaySublineMatchString;

class BuildingRfClassifier : public RfExtractorClassifier
{
public:
  BuildingRfClassifier();

protected:

  virtual void _createExtractors() const;

  /**
   * Creates all the reasonable extractors for a data set. This can be quite large and is good
   * for experimentation.
   */
  void _createAllExtractors() const;

  /**
   * Create the best extractors found during experimentation.
   */
  void _createBestExtractors() const;

};

}

#endif // BUILDINGRFCLASSIFIER_H
