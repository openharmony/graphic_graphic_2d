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

#include "rsnode_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "ui/rs_canvas_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_node.h"
#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool RSSurfaceNodeFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSSurfaceNodeConfig surfaceNodeConfig;
    std::shared_ptr<RSBaseNode> child = RSCanvasNode::Create();
    int index = GetData<int>();
    GraphicColorGamut colorSpace = GetData<GraphicColorGamut>();
    bool isSecurityLayer = GetData<bool>();
    uint8_t alpha = GetData<uint8_t>();
    bool available = GetData<bool>();
    Parcel parcel;
    bool isAppFreeze = GetData<bool>();
    bool hasContainerWindow = GetData<bool>();
    float density = GetData<float>();

    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);
    if (!surfaceNode) {
        return false;
    }
    surfaceNode->AddChild(child, index);
    surfaceNode->RemoveChild(child);
    surfaceNode->SetColorSpace(colorSpace);
    surfaceNode->SetSecurityLayer(isSecurityLayer);
    surfaceNode->SetAbilityBGAlpha(alpha);
    surfaceNode->SetIsNotifyUIBufferAvailable(available);
    surfaceNode->Marshalling(parcel);
    RSSurfaceNode::Unmarshalling(parcel);
    RSSurfaceNode::UnmarshallingAsProxyNode(parcel);
    surfaceNode->SetFreeze(isAppFreeze);
    surfaceNode->SetContainerWindow(hasContainerWindow, density);

    return true;
}

void RSNodeFuzzTestInner01(std::shared_ptr<RSSurfaceNode> surfaceNode)
{
    Vector4f bounds(GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>());
    Vector4f frame(GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>());
    Vector2f pivot(GetData<float>(), GetData<float>());
    Vector4f cornerRadius(GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>());
    Quaternion quaternion(GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>());
    Vector2f translate(GetData<float>(), GetData<float>());
    Vector2f scale(GetData<float>(), GetData<float>());
    Vector2f skew(GetData<float>(), GetData<float>());
    Vector2f persp(GetData<float>(), GetData<float>());

    surfaceNode->SetBounds(bounds);
    surfaceNode->SetBounds(GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>());
    surfaceNode->SetBoundsWidth(GetData<float>());
    surfaceNode->SetBoundsHeight(GetData<float>());

    surfaceNode->SetFrame(frame);
    surfaceNode->SetFrame(GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>());
    surfaceNode->SetFramePositionX(GetData<float>());
    surfaceNode->SetFramePositionY(GetData<float>());

    surfaceNode->SetPositionZ(GetData<float>());

    surfaceNode->SetPivot(pivot);
    surfaceNode->SetPivot(GetData<float>(), GetData<float>());
    surfaceNode->SetPivotX(GetData<float>());
    surfaceNode->SetPivotY(GetData<float>());

    surfaceNode->SetCornerRadius(GetData<float>());
    surfaceNode->SetCornerRadius(cornerRadius);

    surfaceNode->SetRotation(quaternion);
    surfaceNode->SetRotation(GetData<float>(), GetData<float>(), GetData<float>());
    surfaceNode->SetRotation(GetData<float>());
    surfaceNode->SetRotationX(GetData<float>());
    surfaceNode->SetRotationY(GetData<float>());

    surfaceNode->SetTranslate(translate);
    surfaceNode->SetTranslate(GetData<float>(), GetData<float>(), GetData<float>());
    surfaceNode->SetTranslateX(GetData<float>());
    surfaceNode->SetTranslateY(GetData<float>());
    surfaceNode->SetTranslateZ(GetData<float>());

    surfaceNode->SetScale(GetData<float>());
    surfaceNode->SetScale(GetData<float>(), GetData<float>());
    surfaceNode->SetScale(scale);
    surfaceNode->SetScaleX(GetData<float>());
    surfaceNode->SetScaleY(GetData<float>());

    surfaceNode->SetSkew(GetData<float>());
    surfaceNode->SetSkew(GetData<float>(), GetData<float>());
    surfaceNode->SetSkew(skew);
    surfaceNode->SetSkewX(GetData<float>());
    surfaceNode->SetSkewY(GetData<float>());

    surfaceNode->SetPersp(GetData<float>());
    surfaceNode->SetPersp(GetData<float>(), GetData<float>());
    surfaceNode->SetPersp(persp);
    surfaceNode->SetPerspX(GetData<float>());
    surfaceNode->SetPerspY(GetData<float>());

    surfaceNode->SetAlpha(GetData<float>());
    surfaceNode->SetAlphaOffscreen(GetData<float>());
}

void RSNodeFuzzTestInner02(std::shared_ptr<RSSurfaceNode> surfaceNode)
{
    std::shared_ptr<RSShader> shader = RSShader::CreateRSShader();
    Vector4f width(GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>());
    Vector4<BorderStyle> style(
        GetData<BorderStyle>(), GetData<BorderStyle>(), GetData<BorderStyle>(), GetData<BorderStyle>());

    surfaceNode->SetForegroundColor(GetData<uint32_t>());
    surfaceNode->SetBackgroundColor(GetData<uint32_t>());
    surfaceNode->SetBackgroundShader(shader);

    surfaceNode->SetBgImageSize(GetData<float>(), GetData<float>());
    surfaceNode->SetBgImageWidth(GetData<float>());
    surfaceNode->SetBgImageHeight(GetData<float>());
    surfaceNode->SetBgImagePosition(GetData<float>(), GetData<float>());
    surfaceNode->SetBgImagePositionX(GetData<float>());
    surfaceNode->SetBgImagePositionY(GetData<float>());

    surfaceNode->SetBorderColor(GetData<uint32_t>());
    surfaceNode->SetBorderColor(GetData<uint32_t>(), GetData<uint32_t>(), GetData<uint32_t>(), GetData<uint32_t>());
    surfaceNode->SetBorderWidth(GetData<float>());
    surfaceNode->SetBorderWidth(GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>());
    surfaceNode->SetBorderWidth(width);
    surfaceNode->SetBorderStyle(GetData<uint32_t>());
    surfaceNode->SetBorderStyle(GetData<uint32_t>(), GetData<uint32_t>(), GetData<uint32_t>(), GetData<uint32_t>());
    surfaceNode->SetBorderStyle(style);

    surfaceNode->SetShadowColor(GetData<uint32_t>());
    surfaceNode->SetShadowOffset(GetData<float>(), GetData<float>());
    surfaceNode->SetShadowOffsetX(GetData<float>());
    surfaceNode->SetShadowOffsetY(GetData<float>());
    surfaceNode->SetShadowAlpha(GetData<float>());
    surfaceNode->SetShadowElevation(GetData<float>());
    surfaceNode->SetShadowRadius(GetData<float>());

    surfaceNode->SetFrameGravity(GetData<Gravity>());
    surfaceNode->SetClipToBounds(GetData<bool>());
    surfaceNode->SetClipToFrame(GetData<bool>());

    surfaceNode->SetVisible(GetData<bool>());
    surfaceNode->SetPaintOrder(GetData<bool>());
}

bool RSNodeFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RSSurfaceNodeConfig surfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig);

    if (!surfaceNode) {
        return false;
    }

    RSNodeFuzzTestInner01(surfaceNode);
    RSNodeFuzzTestInner02(surfaceNode);

    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSSurfaceNodeFuzzTest(data, size);
    OHOS::Rosen::RSNodeFuzzTest(data, size);
    return 0;
}
