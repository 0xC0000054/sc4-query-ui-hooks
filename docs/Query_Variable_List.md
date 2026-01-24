# Query Variable List

The query variables come in two forms, one with required arguments and one without.

## Query Variables With Required Arguments

| Name | Arguments | Description |
|------|-----------|-------------|
| budget_purpose_type_cost: | The purpose id as a hexadecimal string. | Gets the budget item cost of the specified purpose id. |

## Query Variables Without Required Arguments

| Name | Description |
|------|-------------|
| building_full_funding_capacity | The cost for Education, Fire, Health, Police and Power buildings at the full (100%) capacity. For Fire and Police stations this is the coverage radius. |
| building_full_funding_coverage | The cost for Education, and Health buildings at the full (100%) coverage radius (School Bus/Ambulance). |
| building_is_w2w | Shows a 'Yes' or 'No' value based on whether the building has a W2W occupant group. |
| building_styles | Shows a building's styles in a pipe-separated list. |
| building_style_lines | Shows a list of the building's styles, with each style after the first one on its own line.<br>E.g:`Chicago 1890`<br>`New York 1940` |
| building_summary | The building summary the game shows in its hover tool tips. E.g: `Low-Wealth Residential` |
| cap_relief | Shows the cap relief types that the building provides in a pipe-separated list. |
| cap_relief_lines | Shows a list of cap relief types that the building provides, with each style after the first one on its own line. |
| crime_effect | A string describing the magnitude and radius of the effect. |
| growth_stage | The growth stage of the building's lot. |
| jobs_low_wealth | The building's current low wealth jobs rounded to the nearest whole number. |
| jobs_medium_wealth | The building's current medium wealth jobs rounded to the nearest whole number. |
| jobs_high_wealth | The building's current high wealth jobs rounded to the nearest whole number. |
| landmark_effect | A string describing the magnitude and radius of the effect. |
| mayor_rating_effect | A string describing the magnitude and radius of the effect. |
| park_effect | A string describing the magnitude and radius of the effect. |
| travel_jobs_low_wealth | The number of low wealth workers that travel to the specified lot. Industrial lots can have one lot providing road access for other industrial lots.  |
| travel_jobs_medium_wealth | The number of medium wealth workers that travel to the specified lot. Industrial lots can have one lot providing road access for other industrial lots. |
| travel_jobs_high_wealth | The number of high wealth workers that travel to the specified lot. Industrial lots can have one lot providing road access for other industrial lots. |
| mysim_name | The name of the MySim that lives in the selected residence. |
| r1_occupancy | The current number of R§ occupants. |
| r1_capacity | The R§ occupant capacity. |
| r2_occupancy | The current number of R§§ occupants. |
| r2_capacity | The R§§ occupant capacity. |
| r3_occupancy | The current number of R§§§ occupants. |
| r3_capacity | The R§§§ occupant capacity. |
| cs1_occupancy | The current number of Cs§ occupants. |
| cs1_capacity | The Cs§ occupant capacity. |
| cs2_occupancy | The current number of Cs§§ occupants. |
| cs2_capacity | The Cs§§ occupant capacity. |
| cs3_occupancy | The current number of Cs§§§ occupants. |
| cs3_capacity | The Cs§§§ occupant capacity. |
| co2_occupancy | The current number of Co§§ occupants. |
| co2_capacity | The Co§§ occupant capacity. |
| co3_occupancy | The current number of Co§§§ occupants. |
| co3_capacity | The Co§§§ occupant capacity. |
| ir_occupancy | The current number of I-R occupants. |
| ir_capacity | The I-R occupant capacity. |
| id_occupancy | The current number of I-D occupants. |
| id_capacity | The I-D occupant capacity. |
| im_occupancy | The current number of I-M occupants. |
| im_capacity | The I-M occupant capacity. |
| iht_occupancy | The current number of I-HT occupants. |
| iht_capacity | The I-HT occupant capacity. |
| water_building_source | The water source for a water producing building. |

Multiple `*_occupancy` and `*_capacity` variables can be combined in a LTEXT query to show the filled/available occupancy.
E.g. `#cs1_occupancy#/#cs1_capacity#`.