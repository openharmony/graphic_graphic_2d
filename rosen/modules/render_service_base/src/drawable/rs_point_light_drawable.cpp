/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "drawable/rs_point_light_drawable.h"

#include "ge_render.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"

#include "drawable/rs_property_drawable_utils.h"
#include "effect/rs_render_shader_base.h"
#include "effect/rs_render_shape_base.h"
#include "memory/rs_tag_tracker.h"
#include "platform/common/rs_log.h"
#include "property/rs_point_light_manager.h"
#include "render/rs_effect_luminance_manager.h"
namespace OHOS::Rosen {
namespace DrawableV2 {
namespace {
constexpr int PARAM_TWO = 2;
constexpr char NORMAL_LIGHT_SHADER_STRING[](R"(
    uniform vec2 iResolution;
    uniform float cornerRadius;
    uniform vec4 lightPos[12];
    uniform vec4 viewPos[12];
    uniform vec4 specularLightColor[12];
    uniform float specularStrength[12];
    uniform float roundCurvature;
    uniform float bumpFactor;
    uniform float gradientRadius;
    uniform float bulgeRadius;
    uniform float attenuationCoeff;
    uniform float eps;

    float sdf_g2_rounded_box(vec2 p, vec2 b, float r, float k)
    {
        vec2 q = abs(p) - b;
        float v_x =  max(q.x, 0.0);
        float v_y =  max(q.y, 0.0);
        float dist_corner_shape;
        if (abs(k - 2.0) < 0.001) {
            dist_corner_shape = length(vec2(v_x, v_y));
            return dist_corner_shape + min(max(q.x, q.y), 0.0) - r;
        } else {
            if (v_x == 0.0 && v_y == 0.0) {
                dist_corner_shape = 0.0;
            } else {
                dist_corner_shape = pow(pow(v_x, k) + pow(v_y, k), 1.0 / k);
            }
            return dist_corner_shape + min(max(q.x, q.y), 0.0) - r;
        }
    }

    float sdScene(vec2 fragCoord)
    {
        vec2 boxSize = iResolution.xy * 0.5 - cornerRadius;
        vec2 boxCenter = iResolution.xy * 0.5;
        vec2 p = fragCoord - boxCenter;
        return sdf_g2_rounded_box(p, boxSize, cornerRadius, roundCurvature);
    }

    float height(vec2 p, float radius)
    {
        float d = sdScene(p);
        float h = 0.0;
        if (d < 0.0 && d > -radius) {
            float arg = 2.0 * radius * abs(d) - d * d;
            h = sqrt(max(arg, 0.0));
        } else if (d <= -radius) {
            h = radius;
        }
        return h;
    }

    vec2 computeGradient(vec2 p) {
        vec2 epsVec = vec2(eps, 0.0);
        float dx = height(p - epsVec.xy, gradientRadius) - height(p + epsVec.xy, gradientRadius);
        float dy = height(p - epsVec.yx, gradientRadius) - height(p + epsVec.yx, gradientRadius);
        return vec2(dx, dy);
    }

    mediump vec4 main(vec2 drawing_coord)
    {
        float shininess = 8.0;
        mediump vec4 fragColor = vec4(0.0, 0.0, 0.0, 0.0);
        vec2 grad = computeGradient(drawing_coord);
        float h = height(drawing_coord, bulgeRadius);
        vec3 norm = normalize(vec3(grad * bumpFactor, h));

        for (int i = 0; i < 12; i++) {
            if (abs(specularStrength[i]) > 0.01) {
                vec3 lightVec = vec3(lightPos[i].xy - drawing_coord, lightPos[i].z);
                vec3 lightDir = normalize(lightVec);
                vec3 viewDir = normalize(lightVec); // view vector
                vec3 halfwayDir = normalize(lightDir + viewDir); // half vector

                // lightPos[i].w stores the maximum lighting radius
                float normalizedDistance = length(lightVec) / lightPos[i].w;
                float smoothFactor = smoothstep(0.8, 1.0, normalizedDistance);
                float attenuation = (1.0 - smoothFactor) / (1.0 + attenuationCoeff *
                    normalizedDistance * normalizedDistance);
                float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
                vec4 specularColor = specularLightColor[i];
                fragColor += specularColor * spec * specularStrength[i] * attenuation;
            }
        }
        return vec4(fragColor.rgb, clamp(fragColor.a, 0.0, 1.0));
    }
)");
static constexpr char FEATHERING_BORDER_LIGHT_SHADER_STRING[](R"(
    uniform vec2 iResolution;
    uniform float contentBorderRadius;
    uniform vec4 lightPos[12];
    uniform vec4 viewPos[12];
    uniform vec4 specularLightColor[12];
    uniform float specularStrength[12];
    uniform float borderWidth;

    float sdRoundedBox(vec2 p, vec2 b, float r)
    {
        vec2 q = abs(p) - b + r;
        return (min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r);
    }

    vec2 sdRoundedBoxGradient(vec2 p, vec2 b, float r)
    {
        vec2 signs = vec2(p.x >= 0.0 ? 1.0 : -1.0, p.y >= 0.0 ? 1.0 : -1.0);
        vec2 q = abs(p) - b + r;
        vec2 nor = (q.y > q.x) ? vec2(0.0, 1.0) : vec2(1.0, 0.0);
        nor = (q.x > 0.0 && q.y > 0.0) ? normalize(q) : nor;
        return signs * nor;
    }

    mediump vec4 main(vec2 drawing_coord)
    {
        float shininess = 8.0;
        mediump vec4 fragColor = vec4(0.0, 0.0, 0.0, 0.0);
        vec2 halfResolution = iResolution.xy * 0.5;

        float gradRadius = min(max(contentBorderRadius, abs(borderWidth) * 3.0), iResolution.y * 0.5);
        float drawRadius = min(max(contentBorderRadius, abs(borderWidth) * 1.1), iResolution.y * 0.5);
        float realRoundedBoxSDF =
            sdRoundedBox(drawing_coord.xy - halfResolution, halfResolution, contentBorderRadius);
        float virtualRoundedBoxSDF = sdRoundedBox(drawing_coord.xy - halfResolution, halfResolution, drawRadius);
        vec2 grad = sdRoundedBoxGradient(drawing_coord.xy - halfResolution, halfResolution, gradRadius);
        for (int i = 0; i < 12; i++) {
            if (abs(specularStrength[i]) > 0.01) {
                vec2 lightGrad = sdRoundedBoxGradient(lightPos[i].xy - halfResolution,
                    halfResolution,
                    contentBorderRadius); // lightGrad could be pre-computed
                float angleEfficient = dot(grad, lightGrad);
                if (angleEfficient > 0.0) {
                    vec3 lightDir = normalize(vec3(lightPos[i].xy - drawing_coord, lightPos[i].z));
                    vec3 viewDir = normalize(vec3(viewPos[i].xy - drawing_coord, viewPos[i].z)); // view vector
                    vec3 halfwayDir = normalize(lightDir + viewDir);                             // half vector
                    // exponential relationship of angle
                    float spec = pow(max(halfwayDir.z, 0.0), shininess); // norm is (0.0, 0.0, 1.0)
                    spec *= specularStrength[i];
                    spec *= smoothstep(-borderWidth, 0.0, virtualRoundedBoxSDF);
                    spec *= (smoothstep(1.0, 0.0, spec) * 0.2 + 0.8);
                    vec4 specularColor = specularLightColor[i];
                    fragColor += specularColor * (spec * angleEfficient);
                }
            }
        }
        return vec4(fragColor.rgb, clamp(fragColor.a, 0.0, 1.0));
    }
)");
static constexpr char PHONG_SHADER_STRING[](R"(
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
static constexpr char SDF_CONTENT_LIGHT_SHADER_STRING[](R"(
    uniform shader light;
    uniform shader sdf;

    mediump vec4 main(vec2 coord)
    {
        vec4 lightColor = light.eval(coord);
        float sdfColor = sdf.eval(coord).a;
        return lightColor * mix(1.0, 0.0, step(0.0, sdfColor)) * mix(1.0, -sdfColor, step(-1.0, sdfColor));
    }
)");

static constexpr char SDF_BORDER_LIGHT_SHADER_STRING[](R"(
    uniform shader light;
    uniform shader sdf;
    uniform float borderWidth;

    mediump vec4 main(vec2 coord)
    {
        vec4 lightColor = light.eval(coord);
        float sdfColor = sdf.eval(coord).a;
        float halfBorderWidth = borderWidth * 0.5;
        sdfColor += min(halfBorderWidth, step(halfBorderWidth, -sdfColor));
        return lightColor * clamp((halfBorderWidth - abs(sdfColor + halfBorderWidth)), 0.0, 1.0);
    }
)");
constexpr float SDR_LUMINANCE = 1.0f;
} // namespace

RSDrawable::Ptr RSPointLightDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSPointLightDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

void RSPointLightDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(canvas ? canvas->GetGPUContext() : nullptr,
        RSTagTracker::SOURCETYPE::SOURCE_RSPOINTLIGHTDRAWABLE);
#endif
    DrawLight(canvas);
}

bool RSPointLightDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    const auto& illuminatedPtr = properties.GetIlluminated();
    if (!illuminatedPtr || !illuminatedPtr->IsIlluminatedValid()) {
        return false;
    }
    const auto& lightSourcesAndPosMap = illuminatedPtr->GetLightSourcesAndPosMap();
    if (lightSourcesAndPosMap.empty()) {
        stagingEnableEDREffect_ = false;
        return true;
    }
    stagingLightSourcesAndPosVec_.clear();
    stagingLightSourcesAndPosVec_.reserve(lightSourcesAndPosMap.size());
    for (auto& [lightSourcePtr, pos] : lightSourcesAndPosMap) {
        stagingLightSourcesAndPosVec_.emplace_back(std::move(*lightSourcePtr), std::move(pos));
    }
    stagingIlluminatedType_ = illuminatedPtr->GetIlluminatedType();
    stagingBorderWidth_ = std::max(0.0f, std::ceil(properties.GetIlluminatedBorderWidth()));
    stagingRRect_ = RRect(properties.GetRRect());
    stagingNodeId_ = node.GetId();
    stagingScreenNodeId_ = node.GetScreenNodeId();
    auto sdfShape = properties.GetSDFShape();
    if (sdfShape) {
        std::shared_ptr<Drawing::GEVisualEffect> geVisualEffect = sdfShape->GenerateGEVisualEffect();
        std::shared_ptr<Drawing::GEShaderShape> geShape =
            geVisualEffect ? geVisualEffect->GenerateShaderShape() : nullptr;
        stagingSDFShaderEffect_ = geShape ? geShape->GenerateDrawingShader(0.0f, 0.0f) : nullptr;
    }
    auto begin = stagingLightSourcesAndPosVec_.begin();
    auto end = begin + std::min(static_cast<size_t>(MAX_LIGHT_SOURCES), stagingLightSourcesAndPosVec_.size());
    stagingEnableEDREffect_ = stagingIlluminatedType_ == IlluminatedType::NORMAL_BORDER_CONTENT &&
        std::any_of(begin, end, [](const auto& pair) { return ROSEN_GNE(pair.first.GetLightIntensity(), 1.0f); });
    needSync_ = true;
    return true;
}

void RSPointLightDrawable::OnSync()
{
    if (!needSync_) {
        lightSourcesAndPosVec_.clear();
        return;
    }
    lightSourcesAndPosVec_ = std::move(stagingLightSourcesAndPosVec_);
    if (lightSourcesAndPosVec_.size() > MAX_LIGHT_SOURCES) {
        std::sort(lightSourcesAndPosVec_.begin(), lightSourcesAndPosVec_.end(), [](const auto& x, const auto& y) {
            return x.second.x_ * x.second.x_ + x.second.y_ * x.second.y_ <
                   y.second.x_ * y.second.x_ + y.second.y_ * y.second.y_;
        });
    }
    illuminatedType_ = stagingIlluminatedType_;
    enableEDREffect_ = stagingEnableEDREffect_;
    borderWidth_ = stagingBorderWidth_;
    screenNodeId_ = stagingScreenNodeId_;
    nodeId_ = stagingNodeId_;
    sdfShaderEffect_ = std::move(stagingSDFShaderEffect_);
    // half width and half height requires divide by 2.0f
    Vector4f width = { borderWidth_ / 2.0f, borderWidth_ / 2.0f, borderWidth_ / 2.0f, borderWidth_ / 2.0f };
    auto borderRRect = stagingRRect_.Inset(width);
    borderRRect_ = RSPropertyDrawableUtils::RRect2DrawingRRect(borderRRect);
    contentRRect_ = RSPropertyDrawableUtils::RRect2DrawingRRect(stagingRRect_);

    if (enableEDREffect_) {
        displayHeadroom_ = RSEffectLuminanceManager::GetInstance().GetDisplayHeadroom(screenNodeId_);
    }
    needSync_ = false;
}

float RSPointLightDrawable::GetBrightnessMapping(float headroom, float input)
{
    if (ROSEN_GE(headroom, EFFECT_MAX_LUMINANCE)) {
        return input;
    }
    // Bezier curves describing tone mapping rule, anchors define
    static const std::vector<Vector2f> hdrCurveAnchors = {
        {0.0f, 0.0f}, {0.75f, 0.75f}, {1.0f, 0.95f}, {1.25f, 0.97f},
        {1.5f, 0.98f}, {1.75, 0.9875f}, {1.90f, 0.995f}, {2.0f, 1.0f},
    };

    // Bezier curves describing tone mapping rule, control points define
    static const std::vector<Vector2f> hdrCurveControlPoint = {
        {0.375f, 0.375f}, {0.875, 0.875f}, {1.125f, 0.9625f}, {1.375f, 0.9725f},
        {1.625f, 0.98f}, {1.825f, 0.99f}, {1.95f, 0.998f}, // control points one less than anchor points
    };

    int rangeIndex = -1;
    const float epsilon = 1e-6f;
    for (size_t i = 0; i < hdrCurveAnchors.size(); ++i) {
        if (input - hdrCurveAnchors[i].x_ < epsilon) {
            rangeIndex = static_cast<int>(i) - 1;
            break;
        }
    }
    if (rangeIndex == -1) {
        rangeIndex = static_cast<int>(hdrCurveAnchors.size()) - PARAM_TWO;
    }
    if (rangeIndex >= static_cast<int>(hdrCurveAnchors.size()) - 1) {
        return headroom;
    }
    if (rangeIndex >= static_cast<int>(hdrCurveControlPoint.size())) {
        return headroom;
    }

    // calculate new hdr bightness via bezier curve
    Vector2f start = hdrCurveAnchors[rangeIndex];
    Vector2f end = hdrCurveAnchors[rangeIndex + 1];
    Vector2f control = hdrCurveControlPoint[rangeIndex];

    std::optional<float> resultYOptional = CalcBezierResultY(start, end, control, input);
    const float bezierY = resultYOptional.value_or(input);
    const float weightHDR = (EFFECT_MAX_LUMINANCE - headroom) / (EFFECT_MAX_LUMINANCE - SDR_LUMINANCE);
    const float weightSDR = (headroom - SDR_LUMINANCE) / (EFFECT_MAX_LUMINANCE - SDR_LUMINANCE);
    const float interpolationedY = weightHDR * bezierY + weightSDR * input;
    return std::clamp(interpolationedY, 0.0f, headroom);
}

std::optional<float> RSPointLightDrawable::CalcBezierResultY(
    const Vector2f& start, const Vector2f& end, const Vector2f& control, float input)
{
    // Solve quadratic beziier formula with root formula
    const float a = start[0] - 2 * control[0] + end[0];
    const float b = 2 * (control[0] - start[0]);
    const float c = start[0] - input;
    constexpr float FOUR = 4.0f;
    constexpr float TWO = 2.0f;
    const float discriminant = b * b - FOUR * a * c;

    if (ROSEN_LNE(discriminant, 0.0f)) {
        return std::nullopt;
    }
    float t = 0.0f;
    if (ROSEN_EQ(a, 0.0f)) {
        if (ROSEN_EQ(b, 0.0f)) {
            return std::nullopt;
        }
        t = -c / b;
    } else {
        const float sqrtD = std::sqrt(discriminant);
        const float t1 = (-b + sqrtD) / (TWO * a);
        const float t2 = (-b - sqrtD) / (TWO * a);
        if (ROSEN_GE(t1, 0.0f) && ROSEN_LE(t1, 1.0f)) {
            t = t1;
        } else if (ROSEN_GE(t2, 0.0f) && ROSEN_LE(t2, 1.0f)) {
            t = t2;
        }
    }

    return start[1] + t * (TWO * (control[1] - start[1]) + t * (start[1] - TWO * control[1] + end[1]));
}

bool RSPointLightDrawable::NeedToneMapping(float supportHeadroom)
{
    return ROSEN_GNE(supportHeadroom, 0.0f) && ROSEN_LNE(supportHeadroom, EFFECT_MAX_LUMINANCE);
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> RSPointLightDrawable::MakeFeatheringBoardLightShaderBuilder() const
{
    auto builder = GetFeatheringBorderLightShaderBuilder();
    if (!builder) {
        return nullptr;
    }
    float rectWidth = contentRRect_.GetRect().GetWidth();
    float rectHeight = contentRRect_.GetRect().GetHeight();
    builder->SetUniform("iResolution", rectWidth, rectHeight);
    builder->SetUniform("contentBorderRadius",
        contentRRect_.GetCornerRadius(Drawing::RoundRect::CornerPos::TOP_LEFT_POS).GetX());
    builder->SetUniform("borderWidth", borderWidth_);
    return builder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> RSPointLightDrawable::MakeNormalLightShaderBuilder() const
{
    auto builder = GetNormalLightShaderBuilder();
    if (!builder) {
        return nullptr;
    }
    constexpr float DEFAULT_BUMP_FACTOR = 1.0f;
    constexpr float DEFAULT_GRADIENT_RADIUS = 6.0f;
    constexpr float DEFAULT_BULGE_RADIUS = 8.0f;
    constexpr float DEFAULT_EPSILON = 2.0f;
    constexpr float DEFAULT_ATTENUATION_COEFF = 0.3f;
    constexpr float CORNER_RADIUS_SCALE_FACTOR = 1.31f;
    constexpr float CORNER_THRESHOLD_FACTOR = 1.7f;
    constexpr float G2_CURVEATURE_K = 2.77f;
    constexpr float G1_CURVEATURE_K = 2.0f;
    float rectWidth = contentRRect_.GetRect().GetWidth();
    float rectHeight = contentRRect_.GetRect().GetHeight();
    builder->SetUniform("iResolution", rectWidth, rectHeight);
    float cornerRadius = contentRRect_.GetCornerRadius(Drawing::RoundRect::CornerPos::TOP_LEFT_POS).GetX();
    builder->SetUniform("bumpFactor", DEFAULT_BUMP_FACTOR);
    builder->SetUniform("gradientRadius", DEFAULT_GRADIENT_RADIUS);
    builder->SetUniform("bulgeRadius", DEFAULT_BULGE_RADIUS);
    builder->SetUniform("eps", DEFAULT_EPSILON);
    builder->SetUniform("attenuationCoeff", DEFAULT_ATTENUATION_COEFF);
    if (cornerRadius * CORNER_THRESHOLD_FACTOR * PARAM_TWO < std::min(rectWidth, rectHeight)) {
        builder->SetUniform("roundCurvature", G2_CURVEATURE_K);
        builder->SetUniform("cornerRadius", cornerRadius * CORNER_RADIUS_SCALE_FACTOR);
    } else {
        builder->SetUniform("roundCurvature", G1_CURVEATURE_K);
        builder->SetUniform("cornerRadius", cornerRadius);
    }
    return builder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> RSPointLightDrawable::CreateShaderBuilder() const
{
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = nullptr;
    if (illuminatedType_ == IlluminatedType::NORMAL_BORDER_CONTENT) {
        builder = MakeNormalLightShaderBuilder();
    } else if (illuminatedType_ == IlluminatedType::FEATHERING_BORDER) {
        if (sdfShaderEffect_ != nullptr) { // FEATHERING_BORDER not support SDF effect
            return nullptr;
        }
        builder = MakeFeatheringBoardLightShaderBuilder();
    } else {
        builder = GetPhongShaderBuilder();
    }
    return builder;
}

void RSPointLightDrawable::DrawLightByIlluminatedType(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder,
    const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const
{
    Drawing::Pen pen;
    Drawing::Brush brush;
    pen.SetAntiAlias(true);
    brush.SetAntiAlias(true);
    if ((illuminatedType_ == IlluminatedType::BORDER_CONTENT) ||
        (illuminatedType_ == IlluminatedType::BLEND_BORDER_CONTENT) ||
        (illuminatedType_ == IlluminatedType::NORMAL_BORDER_CONTENT)) {
        DrawContentLight(canvas, builder, brush, lightIntensityArray);
        DrawBorderLight(canvas, builder, pen, lightIntensityArray);
    } else if ((illuminatedType_ == IlluminatedType::CONTENT) ||
        (illuminatedType_ == IlluminatedType::BLEND_CONTENT)) {
        DrawContentLight(canvas, builder, brush, lightIntensityArray);
    } else if ((illuminatedType_ == IlluminatedType::BORDER) ||
        (illuminatedType_ == IlluminatedType::BLEND_BORDER) ||
        (illuminatedType_ == IlluminatedType::FEATHERING_BORDER)) {
        DrawBorderLight(canvas, builder, pen, lightIntensityArray);
    }
}
void RSPointLightDrawable::ProcessLightSourcesData(std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray,
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder) const
{
    constexpr int vectorLen = 4;
    float lightPosArray[vectorLen * MAX_LIGHT_SOURCES] = { 0 };
    float viewPosArray[vectorLen * MAX_LIGHT_SOURCES] = { 0 };
    float lightColorArray[vectorLen * MAX_LIGHT_SOURCES] = { 0 };
    auto iter = lightSourcesAndPosVec_.begin();
    auto cnt = 0;
    bool needToneMapping = NeedToneMapping(displayHeadroom_);
    while (iter != lightSourcesAndPosVec_.end() && cnt < MAX_LIGHT_SOURCES) {
        auto lightPos = iter->second;
        auto lightIntensity = iter->first.GetLightIntensity();
        auto lightColor = iter->first.GetLightColor();
        Vector4f lightColorVec =
            Vector4f(lightColor.GetRed(), lightColor.GetGreen(), lightColor.GetBlue(), lightColor.GetAlpha());
        for (int i = 0; i < vectorLen; i++) {
            lightPosArray[cnt * vectorLen + i] = lightPos[i];
            viewPosArray[cnt * vectorLen + i] = lightPos[i];
            float lightColorNorm = lightColorVec[i] / UINT8_MAX;
            if (enableEDREffect_ && needToneMapping) {
                lightColorNorm = GetBrightnessMapping(displayHeadroom_, lightColorNorm);
            }
            lightColorArray[cnt * vectorLen + i] = lightColorNorm;
        }
        lightIntensityArray[cnt] = std::abs(lightIntensity);
        iter++;
        cnt++;
    }
    builder->SetUniform("lightPos", lightPosArray, vectorLen * MAX_LIGHT_SOURCES);
    builder->SetUniform("viewPos", viewPosArray, vectorLen * MAX_LIGHT_SOURCES);
    builder->SetUniform("specularLightColor", lightColorArray, vectorLen * MAX_LIGHT_SOURCES);
}
void RSPointLightDrawable::DrawLight(Drawing::Canvas* canvas) const
{
    if (lightSourcesAndPosVec_.empty()) {
        return;
    }
    auto builder = CreateShaderBuilder();
    if (!builder || !canvas) {
        return;
    }
    std::array<float, MAX_LIGHT_SOURCES> lightIntensityArray = { 0 };
    ProcessLightSourcesData(lightIntensityArray, builder);
    ROSEN_LOGD("DrawLight type:%{public}d intensity:%{public}f enableEDR:%{public}d nodeId:%{public}" PRIu64 "",
        illuminatedType_, lightIntensityArray[0], enableEDREffect_, nodeId_);
    RS_OPTIONAL_TRACE_FMT("DrawLight intensity:%{public}f nodeId:%{public}" PRIu64 "", lightIntensityArray[0], nodeId_);
    DrawLightByIlluminatedType(*canvas, builder, lightIntensityArray);
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> RSPointLightDrawable::GetPhongShaderBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> shaderBuilder;
    if (shaderBuilder) {
        return shaderBuilder;
    }
    std::shared_ptr<Drawing::RuntimeEffect> effect =
        Drawing::RuntimeEffect::CreateForShader(std::string(PHONG_SHADER_STRING));
    if (!effect) {
        ROSEN_LOGE("RSPointLightDrawable::GetPhongShaderBuilder effect is null");
        return nullptr;
    }
    shaderBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(std::move(effect));
    return shaderBuilder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> RSPointLightDrawable::GetFeatheringBorderLightShaderBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> shaderBuilder;
    if (shaderBuilder) {
        return shaderBuilder;
    }
    std::shared_ptr<Drawing::RuntimeEffect> effect =
        Drawing::RuntimeEffect::CreateForShader(std::string(FEATHERING_BORDER_LIGHT_SHADER_STRING));
    if (!effect) {
        ROSEN_LOGE("RSPointLightDrawable::GetFeatheringBorderLightShaderBuilder effect is null");
        return nullptr;
    }
    shaderBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(std::move(effect));
    return shaderBuilder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> RSPointLightDrawable::GetNormalLightShaderBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> shaderBuilder;
    if (shaderBuilder) {
        return shaderBuilder;
    }
    std::shared_ptr<Drawing::RuntimeEffect> effect =
        Drawing::RuntimeEffect::CreateForShader(std::string(NORMAL_LIGHT_SHADER_STRING));
    if (!effect) {
        ROSEN_LOGE("RSPointLightDrawable::GetNormalLightShaderBuilder effect is null");
        return nullptr;
    }
    shaderBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(std::move(effect));
    return shaderBuilder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> RSPointLightDrawable::GetSDFContentLightShaderBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> shaderBuilder;
    if (shaderBuilder) {
        return shaderBuilder;
    }
    std::shared_ptr<Drawing::RuntimeEffect> effect =
        Drawing::RuntimeEffect::CreateForShader(std::string(SDF_CONTENT_LIGHT_SHADER_STRING));
    if (!effect) {
        ROSEN_LOGE("RSPointLightDrawable::GetSDFContentLightShaderBuilder effect is null");
        return nullptr;
    }
    shaderBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(std::move(effect));
    return shaderBuilder;
}

std::shared_ptr<Drawing::RuntimeShaderBuilder> RSPointLightDrawable::GetSDFBorderLightShaderBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeShaderBuilder> shaderBuilder;
    if (shaderBuilder) {
        return shaderBuilder;
    }
    std::shared_ptr<Drawing::RuntimeEffect> effect =
        Drawing::RuntimeEffect::CreateForShader(std::string(SDF_BORDER_LIGHT_SHADER_STRING));
    if (!effect) {
        ROSEN_LOGE("RSPointLightDrawable::GetSDFBorderLightShaderBuilder effect is null");
        return nullptr;
    }
    shaderBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(std::move(effect));
    return shaderBuilder;
}

bool RSPointLightDrawable::DrawSDFContentLight(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::ShaderEffect>& lightShaderEffect, Drawing::Brush& brush) const
{
    auto sdfLightBuilder = GetSDFContentLightShaderBuilder();
    if (!sdfLightBuilder || !lightShaderEffect) {
        return false;
    }
    sdfLightBuilder->SetChild("light", lightShaderEffect);
    sdfLightBuilder->SetChild("sdf", sdfShaderEffect_);
    lightShaderEffect = sdfLightBuilder->MakeShader(nullptr, false);
    brush.SetShaderEffect(lightShaderEffect);
    if ((illuminatedType_ == IlluminatedType::BLEND_CONTENT) ||
        (illuminatedType_ == IlluminatedType::BLEND_BORDER_CONTENT)) {
        brush.SetAntiAlias(true);
        brush.SetBlendMode(Drawing::BlendMode::OVERLAY);
        Drawing::SaveLayerOps slo(&contentRRect_.GetRect(), &brush);
        canvas.SaveLayer(slo);
        canvas.AttachBrush(brush);
        canvas.DrawRect(contentRRect_.GetRect());
        canvas.DetachBrush();
        canvas.Restore();
    } else {
        canvas.AttachBrush(brush);
        canvas.DrawRect(contentRRect_.GetRect());
        canvas.DetachBrush();
    }
    return true;
}

void RSPointLightDrawable::DrawContentLight(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Brush& brush,
    const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const
{
    float contentIntensityCoefficient = illuminatedType_ == IlluminatedType::NORMAL_BORDER_CONTENT ? 0.5f : 0.3f;
    float specularStrengthArr[MAX_LIGHT_SOURCES] = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        specularStrengthArr[i] = lightIntensityArray[i] * contentIntensityCoefficient;
    }
    lightBuilder->SetUniform("specularStrength", specularStrengthArr, MAX_LIGHT_SOURCES);
    std::shared_ptr<Drawing::ShaderEffect> shader = lightBuilder->MakeShader(nullptr, false);
    if (sdfShaderEffect_) {
        DrawSDFContentLight(canvas, shader, brush);
        return;
    }

    brush.SetShaderEffect(shader);
    if ((illuminatedType_ == IlluminatedType::BLEND_CONTENT) ||
        (illuminatedType_ == IlluminatedType::BLEND_BORDER_CONTENT)) {
        brush.SetAntiAlias(true);
        brush.SetBlendMode(Drawing::BlendMode::OVERLAY);
        Drawing::SaveLayerOps slo(&contentRRect_.GetRect(), &brush);
        canvas.SaveLayer(slo);
        canvas.AttachBrush(brush);
        canvas.DrawRoundRect(contentRRect_);
        canvas.DetachBrush();
        canvas.Restore();
    } else {
        canvas.AttachBrush(brush);
        canvas.DrawRoundRect(contentRRect_);
        canvas.DetachBrush();
    }
}

bool RSPointLightDrawable::DrawSDFBorderLight(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::ShaderEffect>& lightShaderEffect) const
{
    auto sdfLightBuilder = GetSDFBorderLightShaderBuilder();
    if (!sdfLightBuilder || !lightShaderEffect) {
        return false;
    }
    sdfLightBuilder->SetChild("light", lightShaderEffect);
    sdfLightBuilder->SetChild("sdf", sdfShaderEffect_);
    sdfLightBuilder->SetUniform("borderWidth", borderWidth_);
    lightShaderEffect = sdfLightBuilder->MakeShader(nullptr, false);

    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    brush.SetShaderEffect(lightShaderEffect);
    if ((illuminatedType_ == IlluminatedType::BLEND_BORDER) ||
        (illuminatedType_ == IlluminatedType::BLEND_BORDER_CONTENT)) {
        brush.SetBlendMode(Drawing::BlendMode::OVERLAY);
        Drawing::Brush maskPaint;
        Drawing::SaveLayerOps slo(&contentRRect_.GetRect(), &maskPaint);
        canvas.SaveLayer(slo);
        canvas.AttachBrush(brush);
        canvas.DrawRect(contentRRect_.GetRect());
        canvas.DetachBrush();
        canvas.Restore();
    } else {
        canvas.AttachBrush(brush);
        canvas.DrawRect(contentRRect_.GetRect());
        canvas.DetachBrush();
    }
    return true;
}

void RSPointLightDrawable::DrawBorderLight(Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Pen& pen,
    const std::array<float, MAX_LIGHT_SOURCES>& lightIntensityArray) const
{
    float specularStrengthArr[MAX_LIGHT_SOURCES] = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        specularStrengthArr[i] = lightIntensityArray[i];
    }
    lightBuilder->SetUniform("specularStrength", specularStrengthArr, MAX_LIGHT_SOURCES);
    std::shared_ptr<Drawing::ShaderEffect> shader = lightBuilder->MakeShader(nullptr, false);
    if (sdfShaderEffect_) {
        DrawSDFBorderLight(canvas, shader);
        return;
    }
    
    pen.SetShaderEffect(shader);
    float borderWidth = std::ceil(borderWidth_);
    pen.SetWidth(borderWidth);
    if ((illuminatedType_ == IlluminatedType::BLEND_BORDER) ||
        (illuminatedType_ == IlluminatedType::BLEND_BORDER_CONTENT)) {
        Drawing::Brush maskPaint;
        pen.SetBlendMode(Drawing::BlendMode::OVERLAY);
        Drawing::SaveLayerOps slo(&borderRRect_.GetRect(), &maskPaint);
        canvas.SaveLayer(slo);
        canvas.AttachPen(pen);
        canvas.DrawRoundRect(borderRRect_);
        canvas.DetachPen();
        canvas.Restore();
    } else {
        canvas.AttachPen(pen);
        canvas.DrawRoundRect(borderRRect_);
        canvas.DetachPen();
    }
}
} // namespace DrawableV2
} // namespace OHOS::Rosen