#pragma once

#include <Foundation/Types/SharedPtr.h>
#include <RendererVk/RendererVkDLL.h>

namespace ezInternal::Vk
{
  class Adapter;
  class Instance;
  class CommandQueue;

  enum class CommandListType
  {
    Graphics,
    Compute,
    Copy
  };

  class Adapter : public ezRefCounted
  {
  public:
    Adapter(Instance& instance, const vk::PhysicalDevice& physicalDevice);
    const ezString& GetName() const;
    ezGALDevice* CreateDevice();
    Instance& GetInstance();
    vk::PhysicalDevice& GetPhysicalDevice();

    private:
    Instance& m_Instance;
    vk::PhysicalDevice m_PhysicalDevice;
    ezString m_Name;
  };

  class Instance
  {
  public:
    Instance();
    ezDynamicArray<ezSharedPtr<Adapter>> EnumerateAdapters();
    vk::Instance GetInstance();

  private:
    vk::DynamicLoader m_DynamicLoader;
    vk::UniqueInstance m_Instance;
    vk::UniqueDebugReportCallbackEXT m_Callback;
  };

  class CommandList
  {
  };

  class CommandQueue
  {
  public:
    CommandQueue(ezGALDeviceVk& device, CommandListType type, ezUInt32 queueFamilyIndex);
    void Wait(ezGALFence* fence, ezUInt64 value);
    void Signal(ezGALFence* fence, ezUInt64 value);
    void ExecuteCommandList(CommandList* commandList);
    void ExecuteCommandLists(const ezDynamicArray<CommandList*>& commandList);

    ezGALDeviceVk& GetDevice();
    ezUInt32 GetQueueFamilyIndex();
    vk::Queue GetQueue();

  private:
    ezGALDeviceVk& m_Device;
    ezUInt32 m_QueueFamilyIndex;
    vk::Queue m_Queue;
  };

  constexpr const char* win32SurfaceExtension = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;

  const ezDynamicArray<const char*> GetValidationLayers();

  const ezDynamicArray<const char*> GetRequiredExtensions(bool debug);

  const ezDynamicArray<const char*> GetRequiredDeviceExtensions();

  bool CheckValidationLayerSupport();

  bool SupportsRequiredExtensions(bool debug);

  bool SupportsRequiredDeviceExtensions(VkPhysicalDevice device);

  VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

  void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

  VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

  void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

  const ezStaticArray<ezInt32, QueueType::Count> FindQueueFamilyIndices(VkPhysicalDevice device);

  ezInt32 FindPresentQueueFamilyIndex(VkPhysicalDevice device);

  bool CheckPhysicalDevice(VkPhysicalDevice device);
} // namespace ezInternal::Vk
