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

#include "rscanvasnode_fuzzer.h"

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

bool Init(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    DATA = data;
    g_size = size;
    g_pos = 0;
    return true;
}

bool DoCreate()
{
    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create(isRenderServiceNode, isTextureExportNode);
    return true;
}

bool DoBeginRecordingAndFinishRecording()
{
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

bool DoCreateTextureExportRenderNodeInRT()
{
    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create(isRenderServiceNode, isTextureExportNode);
    canvasNode->CreateRenderNodeForTextureExportSwitch();
    return true;
}

bool DoDrawOnNode()
{
    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create(isRenderServiceNode, isTextureExportNode);
    ModifierNG::RSModifierType type = ModifierNG::RSModifierType::INVALID;
    DrawFunc func = [&](std::shared_ptr<Drawing::Canvas>) {};
    canvasNode->DrawOnNode(type, func);
    return true;
}

bool DoGetPaintWidthAndGetPaintHeight()
{
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

bool DoSetFreeze()
{
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

bool DoSetHDRPresent()
{
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

bool DoOnBoundsSizeChanged()
{
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

bool DoSetBoundsChangedCallback()
{
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

bool DoDrawOnNode002()
{
    // test
    bool isRenderServiceNode = GetData<bool>();
    bool isTextureExportNode = GetData<bool>();
    int width = GetData<int>();
    int height = GetData<int>();
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create(isRenderServiceNode, isTextureExportNode);
    ModifierNG::RSModifierType type = ModifierNG::RSModifierType::BOUNDS;
    DrawFunc func = [&](std::shared_ptr<Drawing::Canvas>) {};
    RSCanvasNode rsCanvasNode(isRenderServiceNode, isTextureExportNode);
    rsCanvasNode.DrawOnNode(type, func);
    rsCanvasNode.BeginRecording(width, height);
    rsCanvasNode.IsRecording();
    rsCanvasNode.FinishRecording();
    return true;
}

bool DoSetFreeze002()
{
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
    if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }
    
    /* Run your code on data */
    OHOS::Rosen::DoCreate();
    OHOS::Rosen::DoBeginRecordingAndFinishRecording();
    OHOS::Rosen::DoCreateTextureExportRenderNodeInRT();
    OHOS::Rosen::DoDrawOnNode();
    OHOS::Rosen::DoGetPaintWidthAndGetPaintHeight();
    OHOS::Rosen::DoSetFreeze();
    OHOS::Rosen::DoSetHDRPresent();
    OHOS::Rosen::DoOnBoundsSizeChanged();
    OHOS::Rosen::DoSetBoundsChangedCallback();
    OHOS::Rosen::DoDrawOnNode002();
    OHOS::Rosen::DoSetFreeze002();
    return 0;
}