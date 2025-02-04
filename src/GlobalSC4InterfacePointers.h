///////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-query-ui-hooks, a DLL Plugin for SimCity 4 that
// extends the query UI.
//
// Copyright (c) 2024, 2025 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

class cISC4FlammabilitySimulator;
class cISC4LandValueSimulator;
class cISC4PollutionSimulator;
class cISC4WeatherSimulator;

extern cISC4FlammabilitySimulator* spFlammabilitySimulator;
extern cISC4LandValueSimulator* spLandValueSimulator;
extern cISC4PollutionSimulator* spPollutionSimulator;
extern cISC4WeatherSimulator* spWeatherSimulator;