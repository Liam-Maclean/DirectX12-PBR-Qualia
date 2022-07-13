#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include <type_traits>


// Used to abstract rendering api handles
template <class DX_TYPE, class VK_TYPE>
class ApiHandle final
{
    // Only work with pointers
    static_assert(std::is_pointer_v<DX_TYPE>, "ApiHandle type is not a pointer");
    static_assert(std::is_pointer_v<VK_TYPE>, "ApiHandle type is not a pointer");
   

public:
    using BaseDX = DX_TYPE;
    using BaseVK = VK_TYPE;
    

    ApiHandle() : m_handle(nullptr) {}
    ApiHandle(const DX_TYPE& handle) : m_handle(handle) {}
    ApiHandle(const VK_TYPE& handle) : m_handle(handle) {}
  

    // Operators for auto casting
    operator DX_TYPE() { return static_cast<DX_TYPE>(m_handle); }
    operator DX_TYPE() const { return static_cast<DX_TYPE>(m_handle); }
    operator VK_TYPE() { return static_cast<VK_TYPE>(m_handle); }
    operator VK_TYPE() const { return static_cast<VK_TYPE>(m_handle); }
    operator uintptr_t() { return reinterpret_cast<uintptr_t>(m_handle); }
    operator bool() { return m_handle != nullptr; }
    bool operator!() { return m_handle == nullptr; }

private:
    void* m_handle;
};


template <class T, class HANDLE>
class IHandle
{
public:
    template <class... Params>
    inline static T* Create(Params &&...params)
    {
        static_assert(std::is_base_of_v<IHandle<T, HANDLE>, T>);

        T* ptr = new T(std::forward<Params>(params)...);
        ptr->p = ptr;

        return ptr;
    }

    inline static void Destroy(T* ptr)
    {
        static_assert(std::is_base_of_v<IHandle<T, HANDLE>, T>);

        if (ptr && ptr->p)
        {
            delete ptr->p;
            ptr->p = nullptr;
        }
    }

    virtual ~IHandle() {}
    HANDLE& Handle() { return m_handle; }
    const HANDLE& Handle() const { return m_handle; }

protected:
    IHandle() : m_handle{} {}
    HANDLE m_handle;

private:
    T* p;
};

struct PlaceholderVulkanType
{

};

using CommandListHandle = ApiHandle<ID3D12CommandList*, PlaceholderVulkanType*>;
using DeviceHandle = ApiHandle<ID3D12Device*, PlaceholderVulkanType*>;
using ImageHandle = ApiHandle<ID3D12Resource*, PlaceholderVulkanType*>;
using ImageViewHandle = ApiHandle<ID3D12Resource*, PlaceholderVulkanType*>;
using QueueHandle = ApiHandle<ID3D12CommandQueue*, PlaceholderVulkanType*>;
using FenceHandle = ApiHandle<ID3D12Fence*, PlaceholderVulkanType*>;
using GraphicsAllocatorHandle = ApiHandle<ID3D12CommandAllocator*, PlaceholderVulkanType*>;
using PipelineStateHandle = ApiHandle<ID3D12PipelineState*, PlaceholderVulkanType*>;
using SwapchainHandle = ApiHandle< IDXGISwapChain3*, PlaceholderVulkanType*>;




//using DescriptorSetLayoutHandle = ApiHandle<VkDescriptorSetLayout, Placeholder*>;
//using DescriptorSetHandle = ApiHandle<VkDescriptorSet, Placeholder*>;
//using FrameBufferHandle = ApiHandle<VkFramebuffer, Placeholder*>;


//using SamplerHandle = ApiHandle<VkSampler, Placeholder*>;
//using RenderPassHandle = ApiHandle<VkRenderPass, Placeholder*>;
//using CommandPoolHandle = ApiHandle<VkCommandPool, Placeholder*>;
//using BufferHandle = ApiHandle<VkBuffer, Placeholder*>;
//using PipelineCacheHandle = ApiHandle<VkPipelineCache, Placeholder*>;

//using PipelineHandle = ApiHandle<VkPipeline, Placeholder*>;

//using SemaphoreHandle = ApiHandle<VkSemaphore, Placeholder*>;
//using QueryPoolHandle = ApiHandle<VkQueryPool, Placeholder*>;
//using SwapchainHandle = ApiHandle<VkSwapchainKHR, Placeholder*>;

//using SurfaceHandle = ApiHandle<VkSurfaceKHR, Placeholder*>;
//using InstanceHandle = ApiHandle<VkInstance, Placeholder*>;
//using GpuHandle = ApiHandle<ID3D12Device, Placeholder*>;
//using DebugMessengerHandle = ApiHandle<VkDebugUtilsMessengerEXT, Placeholder*>;

//using DescriptorPoolHandle = ApiHandle<VkDescriptorPool, Placeholder*>;
//using AllocationHandle = ApiHandle<VmaAllocation, Placeholder*>;

