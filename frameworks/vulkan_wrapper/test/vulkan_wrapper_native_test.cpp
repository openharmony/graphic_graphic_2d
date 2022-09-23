/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <iostream>
#include <vector>
#include <dlfcn.h>
#include "directory_ex.h"
#include "vulkan/vulkan.h"

#ifdef __aarch64__
constexpr const char *LIB_CACULATE_PATH = "/system/lib64/libvulkan.so";
#else
constexpr const char *LIB_CACULATE_PATH = "/system/lib/libvulkan.so";
#endif

int32_t main(int32_t argc, const char* argv[])
{
    std::cout << "vulkan wrapper native test is comming :: Loading libvulkan.so..." << std::endl;
    std::string realPath;
    if (!OHOS::PathToRealPath(LIB_CACULATE_PATH, realPath)) {
        std::cout << "file is not real path, file path: " <<  LIB_CACULATE_PATH << std::endl;
        return -1;
    }
    void* libVulkan = dlopen(realPath.c_str(), RTLD_LOCAL | RTLD_NOW);

    if (libVulkan == nullptr) {
        std::cout << "vulkan wrapper native test :: dlopen faild " << std::endl;
        return -1;
    }
    std::cout << "vulkan wrapper native test :: dlopen "<< LIB_CACULATE_PATH <<" success" << std::endl;

    // Test Load base function pointers
    PFN_vkCreateInstance vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(
		dlsym(libVulkan, "vkCreateInstance"));
    if (vkCreateInstance) {
        std::cout << "find vulkan vkCreateInstance success" << std::endl;
    } else {
        std::cout << "find vulkan vkCreateInstance failed" << std::endl;
        return -1;
    }

    std::cout << "vulkan wrapper native test is successful :: Loading libvulkan.so..." << std::endl;
    return 0;
}
