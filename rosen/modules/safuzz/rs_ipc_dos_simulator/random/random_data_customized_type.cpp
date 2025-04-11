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

#include "random/random_data_customized_type.h"

#include <algorithm>
#include <random>

#include "common/safuzz_log.h"
#include "customized/random_animation.h"
#include "customized/random_draw_cmd_list.h"
#include "customized/random_matrix.h"
#include "customized/random_pixel_map.h"
#include "customized/random_properties_map.h"
#include "customized/random_rs_filter.h"
#include "customized/random_rs_gradient_blur_para.h"
#include "customized/random_rs_image.h"
#include "customized/random_rs_mask.h"
#include "customized/random_rs_path.h"
#include "customized/random_rs_render_modifier.h"
#include "customized/random_rs_render_particle.h"
#include "customized/random_rs_render_property_base.h"
#include "customized/random_rs_shader.h"
#include "random/random_data_basic_type.h"
#include "random/random_engine.h"

namespace OHOS {
namespace Rosen {
PropertyUpdateType RandomDataCustomizedType::GetRandomPropertyUpdateType()
{
    static constexpr int PROPERTY_UPDATE_INDEX_MAX = 2;
    int randomIndex = RandomEngine::GetRandomIndex(PROPERTY_UPDATE_INDEX_MAX);
    return static_cast<PropertyUpdateType>(randomIndex);
}

RSUIFirstSwitch RandomDataCustomizedType::GetRandomRSUIFirstSwitch()
{
    static constexpr int RS_UIFIRST_SWITCH_INDEX_MAX = 2;
    int randomIndex = RandomEngine::GetRandomIndex(RS_UIFIRST_SWITCH_INDEX_MAX);
    return static_cast<RSUIFirstSwitch>(randomIndex);
}

OutOfParentType RandomDataCustomizedType::GetRandomOutOfParentType()
{
    static constexpr int OUT_OF_PARENT_TYPE_INDEX_MAX = 2;
    int randomIndex = RandomEngine::GetRandomIndex(OUT_OF_PARENT_TYPE_INDEX_MAX);
    return static_cast<OutOfParentType>(randomIndex);
}

Color RandomDataCustomizedType::GetRandomColor()
{
    int16_t r = RandomDataBasicType::GetRandomInt16();
    int16_t g = RandomDataBasicType::GetRandomInt16();
    int16_t b = RandomDataBasicType::GetRandomInt16();
    int16_t a = RandomDataBasicType::GetRandomInt16();
    Color color(r, g, b, a);
    return color;
}

Gravity RandomDataCustomizedType::GetRandomGravity()
{
    static constexpr int GRAVITY_INDEX_MAX = 15;
    int randomIndex = RandomEngine::GetRandomIndex(GRAVITY_INDEX_MAX);
    return static_cast<Gravity>(randomIndex);
}

Matrix3f RandomDataCustomizedType::GetRandomMatrix3f()
{
    float m00 = RandomDataBasicType::GetRandomFloat();
    float m01 = RandomDataBasicType::GetRandomFloat();
    float m02 = RandomDataBasicType::GetRandomFloat();
    float m10 = RandomDataBasicType::GetRandomFloat();
    float m11 = RandomDataBasicType::GetRandomFloat();
    float m12 = RandomDataBasicType::GetRandomFloat();
    float m20 = RandomDataBasicType::GetRandomFloat();
    float m21 = RandomDataBasicType::GetRandomFloat();
    float m22 = RandomDataBasicType::GetRandomFloat();
    Matrix3f matrix(m00, m01, m02, m10, m11, m12, m20, m21, m22);
    return matrix;
}

RSDynamicBrightnessPara RandomDataCustomizedType::GetRandomRSDynamicBrightnessPara()
{
    RSDynamicBrightnessPara data;
    data.rates_ = GetRandomVector4f();
    data.saturation_ = RandomDataBasicType::GetRandomFloat();
    data.posCoeff_ = GetRandomVector4f();
    data.negCoeff_ = GetRandomVector4f();
    data.fraction_ = RandomDataBasicType::GetRandomFloat();
    return data;
}

RSWaterRipplePara RandomDataCustomizedType::GetRandomRSWaterRipplePara()
{
    RSWaterRipplePara data = {
        .waveCount = RandomDataBasicType::GetRandomUint32(),
        .rippleCenterX = RandomDataBasicType::GetRandomFloat(),
        .rippleCenterY = RandomDataBasicType::GetRandomFloat(),
        .rippleMode = RandomDataBasicType::GetRandomUint32(),
    };
    return data;
}

RSFlyOutPara RandomDataCustomizedType::GetRandomRSFlyOutPara()
{
    RSFlyOutPara data = {
        .flyMode = RandomDataBasicType::GetRandomUint32(),
    };
    return data;
}

std::shared_ptr<MotionBlurParam> RandomDataCustomizedType::GetRandomMotionBlurParamSharedPtr()
{
    float radius = RandomDataBasicType::GetRandomFloat();
    Vector2f scaleAnchor = GetRandomVector2f();
    auto motionBlurParam = std::make_shared<MotionBlurParam>(radius, scaleAnchor);
    return motionBlurParam;
}

std::shared_ptr<RSMagnifierParams> RandomDataCustomizedType::GetRandomRSMagnifierParamsSharedPtr()
{
    auto rsMagnifierParams = std::make_shared<RSMagnifierParams>();
    rsMagnifierParams->factor_ = RandomDataBasicType::GetRandomFloat();
    rsMagnifierParams->width_ = RandomDataBasicType::GetRandomFloat();
    rsMagnifierParams->height_ = RandomDataBasicType::GetRandomFloat();
    rsMagnifierParams->cornerRadius_ = RandomDataBasicType::GetRandomFloat();
    rsMagnifierParams->borderWidth_ = RandomDataBasicType::GetRandomFloat();
    rsMagnifierParams->offsetX_ = RandomDataBasicType::GetRandomFloat();
    rsMagnifierParams->offsetY_ = RandomDataBasicType::GetRandomFloat();
    rsMagnifierParams->shadowOffsetX_ = RandomDataBasicType::GetRandomFloat();
    rsMagnifierParams->shadowOffsetY_ = RandomDataBasicType::GetRandomFloat();
    rsMagnifierParams->shadowSize_ = RandomDataBasicType::GetRandomFloat();
    rsMagnifierParams->shadowStrength_ = RandomDataBasicType::GetRandomFloat();
    rsMagnifierParams->gradientMaskColor1_ = RandomDataBasicType::GetRandomUint32();
    rsMagnifierParams->gradientMaskColor2_ = RandomDataBasicType::GetRandomUint32();
    rsMagnifierParams->outerContourColor1_ = RandomDataBasicType::GetRandomUint32();
    rsMagnifierParams->outerContourColor2_ = RandomDataBasicType::GetRandomUint32();
    return rsMagnifierParams;
}

std::vector<std::shared_ptr<EmitterUpdater>> RandomDataCustomizedType::GetRandomEmitterUpdaterSharedPtrVector(
    const std::string& sizeType)
{
    static constexpr float NULLOPT_PROBABILITY = 0.05f;
    std::vector<std::shared_ptr<EmitterUpdater>> emitterUpdaterVector;
    int vectorLength = 0;
    if (sizeType == "normal") {
        vectorLength = RandomEngine::GetRandomVectorLength();
    } else if (sizeType == "small") {
        vectorLength = RandomEngine::GetRandomSmallVectorLength();
    } else {
        SAFUZZ_LOGE("RandomDataCustomizedType::GetRandomEmitterUpdaterSharedPtrVector unrecognized sizeType %s",
            sizeType.c_str());
        return emitterUpdaterVector;
    }
    for (int i = 0; i < vectorLength; ++i) {
        uint32_t emitterIndex = RandomDataBasicType::GetRandomUint32();
        std::optional<Vector2f> position;
        if (RandomEngine::ChooseByProbability(NULLOPT_PROBABILITY)) {
            position = std::nullopt;
        } else {
            position = GetRandomVector2f();
        }
        std::optional<Vector2f> emitSize;
        if (RandomEngine::ChooseByProbability(NULLOPT_PROBABILITY)) {
            emitSize = std::nullopt;
        } else {
            emitSize = GetRandomVector2f();
        }
        std::optional<int> emitRate;
        if (RandomEngine::ChooseByProbability(NULLOPT_PROBABILITY)) {
            emitRate = std::nullopt;
        } else {
            emitRate = RandomDataBasicType::GetRandomInt();
        }
        auto emitterUpdater = std::make_shared<EmitterUpdater>(emitterIndex, position, emitSize, emitRate);
        emitterUpdaterVector.emplace_back(emitterUpdater);
    }
    return emitterUpdaterVector;
}

ShapeType RandomDataCustomizedType::GetRandomShapeType()
{
    static constexpr int SHAPE_TYPE_INDEX_MAX = 2;
    int randomIndex = RandomEngine::GetRandomIndex(SHAPE_TYPE_INDEX_MAX);
    return static_cast<ShapeType>(randomIndex);
}

std::shared_ptr<ParticleNoiseFields> RandomDataCustomizedType::GetRandomParticleNoiseFieldsSharedPtr(
    const std::string& sizeType)
{
    auto particleNoiseFields = std::make_shared<ParticleNoiseFields>();
    int vectorLength = 0;
    if (sizeType == "normal") {
        vectorLength = RandomEngine::GetRandomVectorLength();
    } else if (sizeType == "small") {
        vectorLength = RandomEngine::GetRandomSmallVectorLength();
    } else {
        SAFUZZ_LOGE("RandomDataCustomizedType::GetRandomParticleNoiseFieldsSharedPtr unrecognized sizeType %s",
            sizeType.c_str());
        return particleNoiseFields;
    }
    for (int i = 0; i < vectorLength; ++i) {
        int fieldStrength = RandomDataBasicType::GetRandomInt();
        ShapeType fieldShape = GetRandomShapeType();
        Vector2f fieldSize = GetRandomVector2f();
        Vector2f fieldCenter = GetRandomVector2f();
        uint16_t fieldFeather = RandomDataBasicType::GetRandomUint16();
        float noiseScale = RandomDataBasicType::GetRandomFloat();
        float noiseFrequency = RandomDataBasicType::GetRandomFloat();
        float noiseAmplitude = RandomDataBasicType::GetRandomFloat();
        auto particleNoiseField = std::make_shared<ParticleNoiseField>(fieldStrength, fieldShape, fieldSize,
            fieldCenter, fieldFeather, noiseScale, noiseFrequency, noiseAmplitude);
        particleNoiseFields->AddField(particleNoiseField);
    }
    return particleNoiseFields;
}

Vector4<uint32_t> RandomDataCustomizedType::GetRandomUint32Vector4()
{
    uint32_t x = RandomDataBasicType::GetRandomUint32();
    uint32_t y = RandomDataBasicType::GetRandomUint32();
    uint32_t z = RandomDataBasicType::GetRandomUint32();
    uint32_t w = RandomDataBasicType::GetRandomUint32();
    Vector4<uint32_t> vector(x, y, z, w);
    return vector;
}

Vector4<Color> RandomDataCustomizedType::GetRandomColorVector4()
{
    Color x = GetRandomColor();
    Color y = GetRandomColor();
    Color z = GetRandomColor();
    Color w = GetRandomColor();
    Vector4<Color> vector(x, y, z, w);
    return vector;
}

std::shared_ptr<RSRenderModifier> RandomDataCustomizedType::GetRandomRSRenderModifierSharedPtr()
{
    return RandomRSRenderModifier::GetRandomRSRenderModifier();
}

std::shared_ptr<RSFilter> RandomDataCustomizedType::GetRandomRSFilterSharedPtr()
{
    return RandomRSFilter::GetRandomRSFilter();
}

std::shared_ptr<RSImage> RandomDataCustomizedType::GetRandomRSImageSharedPtr()
{
    return RandomRSImage::GetRandomRSImage();
}

std::shared_ptr<RSMask> RandomDataCustomizedType::GetRandomRSMaskSharedPtr()
{
    return RandomRSMask::GetRandomRSMask();
}

std::shared_ptr<RSPath> RandomDataCustomizedType::GetRandomRSPathSharedPtr()
{
    return RandomRSPath::GetRandomRSPath();
}

std::shared_ptr<RSLinearGradientBlurPara> RandomDataCustomizedType::GetRandomRSLinearGradientBlurParaSharedPtr()
{
    return RandomRSLinearGradientBlurPara::GetRandomRSLinearGradientBlurPara();
}

std::shared_ptr<RSShader> RandomDataCustomizedType::GetRandomRSShaderSharedPtr()
{
    return RandomRSShader::GetRandomRSShader();
}

Drawing::Matrix RandomDataCustomizedType::GetRandomDrawingMatrix()
{
    return RandomDrawingMatrix::GetRandomDrawingMatrix();
}

std::shared_ptr<Drawing::DrawCmdList> RandomDataCustomizedType::GetRandomDrawingDrawCmdListPtr()
{
    return RandomDrawCmdList::GetRandomDrawCmdList();
}

RSDisplayNodeConfig RandomDataCustomizedType::GetRandomRSDisplayNodeConfig()
{
    RSDisplayNodeConfig data = {
        .screenId = RandomDataBasicType::GetRandomUint64(),
        .isMirrored = RandomDataBasicType::GetRandomBool(),
        .mirrorNodeId = RandomDataBasicType::GetRandomUint64(),
        .isSync = RandomDataBasicType::GetRandomBool(),
    };
    return data;
}

ScreenRotation RandomDataCustomizedType::GetRandomScreenRotation()
{
    static constexpr int SCREEN_ROTATION_INDEX_MAX = 4;
    int randomIndex = RandomEngine::GetRandomIndex(SCREEN_ROTATION_INDEX_MAX);
    return static_cast<ScreenRotation>(randomIndex);
}

RSSurfaceNodeType RandomDataCustomizedType::GetRandomRSSurfaceNodeType()
{
    static constexpr int rsSurfaceNodeTypeIndexMax = 11;
    int randomIndex = RandomEngine::GetRandomIndex(rsSurfaceNodeTypeIndexMax);
    return static_cast<RSSurfaceNodeType>(randomIndex);
}

SurfaceWindowType RandomDataCustomizedType::GetRandomSurfaceWindowType()
{
    static constexpr int SURFACE_WINDOW_TYPE_INDEX_MAX = 1;
    int randomIndex = RandomEngine::GetRandomIndex(SURFACE_WINDOW_TYPE_INDEX_MAX);
    return static_cast<SurfaceWindowType>(randomIndex);
}

Drawing::Rect RandomDataCustomizedType::GetRandomDrawingRect()
{
    float l = RandomDataBasicType::GetRandomFloat();
    float t = RandomDataBasicType::GetRandomFloat();
    float r = RandomDataBasicType::GetRandomFloat();
    float b = RandomDataBasicType::GetRandomFloat();
    Drawing::Rect rect(l, t, r, b);
    return rect;
}

std::optional<Drawing::Rect> RandomDataCustomizedType::GetRandomOptionalDrawingRect()
{
    static constexpr float NULLOPT_PROBABILITY = 0.05f;
    if (RandomEngine::ChooseByProbability(NULLOPT_PROBABILITY)) {
        return std::nullopt;
    }
    return GetRandomDrawingRect();
}

Vector2f RandomDataCustomizedType::GetRandomVector2f()
{
    float x = RandomDataBasicType::GetRandomFloat();
    float y = RandomDataBasicType::GetRandomFloat();
    Vector2f vector(x, y);
    return vector;
}

Vector3f RandomDataCustomizedType::GetRandomVector3f()
{
    float x = RandomDataBasicType::GetRandomFloat();
    float y = RandomDataBasicType::GetRandomFloat();
    float z = RandomDataBasicType::GetRandomFloat();
    Vector3f vector(x, y, z);
    return vector;
}

Vector4f RandomDataCustomizedType::GetRandomVector4f()
{
    float x = RandomDataBasicType::GetRandomFloat();
    float y = RandomDataBasicType::GetRandomFloat();
    float z = RandomDataBasicType::GetRandomFloat();
    float w = RandomDataBasicType::GetRandomFloat();
    Vector4f vector(x, y, z, w);
    return vector;
}

Quaternion RandomDataCustomizedType::GetRandomQuaternion()
{
    float x = RandomDataBasicType::GetRandomFloat();
    float y = RandomDataBasicType::GetRandomFloat();
    float z = RandomDataBasicType::GetRandomFloat();
    float w = RandomDataBasicType::GetRandomFloat();
    Quaternion quaternion(x, y, z, w);
    return quaternion;
}

RectF RandomDataCustomizedType::GetRandomRectF()
{
    float l = RandomDataBasicType::GetRandomFloat();
    float t = RandomDataBasicType::GetRandomFloat();
    float w = RandomDataBasicType::GetRandomFloat();
    float h = RandomDataBasicType::GetRandomFloat();
    RectF rect(l, t, w, h);
    return rect;
}

std::shared_ptr<RectF> RandomDataCustomizedType::GetRandomRectFSharedPtr()
{
    float l = RandomDataBasicType::GetRandomFloat();
    float t = RandomDataBasicType::GetRandomFloat();
    float w = RandomDataBasicType::GetRandomFloat();
    float h = RandomDataBasicType::GetRandomFloat();
    auto rect = std::make_shared<RectF>(l, t, w, h);
    return rect;
}

RRect RandomDataCustomizedType::GetRandomRRect()
{
    static constexpr size_t NUM_RADIUS = 4;
    Vector2f r1 = GetRandomVector2f();
    Vector2f r2 = GetRandomVector2f();
    Vector2f r3 = GetRandomVector2f();
    Vector2f r4 = GetRandomVector2f();
    Vector2f radius[NUM_RADIUS] = { r1, r2, r3, r4 };
    RectF rect = GetRandomRectF();
    RRect rrect(rect, radius);
    return rrect;
}

#ifndef ROSEN_CROSS_PLATFORM
GraphicColorGamut RandomDataCustomizedType::GetRandomGraphicColorGamut()
{
    static constexpr int GRAPHIC_COLOR_GAMUT_INDEX_MAX = 10;
    static constexpr int GRAPHIC_COLOR_GAMUT_INDEX_MIN = -1;
    int randomIndex = RandomEngine::GetRandomIndex(GRAPHIC_COLOR_GAMUT_INDEX_MAX, GRAPHIC_COLOR_GAMUT_INDEX_MIN);
    return static_cast<GraphicColorGamut>(randomIndex);
}
#endif

RSSurfaceNodeAbilityState RandomDataCustomizedType::GetRandomRSSurfaceNodeAbilityState()
{
    static constexpr int RS_SURFACE_NODE_ABILITY_STATE_INDEX_MAX = 1;
    int randomIndex = RandomEngine::GetRandomIndex(RS_SURFACE_NODE_ABILITY_STATE_INDEX_MAX);
    return static_cast<RSSurfaceNodeAbilityState>(randomIndex);
}

std::optional<Drawing::Matrix> RandomDataCustomizedType::GetRandomOptionalDrawingMatrix()
{
    static constexpr float NULLOPT_PROBABILITY = 0.05f;
    if (RandomEngine::ChooseByProbability(NULLOPT_PROBABILITY)) {
        return std::nullopt;
    }
    return RandomDrawingMatrix::GetRandomDrawingMatrix();
}

AnimationCallbackEvent RandomDataCustomizedType::GetRandomAnimationCallbackEvent()
{
    static constexpr int ANIMATION_CALLBACK_EVENT_INDEX_MAX = 2;
    int randomIndex = RandomEngine::GetRandomIndex(ANIMATION_CALLBACK_EVENT_INDEX_MAX);
    return static_cast<AnimationCallbackEvent>(randomIndex);
}

std::vector<std::pair<uint64_t, uint64_t>> RandomDataCustomizedType::GetRandomUint64AndUint64PairVector()
{
    std::vector<std::pair<uint64_t, uint64_t>> vector;
    int vectorLength = RandomEngine::GetRandomVectorLength();
    for (int i = 0; i < vectorLength; ++i) {
        uint64_t x = RandomDataBasicType::GetRandomUint64();
        uint64_t y = RandomDataBasicType::GetRandomUint64();
        auto item = std::make_pair(x, y);
        vector.emplace_back(item);
    }
    return vector;
}

RSInteractiveAnimationPosition RandomDataCustomizedType::GetRandomRSInteractiveAnimationPosition()
{
    static constexpr int RS_INTERACTIVE_ANIMATION_POSITION_INDEX_MAX = 2;
    int randomIndex = RandomEngine::GetRandomIndex(RS_INTERACTIVE_ANIMATION_POSITION_INDEX_MAX);
    return static_cast<RSInteractiveAnimationPosition>(randomIndex);
}

std::shared_ptr<RSRenderCurveAnimation> RandomDataCustomizedType::GetRandomRSRenderCurveAnimationSharedPtr()
{
    return RandomAnimation::GetRandomRSRenderCurveAnimation();
}

std::shared_ptr<RSRenderParticleAnimation> RandomDataCustomizedType::GetRandomRSRenderParticleAnimationSharedPtr()
{
    return RandomAnimation::GetRandomRSRenderParticleAnimation();
}

std::shared_ptr<RSRenderKeyframeAnimation> RandomDataCustomizedType::GetRandomRSRenderKeyframeAnimationSharedPtr()
{
    return RandomAnimation::GetRandomRSRenderKeyframeAnimation();
}

std::shared_ptr<RSRenderPathAnimation> RandomDataCustomizedType::GetRandomRSRenderPathAnimationSharedPtr()
{
    return RandomAnimation::GetRandomRSRenderPathAnimation();
}

std::shared_ptr<RSRenderTransition> RandomDataCustomizedType::GetRandomRSRenderTransitionSharedPtr()
{
    return RandomAnimation::GetRandomRSRenderTransition();
}

std::shared_ptr<RSRenderSpringAnimation> RandomDataCustomizedType::GetRandomRSRenderSpringAnimationSharedPtr()
{
    return RandomAnimation::GetRandomRSRenderSpringAnimation();
}

std::shared_ptr<RSRenderInterpolatingSpringAnimation>
    RandomDataCustomizedType::GetRandomRSRenderInterpolatingSpringAnimationSharedPtr()
{
    return RandomAnimation::GetRandomRSRenderInterpolatingSpringAnimation();
}

std::shared_ptr<RSRenderPropertyBase> RandomDataCustomizedType::GetRandomRSRenderPropertyBaseSharedPtr()
{
    return RandomRSRenderPropertyBase::GetRandomRSRenderPropertyBase();
}

PropertiesMap RandomDataCustomizedType::GetRandomPropertiesMap()
{
    return RandomPropertiesMap::GetRandomPropertiesMap();
}

ComponentScene RandomDataCustomizedType::GetRandomComponentScene()
{
    static constexpr int COMPONENT_SCENE_INDEX_MAX = 1;
    int randomIndex = RandomEngine::GetRandomIndex(COMPONENT_SCENE_INDEX_MAX);
    return static_cast<ComponentScene>(randomIndex);
}

FrameRateRange RandomDataCustomizedType::GetRandomFrameRateRange()
{
    int min = RandomDataBasicType::GetRandomInt();
    int max = RandomDataBasicType::GetRandomInt();
    int preferred = RandomDataBasicType::GetRandomInt();
    uint32_t type = RandomDataBasicType::GetRandomUint32();
    bool isEnergyAssurance = RandomDataBasicType::GetRandomBool();
    ComponentScene componentScene = GetRandomComponentScene();
    FrameRateRange frameRateRange(min, max, preferred, type, componentScene);
    frameRateRange.isEnergyAssurance_ = isEnergyAssurance;
    return frameRateRange;
}

Drawing::Point RandomDataCustomizedType::GetRandomDrawingPoint()
{
    return Drawing::Point(RandomDataBasicType::GetRandomFloat(), RandomDataBasicType::GetRandomFloat());
}

Drawing::Color RandomDataCustomizedType::GetRandomDrawingColor()
{
    return Drawing::Color(GetRandomColor().AsArgbInt());
}

Drawing::RectI RandomDataCustomizedType::GetRandomDrawingRectI()
{
    return Drawing::RectI(RandomDataBasicType::GetRandomInt32(), RandomDataBasicType::GetRandomInt32(),
        RandomDataBasicType::GetRandomInt32(), RandomDataBasicType::GetRandomInt32());
}

SkMatrix RandomDataCustomizedType::GetRandomSkMatrix()
{
    SkMatrix matrix;
    matrix.setAll(
        RandomDataBasicType::GetRandomFloat(), RandomDataBasicType::GetRandomFloat(),
        RandomDataBasicType::GetRandomFloat(), RandomDataBasicType::GetRandomFloat(),
        RandomDataBasicType::GetRandomFloat(), RandomDataBasicType::GetRandomFloat(),
        RandomDataBasicType::GetRandomFloat(), RandomDataBasicType::GetRandomFloat(),
        RandomDataBasicType::GetRandomFloat());
    return matrix;
}

Range<float> RandomDataCustomizedType::GetRandomFloatRange()
{
    return Range<float>(RandomDataBasicType::GetRandomFloat(), RandomDataBasicType::GetRandomFloat());
}

Range<int64_t> RandomDataCustomizedType::GetRandomInt64Range()
{
    return Range<int64_t>(RandomDataBasicType::GetRandomInt64(), RandomDataBasicType::GetRandomInt64());
}

Range<Color> RandomDataCustomizedType::GetRandomColorRange()
{
    return Range<Color>(GetRandomColor(), GetRandomColor());
}

std::vector<std::shared_ptr<EmitterUpdater>> RandomDataCustomizedType::GetRandomSmallEmitterUpdaterSharedPtrVector()
{
    return GetRandomEmitterUpdaterSharedPtrVector("small");
}

std::shared_ptr<ParticleNoiseFields> RandomDataCustomizedType::GetRandomSmallParticleNoiseFieldsSharedPtr()
{
    return GetRandomParticleNoiseFieldsSharedPtr("small");
}

ForegroundColorStrategyType RandomDataCustomizedType::GetRandomForegroundColorStrategyType()
{
    static constexpr int FOREGROUND_COLOR_STRATEGY_TYPE_INDEX_MAX = 1;
    int randomIndex = RandomEngine::GetRandomIndex(FOREGROUND_COLOR_STRATEGY_TYPE_INDEX_MAX);
    return static_cast<ForegroundColorStrategyType>(randomIndex);
}
} // namespace Rosen
} // namespace OHOS
