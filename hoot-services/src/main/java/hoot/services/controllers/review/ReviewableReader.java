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
package hoot.services.controllers.review;

import hoot.services.controllers.review.model.AllReviewableItems;
import hoot.services.controllers.review.model.ReviewableItem;
import hoot.services.controllers.review.model.ReviewableStatistics;
import hoot.services.geo.BoundingBox;


final class ReviewableReader {
    private ReviewableReader() {}

    static ReviewableItem getReviewableItem(long mapId, long sortOrder) {
        return (ReviewableItem) getReviewableQuery(mapId, sortOrder).execQuery();
    }

    static ReviewableItem getRandomReviewableItem(long mapId) {
        return (ReviewableItem) getRandomReviewableQuery(mapId).execQuery();
    }

    static ReviewableStatistics getReviewablesStatistics(long mapId) {
        return (ReviewableStatistics) getReviewableStatisticsQuery(mapId).execQuery();
    }

    static AllReviewableItems getAllReviewableItems(long mapId, BoundingBox bbox) {
        return (AllReviewableItems) getAllReviewableItemsQuery(mapId, bbox).execQuery();
    }

    private static ReviewableQuery getReviewableQuery(long mapId, long seqId) {
        return new ReviewableQuery(mapId, seqId);
    }

    private static RandomReviewableQuery getRandomReviewableQuery(long mapId) {
        return new RandomReviewableQuery(mapId);
    }

    private static ReviewableStatisticsQuery getReviewableStatisticsQuery(long mapId) {
        return new ReviewableStatisticsQuery(mapId);
    }

    private static AllReviewableItemsQuery getAllReviewableItemsQuery(long mapId, BoundingBox bbox) {
        return new AllReviewableItemsQuery(mapId, bbox);
    }
}