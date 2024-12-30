/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "modifier_ng/overlay/rs_point_light_render_modifier.h"

#include "drawable/rs_property_drawable_utils.h"
#include "platform/common/rs_log.h"
#include "property/rs_point_light_manager.h"
#include "property/rs_properties.h"

namespace OHOS::Rosen::ModifierNG {
const RSPointLightRenderModifier::LegacyPropertyApplierMap RSPointLightRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::LIGHT_INTENSITY, RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetLightIntensity> },
    { RSPropertyType::LIGHT_COLOR, RSRenderModifier::PropertyApplyHelper<Color, &RSProperties::SetLightColor> },
    { RSPropertyType::LIGHT_POSITION,
        RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetLightPosition> },
    { RSPropertyType::ILLUMINATED_BORDER_WIDTH,
        RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetIlluminatedBorderWidth> },
    { RSPropertyType::ILLUMINATED_TYPE, RSRenderModifier::PropertyApplyHelper<int, &RSProperties::SetIlluminatedType> },
    { RSPropertyType::BLOOM, RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetBloom> },
};

void RSPointLightRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetLightIntensity(-1.f);
    properties.SetLightColor({});
    properties.SetLightPosition({});
    properties.SetIlluminatedBorderWidth({});
    properties.SetIlluminatedType(-1);
    properties.SetBloom({});
}

void RSPointLightRenderModifier::Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect)
{
    DrawLight(&canvas);
}

bool RSPointLightRenderModifier::OnApply(RSModifierContext& context)
{
    float lightIntensity { 0.0f };

    lightIntensity = Getter(RSPropertyType::LIGHT_INTENSITY, 0.0f);
    stagingBorderWidth_ = Getter(RSPropertyType::ILLUMINATED_BORDER_WIDTH, 0.0f);
    stagingIlluminatedType_ = Getter(RSPropertyType::ILLUMINATED_TYPE, IlluminatedType::INVALID);
    stagingBloomIntensity_ = Getter(RSPropertyType::BLOOM, 0.0f);

    // OnApplyHelper(RSPropertyType::LIGHT_INTENSITY, lightIntensity, 0.0f);
    // OnApplyHelper(RSPropertyType::ILLUMINATED_BORDER_WIDTH, stagingBorderWidth_, 0.0f);
    // OnApplyHelper<RSRenderProperty, IlluminatedType>(RSPropertyType::ILLUMINATED_TYPE, stagingIlluminatedType_,
    // IlluminatedType::INVALID); OnApplyHelper(RSPropertyType::BLOOM, stagingBloomIntensity_, 0.0f);

    if (!lightSourcePtr_) {
        lightSourcePtr_ = std::make_shared<RSLightSource>();
    }
    if (!illuminatedPtr_) {
        illuminatedPtr_ = std::make_shared<RSIlluminated>();
    }
    lightSourcePtr_->SetLightColor(Getter(RSPropertyType::LIGHT_COLOR, Color()));
    lightSourcePtr_->SetLightPosition(Getter(RSPropertyType::LIGHT_POSITION, Vector4f()));
    illuminatedPtr_->SetIlluminatedBorderWidth(stagingBorderWidth_);
    illuminatedPtr_->SetBloomIntensity(stagingBloomIntensity_);

    // Illumination Type
    auto curIlluminateType = IlluminatedType(stagingIlluminatedType_);
    illuminatedPtr_->SetIlluminatedType(curIlluminateType);
    auto renderNode = target_.lock();
    auto preIlluminatedType = illuminatedPtr_->GetPreIlluminatedType();
    bool preTypeIsNone = preIlluminatedType == IlluminatedType::NONE;
    bool curTypeIsNone = curIlluminateType == IlluminatedType::NONE;
    if (preTypeIsNone && !curTypeIsNone) {
        RSPointLightManager::Instance()->RegisterIlluminated(renderNode);
    } else if (!preTypeIsNone && curTypeIsNone) {
        RSPointLightManager::Instance()->UnRegisterIlluminated(renderNode);
    }

    // Light Intensity
    lightSourcePtr_->SetLightIntensity(lightIntensity);
    if (ROSEN_EQ(lightIntensity, INVALID_INTENSITY)) { // skip when resetFunc call
        return false;
    }
    auto preIntensity = lightSourcePtr_->GetPreLightIntensity();
    bool preIntensityIsZero = ROSEN_EQ(preIntensity, 0.f);
    bool curIntensityIsZero = ROSEN_EQ(lightIntensity, 0.f);
    if (preIntensityIsZero && !curIntensityIsZero) { // 0 --> non-zero
        RSPointLightManager::Instance()->RegisterLightSource(renderNode);
    } else if (!preIntensityIsZero && curIntensityIsZero) { // non-zero --> 0
        RSPointLightManager::Instance()->UnRegisterLightSource(renderNode);
    }

    rrect_ = context.GetRRect();

    if (!illuminatedPtr_ || !illuminatedPtr_->IsIlluminatedValid()) {
        return false;
    }

    return true;
}

void RSPointLightRenderModifier::OnSync()
{
    renderLightSourcesAndPosVec_.clear();
    const auto& lightSourcesAndPosMap = illuminatedPtr_->GetLightSourcesAndPosMap();
    for (auto& pair : lightSourcesAndPosMap) {
        renderLightSourcesAndPosVec_.push_back(pair);
    }
    illuminatedPtr_->ClearLightSourcesAndPosMap();
    if (renderLightSourcesAndPosVec_.empty()) {
        return;
    }

    if (renderLightSourcesAndPosVec_.size() > MAX_LIGHT_SOURCES) {
        std::sort(
            renderLightSourcesAndPosVec_.begin(), renderLightSourcesAndPosVec_.end(), [](const auto& x, const auto& y) {
                return x.second.x_ * x.second.x_ + x.second.y_ * x.second.y_ <
                       y.second.x_ * y.second.x_ + y.second.y_ * y.second.y_;
            });
    }
    renderIlluminatedType_ = stagingIlluminatedType_;
    renderBorderWidth_ = std::ceil(stagingBorderWidth_);
    renderRect_ = stagingRect_;

    if (renderIlluminatedType_ == IlluminatedType::BORDER_CONTENT ||
        renderIlluminatedType_ == IlluminatedType::BORDER || renderIlluminatedType_ == IlluminatedType::BLEND_BORDER ||
        renderIlluminatedType_ == IlluminatedType::BLEND_BORDER_CONTENT) {
        // half width and half height requires divide by 2.0f
        Vector4f width = { renderBorderWidth_ / 2.0f };
        auto borderRRect = rrect_.Inset(width);
        renderBorderRRect_ = RSPropertyDrawableUtils::RRect2DrawingRRect(borderRRect);
    }
    if (renderIlluminatedType_ == IlluminatedType::BORDER_CONTENT ||
        renderIlluminatedType_ == IlluminatedType::CONTENT ||
        renderIlluminatedType_ == IlluminatedType::BLEND_CONTENT ||
        renderIlluminatedType_ == IlluminatedType::BLEND_BORDER_CONTENT) {
        renderContentRRect_ = RSPropertyDrawableUtils::RRect2DrawingRRect(rrect_);
    }

    renderBloomIntensity_ = stagingBloomIntensity_;
    // TODO: we may need to extract this bounds geometry from the ModifierContext
    // if (properties_.GetBoundsGeometry()) {
    //     rect_ = properties_.GetBoundsGeometry()->GetAbsRect();
    // }
}

/*
 * Private Methods
 */
void RSPointLightRenderModifier::DrawLight(Drawing::Canvas* canvas) const
{
    if (renderLightSourcesAndPosVec_.empty()) {
        return;
    }
    auto phongShaderBuilder = GetPhongShaderBuilder();
    if (!phongShaderBuilder) {
        return;
    }
    constexpr int vectorLen = 4;
    float lightPosArray[vectorLen * MAX_LIGHT_SOURCES] = { 0 };
    float viewPosArray[vectorLen * MAX_LIGHT_SOURCES] = { 0 };
    float lightColorArray[vectorLen * MAX_LIGHT_SOURCES] = { 0 };
    std::array<float, MAX_LIGHT_SOURCES> lightIntensityArray = { 0 };

    auto iter = renderLightSourcesAndPosVec_.begin();
    auto cnt = 0;
    while (iter != renderLightSourcesAndPosVec_.end() && cnt < MAX_LIGHT_SOURCES) {
        auto lightPos = iter->second;
        auto lightIntensity = iter->first->GetLightIntensity();
        auto lightColor = iter->first->GetLightColor();
        Vector4f lightColorVec =
            Vector4f(lightColor.GetRed(), lightColor.GetGreen(), lightColor.GetBlue(), lightColor.GetAlpha());
        for (int i = 0; i < vectorLen; i++) {
            lightPosArray[cnt * vectorLen + i] = lightPos[i];
            viewPosArray[cnt * vectorLen + i] = lightPos[i];
            lightColorArray[cnt * vectorLen + i] = lightColorVec[i] / UINT8_MAX;
        }
        lightIntensityArray[cnt] = lightIntensity;
        iter++;
        cnt++;
    }
    phongShaderBuilder->SetUniform("lightPos", lightPosArray, vectorLen * MAX_LIGHT_SOURCES);
    phongShaderBuilder->SetUniform("viewPos", viewPosArray, vectorLen * MAX_LIGHT_SOURCES);
    phongShaderBuilder->SetUniform("specularLightColor", lightColorArray, vectorLen * MAX_LIGHT_SOURCES);
    Drawing::Pen pen;
    Drawing::Brush brush;
    pen.SetAntiAlias(true);
    brush.SetAntiAlias(true);
    ROSEN_LOGD("RSPointLightDrawable::DrawLight illuminatedType:%{public}d", renderIlluminatedType_);
    if ((renderIlluminatedType_ == IlluminatedType::BORDER_CONTENT) ||
        (renderIlluminatedType_ == IlluminatedType::BLEND_BORDER_CONTENT)) {
        DrawContentLight(*canvas, phongShaderBuilder, brush, lightIntensityArray);
        DrawBorderLight(*canvas, phongShaderBuilder, pen, lightIntensityArray);
    } else if ((renderIlluminatedType_ == IlluminatedType::CONTENT) ||
               (renderIlluminatedType_ == IlluminatedType::BLEND_CONTENT)) {
        DrawContentLight(*canvas, phongShaderBuilder, brush, lightIntensityArray);
    } else if ((renderIlluminatedType_ == IlluminatedType::BORDER) ||
               (renderIlluminatedType_ == IlluminatedType::BLEND_BORDER)) {
        DrawBorderLight(*canvas, phongShaderBuilder, pen, lightIntensityArray);
    }
}

const std::shared_ptr<Drawing::RuntimeShaderBuilder>& RSPointLightRenderModifier::GetPhongShaderBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> phongShaderBuilder;
    if (phongShaderBuilder) {
        return phongShaderBuilder;
    }
    std::shared_ptr<Drawing::RuntimeEffect> lightEffect;
    const static std::string lightString(R"(
        uniform vec4 lightPos[12];
        uniform vec4 viewPos[12];
        uniform vec4 specularLightColor[12];
        uniform float specularStrength[12];

        mediump vec4 main(vec2 drawing_coord) {
            vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
            float ambientStrength = 0.0;
            vec4 diffuseColor = vec4(1.0, 1.0, 1.0, 1.0);
            float diffuseStrength = 0.0;
            float shininess = 8.0;
            mediump vec4 fragColor = vec4(0.0, 0.0, 0.0, 0.0);
            vec4 NormalMap = vec4(0.0, 0.0, 1.0, 0.0);
            // ambient
            vec4 ambient = lightColor * ambientStrength;
            vec3 norm = normalize(NormalMap.rgb);

            for (int i = 0; i < 12; i++) {
                if (abs(specularStrength[i]) > 0.01) {
                    vec3 lightDir = normalize(vec3(lightPos[i].xy - drawing_coord, lightPos[i].z));
                    float diff = max(dot(norm, lightDir), 0.0);
                    vec4 diffuse = diff * lightColor;
                    vec3 viewDir = normalize(vec3(viewPos[i].xy - drawing_coord, viewPos[i].z)); // view vector
                    vec3 halfwayDir = normalize(lightDir + viewDir); // half vector
                    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess); // exponential relationship of angle
                    vec4 specular = lightColor * spec; // multiply color of incident light
                    vec4 o = ambient + diffuse * diffuseStrength * diffuseColor; // diffuse reflection
                    vec4 specularColor = specularLightColor[i];
                    fragColor = fragColor + o + specular * specularStrength[i] * specularColor;
                }
            }
            return fragColor;
        }
    )");
    std::shared_ptr<Drawing::RuntimeEffect> effect = Drawing::RuntimeEffect::CreateForShader(lightString);
    if (!effect) {
        ROSEN_LOGE("light effect error");
        return phongShaderBuilder;
    }
    lightEffect = std::move(effect);
    phongShaderBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(lightEffect);
    return phongShaderBuilder;
}

void RSPointLightRenderModifier::DrawContentLight(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Brush& brush,
    const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const
{
    constexpr float contentIntensityCoefficient = 0.3f;
    float specularStrengthArr[MAX_LIGHT_SOURCES] = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        specularStrengthArr[i] = lightIntensityArray[i] * contentIntensityCoefficient;
    }
    lightBuilder->SetUniform("specularStrength", specularStrengthArr, MAX_LIGHT_SOURCES);
    std::shared_ptr<Drawing::ShaderEffect> shader = lightBuilder->MakeShader(nullptr, false);
    brush.SetShaderEffect(shader);
    if ((renderIlluminatedType_ == IlluminatedType::BLEND_CONTENT) ||
        (renderIlluminatedType_ == IlluminatedType::BLEND_BORDER_CONTENT)) {
        brush.SetAntiAlias(true);
        brush.SetBlendMode(Drawing::BlendMode::OVERLAY);
        Drawing::SaveLayerOps slo(&renderContentRRect_.GetRect(), &brush);
        canvas.SaveLayer(slo);
        canvas.AttachBrush(brush);
        canvas.DrawRoundRect(renderContentRRect_);
        canvas.DetachBrush();
        canvas.Restore();
    } else {
        canvas.AttachBrush(brush);
        canvas.DrawRoundRect(renderContentRRect_);
        canvas.DetachBrush();
    }
}

void RSPointLightRenderModifier::DrawBorderLight(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Pen& pen,
    const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const
{
    float specularStrengthArr[MAX_LIGHT_SOURCES] = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        specularStrengthArr[i] = lightIntensityArray[i];
    }
    lightBuilder->SetUniform("specularStrength", specularStrengthArr, MAX_LIGHT_SOURCES);
    std::shared_ptr<Drawing::ShaderEffect> shader = lightBuilder->MakeShader(nullptr, false);
    pen.SetShaderEffect(shader);
    float borderWidth = std::ceil(renderBorderWidth_);
    pen.SetWidth(borderWidth);
    if ((renderIlluminatedType_ == IlluminatedType::BLEND_BORDER) ||
        (renderIlluminatedType_ == IlluminatedType::BLEND_BORDER_CONTENT)) {
        Drawing::Brush maskPaint;
        pen.SetBlendMode(Drawing::BlendMode::OVERLAY);
        Drawing::SaveLayerOps slo(&renderBorderRRect_.GetRect(), &maskPaint);
        canvas.SaveLayer(slo);
        canvas.AttachPen(pen);
        canvas.DrawRoundRect(renderBorderRRect_);
        canvas.DetachPen();
        canvas.Restore();
    } else {
        canvas.AttachPen(pen);
        canvas.DrawRoundRect(renderBorderRRect_);
        canvas.DetachPen();
    }
}
} // namespace OHOS::Rosen::ModifierNG