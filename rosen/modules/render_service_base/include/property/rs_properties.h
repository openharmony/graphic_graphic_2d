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

#ifndef RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_H
#define RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_H

#include <vector>

#include "command/rs_command.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector4.h"
#include "property/rs_properties_def.h"
#include "render/rs_filter.h"
#include "render/rs_image.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"
#include "render/rs_shadow.h"

namespace OHOS {
namespace Rosen {
class RSProperties final {
public:
    RSProperties(bool inRenderNode, NodeId id = 0);
    virtual ~RSProperties();

    // geometry properties
    void SetBoundsWidth(float width, bool sendMsg = true);
    void SetBoundsHeight(float height, bool sendMsg = true);
    void SetBoundsPosition(Vector2f position, bool sendMsg = true);
    void SetBoundsPositionX(float positionX, bool sendMsg = true);
    void SetBoundsPositionY(float positionY, bool sendMsg = true);
    float GetBoundsWidth() const;
    float GetBoundsHeight() const;
    Vector2f GetBoundsPosition() const;
    float GetBoundsPositionX() const;
    float GetBoundsPositionY() const;

    void SetFrameWidth(float width, bool sendMsg = true);
    void SetFrameHeight(float height, bool sendMsg = true);
    void SetFramePosition(Vector2f position, bool sendMsg = true);
    void SetFramePositionX(float positionX, bool sendMsg = true);
    void SetFramePositionY(float positionY, bool sendMsg = true);
    float GetFrameWidth() const;
    float GetFrameHeight() const;
    Vector2f GetFramePosition() const;
    float GetFramePositionX() const;
    float GetFramePositionY() const;
    float GetFrameOffsetX() const;
    float GetFrameOffsetY() const;

    void SetPositionZ(float positionZ, bool sendMsg = true);
    float GetPositionZ() const;

    void SetPivotX(float pivotX, bool sendMsg = true);
    void SetPivotY(float pivotY, bool sendMsg = true);
    float GetPivotX() const;
    float GetPivotY() const;

    void SetCornerRadius(float cornerRadius, bool sendMsg = true);
    float GetCornerRadius() const;

    void SetQuaternion(Vector4f quaternion, bool sendMsg = true);
    void SetRotation(float degree, bool sendMsg = true);
    void SetRotationX(float degree, bool sendMsg = true);
    void SetRotationY(float degree, bool sendMsg = true);
    void SetTranslate(Vector2f translate, bool sendMsg = true);
    void SetTranslateX(float translate, bool sendMsg = true);
    void SetTranslateY(float translate, bool sendMsg = true);
    void SetTranslateZ(float translate, bool sendMsg = true);
    void SetScale(float sx, float sy, bool sendMsg = true);
    void SetScaleX(float sx, bool sendMsg = true);
    void SetScaleY(float sy, bool sendMsg = true);
    Vector4f GetQuaternion() const;
    float GetRotation() const;
    float GetRotationX() const;
    float GetRotationY() const;
    float GetScaleX() const;
    float GetScaleY() const;
    Vector2f GetTranslate() const;
    float GetTranslateX() const;
    float GetTranslateY() const;
    float GetTranslateZ() const;

    void SetAlpha(float alpha, bool sendMsg = true);
    float GetAlpha() const;

    void SetSublayerTransform(Matrix3f sublayerTransform, bool sendMsg = true);
    Matrix3f GetSublayerTransform() const;

    // foreground properties
    void SetForegroundColor(Color color, bool sendMsg = true);
    Color GetForegroundColor() const;

    // background properties
    void SetBackgroundColor(Color color, bool sendMsg = true);
    Color GetBackgroundColor() const;
    void SetBackgroundShader(std::shared_ptr<RSShader> shader, bool sendMsg = true);
    std::shared_ptr<RSShader> GetBackgroundShader() const;
    void SetBgImage(std::shared_ptr<RSImage> image, bool sendMsg = true);
    std::shared_ptr<RSImage> GetBgImage() const;
    void SetBgImageWidth(float width, bool sendMsg = true);
    void SetBgImageHeight(float height, bool sendMsg = true);
    void SetBgImagePositionX(float positionX, bool sendMsg = true);
    void SetBgImagePositionY(float positionY, bool sendMsg = true);
    float GetBgImageWidth() const;
    float GetBgImageHeight() const;
    float GetBgImagePositionX() const;
    float GetBgImagePositionY() const;

    // border properties
    void SetBorderColor(Color color, bool sendMsg = true);
    void SetBorderWidth(float width, bool sendMsg = true);
    void SetBorderStyle(BorderStyle style, bool sendMsg = true);
    Color GetBorderColor() const;
    float GetBorderWidth() const;
    BorderStyle GetBorderStyle() const;

    // filter properties
    void SetBackgroundFilter(std::shared_ptr<RSFilter> backgroundFilter, bool sendMsg = true);
    void SetFilter(std::shared_ptr<RSFilter> filter, bool sendMsg = true);
    std::shared_ptr<RSFilter> GetBackgroundFilter() const;
    std::shared_ptr<RSFilter> GetFilter() const;

    // shadow properties
    void SetShadowColor(Color color, bool sendMsg = true);
    void SetShadowOffsetX(float offsetX, bool sendMsg = true);
    void SetShadowOffsetY(float offsetY, bool sendMsg = true);
    void SetShadowAlpha(float alpha, bool sendMsg = true);
    void SetShadowElevation(float radius, bool sendMsg = true);
    void SetShadowRadius(float radius, bool sendMsg = true);
    void SetShadowPath(std::shared_ptr<RSPath> shadowpath, bool sendMsg = true);
    Color GetShadowColor() const;
    float GetShadowOffsetX() const;
    float GetShadowOffsetY() const;
    float GetShadowAlpha() const;
    float GetShadowElevation() const;
    float GetShadowRadius() const;
    std::shared_ptr<RSPath> GetShadowPath() const;

    void SetFrameGravity(Gravity gravity, bool sendMsg = true);
    Gravity GetFrameGravity() const;

    void SetClipBounds(std::shared_ptr<RSPath> path, bool sendMsg = true);
    std::shared_ptr<RSPath> GetClipBounds() const;
    void SetClipToBounds(bool clipToBounds, bool sendMsg = true);
    bool GetClipToBounds() const;
    void SetClipToFrame(bool clipToFrame, bool sendMsg = true);
    bool GetClipToFrame() const;

    void SetVisible(bool visible, bool sendMsg = true);
    bool GetVisible() const;

private:
    const std::shared_ptr<RSObjGeometry>& GetBoundsGeometry() const;
    const std::shared_ptr<RSObjGeometry>& GetFrameGeometry() const;
    bool UpdateGeometry(const RSProperties* parent, bool dirtyFlag);

    bool SetId(NodeId id);
    void SetDirty();
    void ResetDirty();
    bool IsDirty() const;
    void ResetBounds();

    RectF GetBoundsRect() const;
    RectF GetFrameRect() const;
    RectF GetBgImageRect() const;
    RRect GetRRect() const;
    RRect GetInnerRRect() const;
    RectI GetDirtyRect() const;

    bool NeedFilter() const;
    bool NeedClip() const;
    void SendPropertyCommand(std::unique_ptr<RSCommand>& command);

    bool inRenderNode_;
    bool visible_ = true;
    bool clipToBounds_ = false;
    bool clipToFrame_ = false;
    bool isDirty_ = false;
    bool geoDirty_ = false;

    bool hasBounds_ = false;

    Gravity frameGravity_ = Gravity::DEFAULT;

    float alpha_ = 1.f;

    std::unique_ptr<Matrix3f> sublayerTransform_ = nullptr;
    std::unique_ptr<Decoration> decoration_ = nullptr;
    std::unique_ptr<Border> border_ = nullptr;
    std::unique_ptr<RSShadow> shadow_ = nullptr;

    std::shared_ptr<RSObjGeometry> boundsGeo_;
    std::shared_ptr<RSObjGeometry> frameGeo_;
    std::shared_ptr<RSFilter> backgroundFilter_ = nullptr;
    std::shared_ptr<RSFilter> filter_ = nullptr;
    std::shared_ptr<RSPath> clipPath_ = nullptr;

    NodeId id_;

    friend class RSPropertiesPainter;
    friend class RSTextureRenderNode;
    friend class RSPropertyRenderNode;
    friend class RSRenderNode;
    friend class RSNode;
    friend class RSPropertiesUtils;
    friend class RSRenderTransitionEffect;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_H
