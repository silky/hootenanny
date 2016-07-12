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
    complexExample One2one rules
*/

complexExample.rules = {
    // ##### Start of One2One Rules #####

    // One2one rules for Text Fields
    txtBiased : {
     'ADR':'addr:full', // Address
     'RIN_RTN':'ref', // Route Identification <route designation>
     'UFI':'uuid', // Unique Feature Identifier. This gets cleaned up in post processing
   }, // End txtBiased

    // One2one ruled for Number fields
    numBiased : {
     'ARA':'feature_area', // Area - this has been moved from 'area' due to hoot conflicts
     'BNF':'building:levels', // Floor Count
     'HGT':'height', // Height Above Surface Level
     'LTN':'lanes', // Track or Lane Count
     'LZN':'length', // Length
     'SPM':'maxspeed', // Speed Limit (KPH)
     'WID':'width', // Width
     }, // End numBiased
    

    // Common one2one rules. Used for both import and export
    // taken from TDSv60 with the NGA TDSv61 additions
    one2one : [
     // BOT - Bridge Opening Type
     // ['BOT','-999999',undefined,undefined], // Null
     ['BOT','4','bridge:movable','bascule'],
     ['BOT','10','bridge:movable','swing'],
     ['BOT','11','bridge:movable','lift'],
     ['BOT','12','bridge:movable','retractable'],
     ['BOT','14','bridge:movable','submersible'],
     ['BOT','15','bridge:movable','drawbridge'],
     ['BOT','16','bridge:movable','opening'],
     ['BOT','17','bridge:movable','no'],
     ['BOT','999','bridge:movable','other'],

     // FFN - Feature Function
     // ['FFN','-999999',undefined,undefined], // No Information
     ['FFN','2','use','agriculture'], // Agriculture
     ['FFN','3','use','growing_crops'], // Growing of Crops
     ['FFN','9','use','raising_animals'], // Raising of Animals
     ['FFN','14','use','grazing'], // Grazing
     ['FFN','15','use','mixed_farming'], // Mixed Farming
     ['FFN','16','camp:type','migrant_labour'], // Migrant Labour
     ['FFN','20','camp:type','forestry'], // Forestry and/or Logging
     ['FFN','21','landuse','silviculture'], // Silviculture
     ['FFN','27','amenity','forest_warden'], // Forest Warden
     ['FFN','30','leisure','fishing'], // Fishing
     ['FFN','35','use','aquaculture'], // Aquaculture
     ['FFN','40','use','mining_and_quarrying'], // Mining and Quarrying
     ['FFN','50','use','solid_mineral_fuel_mining'], // Solid Mineral Fuel Mining
     ['FFN','60','use','petroleum_andor_gas_extraction'], // Petroleum and/or Gas Extraction
     ['FFN','70','use','metal_ore_mining'], // Metal Ore Mining
     ['FFN','83','use','chemical_mining'], // Chemical Mining
     ['FFN','84','use','peat_mining'], // Peat Extraction
     ['FFN','85','use','salt_mining'], // Salt Extraction
     ['FFN','87','use','mineral_mining'], // Mineral Mining
     ['FFN','91','use','gas_oil_separation'], // Gas Oil Separation
     ['FFN','95','use','ore_dressing'], // Ore Dressing
     ['FFN','99','use','manufacturing'], // Manufacturing
     ['FFN','100','use','food_manufacturing'], // Food Product Manufacture
     ['FFN','101','use','food_processing'], // Food Processing
     ['FFN','102','use','meat_processing'], // Meat Processing
     ['FFN','103','use','seafood_processing'], // Seafood Processing
     ['FFN','104','use','fruit_vegetable_processing'], // Fruit and/or Vegetable Processing
     ['FFN','105','use','oil-mill'], // Oil-mill
     ['FFN','106','crop','dairy'], // Dairying
     ['FFN','107','use','grain-milling'], // Grain Milling
     ['FFN','110','use','baking'], // Baking
     ['FFN','111','use','sugar_manufacturing'], // Sugar Manufacture
     ['FFN','112','use','sugar_milling'], // Sugar Milling
     ['FFN','113','use','sugar_refining'], // Sugar Refining
     ['FFN','114','use','confectionry_manufacturing'], // Confection Manufacture
     ['FFN','115','use','pasta_manufacturing'], // Pasta Manufacture
     ['FFN','116','use','prepared_meals_manufacturing'], // Prepared Meal Manufacture
     ['FFN','118','use','beverage_manufacturing'], // Beverage Manufacture
     ['FFN','119','use','animal_feed_manufacturing'], // Animal Feed Manufacture
     ['FFN','120','use','ice_manufacturing'], // Ice Manufacture
     ['FFN','121','use','distillery'], // Spirit Distillery
     ['FFN','122','use','winery'], // Winery
     ['FFN','123','use','brewing'], // Brewing
     ['FFN','124','use','soft_drink_manufacturing'], // Soft Drink Manufacture
     ['FFN','125','use','tobacco_product_manufacturing'], // Tobacco Product Manufacture
     ['FFN','129','use','textile_apparel_and_leather_manufacturing'], // Textile, Apparel and Leather Manufacture
     ['FFN','130','use','textile_manufacturing'], // Textile Manufacture
     ['FFN','140','use','apparel_manufacturing'], // Apparel Manufacture
     ['FFN','150','use','leather_product_manufacturing'], // Leather Product Manufacture
     ['FFN','155','use','footwear_manufacturing'], // Footwear Manufacturing
     ['FFN','160','use','wood_product_manufacturing'], // Wood-based Manufacturing
     ['FFN','161','craft','sawmilling'], // Sawmilling
     ['FFN','165','use','wooden_construction_product_manufacturing'], // Wooden Construction Product Manufacture
     ['FFN','171','use','paper_mill'], // Paper-mill
     ['FFN','181','use','printing'], // Printing
     ['FFN','190','use','petroleum_and_coal_product_manufacturing'], // Petroleum and Coal Products Manufacturing
     ['FFN','191','use','coke_manufacturing'], // Coke Manufacture
     ['FFN','192','use','petroleum_refining'], // Petroleum Refining
     ['FFN','195','use','chemical_manufacturing'], // Chemical Manufacture
     ['FFN','210','use','medicinal_manufacturing'], // Medicinal Product Manufacture
     ['FFN','221','use','rubber_product_manufacturing'], // Rubber Product Manufacture
     ['FFN','225','use','plastic_product_manufacturing'], // Plastic Product Manufacture
     ['FFN','230','use','nonmetallic_mineral_product_manufacturing'], // Nonmetallic Mineral Product Manufacturing
     ['FFN','231','use','glass_product_manufacturing'], // Glass Product Manufacture
     ['FFN','232','use','refractory_product_manufacturing'], // Refractory Product Manufacture
     ['FFN','233','use','clay_product_manufacturing'], // Clay Product Manufacture
     ['FFN','234','use','ceramic_product_manufacturing'], // Ceramic Product Manufacture
     ['FFN','235','use','cement_mill'], // Cement Mill
     ['FFN','236','use','cement_product_manufacturing'], // Cement Product Manufacture
     ['FFN','237','use','stone_product_manufacturing'], // Stone Product Manufacture
     ['FFN','240','use','metal_manufacturing'], // Primary Metal Manufacturing
     ['FFN','241','use','steel_mill'], // Steel Mill
     ['FFN','242','use','metal_refining'], // Metal Refining
     ['FFN','243','use','foundry'], // Foundry
     ['FFN','250','use','metal_product_manufacturing'], // Metal Product Manufacturing
     ['FFN','251','use','structural_metal_product_manufacturing'], // Structural Metal Product Manufacture
     ['FFN','255','use','munitions_manufacturing'], // Munitions Manufacture
     ['FFN','257','use','fabricated_metal_product_manufacturing'], // Fabricated Metal Product Manufacture
     ['FFN','260','use','electronic_equipment_manufacturing'], // Electronic Equipment Manufacture
     ['FFN','270','use','electrical_equipment_manufacturing'], // Electrical Equipment Manufacture
     ['FFN','272','shop','gas'], // Propane Sale
     ['FFN','280','use','machinery_manufacturing'], // Machinery Manufacture
     ['FFN','289','use','transportation_equipment_manufacturing'], // Transportation Equipment Manufacturing
     ['FFN','290','use','motor_vehicle_manufacturing'], // Motor Vehicle Manufacture
     ['FFN','301','use','ship_manufacturing'], // Ship Construction
     ['FFN','304','use','railway_vehicle_manufacturing'], // Railway Vehicle Manufacture
     ['FFN','305','use','aircraft_manufacturing'], // Aircraft Manufacture
     ['FFN','306','use','military_vehicle_manufacturing'], // Military Vehicle Manufacture
     ['FFN','310','use','furniture_manufacturing'], // Furniture Manufacture
     ['FFN','320','use','miscellaneous_manufacturing'], // Miscellaneous Manufacturing
     ['FFN','321','use','jewellery_manufacturing'], // Jewellery Manufacture
     ['FFN','322','use','musical_instrument_manufacturing'], // Musical Instrument Manufacture
     ['FFN','323','use','sports_goods_manufacturing'], // Sports Goods Manufacture
     ['FFN','324','use','game_and_toy_manufacturing'], // Game and/or Toy Manufacture
     ['FFN','325','use','medical_equipment_manufacturing'], // Medical and/or Dental Equipment Manufacture
     ['FFN','330','repair','yes'], // General Repair
     ['FFN','331','repair','fabricated_metal_products'], // Fabricated Metal Product Repair
     ['FFN','332','repair','electronic_equipment'], // Electronic Equipment Repair
     ['FFN','333','repair','electrical_equipment'], // Electrical Equipment Repair
     ['FFN','334','repair','machinery'], // Machinery Repair
     ['FFN','340','repair','ship'], // Ship Repair
     ['FFN','341','repair','aircraft'], // Aircraft Repair
     ['FFN','342','repair','railway_vehicle'], // Railway Vehicle Repair
     ['FFN','343','shop','car_repair'], // Motor Vehicle Repair
     ['FFN','350','use','utilities'], // Utilities
     ['FFN','351','use','power_generation'], // Power Generation
     ['FFN','352','use','climate_control'], // Climate Control
     ['FFN','355','use','cooling'], // Cooling
     ['FFN','356','use','heating'], // Heating
     ['FFN','360','use','water_supply'], // Water Supply
     ['FFN','361','use','water_collection'], // Water Collection
     ['FFN','362','use','water_treatment'], // Water Treatment
     ['FFN','363','use','water_distribution'], // Water Distribution
     ['FFN','370','use','sewerage'], // Sewerage
     ['FFN','372','use','sewerage_screening'], // Sewerage Screening
     ['FFN','382','amenity','toilets'], // Restroom
     ['FFN','383','use','waste_treatment_and_disposal'], // Waste Treatment and Disposal
     ['FFN','385','use','materials_recovery'], // Materials Recovery
     ['FFN','440','use','commercial'], // Commerce
     ['FFN','459','shop','wholesale'], // Wholesale Merchant
     ['FFN','460','shop','yes'], // Retail Sale
     ['FFN','464','shop','specialized'], // Specialized Store
     ['FFN','465','shop','non-specialized'], // Non-specialized Store
     ['FFN','466','shop','convenience'], // Convenience Store
     ['FFN','470','amenity','fuel'], // Petrol Sale -- links to AQ170 Motor Vehicle Station
     ['FFN','473','amenity','sales_yard'], // Sales Yard
     ['FFN','474','shop','precious_metal'], // Precious Metal Merchant
     ['FFN','475','amenity','marketplace'], // Market
     ['FFN','476','shop','supermarket'], // Grocery
     ['FFN','477','amenity','pharmacy'], // Pharmacy
     ['FFN','478','shop','pet'], // Pet-shop
     ['FFN','480','use','transportation'], // Transport
     ['FFN','481','use','terminal'], // Terminal
     ['FFN','482','public_transport','station'], // Station
     ['FFN','483','amenity','stop'], // Stop
     ['FFN','484','amenity','transfer_hub'], // Transfer Hub
     ['FFN','486','use','signalling'], // Signalling
     ['FFN','487','use','transport_system_maintenance'], // Transport System Maintenance
     ['FFN','488','use','navigation'], // Navigation - Not great, used in a number of FCodes
     ['FFN','489','use','transportation_hub'], // Transportation Hub
     ['FFN','490','use','railway_transport'], // Railway Transport
     ['FFN','491','use','railway_passenger_transport'], // Railway Passenger Transport
     ['FFN','494','use','pedestrian_transport'], // Pedestrian Transport
     ['FFN','495','use','road_transport'], // Road Transport
     ['FFN','496','use','road_passenger_transport'], // Road Passenger Transport
     ['FFN','497','use','road_freight_transport'], // Road Freight Transport
     ['FFN','500','use','pipeline_transport'], // Pipeline Transport
     ['FFN','501','use','pumping'], // Pumping
     ['FFN','505','use','water_transport'], // Water Transport
     ['FFN','507','use','inland_waters_transport'], // Inland Waters Transport
     ['FFN','508','use','canal_transport'], // Canal Transport
     ['FFN','510','use','port_control'], // Port Control
     ['FFN','511','use','maritime_pilotage'], // Maritime Pilotage
     ['FFN','512','use','pilot_station'], // Pilot Station
     ['FFN','513','use','harbour_control'], // Harbour Control
     ['FFN','520','use','air_transport'], // Air Transport
     ['FFN','525','use','air_traffic_control'], // Air Traffic Control
     ['FFN','529','use','transportation_support'], // Transportation Support
     ['FFN','530','use','warehouse'], // Warehousing and Storage
     ['FFN','535','use','garage'], // Motor Vehicle Parking
     ['FFN','536','use','cargo_handling'], // Cargo Handling
     ['FFN','537','amenity','customs'], // Customs Checkpoint
     ['FFN','538','use','inspection_station'], // Inspection Station
     ['FFN','539','use','inspection'], // Inspection
     ['FFN','540','amenity','post_office'], // Postal Activities
     ['FFN','541','use','mail_and_package_transport'], // Mail and Package Transport
     ['FFN','545','use','courier_activities'], // Courier Activities
     ['FFN','548','use','short-term_accommodation'], // Short-term Accommodation
     ['FFN','550','use','accommodation'], // Accommodation
     ['FFN','551','tourism','hotel'], // Hotel
     ['FFN','552','tourism','resort_hotel'], // Resort Hotel
     ['FFN','553','tourism','motel'], // Motel
     ['FFN','554','tourism','guest_house'], // Guest-house
     ['FFN','555','tourism','hostel'], // Hostel
     ['FFN','556','use','dormitory'], // Dormitory
     ['FFN','557','use','vacation_cottage'], // Vacation Cottage
     ['FFN','558','use','dependents_housing'], // Dependents Housing
     ['FFN','559','amenity','public_bath'], // Communal Bath
     ['FFN','563','use','residential'], // Residence
     ['FFN','570','use','food_service'], // Food Service
     ['FFN','572','amenity','restaurant'], // Restaurant
     ['FFN','573','amenity','bar'], // Bar
     ['FFN','574','amenity','dining_hall'], // Dining Hall
     ['FFN','578','amenity','banquet_hall'], // Banquet Hall
     ['FFN','579','amenity','convention_centre'], // Convention Centre
     ['FFN','580','use','publishing_and_broadcasting'], // Publishing and Broadcasting
     ['FFN','582','use','print_publishing'], // Print Publishing
     ['FFN','594','amenity','cinema'], // Cinema
     ['FFN','601','use','radio_broadcasting'], // Radio Broadcasting
     ['FFN','604','use','television_broadcasting'], // Television Broadcasting
     ['FFN','610','use','communication'], // Telecommunications
     ['FFN','612','use','retail_telecommunications'], // Retail Telecommunications
     ['FFN','614','use','wired_telecommunications'], // Wired Telecommunications
     ['FFN','615','use','main_telephone_exchange'], // Main Telephone Exchange
     ['FFN','616','use','branch_telephone_exchange'], // Branch Telephone Exchange
     ['FFN','617','use','wired_repeater'], // Wired Repeater
     ['FFN','620','use','wireless_telecommunications'], // Wireless Telecommunications
     ['FFN','621','use','mobile_phone_service'], // Mobile Phone Service
     ['FFN','622','use','wireless_repeater'], // Wireless Repeater
     ['FFN','625','use','satellite_telecommunications'], // Satellite Telecommunications
     ['FFN','626','man_made','ground_station'], // Satellite Ground Control
     ['FFN','632','amenity','information_service'], // Information Service
     ['FFN','633','amenity','public_records'], // Public Records
     ['FFN','640','office','financial_services'], // Financial Services
     ['FFN','642','office','central_banking'], // Central Banking
     ['FFN','643','amenity','bank'], // Retail Banking
     ['FFN','651','office','insurance'], // Insurance
     ['FFN','662','office','financial_market_administration'], // Financial Market Administration
     ['FFN','663','office','security_brokerage'], // Security Brokerage
     ['FFN','671','office','fund_management'], // Fund Management
     ['FFN','680','office','estate_agent'], // Real Estate Activities
     ['FFN','681','office','professional_scientific_and_technical'], // Professional, Scientific and Technical
     ['FFN','691','office','legal_activities'], // Legal Activities
     ['FFN','696','office','accounting'], // Accounting
     ['FFN','701','office','head_office'], // Head Office
     ['FFN','706','office','business_management'], // Business Management
     ['FFN','711','office','architecture_consulting'], // Architecture Consulting
     ['FFN','714','office','engineering_design'], // Engineering Design
     ['FFN','717','office','surveying'], // Surveying
     ['FFN','720','use','research'], // Scientific Research and Development
     ['FFN','721','use','observation_station'], // Observation Station
     ['FFN','722','monitoring:weather','yes'], // Weather Station
     ['FFN','725','use','nuclear_research_centre'], // Nuclear Research Centre
     ['FFN','730','use','wind_tunnel'], // Wind Tunnel
     ['FFN','741','amenity','advertising'], // Advertising
     ['FFN','752','shop','photo'], // Photography
     ['FFN','757','amenity','veterinary'], // Veterinary
     ['FFN','760','office','business_support'], // Business and Personal Support Services
     ['FFN','761','amenity','car_rental'], // Motor Vehicle Rental
     ['FFN','770','office','employment_agency'], // Employment Agency
     ['FFN','775','shop','travel_agency'], // Travel Agency
     ['FFN','780','use','security_enforcement'], // Security Enforcement
     ['FFN','781','use','guard'], // Guard
     ['FFN','791','amenity','custodial_service'], // Custodial Service
     ['FFN','795','amenity','landscaping_service'], // Landscaping Service
     ['FFN','801','office','office_administration'], // Office Administration
     ['FFN','807','use','call_centre'], // Call Centre
     ['FFN','808','office','public_administration'], // Public Administration
     ['FFN','809','use','headquarters'], // Headquarters
     ['FFN','810','office','administrative'], // Administration
     ['FFN','811','office','government'], // Government
     ['FFN','812','office','local_government'], // Local Government
     ['FFN','813','office','subnational_government'], // Subnational Government
     ['FFN','814','office','national_government'], // National Government
     ['FFN','815','use','palace'], // Palace
     ['FFN','817','use','capitol'], // Capitol
     ['FFN','818','office','executive_activities'], // Executive Activities
     ['FFN','819','office','legislative_activities'], // Legislative Activities
     ['FFN','821','use','polling_station'], // Polling Station
     ['FFN','822','use','civil_activities'], // Civil Activities
     ['FFN','825','amenity','diplomacy'], // Diplomacy
     ['FFN','826','diplomatic','mission'], // Diplomatic Mission
     ['FFN','827','diplomatic','embassy'], // Embassy
     ['FFN','828','diplomatic','consulate'], // Consul
     ['FFN','829','use','maritime_defense'], // Maritime Defense
     ['FFN','830','use','public_order_safety_and_security_services'], // Public Order, Safety and Security Services
     ['FFN','831','use','public_order'], // Public Order
     ['FFN','832','use','safety'], // Safety
     ['FFN','833','use','security_services'], // Security Services
     ['FFN','835','use','military'], // Defence Activities
     ['FFN','836','military','armory'], // Armory
     ['FFN','837','military','reserve_activities'], // Military Reserve Activities
     ['FFN','838','military','recruitment'], // Military Recruitment
     ['FFN','839','military','cbrne_civilian_support'], // CBRNE Civilian Support
     ['FFN','840','amenity','judicial_activities'], // Judicial Activities
     ['FFN','841','amenity','police'], // Law Enforcement
     ['FFN','842','amenity','immigration_control'], // Immigration Control
     ['FFN','843','amenity','prison'], // Imprisonment
     ['FFN','844','amenity','juvenile_corrections'], // Juvenile Corrections
     ['FFN','845','amenity','fire_station'], // Firefighting
     ['FFN','846','amenity','rescue_and_paramedical_services'], // Rescue and Paramedical Services
     ['FFN','847','use','emergency_operations'], // Emergency Operations
     ['FFN','848','use','civil_intelligence'], // Civil Intelligence
     ['FFN','850','use','education'], // Education
     ['FFN','851','isced:level','1'], // Primary Education
     ['FFN','852','isced:level','2,3'], // Secondary Education
     ['FFN','855','building','university'], // Higher Education
     ['FFN','857','use','vocational_education'], // Vocational Education
     ['FFN','860','use','healthcare'], // Human Health Activities
     ['FFN','861','use','in-patient_care'], // In-patient Care
     ['FFN','862','use','out-patient_care'], // Out-patient Care
     ['FFN','863','use','urgent_medical_care'], // Urgent Medical Care
     ['FFN','864','use','human_tissue_repository'], // Human Tissue Repository
     ['FFN','865','use','public_health_activities'], // Public Health Activities
     ['FFN','866','use','leprosy_care'], // Leprosy Care
     ['FFN','871','use','intermediate_care'], // Intermediate Care
     ['FFN','873','use','psychiatric_in-patient_care'], // Psychiatric In-patient Care
     ['FFN','875','use','residential_care'], // Residential Care
     ['FFN','881','social_facility','emergency'], // Emergency Shelter
     ['FFN','882','social_facility','homeless'], // Homeless Shelter
     ['FFN','883','social_facility','refugee'], // Refugee Shelter
     ['FFN','884','social_facility','youth_emergency'], // Emergency Youth Shelter
     ['FFN','885','amenity','child_care'], // Day Care
     ['FFN','887','use','social_work'], // Social Work
     ['FFN','888','use','emergency_relief_services'], // Emergency Relief Services
     ['FFN','890','amenity','cultural_centre'], // Cultural, Arts and Entertainment
     ['FFN','891','amenity','theatre'], // Theatre
     ['FFN','892','amenity','auditorium'], // Auditorium
     ['FFN','893','amenity','community_centre'], // Community Centre
     ['FFN','894','tourism','opera_house'], // Opera House
     ['FFN','895','amenity','nightclub'], // Night Club
     ['FFN','900','use','sports_amusement_and_recreation'], // Sports, Amusement and Recreation
     ['FFN','902','amenity','library'], // Library
     ['FFN','905','tourism','museum'], // Museum
     ['FFN','906','tourism','aquarium'], // Aquarium
     ['FFN','907','use','botanical_and/or_zoological_reserve_activities'], // Botanical and/or Zoological Reserve Activities
     ['FFN','909','leisure','gambling'], // Gambling
     ['FFN','912','use','sports_centre'], // Sports Centre
     ['FFN','913','leisure','fitness_centre'], // Fitness Centre
     ['FFN','914','sport','shooting_range'], // Shooting Range
     ['FFN','919','use','animal_boarding'], // Animal Boarding
     ['FFN','921','use','recreation'], // Recreation
     ['FFN','922','use','amusement'], // Amusement
     ['FFN','923','leisure','hobbies_andor_leisure_activities'], // Hobbies and/or Leisure Activities
     ['FFN','930','use','religious_activities'], // Religious Activities
     ['FFN','931','amenity','place_of_worship'], // Place of Worship
     ['FFN','932','use','islamic_prayer_hall'], // Islamic Prayer Hall
     ['FFN','950','amenity','membership_organization'], // Membership Organization
     ['FFN','954','amenity','club'], // Club
     ['FFN','955','leisure','yacht_club'], // Yacht-club
     ['FFN','961','shop','laundry'], // Laundry
     ['FFN','962','shop','beauty'], // Beauty Treatment
     ['FFN','963','shop','funeral_directors'], // Funeral Services
     ['FFN','964','amenity','crematorium'], // Cremation
     ['FFN','965','amenity','mortuary'], // Mortuary Services
     ['FFN','966','amenity','adult_entertainment'], // Adult Entertainment
     ['FFN','967','amenity','storage_of_human_remains'], // Storage of Human Remains
     ['FFN','970','use','meeting_place'], // Meeting Place
     ['FFN','980','amenity','death_care_services'], // Death Care Services
     ['FFN','999','use','other'], // Other

     // MES - Median Present
     // ['MES','-999999',undefined,undefined], // No Information
     ['MES','1000','median','no'],
     ['MES','1001','median','yes'],

     // ONE - One-way
     // ['ONE','-999999',undefined,undefined], // No Information
     ['ONE','1000','oneway','no'], // Value from OSM
     ['ONE','1001','oneway','yes'],

     // RIN_ROI - Route Designation (route designation type)
     // ['RIN_ROI','-999999',undefined,undefined], // No Information
     ['RIN_ROI','1','ref:road:class','international'], // International
     ['RIN_ROI','2','ref:road:class','national_motorway'], // National Motorway
     ['RIN_ROI','3','ref:road:class','primary'], // National
     ['RIN_ROI','4','ref:road:class','secondary'], // Secondary
     ['RIN_ROI','5','ref:road:class','local'], // Local
     ['RIN_ROI','999','ref:road:class','other'], // Other

     // These give the basic highway types. Post processing fixes them
     // RTY - Roadway Type
     // ['RTY','-999999',undefined,undefined], // No Information
     ['RTY','1','ref:road:type','motorway'], // Motorway
     ['RTY','2','ref:road:type','limited_access_motorway'], // Limited Access Motorway
     ['RTY','3','ref:road:type','road'], // Road - Def: Roads outside a BUA
     ['RTY','4','ref:road:type','street'], // Street - Def: Roads inside a BUA
     ['RTY','999','ref:road:type','other'], // Other


     // PCF - Physical Condition
     // ['PCF','-999999',undefined,undefined], // No Information
     ['PCF','1','condition','construction'], // Construction
     ['PCF','2','condition','functional'], // Intact in spec, using for MGCP compatibility
     ['PCF','3','condition','abandoned'], // Unmaintained in spec
     ['PCF','4','condition','damaged'], // Damaged
     ['PCF','5','condition','dismantled'], // Dismantled
     ['PCF','6','condition','destroyed'], // Destroyed

     // Some of these have been added to the fuzzy table for export
     // ZI016_ROC - Route Pavement Information : Route Surface Composition
     // ['ZI016_ROC','-999999','surface','unknown'], // Trying this instead of undefined
     ['ZI016_ROC','-999999',undefined,undefined], // No Information
//      ['ZI016_ROC','1','surface','ground'], // Unimproved
//      ['ZI016_ROC','2','surface','compacted'], // Stabilized earth
//      ['ZI016_ROC','3','surface','flexible_pavement'], // Flexible Pavement
//      ['ZI016_ROC','4','surface','gravel'], // Aggregate
//      ['ZI016_ROC','5','surface','macadam'], // Macadam
//      ['ZI016_ROC','6','surface','bound_surface'], // Bound Surface
//      ['ZI016_ROC','7','surface','ridgid_pavement'], // Rigid Pavement
//      ['ZI016_ROC','8','surface','concrete'], // Concrete
//      ['ZI016_ROC','9','surface','asphalt'], // Asphalt
//      ['ZI016_ROC','10','surface','asphalt_over_concrete'], // Asphalt over Concrete
//      ['ZI016_ROC','11','surface','cobblestone'], // Cobble-stone
//      ['ZI016_ROC','12','surface','brick'], // Brick
//      ['ZI016_ROC','13','surface','metal'], // Metal
//      ['ZI016_ROC','14','surface','wood'], // Wood
//      ['ZI016_ROC','15','surface','corduroy'], // rough hewn logs...  // Corduroy
//      ['ZI016_ROC','16','surface','wood_plank'], // Wood Plank
//      ['ZI016_ROC','17','surface','ice'], // Ice
//      ['ZI016_ROC','18','surface','snow'], // Snow
//      ['ZI016_ROC','999','surface','other'], // Other

// ###################################

     ], // End one2one

     // Input Translation Rules:
     // These are only used for IMPORT and get added to the standard one2one rules
     one2oneIn : [

     // ['ZI016_ROC','-999999',undefined,undefined], // No Information
     ['ZI016_ROC','1','surface','ground'], // Unimproved
     ['ZI016_ROC','2','surface','compacted'], // Stabilized earth
     ['ZI016_ROC','3','surface','unpaved'], // Flexible Pavement
     ['ZI016_ROC','4','surface','gravel'], // Aggregate
     ['ZI016_ROC','5','surface','macadam'], // Macadam
     ['ZI016_ROC','6','surface','bound_surface'], // Bound Surface
     ['ZI016_ROC','7','surface','paved'], // Rigid Pavement
     ['ZI016_ROC','8','surface','concrete'], // Concrete
     ['ZI016_ROC','9','surface','asphalt'], // Asphalt
     ['ZI016_ROC','10','surface','asphalt_over_concrete'], // Asphalt over Concrete
     ['ZI016_ROC','11','surface','cobblestone'], // Cobble-stone
     ['ZI016_ROC','12','surface','brick'], // Brick
     ['ZI016_ROC','13','surface','metal'], // Metal
     ['ZI016_ROC','14','surface','wood'], // Wood
     ['ZI016_ROC','15','surface','corduroy'], // rough hewn logs...  // Corduroy
     ['ZI016_ROC','16','surface','wood_plank'], // Wood Plank
     ['ZI016_ROC','17','surface','ice'], // Ice
     ['ZI016_ROC','18','surface','snow'], // Snow
     ['ZI016_ROC','999','surface','other'], // Other

    ], // End one2oneIn

    // one2oneOut - This is for Export only. The values are swapped before use
    one2oneOut : [
     // OTH Filler.  These are to build OTH values
     ['RTY','999','highway','other'],
     ['FFN','999','amenity','other'], 
     ['FFN','999','building','other'], 
     ['FFN','999','camp:type','other'], 
     ['FFN','999','craft','other'], 
     ['FFN','999','landuse','other'], 
     ['FFN','999','leisure','other'], 
     ['FFN','999','military','other'], 
     ['FFN','999','office','other'], 
     ['FFN','999','repair','other'], 
     ['FFN','999','shop','other'], 
     ['FFN','999','social_facility','other'], 
     ['FFN','999','tourism','other'], 

     // Funky Road Type attributes from TDSv40
     ['RTY','4','ref:road:type','boulevard'], // Boulevard
     ['RTY','4','ref:road:type','avenue'], // Avenue
     ['RTY','4','ref:road:type','drive'], // Drive
     ['RTY','4','ref:road:type','circle'], // Circle
     ['RTY','4','ref:road:type','close'], // Close
     ['RTY','4','ref:road:type','parkway'], // Parkway
     ['RTY','4','ref:road:type','lane'], // Lane
     ['RTY','4','ref:road:type','terrace'], // Terrace
     ['RTY','4','ref:road:type','place'], // Place
     ['RTY','4','ref:roadtype','roundabout'], // Roundabout

     // Fix up some of the Defaults
     [undefined,undefined,'natural','water'], // to get rid of errors

     // ### From OSM - This list could never end.....
     ['FFN','464','shop','books'], // Specialized Store
     ['FFN','563','building','house'], // Residence
     ['FFN','558','building','dependents_housing'], // Dependents Housing
     ['FFN','610','office','telecommunication'], // telecommunication
     ['FFN','640','shop','bureau_de_change'], // Financial Services
     ['FFN','760','amenity','office'], // Business and Personal Support Services 
     ['FFN','815','building','palace'], // Palace

     // Added to give something for the OTH value to use
     ['ZI016_ROC','999','surface','other'], // Other

   ], // End one2oneOut

    // ##### End of One2One Rules #####

   
    // ##### Start of fuzzyTable #####
    // This list uses the new IsA, IsSimilar etc functions to map a number of input values to a single output
    fuzzyTable : [
     ['ZI016_ROC','1',schemaTools.simple('surface=ground',1)], // Unimproved
     ['ZI016_ROC','2',schemaTools.simple('surface=compacted',1)], // Stabilized earth
     ['ZI016_ROC','3',schemaTools.isA('surface=unpaved', 0.8)], // Flexible Pavement
     ['ZI016_ROC','4',schemaTools.simple('surface=gravel',1)], // Aggregate
     ['ZI016_ROC','5',schemaTools.simple('surface=macadam',1)], // Macadam
     ['ZI016_ROC','6',schemaTools.simple('surface=bound_surface',1)], // Bound Surface
     ['ZI016_ROC','7',schemaTools.isA('surface=paved', 0.8)], // Ridgid Pavement
     ['ZI016_ROC','8',schemaTools.simple('surface=concrete',1)], // Concrete
     ['ZI016_ROC','9',schemaTools.simple('surface=asphalt',1)], // Asphalt
     ['ZI016_ROC','10',schemaTools.simple('surface=asphalt_over_concrete',1)], // Asphalt over Concrete
     ['ZI016_ROC','11',"surface=cobblestone",schemaTools.isA('surface=cobblestone',1),schemaTools.simple('surface=sett')], // Cobble-stone
     ['ZI016_ROC','12',schemaTools.simple('surface=brick',1)], // Brick
     ['ZI016_ROC','13',schemaTools.simple('surface=metal',1)], // Metal
     ['ZI016_ROC','14',schemaTools.simple('surface=wood',1)], // Wood
     ['ZI016_ROC','15',schemaTools.simple('surface=corduroy',1)], // rough hewn logs...  // Corduroy
     ['ZI016_ROC','16',schemaTools.simple('surface=wood_plank',1)], // Wood Plank
     ['ZI016_ROC','17',schemaTools.simple('surface=ice',1)], // Ice
     ['ZI016_ROC','18',schemaTools.simple('surface=snow',1)], // Snow
     ['ZI016_ROC','999',schemaTools.simple('surface=other',1)], // Other
    ],
    // ##### End of fuzzyTable #####

    // ##### Start of txtLength #####
    // This list is for validateing the lengths of text attributes prior to export
    txtLength : {
     'RIN_RTN':24, 'UFI':254
    },
    // ##### End of txtLength #####

    // ##### Start of intList #####
    // This list is for validateing the integer attributes prior to export
    intList : [
    'BNF','LTN'
    ],
    // ##### End of intList#####

} // End of tds61.rules
