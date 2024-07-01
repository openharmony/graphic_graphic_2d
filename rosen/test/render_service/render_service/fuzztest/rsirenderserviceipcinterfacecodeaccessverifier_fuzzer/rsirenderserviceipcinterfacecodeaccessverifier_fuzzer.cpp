/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "rsirenderserviceipcinterfacecodeaccessverifier_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "platform/ohos/rs_irender_service_ipc_interface_code_access_verifier.h"
#include "platform/ohos/rs_irender_service_ipc_interface_code.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, DATA + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool RSIRenderServiceInterfaceCodeAccessVerifierFuzztest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;
    
    // get data
    uint32_t code = GetData<CodeUnderlyingType>();

    RSIRenderServiceInterfaceCodeAccessVerifier verifier;
    verifier.IsExclusiveVerificationPassed(code);
#ifdef ENABLE_IPC_SECURITY
    uint32_t times = GetData<uint32_t>();
    PermissionType permission = PermissionType::CAPTURE_SCREEN;
    verifier.permissionRSIRenderServiceInterfaceMappings_.emplace(code, permission);
    verifier.AddRSIRenderServiceConnectionInterfaceCodePermission();
    verifier.IsAccessTimesVerificationPassed(code, times);
#endif
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSIRenderServiceInterfaceCodeAccessVerifierFuzztest(data, size);
    return 0;
}