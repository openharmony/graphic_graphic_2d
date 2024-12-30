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

#include "rsccanvasnode_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "ui/rs_canvas_node.h"
#include "transaction/rs_transaction_proxy.h"
#include "transaction/rs_transaction_data.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(DATA) which size is according to sizeof(T)
 * tips: only support basic type
 */
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

bool DoCreate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create(isRenderServiceNode, isTextureExportNode);
    return true;
}

bool DoBeginRecordingAndFinishRecording(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create(isRenderServiceNode, isTextureExportNode);
    int width = GetData<int>();
    int height = GetData<int>();
    canvasNode->BeginRecording(width, height);
    canvasNode->IsRecording();
    canvasNode->FinishRecording();
    return true;
}

bool DoCreateTextureExportRenderNodeInRT(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create(isRenderServiceNode, isTextureExportNode);
    canvasNode->CreateRenderNodeForTextureExportSwitch();
    return true;
}

bool DoDrawOnNode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create(isRenderServiceNode, isTextureExportNode);
    RSModifierType type = RSModifierType::INVALID;
    DrawFunc func = [&](std::shared_ptr<Drawing::Canvas>) {};
    canvasNode->DrawOnNode(type, func);
    return true;
}

bool DoGetPaintWidthAndGetPaintHeight(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create(isRenderServiceNode, isTextureExportNode);
    canvasNode->GetPaintWidth();
    canvasNode->GetPaintHeight();
    return true;
}

bool DoSetFreeze(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create(isRenderServiceNode, isTextureExportNode);
    bool isFreeze = GetData<bool>();
    canvasNode->SetFreeze(isFreeze);
    return true;
}

bool DoSetHDRPresent(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create(isRenderServiceNode, isTextureExportNode);
    bool hdrPresent = GetData<bool>();
    canvasNode->SetHDRPresent(hdrPresent);
    return true;
}

bool DoOnBoundsSizeChanged(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create(isRenderServiceNode, isTextureExportNode);
    BoundsChangedCallback callback = [](const Rosen::Vector4f& vector4f) {};
    canvasNode->SetBoundsChangedCallback(callback);
    canvasNode->OnBoundsSizeChanged();
    return true;
}

bool DoSetBoundsChangedCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create(isRenderServiceNode, isTextureExportNode);
    BoundsChangedCallback callback = [](const Rosen::Vector4f& vector4f) {};
    canvasNode->SetBoundsChangedCallback(callback);
    return true;
}

bool DoDrawOnNode002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    int width = GetData<int>();
    int height = GetData<int>();
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create(isRenderServiceNode, isTextureExportNode);
    RSModifierType type = RSModifierType::BOUNDS;
    DrawFunc func = [&](std::shared_ptr<Drawing::Canvas>) {};
    RSCanvasNode rsCanvasNode(isRenderServiceNode, isTextureExportNode);
    rsCanvasNode.DrawOnNode(type, func);
    rsCanvasNode.BeginRecording(width, height);
    rsCanvasNode.IsRecording();
    rsCanvasNode.FinishRecording();
    return true;
}

bool DoSetFreeze002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create(isRenderServiceNode, isTextureExportNode);
    bool isFreeze = GetData<bool>();
    canvasNode->SetFreeze(isFreeze);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoCreate(data, size);
    OHOS::Rosen::DoBeginRecordingAndFinishRecording(data, size);
    OHOS::Rosen::DoCreateTextureExportRenderNodeInRT(data, size);
    OHOS::Rosen::DoDrawOnNode(data, size);
    OHOS::Rosen::DoGetPaintWidthAndGetPaintHeight(data, size);
    OHOS::Rosen::DoSetFreeze(data, size);
    OHOS::Rosen::DoSetHDRPresent(data, size);
    OHOS::Rosen::DoOnBoundsSizeChanged(data, size);
    OHOS::Rosen::DoSetBoundsChangedCallback(data, size);
    OHOS::Rosen::DoDrawOnNode002(data, size);
    OHOS::Rosen::DoSetFreeze002(data, size);
    return 0;
}