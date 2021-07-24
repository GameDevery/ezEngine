#include <RendererVkPCH.h>

#include <RendererVk/Implementation/Adapter.h>
#include <RendererVk/Implementation/Instance.h>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace ezInternal::Vk
{
  static bool SkipIt(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, const ezString& message)
  {
    if (objectType == VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT && flags != VK_DEBUG_REPORT_ERROR_BIT_EXT)
      return true;

    static const char* mutedWarnings[] = {
      "UNASSIGNED-CoreValidation-Shader-InconsistentSpirv",
      "VUID-vkCmdDrawIndexed-None-04007",
      "VUID-vkDestroyDevice-device-00378",
      "VUID-VkSubmitInfo-pWaitSemaphores-03243",
      "VUID-VkSubmitInfo-pSignalSemaphores-03244",
      "VUID-vkCmdPipelineBarrier-pDependencies-02285",
      "VUID-VkImageMemoryBarrier-oldLayout-01213",
      "VUID-vkCmdDrawIndexed-None-02721",
      "VUID-vkCmdDrawIndexed-None-02699",
      "VUID-vkCmdTraceRaysKHR-None-02699",
      "VUID-VkShaderModuleCreateInfo-pCode-04147",
    };

    for (auto& str : mutedWarnings)
    {
      if (message.FindSubString(str) != nullptr)
        return true;
    }
    return false;
  }

  static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objectType,
    uint64_t object,
    size_t location,
    int32_t messageCode,
    const char* pLayerPrefix,
    const char* pMessage,
    void* pUserData)
  {
    constexpr ezUInt64 errorLimit = 1024;
    static ezUInt64 errorCount = 0;
    if (errorCount >= errorLimit || SkipIt(flags, objectType, pMessage))
      return VK_FALSE;
    if (errorCount < errorLimit)
    {
      ezStringBuilder builder;
      builder.Format("{} {} {}", pLayerPrefix, static_cast<VkDebugReportFlagBitsEXT>(flags), pMessage);

      ezLog::Debug(builder.GetData());
    }
    ++errorCount;
    return VK_FALSE;
  }

  Instance::Instance()
  {
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = m_DynamicLoader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);


    auto layers = vk::enumerateInstanceLayerProperties();

    ezSet<ezString> req_layers;
    static const bool debug_enabled = true; // IsDebuggerPresent();
    if (debug_enabled)
      req_layers.Insert("VK_LAYER_KHRONOS_validation");
    ezDynamicArray<const char*> found_layers;
    for (const auto& layer : layers)
    {
      if (req_layers.Contains(layer.layerName.data()))
        found_layers.PushBack(layer.layerName);
    }

    auto extensions = vk::enumerateInstanceExtensionProperties();

    ezSet<ezString> req_extension;
    req_extension.Insert(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    req_extension.Insert(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    req_extension.Insert(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
    req_extension.Insert(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
    req_extension.Insert(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
    req_extension.Insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    req_extension.Insert(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    ezDynamicArray<const char*> found_extension;
    for (const auto& extension : extensions)
    {
      if (req_extension.Contains(extension.extensionName.data()))
        found_extension.PushBack(extension.extensionName);
    }

    vk::ApplicationInfo app_info = {};
    app_info.apiVersion = VK_API_VERSION_1_2;

    vk::InstanceCreateInfo create_info;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledLayerCount = found_layers.GetCount();
    create_info.ppEnabledLayerNames = found_layers.GetData();
    create_info.enabledExtensionCount = found_extension.GetCount();
    create_info.ppEnabledExtensionNames = found_extension.GetData();

    m_Instance = vk::createInstanceUnique(create_info);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Instance.get());
    if (debug_enabled)
    {
      vk::DebugReportCallbackCreateInfoEXT callback_create_info = {};
      callback_create_info.flags = vk::DebugReportFlagBitsEXT::eWarning |
                                   vk::DebugReportFlagBitsEXT::ePerformanceWarning |
                                   vk::DebugReportFlagBitsEXT::eError |
                                   vk::DebugReportFlagBitsEXT::eDebug;
      callback_create_info.pfnCallback = &DebugReportCallback;
      callback_create_info.pUserData = this;
      m_Callback = m_Instance->createDebugReportCallbackEXTUnique(callback_create_info);
    }
  }

  ezDynamicArray<ezSharedPtr<Adapter>> Instance::EnumerateAdapters()
  {
    ezDynamicArray<ezSharedPtr<Adapter>> adapters;

    auto devices = m_Instance->enumeratePhysicalDevices();
    for (const auto& device : devices)
    {
      vk::PhysicalDeviceProperties deviceProperties = device.getProperties();

      if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu ||
          deviceProperties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
      {
        adapters.PushBack(EZ_DEFAULT_NEW(Adapter, *this, device));
      }
    }

    return adapters;
  }

  vk::Instance Instance::GetInstance()
  {
    return m_Instance.get();
  }
} // namespace ezInternal::Vk
