#pragma once

#include "VesselObject.h"
#include "VesselCV.h"
#include "VesselBB.h"
#include "VesselCG.h"
#include "VesselDD.h"
#include "VesselLV.h"
#include "VesselAH.h"
#include "Team.h"
#include "Shell.h"
#include "Torpedo.h"
#include "GeographicSystem.h"
#include "Reef.h"
#include "Flatland.h"
#include "Mountain.h"
#include "HydrometeorologicSystem.h"
#include "Lightning.h"
#include "Typhoon.h"
#include "Fog.h"

#define DEBUG
#ifdef DEBUG
using namespace System::Diagnostics;
#endif // DEBUG
using namespace System;

namespace CommandAnalyze {

	// SET, FIRE, DEFENSE, TAG, MOVE, LASER, ENRICH, LAUNCH
	enum CommandsType {

		SET_SUCCESS, SET_FAIL,			// set		�s�W
		FIRE_SUCCESS, FIRE_FAIL,		// fire		����
		DEFENSE_SUCCESS, DEFENSE_FAIL,	// defense	���m
		TAG_SUCCESS, TAG_FAIL,			// tag		��W
		MOVE_SUCCESS, MOVE_FAIL,		// move		����
		LASER_SUCCESS, LASER_FAIL,		// laser	�p�g
		ENRICH_SUCCESS, ENRICH_FAIL,	// enrich	����
		LAUNCH_SUCCESS, LAUNCH_FAIL,	// launch	���p
		UNKNOWN_CMD_TYPE
	};

	String^ processCommand(int, int, String^, Team^, Team^);
	String^ toUpper(String^);
	double findNumberInString(String^);
}