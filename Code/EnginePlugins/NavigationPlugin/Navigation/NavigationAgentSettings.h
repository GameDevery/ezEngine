#pragma once

#include <Foundation/Strings/String.h>
#include <Foundation/Types/Uuid.h>
#include <NavigationPlugin/NavigationPluginDLL.h>

struct EZ_NAVIGATIONPLUGIN_DLL ezNavigationAgentSettings
{
  /// Height of the actor
  /// The height of the entities in this group. Entities can't enter areas with ceilings lower than this value.
  //[DataMemberRange(0, 3)]
  float Height;


  /// Maximum vertical distance this agent can climb
  /// The maximum height that entities in this group can climb.
  //[DataMemberRange(0, 3)]
  float MaxClimb;

  /// Maximum slope angle this agent can climb
  /// The maximum incline (in degrees) that entities in this group can climb. Entities can't go up or down slopes higher than this value.
  //[DataMember(2)]
  ezAngle MaxSlope;

  /// Radius of the actor
  /// The larger this value, the larger the area of the navigation mesh entities use. Entities can't pass through gaps of less than twice the radius.
  //[DataMemberRange(0, 3)]
  float Radius;

  EZ_FORCE_INLINE ezNavigationAgentSettings() {
    Height = 1.0f;
    MaxClimb = 0.25f;
    MaxSlope = ezAngle::Degree(45.0f);
    Radius = 0.5f;
  }
};
