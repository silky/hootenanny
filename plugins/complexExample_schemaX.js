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
 * @copyright Copyright (C) 2012, 2013 DigitalGlobe (http://www.digitalglobe.com/)
 */

 ////
 // Example schema for the complex template
 ////
 

var _global = (0, eval)('this');
if (!_global.complexExample)
{
  _global.complexExample = {};
}

complexExample.schema = {

getDbSchema: function()
{
    // If you have a list of attributes that is repeated in many features, think about pulling them into an
    // array that can be re-used. Look at the building features for how this is used
    var building_FFN = [
              { name:"No Information", value:"-999999" },
              { name:"Brewing", value:"123" },
              { name:"Munitions Manufacture", value:"255" },
              { name:"Main Telephone Exchange", value:"615" },
              { name:"Hunting", value:"19" },
              { name:"Opera House", value:"894" },
              { name:"Immigration Control", value:"842" },
              { name:"Veterinary", value:"757" },
              { name:"Beverage Manufacture", value:"118" },
              { name:"Grocery", value:"476" }, 
              { name:"Recreation", value:"921" }, 
              { name:"Day Care", value:"885" }
             ];


    var schema = [
        { name:"BRIDGE_P",
          fcode:"AQ040",
          desc:"Bridge",
          geom:"Point",
          columns:[ 
                     { name:"BOT",
                       desc:"Bridge Opening Type" ,
                       optional:"R" ,
                       type:"enumeration",
                       defValue:"-999999", 
                       enumerations:[
                           { name:"No Information", value:"-999999" }, 
                           { name:"Bascule", value:"4" }, 
                           { name:"Swing-bridge", value:"10" }, 
                           { name:"Lift-bridge", value:"11" }, 
                           { name:"Retractable", value:"12" }, 
                           { name:"Submersible", value:"14" }, 
                           { name:"Drawbridge", value:"15" }, 
                           { name:"Opening", value:"16" }, 
                           { name:"Fixed", value:"17" }, 
                           { name:"Other", value:"999" }, 
                        ] // End of Enumerations 
                     }, // End of BOT
                     { name:"F_CODE",
                       desc:"Feature Code" ,
                       optional:"R" ,
                       length:"5",
                       type:"String",
                       defValue:""
                     }, // End of F_CODE
                     { name:"HGT",
                       desc:"Height Above Surface Level" ,
                       optional:"R" ,
                       type:"Real",
                       defValue:"-999999.0" 
                     }, // End of HGT
                     { name:"LZN",
                       desc:"Length" ,
                       optional:"R" ,
                       type:"Real",
                       defValue:"-999999.0" 
                     }, // End of LZN
                     { name:"PCF",
                       desc:"Physical Condition" ,
                       optional:"R" ,
                       type:"enumeration",
                       defValue:"-999999", 
                       enumerations:[
                           { name:"No Information", value:"-999999" }, 
                           { name:"Construction", value:"1" }, 
                           { name:"Intact", value:"2" }, 
                           { name:"Unmaintained", value:"3" }, 
                           { name:"Damaged", value:"4" }, 
                           { name:"Dismantled", value:"5" }, 
                           { name:"Destroyed", value:"6" }, 
                        ] // End of Enumerations 
                     }, // End of PCF
                     { name:"UFI",
                       desc:"Unique Entity Identifier" ,
                       optional:"R" ,
                       length:"254", 
                       type:"String",
                       defValue:"noInformation" 
                     }, // End of UFI
                    ] // End of Columns
          }, // End of feature AQ040

        { name:"BUILDING_P",
          fcode:"AL013",
          desc:"Building",
          geom:"Point",
          columns:[ 
                     { name:"ADR",
                       desc:"Address" ,
                       optional:"R" ,
                       type:"String",
                       defValue:"No Information" 
                     }, // End of ADR
                     { name:"BNF",
                       desc:"Floor Count" ,
                       optional:"R" ,
                       type:"Integer",
                       defValue:"-999999" 
                     }, // End of BNF
                     { name:"F_CODE",
                       desc:"Feature Code" ,
                       optional:"R" ,
                       length:"5",
                       type:"String",
                       defValue:""
                     }, // End of F_CODE
                     { name:"FFN",
                       desc:"Feature Function" ,
                       optional:"R" ,
                       type:"enumeration",
                       defValue:"-999999", 
                       enumerations: building_FFN
                     }, // End of FFN
                     { name:"HGT",
                       desc:"Height Above Surface Level" ,
                       optional:"R" ,
                       type:"Real",
                       defValue:"-999999.0" 
                     }, // End of HGT
                     { name:"UFI",
                       desc:"Unique Entity Identifier" ,
                       optional:"R" ,
                       length:"254", 
                       type:"String",
                       defValue:"noInformation" 
                     }, // End of UFI
                    ] // End of Columns
          }, // End of feature AL013

        { name:"BUILDING_S",
          fcode:"AL013",
          desc:"Building",
          geom:"Area",
          columns:[ 
                     { name:"ADR",
                       desc:"Address" ,
                       optional:"R" ,
                       type:"String",
                       defValue:"No Information" 
                     }, // End of ADR
                     { name:"ARA",
                       desc:"Area" ,
                       optional:"R" ,
                       type:"Real",
                       defValue:"-999999.0" 
                     }, // End of ARA
                     { name:"BNF",
                       desc:"Floor Count" ,
                       optional:"R" ,
                       type:"Integer",
                       defValue:"-999999" 
                     }, // End of BNF
                     { name:"F_CODE",
                       desc:"Feature Code" ,
                       optional:"R" ,
                       length:"5",
                       type:"String",
                       defValue:""
                     }, // End of F_CODE
                     { name:"FFN",
                       desc:"Feature Function" ,
                       optional:"R" ,
                       type:"enumeration",
                       defValue:"-999999", 
                       enumerations: building_FFN
                     }, // End of FFN
                     { name:"HGT",
                       desc:"Height Above Surface Level" ,
                       optional:"R" ,
                       type:"Real",
                       defValue:"-999999.0" 
                     }, // End of HGT
                     { name:"UFI",
                       desc:"Unique Entity Identifier" ,
                       optional:"R" ,
                       length:"254", 
                       type:"String",
                       defValue:"noInformation" 
                     }, // End of UFI

                    ] // End of Columns
          }, // End of feature AL013

        { name:"CART_TRACK_C",
          fcode:"AP010",
          desc:"Cart Track",
          geom:"Line",
          columns:[ 
                     { name:"LZN",
                       desc:"Length" ,
                       optional:"R" ,
                       type:"Real",
                       defValue:"-999999.0" 
                     }, // End of LZN
                     { name:"F_CODE",
                       desc:"Feature Code" ,
                       optional:"R" ,
                       length:"5",
                       type:"String",
                       defValue:""
                     }, // End of F_CODE
                     { name:"UFI",
                       desc:"Unique Entity Identifier" ,
                       optional:"R" ,
                       length:"254", 
                       type:"String",
                       defValue:"noInformation" 
                     }, // End of UFI

                       desc:"Restriction Information : Security Attributes Group <resource owner-producer>" ,
                       optional:"R" ,
                       length:"254", 
                       type:"String",
                       defValue:"noInformation" 
                     } // End of ZSAX_RX4
                    ] // End of Columns
          }, // End of feature AP010

        { name:"ROAD_C",
          fcode:"AP030",
          desc:"Road",
          geom:"Line",
          columns:[ 
                     { name:"F_CODE",
                       desc:"Feature Code" ,
                       optional:"R" ,
                       length:"5",
                       type:"String",
                       defValue:""
                     }, // End of F_CODE
                     { name:"LTN",
                       desc:"Track or Lane Count" ,
                       optional:"R" ,
                       type:"Integer",
                       defValue:"-999999" 
                     }, // End of LTN
                     { name:"LZN",
                       desc:"Length" ,
                       optional:"R" ,
                       type:"Real",
                       defValue:"-999999.0" 
                     }, // End of LZN
                     { name:"MES",
                       desc:"Median Present" ,
                       optional:"R" ,
                       type:"enumeration",
                       defValue:"-999999", 
                       enumerations:[
                           { name:"No Information", value:"-999999" }, 
                           { name:"False", value:"1000" }, 
                           { name:"True", value:"1001" }, 
                        ] // End of Enumerations 
                     }, // End of MES
                     { name:"ONE",
                       desc:"One-way" ,
                       optional:"R" ,
                       type:"enumeration",
                       defValue:"-999999", 
                       enumerations:[
                           { name:"No Information", value:"-999999" }, 
                           { name:"False", value:"1000" }, 
                           { name:"True", value:"1001" }, 
                        ] // End of Enumerations 
                     }, // End of ONE
                     { name:"RIN_ROI",
                       desc:"Route Identification <route designation type>" ,
                       optional:"R" ,
                       type:"enumeration",
                       defValue:"-999999", 
                       enumerations:[
                           { name:"No Information", value:"-999999" }, 
                           { name:"International", value:"1" }, 
                           { name:"National Motorway", value:"2" }, 
                           { name:"National", value:"3" }, 
                           { name:"Secondary", value:"4" }, 
                           { name:"Local", value:"5" }, 
                           { name:"Other", value:"999" }, 
                        ] // End of Enumerations 
                     }, // End of RIN_ROI
                     { name:"RIN_RTN",
                       desc:"Route Identification <route designation>" ,
                       optional:"R" ,
                       length:"24", 
                       type:"String",
                       defValue:"No Information" 
                     }, // End of RIN_RTN
                     { name:"RTY",
                       desc:"Roadway Type" ,
                       optional:"R" ,
                       type:"enumeration",
                       defValue:"-999999", 
                       enumerations:[
                           { name:"No Information", value:"-999999" }, 
                           { name:"Motorway", value:"1" }, 
                           { name:"Limited Access Motorway", value:"2" }, 
                           { name:"Road", value:"3" }, 
                           { name:"Street", value:"4" }, 
                           { name:"Other", value:"999" }, 
                        ] // End of Enumerations 
                     }, // End of RTY
                     { name:"SPM",
                       desc:"Speed Limit (KPH)" ,
                       optional:"R" ,
                       type:"Real",
                       defValue:"-999999.0" 
                     }, // End of SPM
                     { name:"UFI",
                       desc:"Unique Entity Identifier" ,
                       optional:"R" ,
                       length:"254", 
                       type:"String",
                       defValue:"noInformation" 
                     }, // End of UFI
                     { name:"WID",
                       desc:"Width" ,
                       optional:"R" ,
                       type:"Real",
                       defValue:"-999999.0" 
                     }, // End of WID
                     { name:"ZI016_ROC",
                       desc:"Route Pavement Information : Route Surface Composition" ,
                       optional:"R" ,
                       type:"enumeration",
                       defValue:"-999999", 
                       enumerations:[
                           { name:"No Information", value:"-999999" }, 
                           { name:"Unimproved", value:"1" }, 
                           { name:"Stabilized Earth", value:"2" }, 
                           { name:"Flexible Pavement", value:"3" }, 
                           { name:"Aggregate", value:"4" }, 
                           { name:"Macadam", value:"5" }, 
                           { name:"Bound Surface", value:"6" }, 
                           { name:"Rigid Pavement", value:"7" }, 
                           { name:"Concrete", value:"8" }, 
                           { name:"Asphalt", value:"9" }, 
                           { name:"Asphalt over Concrete", value:"10" }, 
                           { name:"Cobble-stone", value:"11" }, 
                           { name:"Brick", value:"12" }, 
                           { name:"Metal", value:"13" }, 
                           { name:"Wood", value:"14" }, 
                           { name:"Corduroy", value:"15" }, 
                           { name:"Wood Plank", value:"16" }, 
                           { name:"Ice", value:"17" }, 
                           { name:"Snow", value:"18" }, 
                           { name:"Other", value:"999" }, 
                        ] // End of Enumerations 
                     }, // End of ZI016_ROC
                    ] // End of Columns
          }, // End of feature AP030

    ]; // End of schema

    return schema; 

} // End of getDbSchema

} // End of complexExample.schema


exports.getDbSchema = complexExample.schema.getDbSchema;

