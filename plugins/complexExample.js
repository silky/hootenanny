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
 * @copyright Copyright (C) 2014 DigitalGlobe (http://www.digitalglobe.com/)
 */

/*
    complexExample conversion script
        complexExample -> OSM+, and
        OSM+ -> complexExample

    Based on mgcp/__init__.js script
*/

complexExample = {
    // getDbSchema - Load the standard schema or modify it into the TDS structure.
    getDbSchema: function() {
        layerNameLookup = {}; // <GLOBAL> Lookup table for converting an FCODE to a layername

        // Warning: This is <GLOBAL> so we can get access to it from other functions
        complexExample.rawSchema = complexExample.schema.getDbSchema();

        // Now build the FCODE/layername lookup table. Note: This is <GLOBAL>
        layerNameLookup = translate.makeLayerNameLookup(complexExample.rawSchema);

        // Debugging:
        // translate.dumpSchema(complexExample.rawSchema);

        return complexExample.rawSchema;

    }, // End getDbSchema


// #####################################################################################################
    // ##### Start of the xxToOsmxx Block #####
    applyToOsmPreProcessing: function(attrs, layerName, geometryType)
    {
        // List of data values to drop/ignore
        var ignoreList = { '-999999.0':1, '-999999':1, 'noinformation':1 };

        // List of attributes that can't have '0' as a value
        var noZeroList = ['BNF','LTN'];

        // This is a handy loop. We use it to:
        // 1) Remove all of the "No Information" and -999999 fields
        // 2) Convert all of the Attrs to uppercase - if needed
       for (var col in attrs)
        {
            // slightly ugly but we would like to account for: 'No Information', 'noInformation' etc
            // First, push to lowercase
            var attrValue = attrs[col].toString().toLowerCase();

            // Get rid of the spaces in the text
            attrValue = attrValue.replace(/\s/g, '');

            // Wipe out the useless values
            if (attrs[col] == '' || attrValue in ignoreList || attrs[col] in ignoreList)
            {
                delete attrs[col]; // debug: Comment this out to leave all of the No Info stuff in for testing
                continue;
            }

            // Remove attributes with '0' values if they can't be '0'
            if (noZeroList.indexOf(col) > -1 && attrs[col] == '0')
            {
                delete attrs[col];
                continue;
            }

            // Push the attribute to upper case - if needed
            var c = col.toUpperCase();
            if (c !== col)
            {
                attrs[c] = attrs[col];
                delete attrs[col];
                col = c; // For the rest of this loop iteration
            }

        } // End in attrs loop


        // Now find an F_CODE
        if (attrs.F_CODE)
        {
            // Nothing to do :-)
        }
        else if (attrs.FCODE)
        {
            attrs.F_CODE = attrs.FCODE;
            delete attrs.FCODE;
        }
        else
        {
            // Time to find an FCODE based on the filename
            var fCodeMap = [
                ['AL013', ['al013','building_s','building_p']], // Building
                ['AP010', ['ap010','cart_track_c']], // Cart Track
                ['AP030', ['ap030','road_c']], // Road
                ['AQ040', ['aq040','bridge_c','bridge_p']], // Bridge 
                ];

            // Funky but it makes life easier
            var llayerName = layerName.toString().toLowerCase();

            for (var row in fCodeMap)
            {
                for (var val in fCodeMap[row][1])
                {
                    if (llayerName == fCodeMap[row][1][val])
                    {
                        attrs.F_CODE = fCodeMap[row][0];
                        break;
                    }
                }
            }
        } // End of Find an FCode

    }, // End of applyToOsmPreProcessing


// #####################################################################################################
    applyToOsmPostProcessing : function (attrs, tags, layerName, geometryType)
    {
        // Roads. complexExample are a bit simpler than TDSv30 & TDSv40
        if (attrs.F_CODE == 'AP030') // Road
        {
             // Set a Default: "It is a road but we don't know what it is"
            tags.highway = 'road';

            // Top level
            if (tags['ref:road:type'] == 'motorway' || tags['ref:road:class'] == 'national_motorway')
            {
                tags.highway = 'motorway';
            }
            else if (tags['ref:road:type'] == 'limited_access_motorway')
            {
                tags.highway = 'trunk';
            }
            else if (tags['ref:road:class'] == 'primary')
            {
                tags.highway = 'primary';
            }
            else if (tags['ref:road:class'] == 'secondary')
            {
                tags.highway = 'secondary';
            }
            else if (tags['ref:road:class'] == 'local')
            {
                if (tags['ref:road:type'] == 'road') // RTY=3
                {
                    tags.highway = 'tertiary';
                }
                else
                {
                    tags.highway = 'unclassified';
                }
            }
            else if (tags['ref:road:type'] == 'pedestrian')
            {
                tags.highway = 'pedestrian';
            }
            else if (tags['ref:road:type'] == 'road') // RTY=3
            {
                tags.highway = 'tertiary';
            }
            else if (tags['ref:road:type'] == 'street') // RTY=4
            {
                tags.highway = 'unclassified';
            }
            // Other should get picked up by the OTH field
            else if (tags['ref:road:type'] == 'other')
            {
                tags.highway = 'road';
            }
        } // End if AP030


        // New complexExample Attribute - ROR (Road Interchange Ramp)
        if (tags.highway && tags.interchange_ramp == 'yes')
        {
            var roadList = ['motorway','trunk','primary','secondary','tertiary'];
            if (roadList.indexOf(tags.highway) !== -1) tags.highway = tags.highway + '_link';
        }

        // Add the LayerName to the source
        tags.source = 'tdsv61:' + layerName.toLowerCase();
        
        // If we have a UFI, store it. Some of the MAAX data has a LINK_ID instead of a UFI
        if (attrs.UFI)
        {
            tags.uuid = '{' + attrs['UFI'].toString().toLowerCase() + '}';
        }
        else
        {
            tags.uuid = createUuid(); 
        }


        if (complexExample.osmPostRules == undefined)
        {
            // ##############
            // A "new" way of specifying rules. Jason came up with this while playing around with NodeJs
            //
            // Rules format:  ["test expression","output result"];
            // Note: t = tags, a = attrs and attrs can only be on the RHS
            var rulesList = [
            ["t['bridge:movable'] && t['bridge:movable'] !== 'no' && t['bridge:movable'] !== 'unknown'","t.bridge = 'movable'"],
            ["t.diplomatic && !(t.amenity)","t.amenity = 'embassy'"],
            ["t.in_tunnel == 'yes' && !(t.tunnel)","t.tunnel = 'yes'; delete t.in_tunnel"],
            ["t.leisure == 'stadium'","t.building = 'yes'"],
            ["t.on_bridge == 'yes' && !(t.bridge)","t.bridge = 'yes'; delete t.on_bridge"]
            ];

            complexExample.osmPostRules = translate.buildComplexRules(rulesList);
        }

        // translate.applyComplexRules(tags,attrs,rulesList);
        translate.applyComplexRules(tags,attrs,complexExample.osmPostRules);

        // ##############

        // Add a building tag to Buildings and Fortified Buildings if we don't have one
        // We can't do this in the funky rules function as it uses "attrs" _and_ "tags"
        if (attrs.F_CODE == 'AL013' && !(tags.building))
        {
            tags.building = 'yes';
        }


        // Fix the building 'use' tag. If the building has a 'use' and no specific building tag. Give it one
        if (tags.building == 'yes' && tags.use)
        {
            if ((tags.use.indexOf('manufacturing') > -1) || (tags.use.indexOf('processing') > -1))
            {
                tags.building = 'industrial';
            }
            // NOTE: Shops have been rempved from "use"
            // Sort out shops
            // else if (hoot.OsmSchema.getTagVertex("shop=" + tags.use).name != "shop=*")
            // {
            //    hoot.warn(hoot.OsmSchema.getTagVertex("shop=" + tags.use));
            //    hoot.warn(hoot.OsmSchema.isAncestor("shop=" + tags.use, "shop"));
            //    hoot.warn(tags.use);
            //    tags.shop = tags.use;
            //    delete tags.use;
            // }

        } // End building & use tags

        // Education:
        if (tags['isced:level'] || tags.use == 'education')
        {
            if (tags.building == 'yes')
            {
                tags.building = 'school'
            }
            else if (tags.facility)
            {
                tags.amenity = 'school';
            }
        }

        if (tags.use == 'vocational_education')
        {
            if (tags.building == 'yes')
            {
                tags.building = 'college'
            }
            else if (tags.facility)
            {
                tags.amenity = 'college';
            }
        }

        // Add defaults for common features
        if (attrs.F_CODE == 'AQ040' && !(tags.bridge)) tags.bridge = 'yes';

        if ('ford' in tags && !(tags.highway)) tags.highway = 'road';

    }, // End of applyToOsmPostProcessing
  
    // ##### End of the xxToOsmxx Block #####

// #####################################################################################################

    // ##### Start of the xxToNfddxx Block #####

    applyToNfddPreProcessing: function(tags, attrs, geometryType)
    {
        // Remove Hoot assigned tags for the source of the data
        if (tags['source:ingest:datetime']) delete tags['source:ingest:datetime'];
        if (tags.source) delete tags.source;
        if (tags.area) delete tags.area;
        if (tags['error:circular']) delete tags['error:circular'];
        if (tags['hoot:status']) delete tags['hoot:status'];

        // Initial cleanup
        for (var i in tags)
        {
            // Remove empty tags
            if (tags[i] == '')
            {
                delete tags[i];
                continue;
            }

            // Convert "abandoned:XXX" and "disused:XXX"features
            if ((i.indexOf('abandoned:') !== -1) || (i.indexOf('disused:') !== -1))
            {
                // Hopeing there is only one ':' in the tag name...
                var tList = i.split(':');
                tags[tList[1]] = tags[i];
                tags.condition = 'abandoned';
                delete tags[i];
                continue;
            }

        } // End Cleanup loop

        if (complexExample.nfddPreRules == undefined)
        {
        // See ToOsmPostProcessing for more details about rulesList.
            var rulesList = [
            ["t.diplomatic && t.amenity == 'embassy'","delete t.amenity"],
            ["t.highway == 'bus_stop'","t['transport:type'] = 'bus'"],
            ["t.highway == 'crossing'","t['transport:type'] = 'road';a.F_CODE = 'AQ062'; delete t.highway"],
            ["t.highway == 'mini_roundabout'","t.junction = 'roundabout'"],
            ["t.historic == 'castle' && t.building","delete t.building"],
            ["t.historic == 'castle' && t.ruins == 'yes'","t.condition = 'destroyed'; delete t.ruins"],
            ["t.leisure == 'stadium' && t.building","delete t.building"],
            ["t.median == 'yes'","t.is_divided = 'yes'"],
            ["t.route == 'road' && !(t.highway)","t.highway = 'road'; delete t.route"],
            ["(t.shop || t.office) &&  !(t.building)","a.F_CODE = 'AL013'"],
            ["t.social_facility == 'shelter'","t.social_facility = t['social_facility:for']; delete t.amenity; delete t['social_facility:for']"],
            ["t.tunnel == 'building_passage'","t.tunnel = 'yes'"]
            ];

            complexExample.nfddPreRules = translate.buildComplexRules(rulesList);
        }

        // Apply the rulesList.
        // translate.applyComplexRules(tags,attrs,rulesList);
        translate.applyComplexRules(tags,attrs,complexExample.nfddPreRules);

        // Some tags imply that they are buildings but don't actually say so.
        // Most of these are taken from raw OSM and the OSM Wiki
        // Not sure if the list of amenities that ARE buildings is shorter than the list of ones that 
        // are not buildings.
        // Taking "place_of_worship" out of this and making it a building
        var notBuildingList = [
            'bbq','biergarten','drinking_water','bicycle_parking','bicycle_rental','boat_sharing',
            'car_sharing','charging_station','grit_bin','parking','parking_entrance','parking_space',
            'taxi','atm','fountain','bench','clock','hunting_stand','marketplace','post_box',
            'recycling', 'vending_machine','waste_disposal','watering_place','water_point',
            'waste_basket','drinking_water','swimming_pool','fire_hydrant','emergency_phone','yes',
            'compressed_air','water','nameplate','picnic_table','life_ring','grass_strip','dog_bin',
            'artwork','dog_waste_bin','street_light','park','hydrant','tricycle_station','loading_dock',
            'trailer_park','game_feeding'
            ]; // End notBuildingList

        if (tags.amenity && !(tags.building) && (notBuildingList.indexOf(tags.amenity) == -1)) attrs.F_CODE = 'AL013';

        // going out on a limb and processing OSM specific tags:
        // - Building == a thing,
        // - Amenity == The area around a thing
        // Note: amenity=place_of_worship is a special case. It _should_ have an associated building tag
        var facilityList = {
            'school':'850', 'university':'855', 'college':'857', 'hospital':'860'
            };

        if (tags.amenity in facilityList)
        {
            if (geometryType == 'Area')
            {
                attrs.F_CODE = 'AL010'; // Facility
            }
            else
            {
                attrs.F_CODE = 'AL013'; // Building
            }

            // If we don't have a Feature Function then assign one.
            if (!(attrs.FFN))
            {
                attrs.FFN = facilityList[tags.amenity];
                // Debug
                // print('PreDropped: amenity = ' + tags.amenity);
                delete tags.amenity;
            }
        }

        // Bridges & Roads.  If we have an area or a line everything is fine.
        // If we have a point, we need to make sure that it becomes a bridge, not a road.
        if (tags.bridge && geometryType =='Point') attrs.F_CODE = 'AQ040';

        // Movable Bridges
        if (tags.bridge == 'movable')
        {
          if (! tags['bridge:movable'])
          {
        	tags['bridge:movable'] = 'unknown';
          }
          tags.bridge = 'yes';
          attrs.F_CODE = 'AQ040';
        }

        // Viaducts
        if (tags.bridge == 'viaduct')
        {
          tags.bridge = 'yes';
          tags.note = translate.appendValue(tags.note,'Viaduct',';');
        }

        // Fix road junctions.
        // TDS has junctions as points. If we can make the feature into a road, railway or bridge then we will
        // If not, it should go to the o2s layer
        if (tags.junction && geometryType !== 'Point')
        {
            if (tags.highway || tags.bridge || tags.railway)
            {
                delete tags.junction;
            }
        } // End AP020 not Point

        // Now use the lookup table to find an FCODE. This is here to stop clashes with the
        // standard one2one rules
        if (!(attrs.F_CODE) && complexExample.fcodeLookup)
        {
            for (var col in tags)
            {
                var value = tags[col];
                if (col in complexExample.fcodeLookup)
                {
                    if (value in complexExample.fcodeLookup[col])
                    {
                        var row = complexExample.fcodeLookup[col][value];
                        attrs.F_CODE = row[1];
                        // print('FCODE: Got ' + attrs.F_CODE);
                    }
                }
            }
        } // End find F_CODE

        // If we still don't have an FCODE, try looking for individual elements
        if (!attrs.F_CODE)
        {
            var fcodeMap = { 
                'highway':'AP030', 'building':'AL013', 'bridge':'AQ040'
                           };

            for (var i in fcodeMap)
            {
                if (i in tags) attrs.F_CODE = fcodeMap[i];
            }
        }


       // Sort out "construction" etc
       // This gets ugly due to the mix of construction, highway & railway
//             ["t.highway == 'construction' && t.construction","t.highway = t.construction; t.condition = 'construction'; delete t.construction"],
//             ["t.railway == 'construction' && t.construction","t.railway = t.construction; t.condition = 'construction'; delete t.construction"],
//             ["t.highway == 'construction' && !(t.construction)","t.highway = 'road'; t.condition = 'construction'"],
//             ["t.railway == 'construction' && !(t.construction)","t.railway = 'rail'; t.condition = 'construction'"],
       if (tags.highway == 'construction' || tags.railway == 'construction')
       {
           if (tags.construction)
           {
                tags.condition = 'construction';
                if (tags.railway)
                {
                    tags.railway = tags.construction;
                }
                else
                {
                    tags.highway = tags.construction;
                }
                delete tags.construction;
           }
           else
           {
                tags.condition = 'construction';
                if (tags.railway)
                {
                    tags.railway = 'rail';
                }
                else
                {
                    tags.highway = 'road';
                }
           }
       } // End Highway & Railway construction

       // Now set the relative levels and transportation types for various features
       if (tags.highway)
       {
           if (tags.bridge && tags.bridge !== 'no')
           {
               complexExample.fixTransType(tags);
               tags.location = 'surface';
               tags.layer = '1';
               tags.on_bridge = 'yes';
           }

           if (tags.tunnel && tags.tunnel !== 'no')
           {
               complexExample.fixTransType(tags);
               // tags.layer = '-1';
               tags.in_tunnel = 'yes';
           }

           if (tags.embankment && tags.embankment !== 'no')
           {
               complexExample.fixTransType(tags);
               tags.layer = '1';
           }

           if (tags.cutting && tags.cutting !== 'no')
           {
               complexExample.fixTransType(tags);
               tags.layer = '-1';
           }

           if (tags.ford && tags.ford !== 'no')
           {
               complexExample.fixTransType(tags);
               tags.location = 'on_waterbody_bottom';
           }

       } // End if highway || railway

       // Debug
       // for (var i in tags) print('End PreProc Tags: ' + i + ': :' + tags[i] + ':');

    }, // End applyToNfddPreProcessing

// #####################################################################################################

    applyToNfddPostProcessing : function (tags, attrs, geometryType, notUsedTags)
    {
        // Sort out the UUID
        if (attrs.UFI)
        {
            var str = attrs['UFI'].split(';');
            attrs.UFI = str[0].replace('{','').replace('}','');
        }
        else
        {
            attrs.UFI = createUuid().replace('{','').replace('}','');
        }

        // Custom Road rules
        // - Fix the "highway=" stuff that cant be done in the one2one rules
        if (attrs.F_CODE == 'AP030') // Road
        {
            // If we havent fixed up the road type/class, have a go with the
            // highway tag.
            if (!attrs.RTY && !attrs.RIN_ROI)
            {
                switch (tags.highway)
                {
                    case 'motorway':
                    case 'motorway_link':
                        attrs.RIN_ROI = '2'; // National Motorway
                        attrs.RTY = '1'; // Motorway
                        break;

                    case 'trunk':
                    case 'trunk_link':
                        attrs.RIN_ROI = '3'; // National/Primary
                        attrs.RTY = '2'; // Limited Access Motorway
                        break;

                    case 'primary':
                    case 'primary_link':
                        attrs.RIN_ROI = '3'; // National
                        attrs.RTY = '3'; // road: Road outside a BUA
                        break;

                    case 'secondary':
                    case 'secondary_link':
                        attrs.RIN_ROI = '4'; // Secondary
                        attrs.RTY = '3'; // road: Road outside a BUA
                        break;

                    case 'tertiary':
                    case 'tertiary_link':
                        attrs.RIN_ROI = '5'; // Local
                        attrs.RTY = '3'; // road: Road outside a BUA
                        break;

                    case 'residential':
                    case 'unclassified':
                    case 'pedestrian':
                    case 'service':
                        attrs.RIN_ROI = '5'; // Local
                        attrs.RTY = '4'; // street: Road inside a BUA
                        break;

                    case 'road':
                        attrs.RIN_ROI = '5'; // Local. Customer requested this translation value
                        attrs.RTY = '-999999'; // No Information
                } // End tags.highway switch
                
            } // End ROI & RIN_ROI
        }
    }, // End applyToNfddPostProcessing

// #####################################################################################################

    // ##### End of the xxToNfddxx Block #####

    // toOsm - Translate Attrs to Tags
    // This is the main routine to convert _TO_ OSM
    toOsm : function(attrs, layerName, geometryType)
    {
        tags = {};  // The final output Tag list

        // Debug:
        if (config.getOgrDebugDumptags() == 'true')
        {
            print('In Layername: ' + layerName);
            var kList = Object.keys(attrs).sort()
            for (var i = 0, fLen = kList.length; i < fLen; i++) print('In Attrs: ' + kList[i] + ': :' + attrs[kList[i]] + ':');
        }

        // Set up the fcode translation rules. We need this due to clashes between the one2one and
        // the fcode one2one rules
        if (complexExample.fcodeLookup == undefined)
        {
            // Add the FCODE rules for Import
            fcodeCommon.one2one.push.apply(fcodeCommon.one2one,complexExample.rules.fcodeOne2oneIn);

            complexExample.fcodeLookup = translate.createLookup(fcodeCommon.one2one);
            // translate.dumpOne2OneLookup(complexExample.fcodeLookup);
        }

        if (complexExample.lookup == undefined)
        {
            // Setup lookup tables to make translation easier. I'm assumeing that since this is not set, the
            // other tables are not set either.

            // Support Import Only attributes
            complexExample.rules.one2one.push.apply(complexExample.rules.one2one,complexExample.rules.one2oneIn);

            complexExample.lookup = translate.createLookup(complexExample.rules.one2one);
        }

        // pre processing
        complexExample.applyToOsmPreProcessing(attrs, layerName, geometryType);

        // Make a copy of the input attributes so we can remove them as they get translated. Looking at what
        // isn't used in the translation - this should end up empty.
        // not in v8 yet: // var tTags = Object.assign({},tags);
        var notUsedAttrs = (JSON.parse(JSON.stringify(attrs)));

        // apply the simple number and text biased rules
        // NOTE: We are not using the intList paramater for applySimpleNumBiased when going to OSM.
        translate.applySimpleNumBiased(notUsedAttrs, tags, complexExample.rules.numBiased, 'forward',[]);
        translate.applySimpleTxtBiased(notUsedAttrs, tags, complexExample.rules.txtBiased, 'forward');

        // one 2 one
        translate.applyOne2One(notUsedAttrs, tags, complexExample.lookup, {'k':'v'});

        // post processing
        complexExample.applyToOsmPostProcessing(attrs, tags, layerName, geometryType);

        // Debug
        // for (var i in notUsedAttrs) print('NotUsed: ' + i + ': :' + notUsedAttrs[i] + ':');

        // Debug:
        if (config.getOgrDebugDumptags() == 'true')
        {
            var kList = Object.keys(tags).sort()
            for (var i = 0, fLen = kList.length; i < fLen; i++) print('Out Tags: ' + kList[i] + ': :' + tags[kList[i]] + ':');
            print('');
        }

        return tags;
    }, // End of toOsm


    // This gets called by translateToOGR and is where the main work gets done
    // We get Tags and return Attrs and a tableName
    // This is the main routine to convert _TO_ NFDD
    toOgr : function(tags, elementType, geometryType)
    {
        var tableName = ''; // The final table name
        var returnData = []; // The array of features to return
        attrs = {}; // The output attributes
        attrs.F_CODE = ''; // Initial setup

        // Check if we have a schema. This is a quick way to workout if various lookup tables have been built
        if (complexExample.rawSchema == undefined)
        {
            var tmp_schema = complexExample.getDbSchema();
        }

        // Start processing here
        // Debug:
        if (config.getOgrDebugDumptags() == 'true')
        {
            print('In Geometry: ' + geometryType + '  In Element Type: ' + elementType);
            var kList = Object.keys(tags).sort()
            for (var i = 0, fLen = kList.length; i < fLen; i++) print('In Tags: ' + kList[i] + ': :' + tags[kList[i]] + ':');
        }

        // The Nuke Option: If we have a relation, drop the feature and carry on
        if (tags['building:part']) return null;

        // The Nuke Option: "Collections" are groups of different feature types: Point, Area and Line.  
        // There is no way we can translate these to a single TDS feature.
        if (geometryType == 'Collection') return null;

        // Set up the fcode translation rules. We need this due to clashes between the one2one and
        // the fcode one2one rules
        if (complexExample.fcodeLookup == undefined)
        {
            // Add the FCODE rules for Export
            fcodeCommon.one2one.push.apply(fcodeCommon.one2one,complexExample.rules.fcodeOne2oneOut);

            complexExample.fcodeLookup = translate.createBackwardsLookup(fcodeCommon.one2one);
            // translate.dumpOne2OneLookup(complexExample.fcodeLookup);
        }

        if (complexExample.lookup == undefined)
        {
            // Add "other" rules to the one2one
            complexExample.rules.one2one.push.apply(complexExample.rules.one2one,complexExample.rules.one2oneOut);

            complexExample.lookup = translate.createBackwardsLookup(complexExample.rules.one2one);
            // translate.dumpOne2OneLookup(complexExample.lookup);
            
            // Make the fuzzy lookup table
            complexExample.fuzzy = schemaTools.generateToOgrTable(complexExample.rules.fuzzyTable);

            // Debug
//             for (var k1 in complexExample.fuzzy)
//             {
//                 for (var v1 in complexExample.fuzzy[k1])
//                 {
//                     print(JSON.stringify([k1, v1, complexExample.fuzzy[k1][v1][0], complexExample.fuzzy[k1][v1][1], complexExample.fuzzy[k1][v1][2]]));
//                 }
//             }
        } // End complexExample.lookup Undefined

        // Pre Processing
        complexExample.applyToNfddPreProcessing(tags, attrs, geometryType);

        // Make a copy of the input tags so we can remove them as they get translated. What is left is
        // the not used tags.
        // not in v8 yet: // var tTags = Object.assign({},tags);
        var notUsedTags = (JSON.parse(JSON.stringify(tags)));

        if (notUsedTags.hoot) delete notUsedTags.hoot; // Added by the UI

        // Apply the simple number and text biased rules
        // NOTE: These are BACKWARD, not forward!
        // NOTE: These delete tags as they are used
        translate.applySimpleNumBiased(attrs, notUsedTags, complexExample.rules.numBiased, 'backward',complexExample.rules.intList);
        translate.applySimpleTxtBiased(attrs, notUsedTags, complexExample.rules.txtBiased, 'backward');

        // Apply the fuzzy rules
        // NOTE: This deletes tags as they are used
        translate.applyOne2OneQuiet(notUsedTags, attrs, complexExample.fuzzy);

        // one 2 one: we call the version that knows about the OTH field
        // NOTE: This deletes tags as they are used
        translate.applyNfddOne2One(notUsedTags, attrs, complexExample.lookup, complexExample.fcodeLookup);

        // Post Processing.
        // We send the original list of tags and the list of tags we haven't used yet.
        // complexExample.applyToNfddPostProcessing(tags, attrs, geometryType);
        complexExample.applyToNfddPostProcessing(tags, attrs, geometryType, notUsedTags);

        var gFcode = geometryType.toString().charAt(0) + attrs.F_CODE;

        returnData.push({attrs: attrs, tableName: layerNameLookup[gFcode.toUpperCase()});

        // Debug:
        if (config.getOgrDebugDumptags() == 'true')
        {
            for (var i = 0, fLen = returnData.length; i < fLen; i++)
            {
                print('TableName ' + i + ': ' + returnData[i]['tableName'] + '  FCode: ' + returnData[i]['attrs']['F_CODE'] + '  Geom: ' + geometryType);
                //for (var j in returnData[i]['attrs']) print('Out Attrs:' + j + ': :' + returnData[i]['attrs'][j] + ':');
                var kList = Object.keys(returnData[i]['attrs']).sort()
                for (var j = 0, kLen = kList.length; j < kLen; j++) print('Out Attrs:' + kList[j] + ': :' + returnData[i]['attrs'][kList[j]] + ':');
            }
            print('');
        }

        return returnData;

    } // End of toNfdd

} // End of complexExample
