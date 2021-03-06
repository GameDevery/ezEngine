#include <CorePCH.h>

EZ_STATICLINK_LIBRARY(Core)
{
  if (bReturn)
    return;

  EZ_STATICLINK_REFERENCE(Core_Assets_Implementation_AssetFileHeader);
  EZ_STATICLINK_REFERENCE(Core_Collection_Implementation_CollectionComponent);
  EZ_STATICLINK_REFERENCE(Core_Collection_Implementation_CollectionResource);
  EZ_STATICLINK_REFERENCE(Core_Collection_Implementation_CollectionUtils);
  EZ_STATICLINK_REFERENCE(Core_Graphics_Implementation_AmbientCubeBasis);
  EZ_STATICLINK_REFERENCE(Core_Graphics_Implementation_Camera);
  EZ_STATICLINK_REFERENCE(Core_Graphics_Implementation_ConvexHull);
  EZ_STATICLINK_REFERENCE(Core_Graphics_Implementation_Geometry);
  EZ_STATICLINK_REFERENCE(Core_Input_DeviceTypes_DeviceTypes);
  EZ_STATICLINK_REFERENCE(Core_Input_Implementation_Action);
  EZ_STATICLINK_REFERENCE(Core_Input_Implementation_InputDevice);
  EZ_STATICLINK_REFERENCE(Core_Input_Implementation_InputManager);
  EZ_STATICLINK_REFERENCE(Core_Input_Implementation_ScancodeTable);
  EZ_STATICLINK_REFERENCE(Core_Input_Implementation_Startup);
  EZ_STATICLINK_REFERENCE(Core_Input_Implementation_VirtualThumbStick);
  EZ_STATICLINK_REFERENCE(Core_Messages_Implementation_EventMessage);
  EZ_STATICLINK_REFERENCE(Core_Messages_Implementation_Messages);
  EZ_STATICLINK_REFERENCE(Core_ResourceManager_Implementation_Resource);
  EZ_STATICLINK_REFERENCE(Core_ResourceManager_Implementation_ResourceHandle);
  EZ_STATICLINK_REFERENCE(Core_ResourceManager_Implementation_ResourceLoading);
  EZ_STATICLINK_REFERENCE(Core_ResourceManager_Implementation_ResourceManager);
  EZ_STATICLINK_REFERENCE(Core_ResourceManager_Implementation_ResourceTypeLoader);
  EZ_STATICLINK_REFERENCE(Core_ResourceManager_Implementation_WorkerTasks);
  EZ_STATICLINK_REFERENCE(Core_Scripting_LuaWrapper_CFunctions);
  EZ_STATICLINK_REFERENCE(Core_Scripting_LuaWrapper_Initialize);
  EZ_STATICLINK_REFERENCE(Core_Scripting_LuaWrapper_Tables);
  EZ_STATICLINK_REFERENCE(Core_Scripting_LuaWrapper_Variables);
  EZ_STATICLINK_REFERENCE(Core_Utils_Implementation_WorldGeoExtractionUtil);
  EZ_STATICLINK_REFERENCE(Core_WorldSerializer_Implementation_ResourceHandleReader);
  EZ_STATICLINK_REFERENCE(Core_WorldSerializer_Implementation_ResourceHandleWriter);
  EZ_STATICLINK_REFERENCE(Core_WorldSerializer_Implementation_WorldReader);
  EZ_STATICLINK_REFERENCE(Core_WorldSerializer_Implementation_WorldWriter);
  EZ_STATICLINK_REFERENCE(Core_World_Implementation_Component);
  EZ_STATICLINK_REFERENCE(Core_World_Implementation_ComponentManager);
  EZ_STATICLINK_REFERENCE(Core_World_Implementation_Declarations);
  EZ_STATICLINK_REFERENCE(Core_World_Implementation_EventMessageHandlerComponent);
  EZ_STATICLINK_REFERENCE(Core_World_Implementation_GameObject);
  EZ_STATICLINK_REFERENCE(Core_World_Implementation_SettingsComponent);
  EZ_STATICLINK_REFERENCE(Core_World_Implementation_SpatialSystem);
  EZ_STATICLINK_REFERENCE(Core_World_Implementation_SpatialSystem_RegularGrid);
  EZ_STATICLINK_REFERENCE(Core_World_Implementation_World);
  EZ_STATICLINK_REFERENCE(Core_World_Implementation_WorldData);
  EZ_STATICLINK_REFERENCE(Core_World_Implementation_WorldModule);
}

