/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "trace3d_helper.h"

#include <vulkan/vk_layer.h>

#define TRACE3D_LAYER_LAYER_NAME  "VK_LAYER_TRACE3D_Loader"

#define TRACE3D_LAYER_DESCRIPTION "Trace3D Layer Loader 0.1.0"
#define TRACE3D_LAYER_VERSION_MAJOR_VALUE 0
#define TRACE3D_LAYER_VERSION_MINOR_VALUE 1
#define TRACE3D_LAYER_VERSION_PATCH_VALUE 0

static const VkLayerProperties g_LayerProps = {
    TRACE3D_LAYER_LAYER_NAME,
    VK_HEADER_VERSION_COMPLETE,
    VK_MAKE_VERSION(TRACE3D_LAYER_VERSION_MAJOR_VALUE,
                    TRACE3D_LAYER_VERSION_MINOR_VALUE,
                    TRACE3D_LAYER_VERSION_PATCH_VALUE),
    TRACE3D_LAYER_DESCRIPTION
};

namespace trace3d {

void* CaptureInit();

VkResult EnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties)
{
    VkResult result = VK_SUCCESS;
    if (pProperties == nullptr) {
        if (pPropertyCount != nullptr) {
            *pPropertyCount = 1;
        }
    } else if ((pPropertyCount != nullptr) && (*pPropertyCount >= 1)) {
        errno_t ret = memcpy_s(pProperties, sizeof(VkLayerProperties), &g_LayerProps, sizeof(g_LayerProps));
        if (ret != EOK) {
            TRACE3D_LOGE("%s:%d ERROR: memcpy_s! Error code: %d\n", __FUNCTION__, __LINE__, ret);
        }
        *pPropertyCount = 1;
    } else {
        result = VK_INCOMPLETE;
    }
    return result;
}

VkResult EnumerateDeviceLayerProperties(
    VkPhysicalDevice physicalDevice, uint32_t *pPropertyCount, VkLayerProperties *pProperties)
{
    (void)physicalDevice;
    return EnumerateInstanceLayerProperties(pPropertyCount, pProperties);
}

VkResult EnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount,
    VkExtensionProperties* pProperties)
{
    VkResult result = VK_SUCCESS;
    if (pLayerName && strcmp(pLayerName, g_LayerProps.layerName) == 0) {
        if (pPropertyCount != nullptr) {
            *pPropertyCount = 0;
        }
    } else {
        result = VK_ERROR_LAYER_NOT_PRESENT;
    }
    if (result != VK_SUCCESS) {
        TRACE3D_LOGE("%s( pLayerName:%p) --> vkresult:%d\n",
            __FUNCTION__, pLayerName, result);
    }
    return result;
}

typedef const void *DispatchKey;

static inline DispatchKey GetDispatchKey(const void *handle)
{
    const DispatchKey *dispatchKey = reinterpret_cast<const DispatchKey *>(handle);
    return (*dispatchKey);
}

struct InstanceInfo {
    VkInstance instance{nullptr};
    PFN_vkGetInstanceProcAddr getInstanceProcAddr{nullptr};
    PFN_vkEnumerateDeviceExtensionProperties enumerateDeviceExtensionProperties{nullptr};
    PFN_vkDestroyInstance destroyInstance{nullptr};
};

struct DeviceInfo {
    VkDevice device{nullptr};
    PFN_vkGetDeviceProcAddr getDeviceProcAddr{nullptr};
    PFN_vkDestroyDevice destroyDevice{nullptr};
};

SafeObject< std::map<DispatchKey, InstanceInfo> > g_InstanceStore;
SafeObject< std::map<DispatchKey, DeviceInfo> > g_DeviceStore;

VkResult EnumerateDeviceExtensionProperties(
    VkPhysicalDevice physicalDevice, const char *pLayerName,
    uint32_t *pPropertyCount, VkExtensionProperties *pProperties)
{
    VkResult result = VK_ERROR_LAYER_NOT_PRESENT;

    if (pLayerName && strcmp(pLayerName, g_LayerProps.layerName) == 0) {
        if (pPropertyCount != nullptr) {
            *pPropertyCount = 0;
        }
        result = VK_SUCCESS;
    } else {
        DispatchKey physKey = GetDispatchKey(physicalDevice);
        auto instanceLock = g_InstanceStore.LockRead();
        const auto &instanceMap = instanceLock.second;

        auto it = instanceMap.find(physKey);
        if (it != instanceMap.end()) {
            auto &instInfo = it->second;

            if (instInfo.enumerateDeviceExtensionProperties) {
                result = instInfo.enumerateDeviceExtensionProperties(physicalDevice, nullptr,
                    pPropertyCount, pProperties);
            }
        }
    }

    if (result != VK_SUCCESS) {
        TRACE3D_LOGE("%s(physicalDevice:%p, pLayerName:%p) --> vkresult:%d\n",
            __FUNCTION__, physicalDevice, pLayerName, result);
    }
    return result;
}

VkLayerInstanceCreateInfo *GetChainInfo(const VkInstanceCreateInfo *pCreateInfo, VkLayerFunction func)
{
    auto chainInfo = static_cast<const VkLayerInstanceCreateInfo *>(pCreateInfo->pNext);
    while (chainInfo != nullptr) {
        if (chainInfo->sType == VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO && chainInfo->function == func) {
            return const_cast<VkLayerInstanceCreateInfo *>(chainInfo);
        }
        chainInfo = static_cast<const VkLayerInstanceCreateInfo *>(chainInfo->pNext);
    }
    TRACE3D_LOGE("%s", "ERROR: Find VkLayerInstanceCreateInfo Failed");
    return nullptr;
}

VkLayerDeviceCreateInfo *GetChainInfo(const VkDeviceCreateInfo *pCreateInfo, VkLayerFunction func)
{
    auto chainInfo = static_cast<const VkLayerDeviceCreateInfo *>(pCreateInfo->pNext);
    while (chainInfo != nullptr) {
        if (chainInfo->sType == VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO && chainInfo->function == func) {
            return const_cast<VkLayerDeviceCreateInfo *>(chainInfo);
        }
        chainInfo = static_cast<const VkLayerDeviceCreateInfo *>(chainInfo->pNext);
    }
    TRACE3D_LOGE("%s", "ERROR: Find VkLayerDeviceCreateInfo Failed");
    return nullptr;
}

VkResult CreateInstance(const VkInstanceCreateInfo *pCreateInfo,
    const VkAllocationCallbacks *pAllocator, VkInstance *pInstance)
{
    VkResult result = VK_ERROR_INITIALIZATION_FAILED;
    VkLayerInstanceCreateInfo *chainInfo = GetChainInfo(pCreateInfo, VK_LAYER_LINK_INFO);
    if (chainInfo == nullptr || chainInfo->u.pLayerInfo == nullptr) {
        TRACE3D_LOGE("%s:%d() --> vkresult:%d\n", __FUNCTION__, __LINE__, result);
        return result;
    }
    auto fpGetInstanceProcAddr = chainInfo->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    auto fpCreateInstance = (PFN_vkCreateInstance)fpGetInstanceProcAddr(nullptr, "vkCreateInstance");
    if (!fpCreateInstance) {
        TRACE3D_LOGE("%s:%d() --> vkresult:%d\n", __FUNCTION__, __LINE__, result);
        return result;
    }

    chainInfo->u.pLayerInfo = chainInfo->u.pLayerInfo->pNext;

    result = fpCreateInstance(pCreateInfo, pAllocator, pInstance);
    if (result != VK_SUCCESS) {
        return result;
    }
    VkInstance instance = *pInstance;
    DispatchKey instanceKey = GetDispatchKey(instance);

    auto fpEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)fpGetInstanceProcAddr(
        instance, "vkEnumerateDeviceExtensionProperties");
    auto fpDestroyInstance = (PFN_vkDestroyInstance)fpGetInstanceProcAddr(instance, "vkDestroyInstance");

    auto instanceLock = g_InstanceStore.LockWrite();
    auto &instanceMap = instanceLock.second;

    instanceMap[GetDispatchKey(*pInstance)] = InstanceInfo{ instance,
        fpGetInstanceProcAddr,
        fpEnumerateDeviceExtensionProperties,
        fpDestroyInstance } ;

    TRACE3D_LOGI("%s:%d() --> instance:%p, instanceKey:%p, vkresult:%d\n",
        __FUNCTION__, __LINE__, instance, instanceKey, result);
    return result;
}

void DestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator)
{
    DispatchKey instanceKey = GetDispatchKey(instance);

    auto instanceLock = g_InstanceStore.LockWrite();
    auto &instanceMap = instanceLock.second;

    auto it = instanceMap.find(instanceKey);
    if (it != instanceMap.end()) {
        auto &instInfo = it->second;

        TRACE3D_LOGI("%s(instance:%p) instanceKey:%p, destroyInstance:%p\n",
            __FUNCTION__, instance, instanceKey, instInfo.destroyInstance);
        if (instInfo.destroyInstance) {
            instInfo.destroyInstance(instance, pAllocator);
        }
        instanceMap.erase(instanceKey);
    }
}

VkResult CreateDevice(VkPhysicalDevice physDev, const VkDeviceCreateInfo *pCreateInfo,
    const VkAllocationCallbacks *pAllocator, VkDevice *pDevice)
{
    VkResult result = VK_ERROR_INITIALIZATION_FAILED;
    const DispatchKey physKey = GetDispatchKey(physDev);

    auto instanceLock = g_InstanceStore.LockWrite();
    auto &instanceMap = instanceLock.second;

    const InstanceInfo instanceInfo = instanceMap[physKey];
    const VkInstance instance = instanceInfo.instance;

    VkLayerDeviceCreateInfo *linkInfo = GetChainInfo(pCreateInfo, VK_LAYER_LINK_INFO);

    if (linkInfo == nullptr || linkInfo->u.pLayerInfo == nullptr || instance == nullptr) {
        TRACE3D_LOGE("%s:%d(physDev:%p) physKey:%p --> vkresult:%d\n",
            __FUNCTION__, __LINE__, physDev, physKey, result);
        return result;
    }
    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr = linkInfo->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkGetDeviceProcAddr fpGetDeviceProcAddr = linkInfo->u.pLayerInfo->pfnNextGetDeviceProcAddr;
    PFN_vkCreateDevice fpCreateDevice =
        (PFN_vkCreateDevice)fpGetInstanceProcAddr(instance, "vkCreateDevice");
    if (!fpCreateDevice) {
        TRACE3D_LOGE("%s:%d(physDev:%p) physKey:%p, instance:%p --> vkresult:%d\n",
            __FUNCTION__, __LINE__, physDev, physKey, instance, result);
        return result;
    }

    linkInfo->u.pLayerInfo = linkInfo->u.pLayerInfo->pNext;

    result = fpCreateDevice(physDev, pCreateInfo, pAllocator, pDevice);
    if (result != VK_SUCCESS) {
        TRACE3D_LOGE("%s:%d(physDev:%p) physKey:%p, instance:%p --> device:%p, vkresult:%d\n",
            __FUNCTION__, __LINE__, physDev, physKey, instance, *pDevice, result);
        return result;
    }
    VkDevice device = *pDevice;
    DispatchKey deviceKey = GetDispatchKey(device);

    PFN_vkDestroyDevice fpDestroyDevice = (PFN_vkDestroyDevice)fpGetDeviceProcAddr(device, "vkDestroyDevice");

    auto deviceLock = g_DeviceStore.LockWrite();
    auto &deviceMap = deviceLock.second;

    deviceMap[deviceKey] = DeviceInfo{device, fpGetDeviceProcAddr, fpDestroyDevice};

    TRACE3D_LOGI("%s(physDev:%p) physKey:%p, instance:%p --> device:%p, deviceKey:%p, vkresult:%d\n",
        __FUNCTION__, physDev, physKey, instance, device, deviceKey, result);
    return result;
}

void DestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator)
{
    DispatchKey deviceKey = GetDispatchKey(device);

    auto deviceLock = g_DeviceStore.LockWrite();
    auto &deviceMap = deviceLock.second;

    auto it = deviceMap.find(deviceKey);
    if (it != deviceMap.end()) {
        auto &devInfo = it->second;

        TRACE3D_LOGI("%s(device:%p) deviceKey:%p, destroyDevice:%p\n",
            __FUNCTION__, device, deviceKey, devInfo.destroyDevice);
        if (devInfo.destroyDevice)
            devInfo.destroyDevice(device, pAllocator);
        deviceMap.erase(deviceKey);
    }
}

} // namespace trace3d

extern "C" {
// Vk Layer Entries

PFN_vkGetInstanceProcAddr g_VkGetInstanceProcAddr = nullptr;
PFN_vkGetDeviceProcAddr g_VkGetDeviceProcAddr = nullptr;
PFN_vkGetInstanceProcAddr g_VkGetPhysicalDeviceProcAddr = nullptr;

TRACE3D_LOADER_API
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance instance, const char *pName)
{
    return g_VkGetInstanceProcAddr ? g_VkGetInstanceProcAddr(instance, pName) : nullptr;
}

TRACE3D_LOADER_API
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(VkDevice device, const char *pName)
{
    return g_VkGetDeviceProcAddr ? g_VkGetDeviceProcAddr(device, pName) : nullptr;
}

VkResult EnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pPropertyCount,
    VkExtensionProperties *pProperties)
{
    return trace3d::EnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties);
}

VkResult EnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties)
{
    return trace3d::EnumerateInstanceLayerProperties(pPropertyCount, pProperties);
}

TRACE3D_LOADER_API
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL VK_LAYER_TRACE3D_LoaderGetInstanceProcAddr(
    VkInstance instance, const char *pName)
{
    PFN_vkVoidFunction procAddr = nullptr;

    if (strcmp("vkGetInstanceProcAddr", pName) == 0) {
        procAddr = (PFN_vkVoidFunction)VK_LAYER_TRACE3D_LoaderGetInstanceProcAddr;
    } else if (strcmp("vkEnumerateInstanceExtensionProperties", pName) == 0) {
        procAddr = reinterpret_cast<PFN_vkVoidFunction>(EnumerateInstanceExtensionProperties);
    } else if (strcmp("vkEnumerateInstanceLayerProperties", pName) == 0) {
        procAddr = reinterpret_cast<PFN_vkVoidFunction>(EnumerateInstanceLayerProperties);
    } else if (strcmp("vkCreateInstance", pName) == 0) {
        procAddr = reinterpret_cast<PFN_vkVoidFunction>(trace3d::CreateInstance);
    } else if (strcmp("vkDestroyInstance", pName) == 0) {
        procAddr = reinterpret_cast<PFN_vkVoidFunction>(trace3d::DestroyInstance);
    } else if (strcmp("vkCreateDevice", pName) == 0) {
        procAddr = reinterpret_cast<PFN_vkVoidFunction>(trace3d::CreateDevice);
    } else if (strcmp("vkEnumerateDeviceExtensionProperties", pName) == 0) {
        procAddr = reinterpret_cast<PFN_vkVoidFunction>(trace3d::EnumerateDeviceExtensionProperties);
    } else {
        if (instance == VK_NULL_HANDLE) {
            TRACE3D_LOGE("%s(instance:%p, pName:'%s') ERROR: instance is null --> %p\n",
                __FUNCTION__, instance, pName, procAddr);
            return procAddr;
        }
        trace3d::DispatchKey instanceKey = trace3d::GetDispatchKey(instance);

        auto instanceLock = trace3d::g_InstanceStore.LockRead();
        const auto &instanceMap = instanceLock.second;

        auto it = instanceMap.find(instanceKey);
        if (it != instanceMap.end()) {
            procAddr = it->second.getInstanceProcAddr(instance, pName);
        }
    }
    return procAddr;
}

TRACE3D_LOADER_API
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL VK_LAYER_TRACE3D_LoaderGetDeviceProcAddr(
    VkDevice device, const char *pName)
{
    PFN_vkVoidFunction procAddr = nullptr;

    if (device == VK_NULL_HANDLE) {
        TRACE3D_LOGE("%s(device:%p, pName:'%s') ERROR: device is null--> %p\n", __FUNCTION__, device, pName, procAddr);
        return procAddr;
    }

    if (strcmp("vkGetDeviceProcAddr", pName) == 0) {
        procAddr = (PFN_vkVoidFunction)VK_LAYER_TRACE3D_LoaderGetDeviceProcAddr;
    } else if (strcmp("vkDestroyDevice", pName) == 0) {
        procAddr = (PFN_vkVoidFunction)trace3d::DestroyDevice;
    } else {
        trace3d::DispatchKey deviceKey = trace3d::GetDispatchKey(device);

        auto deviceLock = trace3d::g_DeviceStore.LockRead();
        const auto &deviceMap = deviceLock.second;

        auto it = deviceMap.find(deviceKey);
        if (it != deviceMap.end()) {
            procAddr = it->second.getDeviceProcAddr(device, pName);
        }
    }
    return procAddr;
}

TRACE3D_LOADER_API
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL VK_LAYER_TRACE3D_Loader_layerGetPhysicalDeviceProcAddr(
    VkInstance instance, const char *pName)
{
    PFN_vkVoidFunction procAddr = nullptr;

    if (strcmp("vk_layerGetPhysicalDeviceProcAddr", pName) == 0) {
        procAddr = (PFN_vkVoidFunction)VK_LAYER_TRACE3D_Loader_layerGetPhysicalDeviceProcAddr;
    } else {
        procAddr = VK_LAYER_TRACE3D_LoaderGetInstanceProcAddr(instance, pName);
    }
    TRACE3D_LOGI("%s(instance:%p, pName:'%s') --> %p\n", __FUNCTION__, instance, pName, procAddr);
    return procAddr;
}

TRACE3D_LOADER_API
VKAPI_ATTR VkResult VKAPI_CALL VK_LAYER_TRACE3D_LoaderNegotiateLoaderLayerInterfaceVersion(
    VkNegotiateLayerInterface *pVersionStruct)
{
    VkResult result = VK_ERROR_INITIALIZATION_FAILED;

    TRACE3D_LOGI("%s(pVerStruct:%p)...\n", __FUNCTION__, pVersionStruct);

    PFN_vkNegotiateLoaderLayerInterfaceVersion pfnNegotiate = nullptr;

    void *libHandle = trace3d::CaptureInit();
    if (libHandle) {
        pfnNegotiate = (PFN_vkNegotiateLoaderLayerInterfaceVersion)dlsym(libHandle,
            "VK_LAYER_TRACE3D_CaptureNegotiateLoaderLayerInterfaceVersion");
    }
    if (pfnNegotiate) {
        result = pfnNegotiate(pVersionStruct);
        if (result == VK_SUCCESS) {
            g_VkGetInstanceProcAddr = pVersionStruct->pfnGetInstanceProcAddr;
            g_VkGetDeviceProcAddr = pVersionStruct->pfnGetDeviceProcAddr;
            g_VkGetPhysicalDeviceProcAddr = pVersionStruct->pfnGetPhysicalDeviceProcAddr;
        }
    } else if (pVersionStruct && pVersionStruct->sType == LAYER_NEGOTIATE_INTERFACE_STRUCT) {
        g_VkGetInstanceProcAddr = VK_LAYER_TRACE3D_LoaderGetInstanceProcAddr;
        g_VkGetDeviceProcAddr = VK_LAYER_TRACE3D_LoaderGetDeviceProcAddr;
        g_VkGetPhysicalDeviceProcAddr = VK_LAYER_TRACE3D_Loader_layerGetPhysicalDeviceProcAddr;

        pVersionStruct->pfnGetInstanceProcAddr = g_VkGetInstanceProcAddr;
        pVersionStruct->pfnGetDeviceProcAddr = g_VkGetDeviceProcAddr;
        pVersionStruct->pfnGetPhysicalDeviceProcAddr = g_VkGetPhysicalDeviceProcAddr;

        if (pVersionStruct->loaderLayerInterfaceVersion > CURRENT_LOADER_LAYER_INTERFACE_VERSION) {
            pVersionStruct->loaderLayerInterfaceVersion = CURRENT_LOADER_LAYER_INTERFACE_VERSION;
        }
        result = VK_SUCCESS;
    }

    TRACE3D_LOGI("%s(pVerStruct:%p) ifaceVer:%d, libHandle:%p --> vkresult:%d\n", __FUNCTION__,
        pVersionStruct, (pVersionStruct ? (int)pVersionStruct->loaderLayerInterfaceVersion : 0), libHandle, result);
    return result;
}

TRACE3D_LOADER_API
VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(
    uint32_t *pPropertyCount, VkLayerProperties *pProperties)
{
    VkResult vkresult = trace3d::EnumerateInstanceLayerProperties(
        pPropertyCount, pProperties);
    return vkresult;
}

TRACE3D_LOADER_API
VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceLayerProperties(
    VkPhysicalDevice physicalDevice, uint32_t *pPropertyCount, VkLayerProperties *pProperties)
{
    VkResult vkresult = trace3d::EnumerateDeviceLayerProperties(
        physicalDevice, pPropertyCount, pProperties);
    return vkresult;
}

TRACE3D_LOADER_API
VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(
    const char *pLayerName, uint32_t *pPropertyCount, VkExtensionProperties *pProperties)
{
    VkResult vkresult = trace3d::EnumerateInstanceExtensionProperties(
        pLayerName, pPropertyCount, pProperties);
    return vkresult;
}

TRACE3D_LOADER_API
VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceExtensionProperties(
    VkPhysicalDevice physicalDevice, const char *pLayerName,
    uint32_t *pPropertyCount, VkExtensionProperties *pProperties)
{
    VkResult vkresult = trace3d::EnumerateDeviceExtensionProperties(
        physicalDevice, pLayerName, pPropertyCount, pProperties);
    return vkresult;
}

// GLES Layer Entries

typedef void (*eglMustCastToProperFunctionPointerType)(void);

typedef eglMustCastToProperFunctionPointerType(*PFNEGLGETNEXTLAYERPROCADDRESSPROC)(void *, const char *funcName);

typedef void*(*PFNEGLGETNEXTLAYERPROCADDRESSPROCOHOS)(void *, const char *funcName);

typedef void* (* DebugLayerInitializeType)(const void *funcTable, PFNEGLGETNEXTLAYERPROCADDRESSPROCOHOS next);
typedef void* (* DebugLayerGetProcAddrType)(const char *funcName, eglMustCastToProperFunctionPointerType next);

DebugLayerInitializeType g_debugLayerInitialize = nullptr;
DebugLayerGetProcAddrType g_debugLayerGetProcAddr = nullptr;

TRACE3D_LOADER_API
void* DebugLayerInitialize(const void *funcTable, PFNEGLGETNEXTLAYERPROCADDRESSPROCOHOS next)
{
    void *ret = nullptr;
    void *libHandle = trace3d::CaptureInit();

    if (libHandle) {
        if (!g_debugLayerInitialize) {
            g_debugLayerInitialize = (DebugLayerInitializeType)dlsym(libHandle, "DebugLayerInitialize");
        }
        if (g_debugLayerInitialize && !g_debugLayerGetProcAddr) {
            g_debugLayerGetProcAddr = (DebugLayerGetProcAddrType)dlsym(libHandle, "DebugLayerGetProcAddr");
            if (!g_debugLayerGetProcAddr) {
                g_debugLayerInitialize = nullptr;
            } else {
                TRACE3D_LOGI("%s:%d init:%p, getProcAddr:%p\n", __FUNCTION__, __LINE__,
                    g_debugLayerInitialize, g_debugLayerGetProcAddr);
            }
        }

        if (g_debugLayerInitialize) {
            ret = g_debugLayerInitialize(funcTable, next);
        }
    }
    TRACE3D_LOGI("%s:%d funcTable:%p next:%p libHandle:%p\n", __FUNCTION__, __LINE__, funcTable, next, libHandle);
    return ret;
}

TRACE3D_LOADER_API
void* DebugLayerGetProcAddr(const char *funcName, eglMustCastToProperFunctionPointerType next)
{
    void *ptr = (void *)next;
    if (g_debugLayerGetProcAddr) {
        ptr = g_debugLayerGetProcAddr(funcName, next);
    }
    return static_cast<void *>(ptr);
}

} // extern "C"
