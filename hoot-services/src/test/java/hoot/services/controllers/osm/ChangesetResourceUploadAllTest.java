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
package hoot.services.controllers.osm;

import static com.querydsl.core.group.GroupBy.groupBy;
import static hoot.services.HootProperties.CHANGESET_BOUNDS_EXPANSION_FACTOR_DEEGREES;
import static hoot.services.models.db.QCurrentNodes.currentNodes;
import static hoot.services.models.db.QCurrentRelationMembers.currentRelationMembers;
import static hoot.services.models.db.QCurrentRelations.currentRelations;
import static hoot.services.models.db.QCurrentWayNodes.currentWayNodes;
import static hoot.services.models.db.QCurrentWays.currentWays;
import static hoot.services.utils.DbUtils.createQuery;
import static org.hamcrest.Matchers.greaterThan;
import static org.hamcrest.Matchers.is;

import java.sql.Timestamp;
import java.util.Calendar;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import javax.ws.rs.WebApplicationException;
import javax.ws.rs.client.Entity;
import javax.ws.rs.core.MediaType;
import javax.xml.xpath.XPath;
import javax.xml.xpath.XPathExpressionException;

import org.apache.xpath.XPathAPI;
import org.junit.Assert;
import org.junit.Ignore;
import org.junit.Test;
import org.junit.experimental.categories.Category;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import hoot.services.UnitTest;
import hoot.services.geo.BoundingBox;
import hoot.services.models.db.Changesets;
import hoot.services.models.db.CurrentNodes;
import hoot.services.models.db.CurrentRelationMembers;
import hoot.services.models.db.CurrentRelations;
import hoot.services.models.db.CurrentWayNodes;
import hoot.services.models.db.CurrentWays;
import hoot.services.models.db.QChangesets;
import hoot.services.models.osm.Changeset;
import hoot.services.osm.OsmResourceTestAbstract;
import hoot.services.osm.OsmTestUtils;
import hoot.services.utils.DbUtils;
import hoot.services.utils.MapUtils;
import hoot.services.utils.PostgresUtils;
import hoot.services.utils.QuadTileCalculator;
import hoot.services.utils.XmlDocumentBuilder;
import hoot.services.utils.XmlUtils;


@Ignore
public class ChangesetResourceUploadAllTest extends OsmResourceTestAbstract {

    public ChangesetResourceUploadAllTest() {}

    /*
     * This test tests basically everything possible in a valid changeset upload
     * request and is a superset of some of the tests in
     * ChangesetResourceUploadCreateTest, ChangesetResourceUploadDeleteTest, and
     * ChangesetResourceUploadModifyTest.
     */
    @Test
    @Category(UnitTest.class)
    public void testUploadAll() throws Exception {
        DbUtils.deleteMapRelatedTablesByMapId(mapId);
        MapUtils.createMap(mapId);

        BoundingBox originalBounds = OsmTestUtils.createStartingTestBounds();
        long changesetId = OsmTestUtils.createTestChangeset(originalBounds);
        Set<Long> nodeIds = OsmTestUtils.createTestNodes(changesetId, originalBounds);
        Long[] nodeIdsArr = nodeIds.toArray(new Long[nodeIds.size()]);
        Set<Long> wayIds = OsmTestUtils.createTestWays(changesetId, nodeIds);
        Long[] wayIdsArr = wayIds.toArray(new Long[wayIds.size()]);
        Set<Long> relationIds = OsmTestUtils.createTestRelations(changesetId, nodeIds, wayIds);
        Long[] relationIdsArr = relationIds.toArray(new Long[relationIds.size()]);

        /*
         * now update the changeset via the service with create, modify, and
         * delete requests in the same diff request
         *
         * The original dataset looks like (0 based ID ordering):
         *
         * nodes
         *
         * node 0: with node tags "key 1" and "key 2" node 1: with no node
         * tags node 2: with no node tags node 3: with node tags "key 3"
         * node 4: with node tags "key 4"
         *
         * ways
         *
         * way 0: with nodes 0, 1, 4; with way tags "key 1" and "key 2" way
         * 1: with nodes 2, 1; with no way tags way 2: with node 0, 1; with
         * way tags "key 3"
         *
         * relations
         *
         * relation 0: with members: node 0, "role1" way 1, "role3" way 0,
         * "role2" node 2, no role relation tags "key 1"/"val 1" relation 1:
         * with members: node 4, "role1" relation 0, "role1" relation tags
         * "key 2"/"val 2" "key 3"/"val 3" relation 2: with members: way 1,
         * no role relation tags "key 4"/"val 4" relation 3: with members:
         * node 2, "role1" no tags
         *
         * This request:
         *
         * creates two nodes: one linked to the existing node tag "key 4"
         * and a new node tag, "key 5" one with no node tags
         *
         * creates two ways: one with node 0 and node 4; with way tag
         * "key 4" one with node 0, node 2, and node 4; with no way tag
         *
         * creates three relations: one with way 1/"role2", node 2/"role3",
         * and node 4/empty role; with no relation tag one with node
         * 0/"role1" and the previous relation created in this list, no
         * role; with relation tag "key 4" one with relation 1/"role2"; with
         * relation tag "key 1"
         *
         * modifies two nodes: modifies node 0 to have tag "key 1b" and
         * "key 2b" and an updated bounds modifies node 1 to have tag
         * "key 5b" and an updated bounds
         *
         * modifies two ways: modifies way 0 to remove node 1; remove tag
         * "key 1" modifies way 1 to swap node 1 for node 4; tags unchanged
         *
         * modifies two relations: modifies relation 0 to remove node 0;
         * remove tag "key 1" modifies relation 1 to reverse the order of
         * node 4 and relation 0; tags unchanged
         *
         * deletes one node: node 3
         *
         * deletes one way way 2
         *
         * deletes one relation relation 2
         */
        BoundingBox updatedBounds = OsmTestUtils.createAfterModifiedTestChangesetBounds();
        Document responseData = null;
        try {
            responseData = target("api/0.6/changeset/" + changesetId + "/upload")
                .queryParam("mapId", String.valueOf(mapId))
                .request(MediaType.TEXT_XML)
                .post(Entity.entity(
                    "<osmChange version=\"0.3\" generator=\"iD\">" +
                        "<create>" +
                            "<node id=\"-1\" lon=\"" + originalBounds.getMinLon() + "\" lat=\"" +
                                 originalBounds.getMinLat() + "\" version=\"0\" changeset=\"" + changesetId + "\">" +
                                "<tag k=\"key 4\" v=\"val 4\"/>" + "<tag k=\"key 5\" v=\"val 5\"/>" +
                            "</node>" +
                            "<node id=\"-2\" lon=\"" + originalBounds.getMaxLon() + "\" lat=\"" +
                                 originalBounds.getMaxLat() + "\" version=\"0\" changeset=\"" + changesetId + "\">" +
                            "</node>" +
                            "<way id=\"-3\" version=\"0\" changeset=\"" + changesetId + "\" >" +
                                "<nd ref=\"" + nodeIdsArr[0] + "\"></nd>" +
                                "<nd ref=\"" + nodeIdsArr[4] + "\"></nd>" +
                                "<tag k=\"key 4\" v=\"val 4\"/>" +
                            "</way>" +
                            "<way id=\"-4\" version=\"0\" changeset=\"" + changesetId + "\" >" +
                                "<nd ref=\"" + nodeIdsArr[0] + "\"></nd>" +
                                "<nd ref=\"" + nodeIdsArr[2] + "\"></nd>" +
                                "<nd ref=\"" + nodeIdsArr[4] + "\"></nd>" +
                            "</way>" +
                            "<relation id=\"-5\" version=\"0\" changeset=\"" + changesetId + "\" >" +
                                "<member type=\"way\" role=\"role2\" ref=\"" + wayIdsArr[1] + "\"></member>" +
                                "<member type=\"node\" role=\"role3\" ref=\"" + nodeIdsArr[2] + "\"></member>" +
                                "<member type=\"node\" role=\"\" ref=\"" + nodeIdsArr[4] + "\"></member>" +
                            "</relation>" +
                            "<relation id=\"-6\" version=\"0\" changeset=\"" + changesetId + "\" >" +
                                "<member type=\"node\" role=\"role1\" ref=\"" + nodeIdsArr[0] + "\"></member>" +
                                "<member type=\"relation\" ref=\"-5\"></member>" +
                                "<tag k=\"key 4\" v=\"val 4\"></tag>" +
                            "</relation>" +
                            "<relation id=\"-7\" version=\"0\" changeset=\"" + changesetId + "\" >" +
                                "<member type=\"relation\" role=\"role2\" ref=\"" + relationIdsArr[1] + "\"></member>" +
                                "<tag k=\"key 1\" v=\"val 1\"></tag>" +
                            "</relation>" +
                        "</create>" +
                        "<modify>" +
                            "<node id=\"" + nodeIdsArr[0] + "\" lon=\"" + updatedBounds.getMinLon() + "\" " +
                                "lat=\"" + updatedBounds.getMinLat() + "\" version=\"1\" changeset=\"" + changesetId + "\">" +
                                "<tag k=\"key 1b\" v=\"val 1b\"></tag>" +
                                "<tag k=\"key 2b\" v=\"val 2b\"></tag>" +
                            "</node>" +
                            "<node id=\"" + nodeIdsArr[1] + "\" lon=\"" + updatedBounds.getMinLon() + "\" " +
                                "lat=\"" + updatedBounds.getMinLat() + "\" version=\"1\" changeset=\"" + changesetId + "\">" +
                                "<tag k=\"key 5b\" v=\"val 5b\"></tag>" +
                            "</node>" +
                            "<way id=\"" + wayIdsArr[0] + "\" version=\"1\" changeset=\"" + changesetId + "\" >" +
                                "<nd ref=\"" + nodeIdsArr[0] + "\"></nd>" +
                                "<nd ref=\"" + nodeIdsArr[4] + "\"></nd>" +
                                "<tag k=\"key 2\" v=\"val 2\"></tag>" +
                            "</way>" +
                            "<way id=\"" + wayIdsArr[1] + "\" version=\"1\" changeset=\"" + changesetId + "\" >" +
                                "<nd ref=\"" + nodeIdsArr[4] + "\"></nd>" +
                                "<nd ref=\"" + nodeIdsArr[2] + "\"></nd>" +
                            "</way>" +
                            "<relation id=\"" + relationIdsArr[0] + "\" version=\"1\" changeset=\"" + changesetId + "\" >" +
                                "<member type=\"way\" role=\"role4\" ref=\"" + wayIdsArr[1] + "\"></member>" +
                                "<member type=\"way\" role=\"role2\" ref=\"" + wayIdsArr[0] + "\"></member>" +
                                "<member type=\"node\" ref=\"" + nodeIdsArr[2] + "\"></member>" +
                            "</relation>" +
                            "<relation id=\"" + relationIdsArr[1] + "\" version=\"1\" changeset=\"" + changesetId + "\" >" +
                                "<member type=\"relation\" role=\"role1\" ref=\"" + relationIdsArr[0] + "\"></member>" +
                                "<member type=\"node\" ref=\"" + nodeIdsArr[4] + "\"></member>" +
                                "<tag k=\"key 2\" v=\"val 2\"></tag>" + "<tag k=\"key 3\" v=\"val 3\"></tag>" +
                            "</relation>" +
                        "</modify>" +
                        "<delete if-unused=\"true\">" +
                            "<node id=\"" + nodeIdsArr[3] +
                                "\" lon=\"" + originalBounds.getMinLon() + "\" " + "lat=\"" + originalBounds.getMinLat() +
                                "\" version=\"1\" " + "changeset=\"" + changesetId + "\">" +
                            "</node>" +
                            "<way id=\"" + wayIdsArr[2] + "\" version=\"1\" " + "changeset=\"" + changesetId + "\">" +
                                "<nd ref=\"" + nodeIdsArr[0] + "\"></nd>" +
                                "<nd ref=\"" + nodeIdsArr[1] + "\"></nd>" +
                            "</way>" +
                            "<relation id=\"" + relationIdsArr[2] + "\" version=\"1\" " + "changeset=\"" + changesetId + "\">" +
                                "<member type=\"way\" ref=\"" + wayIdsArr[1] + "\"></member>" +
                            "</relation>" +
                        "</delete>" +
                    "</osmChange>", MediaType.TEXT_XML_TYPE), Document.class);
        }
        catch (WebApplicationException e) {
            Assert.fail("Unexpected response: " + e.getResponse());
        }

        // Log responseData.  For debugging purposes...
        String xml = XmlDocumentBuilder.toString(responseData);
        //logger.info(xml);

        Assert.assertNotNull(responseData);

        XPath xpath = XmlUtils.createXPath();
        Set<Long> createdNodeIds = new LinkedHashSet<>();
        Set<Long> createdWayIds = new LinkedHashSet<>();
        Set<Long> createdRelationIds = new LinkedHashSet<>();

        try {
            NodeList returnedNodes = XPathAPI.selectNodeList(responseData, "//osm/diffResult/node");
            Assert.assertEquals(5, returnedNodes.getLength());

            // check the created nodes
            Assert.assertEquals(-1,
                    Long.parseLong(xpath.evaluate("//osm/diffResult/node[1]/@old_id", responseData)));
            long newId = Long.parseLong(xpath.evaluate("//osm/diffResult/node[1]/@new_id", responseData));
            Assert.assertThat(newId, is(greaterThan(0L)));
            createdNodeIds.add(newId);
            Assert.assertEquals(1,
                    Long.parseLong(xpath.evaluate("//osm/diffResult/node[1]/@new_version", responseData)));

            Assert.assertEquals(-2,
                    Long.parseLong(xpath.evaluate("//osm/diffResult/node[2]/@old_id", responseData)));
            newId = Long.parseLong(xpath.evaluate("//osm/diffResult/node[2]/@new_id", responseData));
            Assert.assertThat(newId, is(greaterThan(0L)));
            createdNodeIds.add(newId);
            Assert.assertEquals(1,
                    Long.parseLong(xpath.evaluate("//osm/diffResult/node[2]/@new_version", responseData)));

            // check the modified nodes
            Assert.assertEquals((long) nodeIdsArr[0],
                    Long.parseLong(xpath.evaluate("//osm/diffResult/node[3]/@old_id", responseData)));
            Assert.assertEquals(Long.parseLong(xpath.evaluate("//osm/diffResult/node[3]/@old_id", responseData)),
                    Long.parseLong(xpath.evaluate("//osm/diffResult/node[3]/@new_id", responseData)));
            Assert.assertEquals(2,
                    Long.parseLong(xpath.evaluate("//osm/diffResult/node[3]/@new_version", responseData)));

            Assert.assertEquals((long) nodeIdsArr[1],
                    Long.parseLong(xpath.evaluate("//osm/diffResult/node[4]/@old_id", responseData)));
            Assert.assertEquals(Long.parseLong(xpath.evaluate("//osm/diffResult/node[4]/@old_id", responseData)),
                    Long.parseLong(xpath.evaluate("//osm/diffResult/node[4]/@new_id", responseData)));
            Assert.assertEquals(2,
                    Long.parseLong(xpath.evaluate("//osm/diffResult/node[4]/@new_version", responseData)));

            // check the deleted node
            Assert.assertEquals((long) nodeIdsArr[3],
                    Long.parseLong(xpath.evaluate("//osm/diffResult/node[5]/@old_id", responseData)));

            Node deletedXml = XPathAPI.selectSingleNode(responseData, "//osm/diffResult/node[5]");
            Assert.assertNull(deletedXml.getAttributes().getNamedItem("new_id"));
            Assert.assertNull(deletedXml.getAttributes().getNamedItem("new_version"));

            NodeList returnedWays = XPathAPI.selectNodeList(responseData, "//osm/diffResult/way");
            Assert.assertEquals(5, returnedWays.getLength());

            // check the created ways
            Assert.assertEquals(-3,
                    Long.parseLong(xpath.evaluate("//osm/diffResult/way[1]/@old_id", responseData)));
            newId = Long.parseLong(xpath.evaluate("//osm/diffResult/way[1]/@new_id", responseData));
            Assert.assertThat(newId, is(greaterThan(0L)));
            createdWayIds.add(newId);
            Assert.assertEquals(1,
                    Long.parseLong(xpath.evaluate("//osm/diffResult/way[1]/@new_version", responseData)));

            Assert.assertEquals(-4,
                    Long.parseLong(xpath.evaluate("//osm/diffResult/way[2]/@old_id", responseData)));
            newId = Long.parseLong(xpath.evaluate("//osm/diffResult/way[2]/@new_id", responseData));
            Assert.assertThat(newId, is(greaterThan(0L)));
            createdWayIds.add(newId);
            Assert.assertEquals(1,
                    Long.parseLong(xpath.evaluate("//osm/diffResult/way[2]/@new_version", responseData)));

            // check the modified ways
            Assert.assertEquals((long) wayIdsArr[0],
                    Long.parseLong(xpath.evaluate("//osm/diffResult/way[3]/@old_id", responseData)));
            Assert.assertEquals(Long.parseLong(xpath.evaluate("//osm/diffResult/way[3]/@old_id", responseData)),
                    Long.parseLong(xpath.evaluate("//osm/diffResult/way[3]/@new_id", responseData)));
            Assert.assertEquals(2,
                    Long.parseLong(xpath.evaluate("//osm/diffResult/way[3]/@new_version", responseData)));

            Assert.assertEquals((long) wayIdsArr[1],
                    Long.parseLong(xpath.evaluate("//osm/diffResult/way[4]/@old_id", responseData)));
            Assert.assertEquals(Long.parseLong(xpath.evaluate("//osm/diffResult/way[4]/@old_id", responseData)),
                    Long.parseLong(xpath.evaluate("//osm/diffResult/way[4]/@new_id", responseData)));
            Assert.assertEquals(2,
                    Long.parseLong(xpath.evaluate("//osm/diffResult/way[4]/@new_version", responseData)));

            // check the deleted way
            Assert.assertEquals((long) wayIdsArr[2],
                    Long.parseLong(xpath.evaluate("//osm/diffResult/way[5]/@old_id", responseData)));
            deletedXml = XPathAPI.selectSingleNode(responseData, "//osm/diffResult/way[5]");
            Assert.assertNull(deletedXml.getAttributes().getNamedItem("new_id"));
            Assert.assertNull(deletedXml.getAttributes().getNamedItem("new_version"));

            NodeList returnedRelations = XPathAPI.selectNodeList(responseData, "//osm/diffResult/relation");

            /* **** THIS ASSERT FAILS INTERMITENTLY ON CentOS **** */
            Assert.assertEquals(xml, 6, returnedRelations.getLength());

            // check the created relations
            Assert.assertEquals(-5,
                    Long.parseLong(xpath.evaluate("//osm/diffResult/relation[1]/@old_id", responseData)));
            newId = Long.parseLong(xpath.evaluate("//osm/diffResult/relation[1]/@new_id", responseData));
            Assert.assertThat(newId, is(greaterThan(0L)));
            createdRelationIds.add(newId);
            Assert.assertEquals(1,
                    Long.parseLong(xpath.evaluate("//osm/diffResult/relation[1]/@new_version", responseData)));
            Assert.assertEquals(-6,
                    Long.parseLong(xpath.evaluate("//osm/diffResult/relation[2]/@old_id", responseData)));
            newId = Long.parseLong(xpath.evaluate("//osm/diffResult/relation[2]/@new_id", responseData));
            Assert.assertThat(newId, is(greaterThan(0L)));
            createdRelationIds.add(newId);
            Assert.assertEquals(1,
                    Long.parseLong(xpath.evaluate("//osm/diffResult/relation[2]/@new_version", responseData)));
            Assert.assertEquals(-7,
                    Long.parseLong(xpath.evaluate("//osm/diffResult/relation[3]/@old_id", responseData)));
            newId = Long.parseLong(xpath.evaluate("//osm/diffResult/relation[3]/@new_id", responseData));
            Assert.assertThat(newId, is(greaterThan(0L)));
            createdRelationIds.add(newId);
            Assert.assertEquals(1,
                    Long.parseLong(xpath.evaluate("//osm/diffResult/relation[3]/@new_version", responseData)));

            // check the modified relations
            Assert.assertEquals((long) relationIdsArr[0],
                    Long.parseLong(xpath.evaluate("//osm/diffResult/relation[4]/@old_id", responseData)));
            Assert.assertEquals(
                    Long.parseLong(xpath.evaluate("//osm/diffResult/relation[4]/@old_id", responseData)),
                    Long.parseLong(xpath.evaluate("//osm/diffResult/relation[4]/@new_id", responseData)));
            Assert.assertEquals(2,
                    Long.parseLong(xpath.evaluate("//osm/diffResult/relation[4]/@new_version", responseData)));
            Assert.assertEquals((long) relationIdsArr[1],
                    Long.parseLong(xpath.evaluate("//osm/diffResult/relation[5]/@old_id", responseData)));
            Assert.assertEquals(
                    Long.parseLong(xpath.evaluate("//osm/diffResult/relation[5]/@old_id", responseData)),
                    Long.parseLong(xpath.evaluate("//osm/diffResult/relation[5]/@new_id", responseData)));
            Assert.assertEquals(2,
                    Long.parseLong(xpath.evaluate("//osm/diffResult/relation[5]/@new_version", responseData)));

            // check the deleted relation
            Assert.assertEquals((long)relationIdsArr[2],
                                Long.parseLong(xpath.evaluate("//osm/diffResult/relation[6]/@old_id", responseData)));
            deletedXml = XPathAPI.selectSingleNode(responseData, "//osm/diffResult/relation[6]");
            Assert.assertNull(deletedXml.getAttributes().getNamedItem("new_id"));
            Assert.assertNull(deletedXml.getAttributes().getNamedItem("new_version"));
        }
        catch (XPathExpressionException e) {
            Assert.fail("Error parsing response document: " + e.getMessage());
        }

        Long[] createdNodeIdsArr = createdNodeIds.toArray(new Long[createdNodeIds.size()]);
        Long[] createdWayIdsArr = createdWayIds.toArray(new Long[createdWayIds.size()]);
        Long[] createdRelationIdsArr = createdRelationIds.toArray(new Long[createdRelationIds.size()]);

        Timestamp now = new Timestamp(Calendar.getInstance().getTimeInMillis());
        try {
            Map<Long, CurrentNodes> nodes = createQuery(mapId)
                            .from(currentNodes)
                            .transform(groupBy(currentNodes.id).as(currentNodes));

            Assert.assertEquals(6, nodes.size());

            // verify the updated nodes
            CurrentNodes nodeRecord = nodes.get(nodeIdsArr[0]);
            Assert.assertEquals(new Long(changesetId), nodeRecord.getChangesetId());
            Assert.assertEquals(new Double(updatedBounds.getMinLat()), nodeRecord.getLatitude());
            Assert.assertEquals(new Double(updatedBounds.getMinLon()), nodeRecord.getLongitude());
            Assert.assertEquals(nodeIdsArr[0], nodeRecord.getId());
            Assert.assertEquals(
                    new Long(QuadTileCalculator.tileForPoint(nodeRecord.getLatitude(), nodeRecord.getLongitude())),
                    nodeRecord.getTile());
            Assert.assertTrue(nodeRecord.getTimestamp().before(now));
            Assert.assertEquals(new Long(2), nodeRecord.getVersion());
            Assert.assertTrue(nodeRecord.getVisible());

            // verify the modified tags
            Map<String, String> tags = PostgresUtils.postgresObjToHStore(nodeRecord.getTags());
            Assert.assertNotNull(tags);
            Assert.assertEquals(2, tags.size());
            Assert.assertEquals("val 1b", tags.get("key 1b"));
            Assert.assertEquals("val 2b", tags.get("key 2b"));

            nodeRecord = nodes.get(nodeIdsArr[1]);
            Assert.assertEquals(new Long(changesetId), nodeRecord.getChangesetId());
            Assert.assertEquals(new Double(updatedBounds.getMinLat()), nodeRecord.getLatitude());
            Assert.assertEquals(new Double(updatedBounds.getMinLon()), nodeRecord.getLongitude());
            Assert.assertEquals(nodeIdsArr[1], nodeRecord.getId());
            Assert.assertEquals(
                    new Long(QuadTileCalculator.tileForPoint(nodeRecord.getLatitude(), nodeRecord.getLongitude())),
                    nodeRecord.getTile());
            Assert.assertTrue(nodeRecord.getTimestamp().before(now));
            Assert.assertEquals(new Long(2), nodeRecord.getVersion());
            Assert.assertTrue(nodeRecord.getVisible());

            // verify the created tags
            tags = PostgresUtils.postgresObjToHStore(nodeRecord.getTags());

            Assert.assertNotNull(tags);
            Assert.assertEquals(1, tags.size());
            Assert.assertEquals("val 5b", tags.get("key 5b"));

            // verify the created nodes
            nodeRecord = nodes.get(createdNodeIdsArr[0]);

            Assert.assertEquals(new Long(changesetId), nodeRecord.getChangesetId());
            Assert.assertEquals(new Double(originalBounds.getMinLat()), nodeRecord.getLatitude());
            Assert.assertEquals(new Double(originalBounds.getMinLon()), nodeRecord.getLongitude());
            Assert.assertEquals(createdNodeIdsArr[0], nodeRecord.getId());
            Assert.assertEquals(
                    new Long(QuadTileCalculator.tileForPoint(nodeRecord.getLatitude(), nodeRecord.getLongitude())),
                    nodeRecord.getTile());
            Assert.assertTrue(nodeRecord.getTimestamp().before(now));
            Assert.assertEquals(new Long(1), nodeRecord.getVersion());
            Assert.assertTrue(nodeRecord.getVisible());

            tags = PostgresUtils.postgresObjToHStore(nodeRecord.getTags());

            Assert.assertNotNull(tags);
            Assert.assertEquals(2, tags.size());
            Assert.assertEquals("val 4", tags.get("key 4"));
            Assert.assertEquals("val 5", tags.get("key 5"));

            nodeRecord = nodes.get(createdNodeIdsArr[1]);

            Assert.assertEquals(new Long(changesetId), nodeRecord.getChangesetId());
            Assert.assertEquals(new Double(originalBounds.getMaxLat()), nodeRecord.getLatitude());
            Assert.assertEquals(new Double(originalBounds.getMaxLon()), nodeRecord.getLongitude());
            Assert.assertEquals(createdNodeIdsArr[1], nodeRecord.getId());
            Assert.assertEquals(
                    new Long(QuadTileCalculator.tileForPoint(nodeRecord.getLatitude(), nodeRecord.getLongitude())),
                    nodeRecord.getTile());
            Assert.assertTrue(nodeRecord.getTimestamp().before(now));
            Assert.assertEquals(new Long(1), nodeRecord.getVersion());
            Assert.assertTrue(nodeRecord.getVisible());

            // verify the nodes with no tags
            Assert.assertTrue((nodeRecord.getTags() == null)
                    || PostgresUtils.postgresObjToHStore(nodeRecord.getTags()).isEmpty());

            Assert.assertNull(nodes.get(nodeIdsArr[3]));

            // verify the previously existing tags
            nodeRecord = nodes.get(nodeIdsArr[4]);
            tags = PostgresUtils.postgresObjToHStore(nodeRecord.getTags());
            Assert.assertNotNull(tags);
            Assert.assertEquals(1, tags.size());
            Assert.assertEquals("val 4", tags.get("key 4"));
        }
        catch (Exception e) {
            Assert.fail("Error checking nodes: " + e.getMessage());
        }

        try {
            Map<Long, CurrentWays> ways = createQuery(mapId)
                            .from(currentWays)
                            .transform(groupBy(currentWays.id).as(currentWays));

            Assert.assertEquals(4, ways.size());

            // verify the updated ways
            CurrentWays wayRecord = ways.get(wayIdsArr[0]);
            Assert.assertEquals(new Long(changesetId), wayRecord.getChangesetId());
            Assert.assertEquals(wayIdsArr[0], wayRecord.getId());
            Assert.assertTrue(wayRecord.getTimestamp().before(now));
            Assert.assertEquals(new Long(2), wayRecord.getVersion());
            Assert.assertTrue(wayRecord.getVisible());

            List<CurrentWayNodes> wayNodes = createQuery(mapId)
                    .select(currentWayNodes)
                    .from(currentWayNodes)
                    .where(currentWayNodes.wayId.eq(wayIdsArr[0]))
                    .orderBy(currentWayNodes.sequenceId.asc())
                    .fetch();

            Assert.assertEquals(2, wayNodes.size());
            CurrentWayNodes wayNode = wayNodes.get(0);
            Assert.assertEquals(nodeIdsArr[0], wayNode.getNodeId());
            Assert.assertEquals(new Long(1), wayNode.getSequenceId());
            Assert.assertEquals(wayRecord.getId(), wayNode.getWayId());
            wayNode = wayNodes.get(1);
            Assert.assertEquals(nodeIdsArr[4], wayNode.getNodeId());
            Assert.assertEquals(new Long(2), wayNode.getSequenceId());
            Assert.assertEquals(wayRecord.getId(), wayNode.getWayId());

            // verify the previously existing tags
            Map<String, String> tags = PostgresUtils.postgresObjToHStore(wayRecord.getTags());
            Assert.assertNotNull(tags);
            Assert.assertEquals(1, tags.size());
            Assert.assertEquals("val 2", tags.get("key 2"));

            wayRecord = ways.get(wayIdsArr[1]);
            Assert.assertEquals(new Long(changesetId), wayRecord.getChangesetId());
            Assert.assertEquals(wayIdsArr[1], wayRecord.getId());
            Assert.assertTrue(wayRecord.getTimestamp().before(now));
            Assert.assertEquals(new Long(2), wayRecord.getVersion());
            Assert.assertTrue(wayRecord.getVisible());

            wayNodes = createQuery(mapId)
                    .select(currentWayNodes)
                    .from(currentWayNodes)
                    .where(currentWayNodes.wayId.eq(wayIdsArr[1]))
                    .orderBy(currentWayNodes.sequenceId.asc())
                    .fetch();

            Assert.assertEquals(2, wayNodes.size());
            wayNode = wayNodes.get(0);
            Assert.assertEquals(nodeIdsArr[4], wayNode.getNodeId());
            Assert.assertEquals(new Long(1), wayNode.getSequenceId());
            Assert.assertEquals(wayRecord.getId(), wayNode.getWayId());
            wayNode = wayNodes.get(1);
            Assert.assertEquals(nodeIdsArr[2], wayNode.getNodeId());
            Assert.assertEquals(new Long(2), wayNode.getSequenceId());
            Assert.assertEquals(wayRecord.getId(), wayNode.getWayId());
            Assert.assertTrue((wayRecord.getTags() == null)
                    || PostgresUtils.postgresObjToHStore(wayRecord.getTags()).isEmpty());

            // verify the created ways
            wayRecord = ways.get(createdWayIdsArr[0]);
            Assert.assertEquals(new Long(changesetId), wayRecord.getChangesetId());
            Assert.assertEquals(createdWayIdsArr[0], wayRecord.getId());
            Assert.assertTrue(wayRecord.getTimestamp().before(now));
            Assert.assertEquals(new Long(1), wayRecord.getVersion());
            Assert.assertTrue(wayRecord.getVisible());

            wayNodes = createQuery(mapId)
                    .select(currentWayNodes)
                    .from(currentWayNodes)
                    .where(currentWayNodes.wayId.eq(createdWayIdsArr[0]))
                    .orderBy(currentWayNodes.sequenceId.asc())
                    .fetch();

            Assert.assertEquals(2, wayNodes.size());
            wayNode = wayNodes.get(0);
            Assert.assertEquals(nodeIdsArr[0], wayNode.getNodeId());
            Assert.assertEquals(new Long(1), wayNode.getSequenceId());
            Assert.assertEquals(wayRecord.getId(), wayNode.getWayId());
            wayNode = wayNodes.get(1);
            Assert.assertEquals(nodeIdsArr[4], wayNode.getNodeId());
            Assert.assertEquals(new Long(2), wayNode.getSequenceId());
            Assert.assertEquals(wayRecord.getId(), wayNode.getWayId());

            // verify the created tags
            tags = PostgresUtils.postgresObjToHStore(wayRecord.getTags());
            Assert.assertNotNull(tags);
            Assert.assertEquals(1, tags.size());
            Assert.assertEquals("val 4", tags.get("key 4"));

            wayRecord = ways.get(createdWayIdsArr[1]);
            Assert.assertEquals(new Long(changesetId), wayRecord.getChangesetId());
            Assert.assertEquals(createdWayIdsArr[1], wayRecord.getId());
            Assert.assertTrue(wayRecord.getTimestamp().before(now));
            Assert.assertEquals(new Long(1), wayRecord.getVersion());
            Assert.assertTrue(wayRecord.getVisible());

            wayNodes = createQuery(mapId)
                    .select(currentWayNodes)
                    .from(currentWayNodes)
                    .where(currentWayNodes.wayId.eq(createdWayIdsArr[1]))
                    .orderBy(currentWayNodes.sequenceId.asc())
                    .fetch();

            Assert.assertEquals(3, wayNodes.size());
            wayNode = wayNodes.get(0);
            Assert.assertEquals(nodeIdsArr[0], wayNode.getNodeId());
            Assert.assertEquals(new Long(1), wayNode.getSequenceId());
            Assert.assertEquals(wayRecord.getId(), wayNode.getWayId());
            wayNode = wayNodes.get(1);
            Assert.assertEquals(nodeIdsArr[2], wayNode.getNodeId());
            Assert.assertEquals(new Long(2), wayNode.getSequenceId());
            Assert.assertEquals(wayRecord.getId(), wayNode.getWayId());
            wayNode = wayNodes.get(2);
            Assert.assertEquals(nodeIdsArr[4], wayNode.getNodeId());
            Assert.assertEquals(new Long(3), wayNode.getSequenceId());
            Assert.assertEquals(wayRecord.getId(), wayNode.getWayId());

            // verify the ways with no tags
            Assert.assertTrue((wayRecord.getTags() == null)
                    || PostgresUtils.postgresObjToHStore(wayRecord.getTags()).isEmpty());

            // verify the deleted way
            Assert.assertNull(createQuery(mapId)
                    .select(currentWays)
                    .from(currentWays)
                    .where(currentWays.id.eq(wayIdsArr[2]))
                    .fetchOne());
        }
        catch (Exception e) {
            Assert.fail("Error checking ways: " + e.getMessage());
        }

        try {
            Map<Long, CurrentRelations> relations = createQuery(mapId)
                            .from(currentRelations)
                            .transform(groupBy(currentRelations.id).as(currentRelations));

            Assert.assertEquals(6, relations.size());

            // verify the updated relations
            CurrentRelations relationRecord = relations.get(relationIdsArr[0]);
            Assert.assertEquals(new Long(changesetId), relationRecord.getChangesetId());
            Assert.assertEquals(relationIdsArr[0], relationRecord.getId());
            Assert.assertTrue(relationRecord.getTimestamp().before(now));
            Assert.assertEquals(new Long(2), relationRecord.getVersion());
            Assert.assertTrue(relationRecord.getVisible());

            List<CurrentRelationMembers> members = createQuery(mapId)
                            .select(currentRelationMembers)
                            .from(currentRelationMembers)
                            .where(currentRelationMembers.relationId.eq(relationIdsArr[0]))
                            .orderBy(currentRelationMembers.sequenceId.asc())
                            .fetch();

            Assert.assertEquals(3, members.size());
            CurrentRelationMembers member = members.get(0);
            Assert.assertEquals(relationRecord.getId(), member.getRelationId());
            Assert.assertEquals(DbUtils.nwr_enum.way, member.getMemberType());
            Assert.assertEquals("role4", member.getMemberRole());
            Assert.assertEquals(new Integer(1), member.getSequenceId());

            Assert.assertEquals(wayIdsArr[1], member.getMemberId());
            member = members.get(1);
            Assert.assertEquals(relationRecord.getId(), member.getRelationId());
            Assert.assertEquals(DbUtils.nwr_enum.way, member.getMemberType());
            Assert.assertEquals("role2", member.getMemberRole());
            Assert.assertEquals(new Integer(2), member.getSequenceId());

            Assert.assertEquals(wayIdsArr[0], member.getMemberId());
            member = members.get(2);
            Assert.assertEquals(relationRecord.getId(), member.getRelationId());
            Assert.assertEquals(DbUtils.nwr_enum.node, member.getMemberType());
            Assert.assertEquals("", member.getMemberRole());
            Assert.assertEquals(new Integer(3), member.getSequenceId());
            Assert.assertEquals(nodeIdsArr[2], member.getMemberId());

            // verify tags that should no longer exist
            Assert.assertTrue((relationRecord.getTags() == null)
                    || PostgresUtils.postgresObjToHStore(relationRecord.getTags()).isEmpty());

            relationRecord = relations.get(relationIdsArr[1]);
            Assert.assertEquals(new Long(changesetId), relationRecord.getChangesetId());
            Assert.assertEquals(relationIdsArr[1], relationRecord.getId());
            Assert.assertTrue(relationRecord.getTimestamp().before(now));
            Assert.assertEquals(new Long(2), relationRecord.getVersion());
            Assert.assertTrue(relationRecord.getVisible());

            members = createQuery(mapId)
                    .select(currentRelationMembers)
                    .from(currentRelationMembers)
                    .where(currentRelationMembers.relationId.eq(relationIdsArr[1]))
                    .orderBy(currentRelationMembers.sequenceId.asc())
                    .fetch();

            Assert.assertEquals(2, members.size());
            member = members.get(0);
            Assert.assertEquals(relationRecord.getId(), member.getRelationId());
            Assert.assertEquals(DbUtils.nwr_enum.relation, member.getMemberType());
            Assert.assertEquals("role1", member.getMemberRole());
            Assert.assertEquals(new Integer(1), member.getSequenceId());

            Assert.assertEquals(relationIdsArr[0], member.getMemberId());
            member = members.get(1);
            Assert.assertEquals(relationRecord.getId(), member.getRelationId());
            Assert.assertEquals(DbUtils.nwr_enum.node, member.getMemberType());
            Assert.assertEquals("", member.getMemberRole());
            Assert.assertEquals(new Integer(2), member.getSequenceId());

            Assert.assertEquals(nodeIdsArr[4], member.getMemberId());
            // verify the previously existing tags
            Map<String, String> tags = PostgresUtils.postgresObjToHStore(relationRecord.getTags());
            Assert.assertNotNull(tags);
            Assert.assertEquals(2, tags.size());
            Assert.assertEquals("val 2", tags.get("key 2"));
            Assert.assertEquals("val 3", tags.get("key 3"));

            // verify the unmodified relation
            relationRecord = relations.get(relationIdsArr[3]);
            Assert.assertEquals(new Long(changesetId), relationRecord.getChangesetId());
            Assert.assertEquals(relationIdsArr[3], relationRecord.getId());
            Assert.assertTrue(relationRecord.getTimestamp().before(now));
            Assert.assertEquals(new Long(1), relationRecord.getVersion());
            Assert.assertTrue(relationRecord.getVisible());

            members = createQuery(mapId)
                    .select(currentRelationMembers)
                    .from(currentRelationMembers)
                    .where(currentRelationMembers.relationId.eq(relationIdsArr[3]))
                    .orderBy(currentRelationMembers.sequenceId.asc())
                    .fetch();

            Assert.assertEquals(1, members.size());
            member = members.get(0);
            Assert.assertEquals(relationRecord.getId(), member.getRelationId());
            Assert.assertEquals(DbUtils.nwr_enum.node, member.getMemberType());
            Assert.assertEquals("role1", member.getMemberRole());
            Assert.assertEquals(new Integer(1), member.getSequenceId());

            Assert.assertEquals(nodeIdsArr[2], member.getMemberId());
            Assert.assertTrue((relationRecord.getTags() == null)
                    || PostgresUtils.postgresObjToHStore(relationRecord.getTags()).isEmpty());

            // verify the created relations
            relationRecord = relations.get(createdRelationIdsArr[0]);
            Assert.assertEquals(new Long(changesetId), relationRecord.getChangesetId());
            Assert.assertEquals(createdRelationIdsArr[0], relationRecord.getId());
            Assert.assertTrue(relationRecord.getTimestamp().before(now));
            Assert.assertEquals(new Long(1), relationRecord.getVersion());
            Assert.assertTrue(relationRecord.getVisible());

            members = createQuery(mapId)
                    .select(currentRelationMembers)
                    .from(currentRelationMembers)
                    .where(currentRelationMembers.relationId.eq(createdRelationIdsArr[0]))
                    .orderBy(currentRelationMembers.sequenceId.asc())
                    .fetch();

            Assert.assertEquals(3, members.size());
            member = members.get(0);
            Assert.assertEquals(relationRecord.getId(), member.getRelationId());
            Assert.assertEquals(DbUtils.nwr_enum.way, member.getMemberType());
            Assert.assertEquals("role2", member.getMemberRole());
            Assert.assertEquals(new Integer(1), member.getSequenceId());

            Assert.assertEquals(wayIdsArr[1], member.getMemberId());
            member = members.get(1);
            Assert.assertEquals(relationRecord.getId(), member.getRelationId());
            Assert.assertEquals(DbUtils.nwr_enum.node, member.getMemberType());
            Assert.assertEquals("role3", member.getMemberRole());
            Assert.assertEquals(new Integer(2), member.getSequenceId());

            Assert.assertEquals(nodeIdsArr[2], member.getMemberId());
            member = members.get(2);
            Assert.assertEquals(relationRecord.getId(), member.getRelationId());
            Assert.assertEquals(DbUtils.nwr_enum.node, member.getMemberType());
            Assert.assertEquals("", member.getMemberRole());
            Assert.assertEquals(new Integer(3), member.getSequenceId());

            Assert.assertEquals(nodeIdsArr[4], member.getMemberId());
            Assert.assertTrue((relationRecord.getTags() == null)
                    || PostgresUtils.postgresObjToHStore(relationRecord.getTags()).isEmpty());

            relationRecord = relations.get(createdRelationIdsArr[1]);
            Assert.assertEquals(new Long(changesetId), relationRecord.getChangesetId());
            Assert.assertEquals(createdRelationIdsArr[1], relationRecord.getId());
            Assert.assertTrue(relationRecord.getTimestamp().before(now));
            Assert.assertEquals(new Long(1), relationRecord.getVersion());
            Assert.assertTrue(relationRecord.getVisible());

            members = createQuery(mapId)
                    .select(currentRelationMembers)
                    .from(currentRelationMembers)
                    .where(currentRelationMembers.relationId.eq(createdRelationIdsArr[1]))
                    .orderBy(currentRelationMembers.sequenceId.asc())
                    .fetch();

            Assert.assertEquals(2, members.size());
            member = members.get(0);
            Assert.assertEquals(relationRecord.getId(), member.getRelationId());
            Assert.assertEquals(DbUtils.nwr_enum.node, member.getMemberType());
            Assert.assertEquals("role1", member.getMemberRole());
            Assert.assertEquals(new Integer(1), member.getSequenceId());

            Assert.assertEquals(nodeIdsArr[0], member.getMemberId());
            member = members.get(1);
            Assert.assertEquals(relationRecord.getId(), member.getRelationId());
            Assert.assertEquals(DbUtils.nwr_enum.relation, member.getMemberType());
            Assert.assertEquals("", member.getMemberRole());
            Assert.assertEquals(new Integer(2), member.getSequenceId());

            Assert.assertEquals(createdRelationIdsArr[0], member.getMemberId());
            // verify the created tags
            tags = PostgresUtils.postgresObjToHStore(relationRecord.getTags());
            Assert.assertNotNull(tags);
            Assert.assertEquals(1, tags.size());
            Assert.assertEquals("val 4", tags.get("key 4"));

            relationRecord = relations.get(createdRelationIdsArr[2]);
            Assert.assertEquals(new Long(changesetId), relationRecord.getChangesetId());
            Assert.assertEquals(createdRelationIdsArr[2], relationRecord.getId());
            Assert.assertTrue(relationRecord.getTimestamp().before(now));
            Assert.assertEquals(new Long(1), relationRecord.getVersion());
            Assert.assertTrue(relationRecord.getVisible());

            members = createQuery(mapId)
                    .select(currentRelationMembers)
                    .from(currentRelationMembers)
                    .where(currentRelationMembers.relationId.eq(createdRelationIdsArr[2]))
                    .orderBy(currentRelationMembers.sequenceId.asc())
                    .fetch();

            Assert.assertEquals(1, members.size());
            member = members.get(0);
            Assert.assertEquals(relationRecord.getId(), member.getRelationId());
            Assert.assertEquals(DbUtils.nwr_enum.relation, member.getMemberType());
            Assert.assertEquals("role2", member.getMemberRole());
            Assert.assertEquals(new Integer(1), member.getSequenceId());

            Assert.assertEquals(relationIdsArr[1], member.getMemberId());
            tags = PostgresUtils.postgresObjToHStore(relationRecord.getTags());
            Assert.assertNotNull(tags);
            Assert.assertEquals(1, tags.size());
            Assert.assertEquals("val 1", tags.get("key 1"));

            // verify the deleted relation
            Assert.assertNull(createQuery(mapId)
                    .select(currentRelations)
                    .from(currentRelations)
                    .where(currentRelations.id.eq(relationIdsArr[2]))
                    .fetchOne());

            // verify the relations with no tags
            relationRecord = relations.get(relationIdsArr[3]);
            Assert.assertTrue((relationRecord.getTags() == null)
                    || PostgresUtils.postgresObjToHStore(relationRecord.getTags()).isEmpty());
        }
        catch (Exception e) {
            Assert.fail("Error checking relations: " + e.getMessage());
        }

        try {
            QChangesets changesets = QChangesets.changesets;
            Changesets changeset = createQuery(mapId)
                    .select(changesets)
                    .from(changesets)
                    .where(changesets.id.eq(changesetId))
                    .fetchOne();

            Assert.assertNotNull(changeset);
            Assert.assertTrue(changeset.getCreatedAt().before(now));
            Assert.assertTrue(changeset.getClosedAt().after(changeset.getCreatedAt()));
            Assert.assertEquals(new Integer(28), changeset.getNumChanges());
            Assert.assertEquals(new Long(userId), changeset.getUserId());

            BoundingBox expandedBounds = new BoundingBox();
            double boundsExpansionFactor = Double.parseDouble(CHANGESET_BOUNDS_EXPANSION_FACTOR_DEEGREES);
            expandedBounds.expand(originalBounds, boundsExpansionFactor);
            expandedBounds.expand(updatedBounds, boundsExpansionFactor);
            Changeset hootChangeset = new Changeset(mapId, changesetId);
            hootChangeset.getBounds();
        }
        catch (Exception e) {
            Assert.fail("Error checking changeset: " + e.getMessage());
        }
    }
}
