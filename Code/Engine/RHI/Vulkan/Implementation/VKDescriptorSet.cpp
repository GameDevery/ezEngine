#include <RHI/Vulkan/VKStd.h>

#include <RHI/Vulkan/States/VKSampler.h>
#include <RHI/Vulkan/VKBuffer.h>
#include <RHI/Vulkan/VKCommands.h>
#include <RHI/Vulkan/VKDescriptorSet.h>
#include <RHI/Vulkan/VKDescriptorSetLayout.h>
#include <RHI/Vulkan/VKDevice.h>
#include <RHI/Vulkan/VKPipelineLayout.h>
#include <RHI/Vulkan/VKShader.h>
#include <RHI/Vulkan/VKTexture.h>

CCVKDescriptorSet::CCVKDescriptorSet()
{
  m_TypedID = GenerateObjectID<decltype(this)>();
}

CCVKDescriptorSet::~CCVKDescriptorSet()
{
  destroy();
}

void CCVKDescriptorSet::doInit(const DescriptorSetInfo& /*info*/)
{
  CCVKGPUDescriptorSetLayout* gpuDescriptorSetLayout = static_cast<CCVKDescriptorSetLayout*>(_layout)->gpuDescriptorSetLayout();
  ezUInt32 bindingCount = gpuDescriptorSetLayout->bindings.GetCount();
  ezUInt32 descriptorCount = gpuDescriptorSetLayout->descriptorCount;

  _gpuDescriptorSet = EZ_DEFAULT_NEW(CCVKGPUDescriptorSet);
  _gpuDescriptorSet->gpuDescriptors.SetCount(descriptorCount, {});
  _gpuDescriptorSet->layoutID = gpuDescriptorSetLayout->id;

  for (ezUInt32 i = 0U, k = 0U; i < bindingCount; ++i)
  {
    const DescriptorSetLayoutBinding& binding = gpuDescriptorSetLayout->bindings[i];
    for (ezUInt32 j = 0; j < binding.count; ++j, ++k)
    {
      CCVKGPUDescriptor& gpuDescriptor = _gpuDescriptorSet->gpuDescriptors[k];
      gpuDescriptor.type = binding.descriptorType;
      switch (binding.descriptorType)
      {
        case DescriptorType::UNIFORM_BUFFER:
        case DescriptorType::DYNAMIC_UNIFORM_BUFFER:
          if (HasFlag(binding.stageFlags, ShaderStageFlags::COMPUTE))
            gpuDescriptor.accessTypes.PushBack(THSVS_ACCESS_COMPUTE_SHADER_READ_UNIFORM_BUFFER);
          if (HasFlag(binding.stageFlags, ShaderStageFlags::VERTEX))
            gpuDescriptor.accessTypes.PushBack(THSVS_ACCESS_VERTEX_SHADER_READ_UNIFORM_BUFFER);
          if (HasFlag(binding.stageFlags, ShaderStageFlags::FRAGMENT))
            gpuDescriptor.accessTypes.PushBack(THSVS_ACCESS_FRAGMENT_SHADER_READ_UNIFORM_BUFFER);
          break;
        case DescriptorType::STORAGE_BUFFER:
        case DescriptorType::DYNAMIC_STORAGE_BUFFER:
        case DescriptorType::STORAGE_IMAGE:
          // write accesses should be handled manually
          if (HasFlag(binding.stageFlags, ShaderStageFlags::COMPUTE))
            gpuDescriptor.accessTypes.PushBack(THSVS_ACCESS_COMPUTE_SHADER_READ_OTHER);
          if (HasFlag(binding.stageFlags, ShaderStageFlags::VERTEX))
            gpuDescriptor.accessTypes.PushBack(THSVS_ACCESS_VERTEX_SHADER_READ_OTHER);
          if (HasFlag(binding.stageFlags, ShaderStageFlags::FRAGMENT))
            gpuDescriptor.accessTypes.PushBack(THSVS_ACCESS_FRAGMENT_SHADER_READ_OTHER);
          break;
        case DescriptorType::SAMPLER_TEXTURE:
        case DescriptorType::TEXTURE:
          if (HasFlag(binding.stageFlags, ShaderStageFlags::COMPUTE))
            gpuDescriptor.accessTypes.PushBack(THSVS_ACCESS_COMPUTE_SHADER_READ_SAMPLED_IMAGE_OR_UNIFORM_TEXEL_BUFFER);
          if (HasFlag(binding.stageFlags, ShaderStageFlags::VERTEX))
            gpuDescriptor.accessTypes.PushBack(THSVS_ACCESS_VERTEX_SHADER_READ_SAMPLED_IMAGE_OR_UNIFORM_TEXEL_BUFFER);
          if (HasFlag(binding.stageFlags, ShaderStageFlags::FRAGMENT))
            gpuDescriptor.accessTypes.PushBack(THSVS_ACCESS_FRAGMENT_SHADER_READ_SAMPLED_IMAGE_OR_UNIFORM_TEXEL_BUFFER);
          break;
        case DescriptorType::INPUT_ATTACHMENT:
          gpuDescriptor.accessTypes.PushBack(THSVS_ACCESS_FRAGMENT_SHADER_READ_COLOR_INPUT_ATTACHMENT);
          break;
        case DescriptorType::SAMPLER:
        default:
          break;
      }
    }
  }

  CCVKGPUDevice* gpuDevice = CCVKDevice::getInstance()->gpuDevice();
  _gpuDescriptorSet->gpuLayout = gpuDescriptorSetLayout;
  _gpuDescriptorSet->instances.SetCount(gpuDevice->backBufferCount);

  for (ezUInt32 t = 0U; t < gpuDevice->backBufferCount; ++t)
  {
    CCVKGPUDescriptorSet::Instance& instance = _gpuDescriptorSet->instances[t];
    instance.vkDescriptorSet = gpuDevice->getDescriptorSetPool(_gpuDescriptorSet->layoutID)->request(t);
    instance.descriptorInfos.SetCount(descriptorCount, {});

    for (ezUInt32 i = 0U, k = 0U; i < bindingCount; ++i)
    {
      const DescriptorSetLayoutBinding& binding = gpuDescriptorSetLayout->bindings[i];
      for (ezUInt32 j = 0; j < binding.count; ++j, ++k)
      {
        if (HasFlag(DESCRIPTOR_BUFFER_TYPE, binding.descriptorType))
        {
          instance.descriptorInfos[k].buffer.buffer = gpuDevice->defaultBuffer.vkBuffer;
          instance.descriptorInfos[k].buffer.offset = gpuDevice->defaultBuffer.getStartOffset(t);
          instance.descriptorInfos[k].buffer.range = gpuDevice->defaultBuffer.size;
        }
        else if (HasFlag(DESCRIPTOR_TEXTURE_TYPE, binding.descriptorType))
        {
          instance.descriptorInfos[k].image.sampler = gpuDevice->defaultSampler.vkSampler;
          instance.descriptorInfos[k].image.imageView = gpuDevice->defaultTextureView.vkImageView;
          instance.descriptorInfos[k].image.imageLayout = HasFlag(binding.descriptorType, DescriptorType::STORAGE_IMAGE)
                                                            ? VK_IMAGE_LAYOUT_GENERAL
                                                            : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }
      }
    }

    if (!gpuDevice->useDescriptorUpdateTemplate)
    {
      ezDynamicArray<VkWriteDescriptorSet>& entries = instance.descriptorUpdateEntries;
      entries.SetCount(descriptorCount, {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET});

      for (ezUInt32 i = 0U, j = 0U; i < bindingCount; i++)
      {
        const VkDescriptorSetLayoutBinding& descriptor = gpuDescriptorSetLayout->vkBindings[i];
        for (ezUInt32 k = 0U; k < descriptor.descriptorCount; k++, j++)
        {
          entries[j].dstSet = instance.vkDescriptorSet;
          entries[j].dstBinding = descriptor.binding;
          entries[j].dstArrayElement = k;
          entries[j].descriptorCount = 1; // better not to assume that the descriptor infos would be contiguous
          entries[j].descriptorType = descriptor.descriptorType;
          switch (entries[j].descriptorType)
          {
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
              entries[j].pBufferInfo = &instance.descriptorInfos[j].buffer;
              break;
            case VK_DESCRIPTOR_TYPE_SAMPLER:
            case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
              entries[j].pImageInfo = &instance.descriptorInfos[j].image;
              break;
            case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
            case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
              entries[j].pTexelBufferView = &instance.descriptorInfos[j].texelBufferView;
              break;
            default:
              break;
          }
        }
      }
    }
  }
}

void CCVKDescriptorSet::doDestroy()
{
  if (_gpuDescriptorSet)
  {
    CCVKGPUDevice* gpuDevice = CCVKDevice::getInstance()->gpuDevice();
    CCVKGPUDescriptorHub* descriptorHub = CCVKDevice::getInstance()->gpuDescriptorHub();
    ezUInt32 instanceCount = _gpuDescriptorSet->instances.GetCount();

    for (ezUInt32 t = 0U; t < instanceCount; ++t)
    {
      CCVKGPUDescriptorSet::Instance& instance = _gpuDescriptorSet->instances[t];

      for (ezUInt32 i = 0U; i < _gpuDescriptorSet->gpuDescriptors.GetCount(); i++)
      {
        CCVKGPUDescriptor& binding = _gpuDescriptorSet->gpuDescriptors[i];

        CCVKDescriptorInfo& descriptorInfo = instance.descriptorInfos[i];
        if (binding.gpuBufferView)
        {
          descriptorHub->disengage(_gpuDescriptorSet, binding.gpuBufferView, &descriptorInfo.buffer);
        }
        if (binding.gpuTextureView)
        {
          descriptorHub->disengage(_gpuDescriptorSet, binding.gpuTextureView, &descriptorInfo.image);
        }
        if (binding.gpuSampler)
        {
          descriptorHub->disengage(binding.gpuSampler, &descriptorInfo.image);
        }
      }

      if (instance.vkDescriptorSet)
      {
        gpuDevice->getDescriptorSetPool(_gpuDescriptorSet->layoutID)->yield(instance.vkDescriptorSet, t);
      }
    }

    CCVKDevice::getInstance()->gpuDescriptorSetHub()->erase(_gpuDescriptorSet);

    EZ_DEFAULT_DELETE(_gpuDescriptorSet);
    _gpuDescriptorSet = nullptr;
  }
}

void CCVKDescriptorSet::update()
{
  if (_isDirty && _gpuDescriptorSet)
  {
    CCVKGPUDescriptorHub* descriptorHub = CCVKDevice::getInstance()->gpuDescriptorHub();
    CCVKGPUBarrierManager* layoutMgr = CCVKDevice::getInstance()->gpuBarrierManager();
    ezUInt32 descriptorCount = _gpuDescriptorSet->gpuDescriptors.GetCount();
    ezUInt32 instanceCount = _gpuDescriptorSet->instances.GetCount();

    for (ezUInt32 i = 0U; i < descriptorCount; i++)
    {
      CCVKGPUDescriptor& binding = _gpuDescriptorSet->gpuDescriptors[i];

      if (HasFlag(DESCRIPTOR_BUFFER_TYPE, binding.type))
      {
        if (_buffers[i])
        {
          CCVKGPUBufferView* bufferView = static_cast<CCVKBuffer*>(_buffers[i])->gpuBufferView();
          if (binding.gpuBufferView != bufferView)
          {
            for (ezUInt32 t = 0U; t < instanceCount; ++t)
            {
              CCVKDescriptorInfo& descriptorInfo = _gpuDescriptorSet->instances[t].descriptorInfos[i];

              if (binding.gpuBufferView)
              {
                descriptorHub->disengage(_gpuDescriptorSet, binding.gpuBufferView, &descriptorInfo.buffer);
              }
              if (bufferView)
              {
                descriptorHub->connect(_gpuDescriptorSet, bufferView, &descriptorInfo.buffer, t);
                descriptorHub->update(bufferView, &descriptorInfo.buffer);
              }
            }
            binding.gpuBufferView = bufferView;
          }
        }
      }
      else if (HasFlag(DESCRIPTOR_TEXTURE_TYPE, binding.type))
      {
        if (_textures[i])
        {
          CCVKGPUTextureView* textureView = static_cast<CCVKTexture*>(_textures[i])->gpuTextureView();
          if (binding.gpuTextureView != textureView)
          {
            for (auto& instance : _gpuDescriptorSet->instances)
            {
              CCVKDescriptorInfo& descriptorInfo = instance.descriptorInfos[i];
              if (binding.gpuTextureView)
              {
                descriptorHub->disengage(_gpuDescriptorSet, binding.gpuTextureView, &descriptorInfo.image);
              }
              if (textureView)
              {
                descriptorHub->connect(_gpuDescriptorSet, textureView, &descriptorInfo.image);
                descriptorHub->update(textureView, &descriptorInfo.image);
                layoutMgr->checkIn(textureView->gpuTexture, binding.accessTypes.GetData(), binding.accessTypes.GetCount());
              }
            }
            binding.gpuTextureView = textureView;
          }
        }
        if (_samplers[i])
        {
          CCVKGPUSampler* sampler = static_cast<CCVKSampler*>(_samplers[i])->gpuSampler();
          if (binding.gpuSampler != sampler)
          {
            for (auto& instance : _gpuDescriptorSet->instances)
            {
              CCVKDescriptorInfo& descriptorInfo = instance.descriptorInfos[i];
              if (binding.gpuSampler)
              {
                descriptorHub->disengage(binding.gpuSampler, &descriptorInfo.image);
              }
              if (sampler)
              {
                descriptorHub->connect(sampler, &descriptorInfo.image);
                descriptorHub->update(sampler, &descriptorInfo.image);
              }
            }
            binding.gpuSampler = sampler;
          }
        }
      }
    }
    CCVKDevice::getInstance()->gpuDescriptorSetHub()->record(_gpuDescriptorSet);
    _isDirty = false;
  }
}
