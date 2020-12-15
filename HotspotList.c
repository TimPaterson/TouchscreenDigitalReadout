//****************************************************************************
// HotspotList.c
//
// Created 11/6/2020 4:35:23 PM by Tim
//
//****************************************************************************


#include <standard.h>
#include "HotspotList.h"


//****************************************************************************
// Read in the hotspot coordinates

#define START_GROUP(group)	HOTSPOT_GROUP_##group,

enum HotspotGroups
{
	#include "Images/Screen.h"
};

#define HOTSPOT_COUNT(name, count)	name##HotspotCount = count,

enum HotspotCounts
{
	#include "Images/Screen.h"
};

#define START_HOTSPOTS(name) const HotspotList name##HotspotList = { name##HotspotCount, {
#define DEFINE_HOTSPOT(name, group, x1, y1, x2, y2)	{x1, y1, x2, y2, {name, HOTSPOT_GROUP_##group}},
#define END_HOTSPOTS(name) }};
#define HOTSPOT_COUNT(name, count)

#include "Images/Screen.h"
