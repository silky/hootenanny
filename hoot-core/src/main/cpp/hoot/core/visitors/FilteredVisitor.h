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
 * @copyright Copyright (C) 2015, 2016 DigitalGlobe (http://www.digitalglobe.com/)
 */
#ifndef FILTEREDVISITOR_H
#define FILTEREDVISITOR_H

// hoot
#include <hoot/core/ConstOsmMapConsumer.h>
#include <hoot/core/elements/Element.h>
#include <hoot/core/filters/ElementCriterion.h>
#include <hoot/core/filters/ElementCriterionConsumer.h>

#include "ElementVisitorConsumer.h"

namespace hoot
{

class FilteredVisitor :
    public ElementVisitor,
    public ConstOsmMapConsumer,
    public ElementCriterionConsumer,
    public ElementVisitorConsumer
{
public:

  static std::string className() { return "hoot::FilteredVisitor"; }

  FilteredVisitor() : _criterion(0), _visitor(0) {}

  /**
   * Calls the visit method on visitor whenever ElementCriterion::isSatisfied == true.
   */
  FilteredVisitor(const ElementCriterion& criterion, ElementVisitor& visitor);

  /**
   * Similar to above but this is convenient if you want to pass in a temporary visitor. In this
   * case FilteredVisitor will take ownership of the visitor and delete it when destructed.
   */
  FilteredVisitor(const ElementCriterion& criterion, ElementVisitor* visitor);

  /**
   * Similar to above but this is convenient if you want to pass in a temporary criterion and
   * visitor. In this case FilteredVisitor will take ownership of the criterion and visitor and
   * delete it when destructed.
   */
  FilteredVisitor(ElementCriterion *criterion, ElementVisitor* visitor);

  virtual void addCriterion(const ElementCriterionPtr& e);

  virtual void addVisitor(const ElementVisitorPtr& v);

  ElementVisitor& getChildVisitor() const { return *_visitor; }

  virtual void setOsmMap(OsmMap* map);
  virtual void setOsmMap(const OsmMap* map);

  virtual void visit(const ConstElementPtr& e);

private:
  const ElementCriterion* _criterion;
  shared_ptr<ElementCriterion> _criterionDelete;
  const OsmMap* _map;
  ElementVisitor* _visitor;
  shared_ptr<ElementVisitor> _visitDelete;
};

}

#endif // FILTEREDVISITOR_H
