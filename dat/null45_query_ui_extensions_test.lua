--#-package:04551551# -- package signature --
-- Any Lua files that depend on this file must have higher package numbers.


-- The following code is used to test the null45_query_ui_extensions.get_property_value
-- function with various building exemplar properties.
-- It also serves as examples of how to use the function.
-- All test functions take no parameters and return a string.

-- Demonstrates getting the exemplar name property, a string value.
function null45_query_ui_extensions_test_get_exemplar_name()
  local exemplar_name = null45_query_ui_extensions.get_property_value('00000020')
  
  if exemplar_name == nil then
    -- Return an empty string if the item does not have an exemplar name property.
    exemplar_name = ''
  end
  
  return exemplar_name
end

-- Demonstrates getting the bulldoze cost property, a single number value.
function null45_query_ui_extensions_test_get_bulldoze_cost()
  local bulldoze_cost = null45_query_ui_extensions.get_property_value(hex2dec('099afacd'))
  
  if bulldoze_cost == nil then
    -- Report a cost of zero if the property does not exist.
    bulldoze_cost = 0
  end
  
  return tostring(bulldoze_cost)
end

-- Demonstrates extracting the garbage pollution value from the
-- 'Pollution at Center' property array.
function null45_query_ui_extensions_test_get_garbage_pollution_at_center()
  local pollution_at_center = null45_query_ui_extensions.get_property_value(hex2dec('27812851'))
  local garbage_value_string = 'None'
  
  if pollution_at_center ~= nil then
    -- If the property is not nil, then it is an array of 4 numbers
	-- in the order: air, water, garbage, radiation.
	-- See the property description in Ingred.ini
	-- Lua arrays start at 1, so we pick item 3.
    garbage_value_string = tostring(pollution_at_center[3])
  end
  
  return garbage_value_string
end