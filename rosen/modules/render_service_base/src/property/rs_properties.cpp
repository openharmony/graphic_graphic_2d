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

#include "property/rs_properties.h"

#include <algorithm>

#include "command/rs_property_node_command.h"
#include "platform/common/rs_log.h"
#include "render/rs_filter.h"
#include "transaction/rs_transaction_proxy.h"
#ifdef ROSEN_OHOS
#include "common/rs_obj_abs_geometry.h"
#else
#include "common/rs_obj_geometry.h"
#endif

namespace OHOS {
namespace Rosen {
RSProperties::RSProperties(bool inRenderNode, NodeId id) : inRenderNode_(inRenderNode), id_(id)
{
#ifdef ROSEN_OHOS
    if (inRenderNode) {
        boundsGeo_ = std::make_shared<RSObjAbsGeometry>();
    } else {
        boundsGeo_ = std::make_shared<RSObjGeometry>();
    }
#else
    boundsGeo_ = std::make_shared<RSObjGeometry>();
#endif
    frameGeo_ = std::make_shared<RSObjGeometry>();
}

RSProperties::~RSProperties() {}

void RSProperties::SetBoundsWidth(float width, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeBoundsWidthChanged>(id_, width);
        SendPropertyCommand(command);
    }
    boundsGeo_->SetWidth(width);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetBoundsHeight(float height, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeBoundsHeightChanged>(id_, height);
        SendPropertyCommand(command);
    }
    boundsGeo_->SetHeight(height);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetBoundsPositionX(float positionX, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeBoundsPositionXChanged>(id_, positionX);
        SendPropertyCommand(command);
    }
    boundsGeo_->SetX(positionX);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetBoundsPositionY(float positionY, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeBoundsPositionYChanged>(id_, positionY);
        SendPropertyCommand(command);
    }
    boundsGeo_->SetY(positionY);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetBoundsPosition(Vector2f position, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> commandX = std::make_unique<RSPropertyNodeBoundsPositionXChanged>(id_, position[0]);
        SendPropertyCommand(commandX);
        std::unique_ptr<RSCommand> commandY = std::make_unique<RSPropertyNodeBoundsPositionYChanged>(id_, position[1]);
        SendPropertyCommand(commandY);
    }
    boundsGeo_->SetX(position[0]);
    boundsGeo_->SetY(position[1]);
    geoDirty_ = true;
    SetDirty();
}

float RSProperties::GetBoundsWidth() const
{
    return boundsGeo_->GetWidth();
}

float RSProperties::GetBoundsHeight() const
{
    return boundsGeo_->GetHeight();
}

float RSProperties::GetBoundsPositionX() const
{
    return boundsGeo_->GetX();
}

float RSProperties::GetBoundsPositionY() const
{
    return boundsGeo_->GetY();
}

Vector2f RSProperties::GetBoundsPosition() const
{
    return Vector2f(GetBoundsPositionX(), GetBoundsPositionY());
}

void RSProperties::SetFrameWidth(float width, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeFrameWidthChanged>(id_, width);
        SendPropertyCommand(command);
    }
    frameGeo_->SetWidth(width);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetFrameHeight(float height, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeFrameHeightChanged>(id_, height);
        SendPropertyCommand(command);
    }
    frameGeo_->SetHeight(height);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetFramePositionX(float positionX, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeFramePositionXChanged>(id_, positionX);
        SendPropertyCommand(command);
    }
    frameGeo_->SetX(positionX);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetFramePositionY(float positionY, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeFramePositionYChanged>(id_, positionY);
        SendPropertyCommand(command);
    }
    frameGeo_->SetY(positionY);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetFramePosition(Vector2f position, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> commandX = std::make_unique<RSPropertyNodeFramePositionXChanged>(id_, position[0]);
        SendPropertyCommand(commandX);
        std::unique_ptr<RSCommand> commandY = std::make_unique<RSPropertyNodeFramePositionYChanged>(id_, position[1]);
        SendPropertyCommand(commandY);
    }
    frameGeo_->SetX(position[0]);
    frameGeo_->SetY(position[1]);
    geoDirty_ = true;
    SetDirty();
}

float RSProperties::GetFrameWidth() const
{
    return frameGeo_->GetWidth();
}

float RSProperties::GetFrameHeight() const
{
    return frameGeo_->GetHeight();
}

float RSProperties::GetFramePositionX() const
{
    return frameGeo_->GetX();
}

float RSProperties::GetFramePositionY() const
{
    return frameGeo_->GetY();
}

Vector2f RSProperties::GetFramePosition() const
{
    return Vector2f(GetFramePositionX(), GetFramePositionY());
}

float RSProperties::GetFrameOffsetX() const
{
    return frameGeo_->GetX() - boundsGeo_->GetX();
}

float RSProperties::GetFrameOffsetY() const
{
    return frameGeo_->GetY() - boundsGeo_->GetY();
}

const std::shared_ptr<RSObjGeometry>& RSProperties::GetBoundsGeometry() const
{
    return boundsGeo_;
}

const std::shared_ptr<RSObjGeometry>& RSProperties::GetFrameGeometry() const
{
    return frameGeo_;
}

bool RSProperties::UpdateGeometry(const RSProperties* parent, bool dirtyFlag)
{
    if (boundsGeo_ == nullptr) {
        return false;
    }
    if (boundsGeo_->IsEmpty()) {
        boundsGeo_->SetPosition(frameGeo_->GetX(), frameGeo_->GetY());
        boundsGeo_->SetSize(frameGeo_->GetWidth(), frameGeo_->GetHeight());
        hasBounds_ = false;
    } else {
        hasBounds_ = true;
    }
#ifdef ROSEN_OHOS
    auto boundsGeoPtr = std::static_pointer_cast<RSObjAbsGeometry>(boundsGeo_);

    if (dirtyFlag || geoDirty_) {
        if (parent == nullptr) {
            boundsGeoPtr->UpdateMatrix(nullptr, 0.f, 0.f);
        } else {
            auto parentGeo = std::static_pointer_cast<RSObjAbsGeometry>(parent->boundsGeo_);
            boundsGeoPtr->UpdateMatrix(parentGeo, parent->GetFrameOffsetX(), parent->GetFrameOffsetY());
        }
    }
#endif
    return dirtyFlag || geoDirty_;
}

void RSProperties::SetPositionZ(float positionZ, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodePositionZChanged>(id_, positionZ);
        SendPropertyCommand(command);
    }
    boundsGeo_->SetZ(positionZ);
    frameGeo_->SetZ(positionZ);
    geoDirty_ = true;
    SetDirty();
}

float RSProperties::GetPositionZ() const
{
    return boundsGeo_->GetZ();
}

void RSProperties::SetPivotX(float pivotX, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodePivotXChanged>(id_, pivotX);
        SendPropertyCommand(command);
    }
    boundsGeo_->SetPivotX(pivotX);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetPivotY(float pivotY, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodePivotYChanged>(id_, pivotY);
        SendPropertyCommand(command);
    }
    boundsGeo_->SetPivotY(pivotY);
    geoDirty_ = true;
    SetDirty();
}

float RSProperties::GetPivotX() const
{
    return boundsGeo_->GetPivotX();
}

float RSProperties::GetPivotY() const
{
    return boundsGeo_->GetPivotY();
}

void RSProperties::SetCornerRadius(float cornerRadius, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeCornerRadiusChanged>(id_, cornerRadius);
        SendPropertyCommand(command);
    }
    if (!border_) {
        border_ = std::make_unique<Border>();
    }
    border_->cornerRadius_ = cornerRadius;
    SetDirty();
}

float RSProperties::GetCornerRadius() const
{
    return border_ ? border_->cornerRadius_ : 0.f;
}

void RSProperties::SetQuaternion(Vector4f quaternion, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeQuaternionChanged>(id_, quaternion);
        SendPropertyCommand(command);
    }
    boundsGeo_->SetQuaternion(quaternion);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetRotation(float degree, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeRotationChanged>(id_, degree);
        SendPropertyCommand(command);
    }
    boundsGeo_->SetRotation(degree);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetRotationX(float degree, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeRotationXChanged>(id_, degree);
        SendPropertyCommand(command);
    }
    boundsGeo_->SetRotationX(degree);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetRotationY(float degree, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeRotationYChanged>(id_, degree);
        SendPropertyCommand(command);
    }
    boundsGeo_->SetRotationY(degree);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetScaleX(float sx, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeScaleXChanged>(id_, sx);
        SendPropertyCommand(command);
    }
    boundsGeo_->SetScaleX(sx);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetScaleY(float sy, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeScaleYChanged>(id_, sy);
        SendPropertyCommand(command);
    }
    boundsGeo_->SetScaleY(sy);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetTranslate(Vector2f translate, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> commandTranslateX =
            std::make_unique<RSPropertyNodeTranslateXChanged>(id_, translate[0]);
        SendPropertyCommand(commandTranslateX);
        std::unique_ptr<RSCommand> commandTranslateY =
            std::make_unique<RSPropertyNodeTranslateYChanged>(id_, translate[1]);
        SendPropertyCommand(commandTranslateY);
    }
    boundsGeo_->SetTranslateX(translate[0]);
    boundsGeo_->SetTranslateY(translate[1]);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetTranslateX(float translate, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeTranslateXChanged>(id_, translate);
        SendPropertyCommand(command);
    }
    boundsGeo_->SetTranslateX(translate);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetTranslateY(float translate, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeTranslateYChanged>(id_, translate);
        SendPropertyCommand(command);
    }
    boundsGeo_->SetTranslateY(translate);
    geoDirty_ = true;
    SetDirty();
}

void RSProperties::SetTranslateZ(float translate, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeTranslateZChanged>(id_, translate);
        SendPropertyCommand(command);
    }
    boundsGeo_->SetTranslateZ(translate);
    geoDirty_ = true;
    SetDirty();
}

Vector4f RSProperties::GetQuaternion() const
{
    return boundsGeo_->GetQuaternion();
}

float RSProperties::GetRotation() const
{
    return boundsGeo_->GetRotation();
}

float RSProperties::GetRotationX() const
{
    return boundsGeo_->GetRotationX();
}

float RSProperties::GetRotationY() const
{
    return boundsGeo_->GetRotationY();
}

float RSProperties::GetScaleX() const
{
    return boundsGeo_->GetScaleX();
}

float RSProperties::GetScaleY() const
{
    return boundsGeo_->GetScaleY();
}

Vector2f RSProperties::GetTranslate() const
{
    return Vector2f(GetTranslateX(), GetTranslateY());
}

float RSProperties::GetTranslateX() const
{
    return boundsGeo_->GetTranslateX();
}

float RSProperties::GetTranslateY() const
{
    return boundsGeo_->GetTranslateY();
}

float RSProperties::GetTranslateZ() const
{
    return boundsGeo_->GetTranslateZ();
}

void RSProperties::SetAlpha(float alpha, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeAlphaChanged>(id_, alpha);
        SendPropertyCommand(command);
    }
    alpha_ = alpha;
    SetDirty();
}

float RSProperties::GetAlpha() const
{
    return alpha_;
}

void RSProperties::SetSublayerTransform(Matrix3f sublayerTransform, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command =
            std::make_unique<RSPropertyNodeSublayerTransformChanged>(id_, sublayerTransform);
        SendPropertyCommand(command);
    }
    if (sublayerTransform_) {
        *sublayerTransform_ = sublayerTransform;
    } else {
        sublayerTransform_ = std::make_unique<Matrix3f>(sublayerTransform);
    }
    SetDirty();
}

Matrix3f RSProperties::GetSublayerTransform() const
{
    return (sublayerTransform_ != nullptr) ? *sublayerTransform_ : Matrix3f::IDENTITY;
}

// foreground properties
void RSProperties::SetForegroundColor(Color color, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeForegroundColorChanged>(id_, color);
        SendPropertyCommand(command);
    }
    if (!decoration_) {
        decoration_ = std::make_unique<Decoration>();
    }
    decoration_->foregroundColor_ = color;
    SetDirty();
}

Color RSProperties::GetForegroundColor() const
{
    return decoration_ ? decoration_->foregroundColor_ : RgbPalette::Transparent();
}

// background properties
void RSProperties::SetBackgroundColor(Color color, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeBackgroundColorChanged>(id_, color);
        SendPropertyCommand(command);
    }
    if (!decoration_) {
        decoration_ = std::make_unique<Decoration>();
    }
    decoration_->backgroundColor_ = color;
    SetDirty();
}

Color RSProperties::GetBackgroundColor() const
{
    return decoration_ ? decoration_->backgroundColor_ : RgbPalette::Transparent();
}

void RSProperties::SetBackgroundShader(std::shared_ptr<RSShader> shader, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeBackgroundShaderChanged>(id_, shader);
        SendPropertyCommand(command);
    }
    if (!decoration_) {
        decoration_ = std::make_unique<Decoration>();
    }
    decoration_->bgShader_ = shader;
    SetDirty();
}

std::shared_ptr<RSShader> RSProperties::GetBackgroundShader() const
{
    return decoration_ ? decoration_->bgShader_ : nullptr;
}

void RSProperties::SetBgImage(std::shared_ptr<RSImage> image, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeBgImageChanged>(id_, image);
        SendPropertyCommand(command);
    }
    if (!decoration_) {
        decoration_ = std::make_unique<Decoration>();
    }
    decoration_->bgImage_ = image;
    SetDirty();
}

std::shared_ptr<RSImage> RSProperties::GetBgImage() const
{
    return decoration_ ? decoration_->bgImage_ : nullptr;
}

void RSProperties::SetBgImageWidth(float width, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeBgImageWidthChanged>(id_, width);
        SendPropertyCommand(command);
    }
    if (!decoration_) {
        decoration_ = std::make_unique<Decoration>();
    }
    decoration_->bgImageRect_.width_ = width;
    SetDirty();
}

void RSProperties::SetBgImageHeight(float height, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeBgImageHeightChanged>(id_, height);
        SendPropertyCommand(command);
    }
    if (!decoration_) {
        decoration_ = std::make_unique<Decoration>();
    }
    decoration_->bgImageRect_.height_ = height;
    SetDirty();
}

void RSProperties::SetBgImagePositionX(float positionX, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command =
            std::make_unique<RSPropertyNodeBgImagePositionXChanged>(id_, positionX);
        SendPropertyCommand(command);
    }
    if (!decoration_) {
        decoration_ = std::make_unique<Decoration>();
    }
    decoration_->bgImageRect_.left_ = positionX;
    SetDirty();
}

void RSProperties::SetBgImagePositionY(float positionY, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command =
            std::make_unique<RSPropertyNodeBgImagePositionYChanged>(id_, positionY);
        SendPropertyCommand(command);
    }
    if (!decoration_) {
        decoration_ = std::make_unique<Decoration>();
    }
    decoration_->bgImageRect_.top_ = positionY;
    SetDirty();
}

float RSProperties::GetBgImageWidth() const
{
    return decoration_ ? decoration_->bgImageRect_.width_ : 0.f;
}

float RSProperties::GetBgImageHeight() const
{
    return decoration_ ? decoration_->bgImageRect_.height_ : 0.f;
}

float RSProperties::GetBgImagePositionX() const
{
    return decoration_ ? decoration_->bgImageRect_.left_ : 0.f;
}

float RSProperties::GetBgImagePositionY() const
{
    return decoration_ ? decoration_->bgImageRect_.top_ : 0.f;
}

// border properties
void RSProperties::SetBorderColor(Color color, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeBorderColorChanged>(id_, color);
        SendPropertyCommand(command);
    }
    if (!border_) {
        border_ = std::make_unique<Border>();
    }
    border_->borderColor_ = color;
    SetDirty();
}

void RSProperties::SetBorderWidth(float width, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeBorderWidthChanged>(id_, width);
        SendPropertyCommand(command);
    }
    if (!border_) {
        border_ = std::make_unique<Border>();
    }
    border_->borderWidth_ = width;
    SetDirty();
}

void RSProperties::SetBorderStyle(BorderStyle style, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeBorderStyleChanged>(id_, style);
        SendPropertyCommand(command);
    }
    if (!border_) {
        border_ = std::make_unique<Border>();
    }
    border_->borderStyle_ = style;
    SetDirty();
}

Color RSProperties::GetBorderColor() const
{
    return border_ ? border_->borderColor_ : RgbPalette::Transparent();
}

float RSProperties::GetBorderWidth() const
{
    return border_ ? border_->borderWidth_ : 0.f;
}

BorderStyle RSProperties::GetBorderStyle() const
{
    return border_ ? border_->borderStyle_ : BorderStyle::SOLID;
}

void RSProperties::SetBackgroundFilter(std::shared_ptr<RSFilter> backgroundFilter, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command =
            std::make_unique<RSPropertyNodeBackgroundFilterChanged>(id_, backgroundFilter);
        SendPropertyCommand(command);
    }
    backgroundFilter_ = backgroundFilter;
    SetDirty();
}

void RSProperties::SetFilter(std::shared_ptr<RSFilter> filter, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeFilterChanged>(id_, filter);
        SendPropertyCommand(command);
    }
    filter_ = filter;
    SetDirty();
}

std::shared_ptr<RSFilter> RSProperties::GetBackgroundFilter() const
{
    return backgroundFilter_;
}

std::shared_ptr<RSFilter> RSProperties::GetFilter() const
{
    return filter_;
}

// shadow properties
void RSProperties::SetShadowColor(Color color, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeShadowColorChanged>(id_, color);
        SendPropertyCommand(command);
    }
    if (shadow_ == nullptr) {
        shadow_ = std::make_unique<RSShadow>();
    }
    shadow_->SetColor(color);
    SetDirty();
}

void RSProperties::SetShadowOffsetX(float offsetX, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeShadowOffsetXChanged>(id_, offsetX);
        SendPropertyCommand(command);
    }
    if (shadow_ == nullptr) {
        shadow_ = std::make_unique<RSShadow>();
    }
    shadow_->SetOffsetX(offsetX);
    SetDirty();
}

void RSProperties::SetShadowOffsetY(float offsetY, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeShadowOffsetYChanged>(id_, offsetY);
        SendPropertyCommand(command);
    }
    if (shadow_ == nullptr) {
        shadow_ = std::make_unique<RSShadow>();
    }
    shadow_->SetOffsetY(offsetY);
    SetDirty();
}

void RSProperties::SetShadowAlpha(float alpha, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeShadowAlphaChanged>(id_, alpha);
        SendPropertyCommand(command);
    }
    if (shadow_ == nullptr) {
        shadow_ = std::make_unique<RSShadow>();
    }
    shadow_->SetAlpha(alpha);
    SetDirty();
}

void RSProperties::SetShadowElevation(float elevation, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeShadowElevationChanged>(id_, elevation);
        SendPropertyCommand(command);
    }
    if (shadow_ == nullptr) {
        shadow_ = std::make_unique<RSShadow>();
    }
    shadow_->SetElevation(elevation);
    SetDirty();
}

void RSProperties::SetShadowRadius(float radius, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeShadowRadiusChanged>(id_, radius);
        SendPropertyCommand(command);
    }
    if (shadow_ == nullptr) {
        shadow_ = std::make_unique<RSShadow>();
    }
    shadow_->SetRadius(radius);
    SetDirty();
}

void RSProperties::SetShadowPath(std::shared_ptr<RSPath> shadowPath, bool sendMsg)
{
    if (sendMsg) {
        // send a Command
        std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeShadowPathChanged>(id_, shadowPath);
        SendPropertyCommand(command);
    }
    if (shadow_ == nullptr) {
        shadow_ = std::make_unique<RSShadow>();
    }
    shadow_->SetPath(shadowPath);
    SetDirty();
}

Color RSProperties::GetShadowColor() const
{
    return shadow_ ? shadow_->GetColor() : Color(DEFAULT_SPOT_COLOR);
}

float RSProperties::GetShadowOffsetX() const
{
    return shadow_ ? shadow_->GetOffsetX() : DEFAULT_SHADOW_OFFSET_X;
}

float RSProperties::GetShadowOffsetY() const
{
    return shadow_ ? shadow_->GetOffsetY() : DEFAULT_SHADOW_OFFSET_Y;
}

float RSProperties::GetShadowAlpha() const
{
    return shadow_ ? shadow_->GetAlpha() : 0.f;
}

float RSProperties::GetShadowElevation() const
{
    return shadow_ ? shadow_->GetElevation() : 0.f;
}

float RSProperties::GetShadowRadius() const
{
    return shadow_ ? shadow_->GetRadius() : 0.f;
}

std::shared_ptr<RSPath> RSProperties::GetShadowPath() const
{
    return shadow_ ? shadow_->GetPath() : nullptr;
}

void RSProperties::SetFrameGravity(Gravity gravity, bool sendMsg)
{
    if (frameGravity_ != gravity) {
        if (sendMsg) {
            // send a Command
            std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeFrameGravityChanged>(id_, gravity);
            SendPropertyCommand(command);
        }
        frameGravity_ = gravity;
        SetDirty();
    }
}

Gravity RSProperties::GetFrameGravity() const
{
    return frameGravity_;
}

void RSProperties::SetClipBounds(std::shared_ptr<RSPath> path, bool sendMsg)
{
    if (clipPath_ != path) {
        if (sendMsg) {
            // send a Command
            std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeClipBoundsChanged>(id_, path);
            SendPropertyCommand(command);
        }
        clipPath_ = path;
        SetDirty();
    }
}

std::shared_ptr<RSPath> RSProperties::GetClipBounds() const
{
    return clipPath_;
}

void RSProperties::SetClipToBounds(bool clipToBounds, bool sendMsg)
{
    if (clipToBounds_ != clipToBounds) {
        if (sendMsg) {
            // send a Command
            std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeClipToBoundsChanged>(id_, clipToBounds);
            SendPropertyCommand(command);
        }
        clipToBounds_ = clipToBounds;
        SetDirty();
    }
}

bool RSProperties::GetClipToBounds() const
{
    return clipToBounds_;
}

void RSProperties::SetClipToFrame(bool clipToFrame, bool sendMsg)
{
    if (clipToFrame_ != clipToFrame) {
        if (sendMsg) {
            // send a Command
            std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeClipToFrameChanged>(id_, clipToFrame);
            SendPropertyCommand(command);
        }
        clipToFrame_ = clipToFrame;
        SetDirty();
    }
}

bool RSProperties::GetClipToFrame() const
{
    return clipToFrame_;
}

RectF RSProperties::GetBoundsRect() const
{
    if (boundsGeo_->IsEmpty()) {
        return RectF(0, 0, GetFrameWidth(), GetFrameHeight());
    } else {
        return RectF(0, 0, GetBoundsWidth(), GetBoundsHeight());
    }
}

RectF RSProperties::GetFrameRect() const
{
    return RectF(0, 0, GetFrameWidth(), GetFrameHeight());
}

RectF RSProperties::GetBgImageRect() const
{
    return decoration_ ? decoration_->bgImageRect_ : RectF();
}

void RSProperties::SetVisible(bool visible, bool sendMsg)
{
    if (visible_ != visible) {
        if (sendMsg) {
            // send a Command
            std::unique_ptr<RSCommand> command = std::make_unique<RSPropertyNodeVisibleChanged>(id_, visible);
            SendPropertyCommand(command);
        }
        visible_ = visible;
        SetDirty();
    }
}

bool RSProperties::GetVisible() const
{
    return visible_;
}

RRect RSProperties::GetRRect() const
{
    RectF rect = GetBoundsRect();
    RRect rrect = RRect(rect, GetCornerRadius(), GetCornerRadius());
    return rrect;
}

RRect RSProperties::GetInnerRRect() const
{
    auto rect = GetBoundsRect();
    float borderWidth = GetBorderWidth();
    rect.SetAll(rect.left_ + borderWidth, rect.top_ + borderWidth, rect.width_ - borderWidth * 2,
        rect.height_ - borderWidth * 2);
    float cornerRadius = std::max(0.0f, GetCornerRadius() - borderWidth);
    RRect rrect = RRect(rect, cornerRadius, cornerRadius);
    return rrect;
}

bool RSProperties::NeedFilter() const
{
    return (backgroundFilter_ != nullptr) || (filter_ != nullptr);
}

bool RSProperties::NeedClip() const
{
    return clipToBounds_ || clipToFrame_;
}

void RSProperties::SendPropertyCommand(std::unique_ptr<RSCommand>& command)
{
    if (inRenderNode_) {
        // todo send cmds to ui
        // RSMessageProcessor::Instance().AddUIMessage(msg);
    } else {
        RSTransactionProxy::GetInstance().AddCommand(command);
    }
}

bool RSProperties::SetId(NodeId id)
{
    if (id_ != 0) {
        return false;
    }
    id_ = id;
    return true;
}

void RSProperties::SetDirty()
{
    isDirty_ = true;
}

void RSProperties::ResetDirty()
{
    isDirty_ = false;
    geoDirty_ = false;
}

bool RSProperties::IsDirty() const
{
    return isDirty_;
}

RectI RSProperties::GetDirtyRect() const
{
#ifdef ROSEN_OHOS
    auto boundsGeometry = std::static_pointer_cast<RSObjAbsGeometry>(boundsGeo_);
    if (clipToBounds_) {
        return boundsGeometry->GetAbsRect();
    } else {
        auto frameRect =
            boundsGeometry->MapAbsRect(RectF(GetFrameOffsetX(), GetFrameOffsetY(), GetFrameWidth(), GetFrameHeight()));
        return boundsGeometry->GetAbsRect().JoinRect(frameRect);
    }
#else
    return RectI();
#endif
}

void RSProperties::ResetBounds()
{
    if (!hasBounds_) {
        boundsGeo_->SetRect(0.f, 0.f, 0.f, 0.f);
    }
}
} // namespace Rosen
} // namespace OHOS
