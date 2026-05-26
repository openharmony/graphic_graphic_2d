
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
#include "gtest/gtest.h"
#include "ge_visual_effect_impl.h"
#include "effect/rs_render_filter_base.h"
#include "pipeline/rs_render_node.h"
#include "render/rs_render_filter_base.h"
#include "transaction/rs_marshalling_helper.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "parcel.h"
#include "render/rs_path.h"
#include "effect/rs_render_mask_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderFilterBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderFilterBaseTest::SetUpTestCase() {}
void RSRenderFilterBaseTest::TearDownTestCase() {}
void RSRenderFilterBaseTest::SetUp() {}
void RSRenderFilterBaseTest::TearDown() {}

/**
 * @tc.name: UpdateVisualEffectParamImpl001
 * @tc.desc: Test the UpdateVisualEffectParamImpl
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, UpdateVisualEffectParamImpl001, TestSize.Level1)
{
    auto testEffect = std::make_shared<Drawing::GEVisualEffect>(
        RSNGRenderEffectHelper::GetEffectTypeString(RSNGEffectType::SDF_RRECT_SHAPE));
    EXPECT_NE(testEffect, nullptr);

    RectT<float> rect;
    RRect testRRect(rect, 0.5f, 0.5f);
    RSNGRenderEffectHelper::UpdateVisualEffectParamImpl(*testEffect, "test", testRRect);
}

/**
 * @tc.name: UpdateVisualEffectParamImpl002
 * @tc.desc: Test the UpdateVisualEffectParamImpl
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, UpdateVisualEffectParamImpl002, TestSize.Level1)
{
    auto testEffect = std::make_shared<Drawing::GEVisualEffect>(Drawing::GE_FILTER_KAWASE_BLUR);
    EXPECT_NE(testEffect, nullptr);

    // 1 test value
    RSNGRenderEffectHelper::UpdateVisualEffectParamImpl(*testEffect, Drawing::GE_FILTER_KAWASE_BLUR_RADIUS, 1);
    auto impl = testEffect->GetImpl();
    EXPECT_EQ(impl->GetKawaseParams()->radius, 1);
}

/**
 * @tc.name: GenerateGEVisualEffect
 * @tc.desc: Test the GenerateGEVisualEffect method
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, GenerateGEVisualEffect, TestSize.Level1)
{
    std::shared_ptr<RSNGRenderFilterBase> filterNull = nullptr;
    RSNGRenderFilterHelper::GenerateGEVisualEffect(filterNull);
    std::shared_ptr<RSNGRenderFilterBase> filter = std::make_shared<RSNGRenderBlurFilter>();
    RSNGRenderFilterHelper::GenerateGEVisualEffect(filter);
    EXPECT_NE(filter->geFilter_, nullptr);
}

/**
 * @tc.name: CreateAndGetType001
 * @tc.desc: Test the factory method can create filter with correct type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, CreateAndGetType001, TestSize.Level1)
{
    // normal filter types
    auto filterTypes = {
        RSNGEffectType::BLUR,
        RSNGEffectType::EDGE_LIGHT,
        RSNGEffectType::SOUND_WAVE,
        RSNGEffectType::DISPERSION,
        RSNGEffectType::EDGE_LIGHT,
        RSNGEffectType::GASIFY_SCALE_TWIST,
        RSNGEffectType::GASIFY_BLUR,
        RSNGEffectType::GASIFY
    };
    for (const auto& type : filterTypes) {
        auto filter = RSNGRenderFilterBase::Create(type);
        ASSERT_NE(filter, nullptr);
        EXPECT_EQ(filter->GetType(), type);
    }

    // invalid filter type
    auto invalidFilter = RSNGRenderFilterBase::Create(RSNGEffectType::INVALID);
    EXPECT_EQ(invalidFilter, nullptr);
    auto noneFilter = RSNGRenderFilterBase::Create(RSNGEffectType::NONE);
    EXPECT_EQ(noneFilter, nullptr);
}

/**
 * @tc.name: UpdateCacheData
 * @tc.desc: Test the UpdateCacheData method
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, UpdateCacheData, TestSize.Level1)
{
    auto src = std::make_shared<Drawing::GEVisualEffect>("KAWASE_BLUR");
    auto dest = std::make_shared<Drawing::GEVisualEffect>("KAWASE_BLUR");
    auto other = std::make_shared<Drawing::GEVisualEffect>("MESA_BLUR");
    RSNGRenderFilterHelper::UpdateCacheData(src, dest);
    EXPECT_EQ(dest->GetImpl()->GetCache(), nullptr);
    src->GetImpl()->SetCache(std::make_shared<std::any>(std::make_any<float>(1.0)));
    std::shared_ptr<Drawing::GEVisualEffect> null = nullptr;
    RSNGRenderFilterHelper::UpdateCacheData(null, null);
    EXPECT_EQ(null, nullptr);
    RSNGRenderFilterHelper::UpdateCacheData(null, dest);
    RSNGRenderFilterHelper::UpdateCacheData(src, null);
    EXPECT_EQ(null, nullptr);
    ASSERT_NE(src->GetImpl()->GetFilterType(), other->GetImpl()->GetFilterType());
    RSNGRenderFilterHelper::UpdateCacheData(src, other);
    EXPECT_EQ(other->GetImpl()->GetCache(), nullptr);
    RSNGRenderFilterHelper::UpdateCacheData(src, dest);
    auto cachePtr = dest->GetImpl()->GetCache();
    auto cache = std::any_cast<float>(*cachePtr);
    EXPECT_EQ(cache, 1.0);
}

/**
 * @tc.name: GetEffectCount001
 * @tc.desc: 1. Test the behavior of Append method, including appending self, appending valid filters,
 *           handling null filters, and ensuring filters already in the chain are not appended again
 *           2. Test GetEffectCount method to verify the number of filters in the chain
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, GetEffectCount001, TestSize.Level1)
{
    // Test single filter
    auto filter1 = RSNGRenderFilterBase::Create(RSNGEffectType::BLUR);
    EXPECT_EQ(filter1->GetEffectCount(), 1u);

    // Test append filter: BLUR -> EDGE_LIGHT
    auto filter2 = RSNGRenderFilterBase::Create(RSNGEffectType::EDGE_LIGHT);
    filter1->nextEffect_ = filter2;
    EXPECT_EQ(filter1->nextEffect_, filter2);
    EXPECT_EQ(filter1->GetEffectCount(), 2u);

    // Test append nullptr
    filter2->nextEffect_ = nullptr;
    EXPECT_EQ(filter1->GetEffectCount(), 2u);

    // Test append filter: BLUR -> EDGE_LIGHT -> SOUND_WAVE
    auto filter3 = RSNGRenderFilterBase::Create(RSNGEffectType::SOUND_WAVE);
    filter2->nextEffect_ = filter3;
    EXPECT_EQ(filter1->GetEffectCount(), 3u);

    // Test set next filter at middle
    auto filter4 = RSNGRenderFilterBase::Create(RSNGEffectType::DISPERSION);
    filter2->nextEffect_ = filter4;
    EXPECT_EQ(filter2->nextEffect_, filter4);
    EXPECT_EQ(filter3->nextEffect_, nullptr);
    EXPECT_EQ(filter1->GetEffectCount(), 3u);
}

/**
 * @tc.name: Dump001
 * @tc.desc: Test the Dump method outputs correct filter type information
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, Dump001, TestSize.Level1)
{
    std::shared_ptr<RSNGRenderFilterBase> filter = std::make_shared<RSNGRenderBlurFilter>();
    std::string out;
    filter->Dump(out);
    EXPECT_NE(out.find("Blur"), std::string::npos);

    std::shared_ptr<RSNGRenderFilterBase> nextFilter = std::make_shared<RSNGRenderEdgeLightFilter>();
    filter->nextEffect_ = nextFilter;
    filter->Dump(out);
    EXPECT_NE(out.find("EdgeLight"), std::string::npos);
}

/**
 * @tc.name: TemplateContains001
 * @tc.desc: querify the RSNGRenderFilterTemplate's Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, TemplateContains001, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderBlurFilter>();
    EXPECT_TRUE(filter->Contains<BlurRadiusXRenderTag>());
    EXPECT_FALSE(filter->Contains<EdgeLightColorRenderTag>());
}

/**
 * @tc.name: TemplateGetterSetter001
 * @tc.desc: querify the RSNGRenderFilterTemplate's Getter and Setter template interfaces
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, TemplateGetterSetter001, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderBlurFilter>();
    using TargetTag = BlurRadiusXRenderTag;
    constexpr float value = 1.23f;
    filter->Setter<TargetTag>(value);
    auto val = filter->Getter<TargetTag>();
    ASSERT_NE(val, nullptr);
    EXPECT_FLOAT_EQ(val->Get(), value);
}

/**
 * @tc.name: MarshallingAndUnmarshalling001
 * @tc.desc: Test the Marshalling and Unmarshalling methods can
 *           serialize and deserialize a single filter correctly
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, MarshallingAndUnmarshalling001, TestSize.Level1)
{
    // Test Marshalling and Unmarshalling of a single filter and chained filters
    Parcel parcel;
    auto filter1 = std::make_shared<RSNGRenderBlurFilter>();
    auto filter2 = RSNGRenderFilterBase::Create(RSNGEffectType::EDGE_LIGHT);
    filter1->nextEffect_ = filter2;
    std::shared_ptr<RSNGRenderFilterBase> outFilter;
    auto testFunc = [&parcel](const auto& inFilter, auto& outFilter) {
        EXPECT_NE(inFilter, nullptr);
        bool ret = inFilter->Marshalling(parcel);
        EXPECT_TRUE(ret);
        ret = RSNGRenderFilterBase::Unmarshalling(parcel, outFilter);
        EXPECT_TRUE(ret);
        EXPECT_NE(outFilter, nullptr);
        EXPECT_EQ(outFilter->GetType(), inFilter->GetType());
        EXPECT_EQ(outFilter->GetEffectCount(), inFilter->GetEffectCount());
        if (inFilter->nextEffect_ != nullptr) {
            ASSERT_NE(outFilter->nextEffect_, nullptr);
            EXPECT_EQ(outFilter->nextEffect_->GetType(), inFilter->nextEffect_->GetType());
        } else {
            EXPECT_EQ(outFilter->nextEffect_, nullptr);
        }
    };
    testFunc(filter1, outFilter);
    testFunc(filter2, outFilter);

    // Test Unmarshalling empty parcel
    Parcel emptyParcel;
    std::shared_ptr<RSNGRenderFilterBase> emptyFilter;
    bool ret = RSNGRenderFilterBase::Unmarshalling(emptyParcel, emptyFilter);

    EXPECT_FALSE(ret);
    // Test Unmarshalling none type
    Parcel noneParcel;
    RSMarshallingHelper::Marshalling(noneParcel, static_cast<RSNGEffectTypeUnderlying>(RSNGEffectType::NONE));
    std::shared_ptr<RSNGRenderFilterBase> noneFilter;
    ret = RSNGRenderFilterBase::Unmarshalling(noneParcel, noneFilter);
    EXPECT_FALSE(ret);
    EXPECT_EQ(noneFilter, nullptr); // NONE should return nullptr

    // Test Unmarshalling END_OF_CHAIN type
    Parcel endOfChainParcel;
    RSMarshallingHelper::Marshalling(endOfChainParcel, END_OF_CHAIN);
    std::shared_ptr<RSNGRenderFilterBase> endOfChainFilter;
    ret = RSNGRenderFilterBase::Unmarshalling(endOfChainParcel, endOfChainFilter);
    EXPECT_TRUE(ret);
    EXPECT_EQ(endOfChainFilter, nullptr); // END_OF_CHAIN should return nullptr
}

/**
 * @tc.name: MarshallingAndUnmarshalling002
 * @tc.desc: Test the Marshalling and Unmarshalling methods can handle
 *           a filter chain that exceeds the RSNGRenderFilterBase::EFFECT_COUNT_LIMIT
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, MarshallingAndUnmarshalling002, TestSize.Level1)
{
    // Test Marshalling long filter chain
    auto longFilter = std::make_shared<RSNGRenderBlurFilter>();
    std::shared_ptr<RSNGRenderFilterBase> current = longFilter;
    for (size_t i = 1; i < RSNGRenderFilterBase::EFFECT_COUNT_LIMIT; ++i) {
        auto next = std::make_shared<RSNGRenderBlurFilter>();
        current->nextEffect_ = next;
        current = next;
    }
    Parcel parcel;
    auto ret = longFilter->Marshalling(parcel);
    EXPECT_FALSE(ret);

    // Test Unmarshalling with a parcel that has exceeded the limit
    Parcel parcel2;
    auto filter = std::make_shared<RSNGRenderBlurFilter>();
    ret = (filter->Marshalling(parcel2));
    EXPECT_TRUE(ret);
    RSNGEffectTypeUnderlying val = 0;
    ret = RSMarshallingHelper::Unmarshalling(parcel2, val);
    EXPECT_TRUE(ret);
    current = filter;
    for (size_t i = 1; i < RSNGRenderFilterBase::EFFECT_COUNT_LIMIT; ++i) {
        auto nextFilter = std::make_shared<RSNGRenderBlurFilter>();
        current->nextEffect_ = nextFilter;
        current = nextFilter;
    }
    ret = (filter->Marshalling(parcel2));
    EXPECT_FALSE(ret);
    std::shared_ptr<RSNGRenderFilterBase> outFilter = nullptr;
    ret = RSNGRenderFilterBase::Unmarshalling(parcel2, outFilter);
    EXPECT_TRUE(ret);
    EXPECT_EQ(outFilter, nullptr);
}

/**
 * @tc.name: AttachDetach001
 * @tc.desc: Test the Attach and Detach methods can
 *           correctly manage the node association with properties of a single filter
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, AttachDetach001, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderBlurFilter>();
    auto prop = filter->Getter<BlurRadiusXRenderTag>();
    auto radiusX = prop->Get();
    auto node = std::make_shared<RSRenderNode>(1);
    filter->Attach(*node, {});
    EXPECT_EQ(prop->node_.lock(), node);
    filter->Detach();
    EXPECT_EQ(prop->node_.lock(), nullptr);
    EXPECT_FLOAT_EQ(prop->Get(), radiusX);
}

/**
 * @tc.name: AttachDetach002
 * @tc.desc: Test the Attach and Detach methods can
 *           correctly manage the node association with properties of a filter chain
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, AttachDetach002, TestSize.Level1)
{
    auto filter1 = std::make_shared<RSNGRenderBlurFilter>();
    auto filter2 = std::make_shared<RSNGRenderEdgeLightFilter>();
    filter1->nextEffect_ = filter2;
    auto node = std::make_shared<RSRenderNode>(1);
    filter1->Attach(*node, {});
    EXPECT_EQ(filter1->Getter<BlurRadiusXRenderTag>()->node_.lock(), node);
    EXPECT_EQ(filter2->Getter<EdgeLightColorRenderTag>()->node_.lock(), node);
    filter1->Detach();
    EXPECT_EQ(filter1->Getter<BlurRadiusXRenderTag>()->node_.lock(), nullptr);
    EXPECT_EQ(filter2->Getter<EdgeLightColorRenderTag>()->node_.lock(), nullptr);
}

/**
 * @tc.name: DumpProperties001
 * @tc.desc: Test the DumpProperties method outputs correct filter properties
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, DumpProperties001, TestSize.Level1)
{
    using BlurFilter = RSNGRenderBlurFilter;
    auto filter1 = std::make_shared<BlurFilter>();
    std::string out;
    filter1->DumpProperties(out);
    EXPECT_FALSE(out.empty());
}

/**
 * @tc.name: CheckEnableEDR001
 * @tc.desc: Test the CheckEnableEDR method could check enable edr for filters
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, CheckEnableEDR001, TestSize.Level1)
{
    std::shared_ptr<RSNGRenderFilterBase> filter1 = std::make_shared<RSNGRenderBlurFilter>();
    auto filter2 = std::make_shared<RSNGRenderEdgeLightFilter>();
    filter1->nextEffect_ = filter2;
    EXPECT_FALSE(RSNGRenderFilterHelper::CheckEnableEDR(filter1));

    Vector4f color{0.5f, 0.5f, 1.5f, 1.0f};
    filter2->Setter<EdgeLightColorRenderTag>(color);
    EXPECT_TRUE(RSNGRenderFilterHelper::CheckEnableEDR(filter1));
}

/**
 * @tc.name: CalculatePropTagHashImplRRect
 * @tc.desc: test CalculatePropTagHashImpl(uint32_t& hash, const RRect& value)
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, CalculatePropTagHashImplRRect, TestSize.Level1)
{
    uint32_t hash = 0;

    RectT<float> rect;
    RRect value(rect, 0.5f, 0.5f);
    RSNGRenderEffectHelper::CalculatePropTagHashImpl(hash, value);
}

/**
 * @tc.name: AdaptiveParamDark
 * @tc.desc: Test adaptive parameter update when dark scale is enabled
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, AdaptiveParamDark, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderFrostedGlassFilter>();
    // set base and dark-mode specific properties
    filter->Setter<FrostedGlassBgPosRenderTag>(Vector3f(1.0f, 2.0f, 3.0f));
    filter->Setter<FrostedGlassDarkModeBgPosRenderTag>(Vector3f(9.0f, 8.0f, 7.0f));

    // should not crash and should produce a geFilter
    filter->GenerateGEVisualEffect();
    EXPECT_NE(filter->geFilter_, nullptr);
}

/**
 * @tc.name: CalcRect001
 * @tc.desc: Test the CalcRect method
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, CalcRect001, TestSize.Level1)
{
    RectF bound(0.f, 0.f, 10.f, 10.f);
    EXPECT_EQ(RSNGRenderFilterHelper::CalcRect(nullptr, bound, EffectRectType::TOTAL), RectF());

    std::shared_ptr<RSNGRenderFilterBase> filter1 = std::make_shared<RSNGRenderBlurFilter>();
    auto filter2 = std::make_shared<RSNGRenderEdgeLightFilter>();
    filter1->nextEffect_ = filter2;
    EXPECT_EQ(RSNGRenderFilterHelper::CalcRect(filter1, bound, EffectRectType::SNAPSHOT), bound);
    EXPECT_EQ(RSNGRenderFilterHelper::CalcRect(filter1, bound, EffectRectType::DRAW), bound);
    EXPECT_EQ(RSNGRenderFilterHelper::CalcRect(filter1, bound, EffectRectType::TOTAL), RectF());
}

/**
 * @tc.name: CalcRectBlurExpandDrawRegion001
 * @tc.desc: Test CalcRect with BLUR filter and expandDrawRegion enabled
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, CalcRectBlurExpandDrawRegion001, TestSize.Level1)
{
    RectF bound(0.f, 0.f, 100.f, 100.f);
    auto blurFilter = std::make_shared<RSNGRenderBlurFilter>();
    blurFilter->Setter<BlurRadiusXRenderTag>(10.0f);
    blurFilter->Setter<BlurExpandDrawRegionRenderTag>(true);

    RectF result = RSNGRenderFilterHelper::CalcRect(blurFilter, bound, EffectRectType::DRAW);
    constexpr float extensionScale = 3.f;
    float expectedExtension = std::ceil(extensionScale * 10.0f);
    RectF expected = bound.MakeOutset(expectedExtension);
    EXPECT_EQ(result.GetLeft(), expected.GetLeft());
    EXPECT_EQ(result.GetTop(), expected.GetTop());
    EXPECT_EQ(result.GetRight(), expected.GetRight());
    EXPECT_EQ(result.GetBottom(), expected.GetBottom());
}

/**
 * @tc.name: CalcRectBlurExpandDrawRegion002
 * @tc.desc: Test CalcRect with BLUR filter and expandDrawRegion disabled
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, CalcRectBlurExpandDrawRegion002, TestSize.Level1)
{
    RectF bound(0.f, 0.f, 100.f, 100.f);
    auto blurFilter = std::make_shared<RSNGRenderBlurFilter>();
    blurFilter->Setter<BlurRadiusXRenderTag>(10.0f);
    blurFilter->Setter<BlurExpandDrawRegionRenderTag>(false);

    RectF result = RSNGRenderFilterHelper::CalcRect(blurFilter, bound, EffectRectType::SNAPSHOT);
    EXPECT_EQ(result.GetLeft(), bound.GetLeft());
    EXPECT_EQ(result.GetTop(), bound.GetTop());
    EXPECT_EQ(result.GetRight(), bound.GetRight());
    EXPECT_EQ(result.GetBottom(), bound.GetBottom());
}

/**
 * @tc.name: CalculatePropTagHashImplInt
 * @tc.desc: test CalculatePropTagHashImpl(uint32_t& hash, int value)
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, CalculatePropTagHashImplInt, TestSize.Level1)
{
    uint32_t hash1 = 0;
    uint32_t hash2 = 0;
    RSNGRenderEffectHelper::CalculatePropTagHashImpl(hash1, 1);
    RSNGRenderEffectHelper::CalculatePropTagHashImpl(hash2, 2);
    EXPECT_NE(hash1, hash2);
}

/**
 * @tc.name: HasCustomRegion001
 * @tc.desc: Test HasCustomRegion with null filter
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, HasCustomRegion001, TestSize.Level1)
{
    std::shared_ptr<RSNGRenderFilterBase> filter = nullptr;
    EXPECT_FALSE(RSNGRenderFilterHelper::HasCustomRegion(filter));
}

/**
 * @tc.name: HasCustomRegion002
 * @tc.desc: Test HasCustomRegion with blur filter
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, HasCustomRegion002, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderBlurFilter>();
    EXPECT_FALSE(RSNGRenderFilterHelper::HasCustomRegion(filter));
}

/**
 * @tc.name: HasCustomRegion004
 * @tc.desc: Test HasCustomRegion with filter chain
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, HasCustomRegion004, TestSize.Level1)
{
    auto filter1 = std::make_shared<RSNGRenderBlurFilter>();
    auto filter2 = std::make_shared<RSNGRenderEdgeLightFilter>();
    filter1->nextEffect_ = filter2;
    EXPECT_FALSE(RSNGRenderFilterHelper::HasCustomRegion(filter1));
}

/*
 * @tc.name: DispDistortFilterCreate
 * @tc.desc: Test creating DispDistort filter and verify type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, DispDistortFilterCreate, TestSize.Level1)
{
    auto filter = RSNGRenderFilterBase::Create(RSNGEffectType::DISPLACEMENT_DISTORT);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->GetType(), RSNGEffectType::DISPLACEMENT_DISTORT);
}

/**
 * @tc.name: DispDistortFilterSetterGetter
 * @tc.desc: Test DispDistort filter Setter and Getter for Factor property
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, DispDistortFilterSetterGetter, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderDispDistortFilter>();
    Vector2f factor { 1.5f, 2.0f };
    filter->Setter<DispDistortFactorRenderTag>(factor);
    auto val = filter->Getter<DispDistortFactorRenderTag>();
    ASSERT_NE(val, nullptr);
    EXPECT_FLOAT_EQ(val->Get().x_, factor.x_);
    EXPECT_FLOAT_EQ(val->Get().y_, factor.y_);
}

/**
 * @tc.name: DirectionLightFilterCreate
 * @tc.desc: Test creating DirectionLight filter and verify type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, DirectionLightFilterCreate, TestSize.Level1)
{
    auto filter = RSNGRenderFilterBase::Create(RSNGEffectType::DIRECTION_LIGHT);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->GetType(), RSNGEffectType::DIRECTION_LIGHT);
}

/**
 * @tc.name: DirectionLightFilterSetterGetter
 * @tc.desc: Test DirectionLight filter Setter and Getter for properties
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, DirectionLightFilterSetterGetter, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderDirectionLightFilter>();

    float factor = 0.5f;
    filter->Setter<DirectionLightFactorRenderTag>(factor);
    auto factorVal = filter->Getter<DirectionLightFactorRenderTag>();
    ASSERT_NE(factorVal, nullptr);
    EXPECT_FLOAT_EQ(factorVal->Get(), factor);

    Vector3f direction { 1.0f, 0.0f, 0.0f };
    filter->Setter<DirectionLightDirectionRenderTag>(direction);
    auto directionVal = filter->Getter<DirectionLightDirectionRenderTag>();
    ASSERT_NE(directionVal, nullptr);
    EXPECT_FLOAT_EQ(directionVal->Get().x_, direction.x_);

    Vector4f color { 1.0f, 0.5f, 0.0f, 1.0f };
    filter->Setter<DirectionLightColorRenderTag>(color);
    auto colorVal = filter->Getter<DirectionLightColorRenderTag>();
    ASSERT_NE(colorVal, nullptr);
    EXPECT_FLOAT_EQ(colorVal->Get().x_, color.x_);

    float intensity = 1.5f;
    filter->Setter<DirectionLightIntensityRenderTag>(intensity);
    auto intensityVal = filter->Getter<DirectionLightIntensityRenderTag>();
    ASSERT_NE(intensityVal, nullptr);
    EXPECT_FLOAT_EQ(intensityVal->Get(), intensity);
}

/**
 * @tc.name: MaskTransitionFilterCreate
 * @tc.desc: Test creating MaskTransition filter and verify type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, MaskTransitionFilterCreate, TestSize.Level1)
{
    auto filter = RSNGRenderFilterBase::Create(RSNGEffectType::MASK_TRANSITION);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->GetType(), RSNGEffectType::MASK_TRANSITION);
}

/**
 * @tc.name: MaskTransitionFilterSetterGetter
 * @tc.desc: Test MaskTransition filter Setter and Getter for Factor and Inverse properties
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, MaskTransitionFilterSetterGetter, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderMaskTransitionFilter>();

    float factor = 0.8f;
    filter->Setter<MaskTransitionFactorRenderTag>(factor);
    auto factorVal = filter->Getter<MaskTransitionFactorRenderTag>();
    ASSERT_NE(factorVal, nullptr);
    EXPECT_FLOAT_EQ(factorVal->Get(), factor);

    bool inverse = true;
    filter->Setter<MaskTransitionInverseRenderTag>(inverse);
    auto inverseVal = filter->Getter<MaskTransitionInverseRenderTag>();
    ASSERT_NE(inverseVal, nullptr);
    EXPECT_EQ(inverseVal->Get(), inverse);
}

/**
 * @tc.name: VariableRadiusBlurFilterCreate
 * @tc.desc: Test creating VariableRadiusBlur filter and verify type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, VariableRadiusBlurFilterCreate, TestSize.Level1)
{
    auto filter = RSNGRenderFilterBase::Create(RSNGEffectType::VARIABLE_RADIUS_BLUR);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->GetType(), RSNGEffectType::VARIABLE_RADIUS_BLUR);
}

/**
 * @tc.name: VariableRadiusBlurFilterSetterGetter
 * @tc.desc: Test VariableRadiusBlur filter Setter and Getter for Radius property
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, VariableRadiusBlurFilterSetterGetter, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderVariableRadiusBlurFilter>();

    float radius = 10.0f;
    filter->Setter<VariableRadiusBlurRadiusRenderTag>(radius);
    auto radiusVal = filter->Getter<VariableRadiusBlurRadiusRenderTag>();
    ASSERT_NE(radiusVal, nullptr);
    EXPECT_FLOAT_EQ(radiusVal->Get(), radius);
}

/**
 * @tc.name: GridWarpFilterCreate
 * @tc.desc: Test creating GridWarp filter and verify type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, GridWarpFilterCreate, TestSize.Level1)
{
    auto filter = RSNGRenderFilterBase::Create(RSNGEffectType::GRID_WARP);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->GetType(), RSNGEffectType::GRID_WARP);
}

/**
 * @tc.name: GridWarpFilterSetterGetter
 * @tc.desc: Test GridWarp filter Setter and Getter for GridPoint properties
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, GridWarpFilterSetterGetter, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderGridWarpFilter>();

    Vector2f gridPoint0 { 0.1f, 0.2f };
    filter->Setter<GridWarpGridPoint0RenderTag>(gridPoint0);
    auto gridPoint0Val = filter->Getter<GridWarpGridPoint0RenderTag>();
    ASSERT_NE(gridPoint0Val, nullptr);
    EXPECT_FLOAT_EQ(gridPoint0Val->Get().x_, gridPoint0.x_);
    EXPECT_FLOAT_EQ(gridPoint0Val->Get().y_, gridPoint0.y_);

    Vector2f gridPoint1 { 0.3f, 0.4f };
    filter->Setter<GridWarpGridPoint1RenderTag>(gridPoint1);
    auto gridPoint1Val = filter->Getter<GridWarpGridPoint1RenderTag>();
    ASSERT_NE(gridPoint1Val, nullptr);
    EXPECT_FLOAT_EQ(gridPoint1Val->Get().x_, gridPoint1.x_);

    Vector2f rotationAngle0 { 1.0f, 0.0f };
    filter->Setter<GridWarpRotationAngle0RenderTag>(rotationAngle0);
    auto rotationAngle0Val = filter->Getter<GridWarpRotationAngle0RenderTag>();
    ASSERT_NE(rotationAngle0Val, nullptr);
    EXPECT_FLOAT_EQ(rotationAngle0Val->Get().x_, rotationAngle0.x_);
}

/**
 * @tc.name: RadialGradientMaskCreate
 * @tc.desc: Test creating RadialGradientMask and verify type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, RadialGradientMaskCreate, TestSize.Level1)
{
    auto mask = RSNGRenderMaskBase::Create(RSNGEffectType::RADIAL_GRADIENT_MASK);
    ASSERT_NE(mask, nullptr);
    EXPECT_EQ(mask->GetType(), RSNGEffectType::RADIAL_GRADIENT_MASK);
}

/**
 * @tc.name: RadialGradientMaskSetterGetter
 * @tc.desc: Test RadialGradientMask Setter and Getter for properties
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, RadialGradientMaskSetterGetter, TestSize.Level1)
{
    auto mask = std::make_shared<RSNGRenderRadialGradientMask>();

    Vector2f center { 0.5f, 0.5f };
    mask->Setter<RadialGradientMaskCenterRenderTag>(center);
    auto centerVal = mask->Getter<RadialGradientMaskCenterRenderTag>();
    ASSERT_NE(centerVal, nullptr);
    EXPECT_FLOAT_EQ(centerVal->Get().x_, center.x_);
    EXPECT_FLOAT_EQ(centerVal->Get().y_, center.y_);

    float radiusX = 0.3f;
    mask->Setter<RadialGradientMaskRadiusXRenderTag>(radiusX);
    auto radiusXVal = mask->Getter<RadialGradientMaskRadiusXRenderTag>();
    ASSERT_NE(radiusXVal, nullptr);
    EXPECT_FLOAT_EQ(radiusXVal->Get(), radiusX);

    float radiusY = 0.4f;
    mask->Setter<RadialGradientMaskRadiusYRenderTag>(radiusY);
    auto radiusYVal = mask->Getter<RadialGradientMaskRadiusYRenderTag>();
    ASSERT_NE(radiusYVal, nullptr);
    EXPECT_FLOAT_EQ(radiusYVal->Get(), radiusY);

    std::vector<float> colors = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f };
    mask->Setter<RadialGradientMaskColorsRenderTag>(colors);
    auto colorsVal = mask->Getter<RadialGradientMaskColorsRenderTag>();
    ASSERT_NE(colorsVal, nullptr);
    EXPECT_EQ(colorsVal->Get().size(), colors.size());
}

/**
 * @tc.name: WaveGradientMaskCreate
 * @tc.desc: Test creating WaveGradientMask and verify type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, WaveGradientMaskCreate, TestSize.Level1)
{
    auto mask = RSNGRenderMaskBase::Create(RSNGEffectType::WAVE_GRADIENT_MASK);
    ASSERT_NE(mask, nullptr);
    EXPECT_EQ(mask->GetType(), RSNGEffectType::WAVE_GRADIENT_MASK);
}

/**
 * @tc.name: WaveGradientMaskSetterGetter
 * @tc.desc: Test WaveGradientMask Setter and Getter for properties
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, WaveGradientMaskSetterGetter, TestSize.Level1)
{
    auto mask = std::make_shared<RSNGRenderWaveGradientMask>();

    Vector2f waveCenter { 0.5f, 0.5f };
    mask->Setter<WaveGradientMaskWaveCenterRenderTag>(waveCenter);
    auto waveCenterVal = mask->Getter<WaveGradientMaskWaveCenterRenderTag>();
    ASSERT_NE(waveCenterVal, nullptr);
    EXPECT_FLOAT_EQ(waveCenterVal->Get().x_, waveCenter.x_);
    EXPECT_FLOAT_EQ(waveCenterVal->Get().y_, waveCenter.y_);

    float waveWidth = 10.0f;
    mask->Setter<WaveGradientMaskWaveWidthRenderTag>(waveWidth);
    auto waveWidthVal = mask->Getter<WaveGradientMaskWaveWidthRenderTag>();
    ASSERT_NE(waveWidthVal, nullptr);
    EXPECT_FLOAT_EQ(waveWidthVal->Get(), waveWidth);

    float propagationRadius = 50.0f;
    mask->Setter<WaveGradientMaskPropagationRadiusRenderTag>(propagationRadius);
    auto propagationRadiusVal = mask->Getter<WaveGradientMaskPropagationRadiusRenderTag>();
    ASSERT_NE(propagationRadiusVal, nullptr);
    EXPECT_FLOAT_EQ(propagationRadiusVal->Get(), propagationRadius);

    float turbulenceStrength = 0.5f;
    mask->Setter<WaveGradientMaskTurbulenceStrengthRenderTag>(turbulenceStrength);
    auto turbulenceStrengthVal = mask->Getter<WaveGradientMaskTurbulenceStrengthRenderTag>();
    ASSERT_NE(turbulenceStrengthVal, nullptr);
    EXPECT_FLOAT_EQ(turbulenceStrengthVal->Get(), turbulenceStrength);
}

/**
 * @tc.name: FrameGradientMaskCreate
 * @tc.desc: Test creating FrameGradientMask and verify type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, FrameGradientMaskCreate, TestSize.Level1)
{
    auto mask = RSNGRenderMaskBase::Create(RSNGEffectType::FRAME_GRADIENT_MASK);
    ASSERT_NE(mask, nullptr);
    EXPECT_EQ(mask->GetType(), RSNGEffectType::FRAME_GRADIENT_MASK);
}

/**
 * @tc.name: FrameGradientMaskSetterGetter
 * @tc.desc: Test FrameGradientMask Setter and Getter for properties
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, FrameGradientMaskSetterGetter, TestSize.Level1)
{
    auto mask = std::make_shared<RSNGRenderFrameGradientMask>();

    Vector4f innerBezier { 0.0f, 0.0f, 1.0f, 1.0f };
    mask->Setter<FrameGradientMaskInnerBezierRenderTag>(innerBezier);
    auto innerBezierVal = mask->Getter<FrameGradientMaskInnerBezierRenderTag>();
    ASSERT_NE(innerBezierVal, nullptr);
    EXPECT_FLOAT_EQ(innerBezierVal->Get().x_, innerBezier.x_);

    Vector4f outerBezier { 0.0f, 1.0f, 0.0f, 1.0f };
    mask->Setter<FrameGradientMaskOuterBezierRenderTag>(outerBezier);
    auto outerBezierVal = mask->Getter<FrameGradientMaskOuterBezierRenderTag>();
    ASSERT_NE(outerBezierVal, nullptr);
    EXPECT_FLOAT_EQ(outerBezierVal->Get().x_, outerBezier.x_);

    float cornerRadius = 20.0f;
    mask->Setter<FrameGradientMaskCornerRadiusRenderTag>(cornerRadius);
    auto cornerRadiusVal = mask->Getter<FrameGradientMaskCornerRadiusRenderTag>();
    ASSERT_NE(cornerRadiusVal, nullptr);
    EXPECT_FLOAT_EQ(cornerRadiusVal->Get(), cornerRadius);

    float innerFrameWidth = 10.0f;
    mask->Setter<FrameGradientMaskInnerFrameWidthRenderTag>(innerFrameWidth);
    auto innerFrameWidthVal = mask->Getter<FrameGradientMaskInnerFrameWidthRenderTag>();
    ASSERT_NE(innerFrameWidthVal, nullptr);
    EXPECT_FLOAT_EQ(innerFrameWidthVal->Get(), innerFrameWidth);

    float outerFrameWidth = 20.0f;
    mask->Setter<FrameGradientMaskOuterFrameWidthRenderTag>(outerFrameWidth);
    auto outerFrameWidthVal = mask->Getter<FrameGradientMaskOuterFrameWidthRenderTag>();
    ASSERT_NE(outerFrameWidthVal, nullptr);
    EXPECT_FLOAT_EQ(outerFrameWidthVal->Get(), outerFrameWidth);

    Vector2f rectWH { 100.0f, 100.0f };
    mask->Setter<FrameGradientMaskRectWHRenderTag>(rectWH);
    auto rectWHVal = mask->Getter<FrameGradientMaskRectWHRenderTag>();
    ASSERT_NE(rectWHVal, nullptr);
    EXPECT_FLOAT_EQ(rectWHVal->Get().x_, rectWH.x_);

    Vector2f rectPos { 0.0f, 0.0f };
    mask->Setter<FrameGradientMaskRectPosRenderTag>(rectPos);
    auto rectPosVal = mask->Getter<FrameGradientMaskRectPosRenderTag>();
    ASSERT_NE(rectPosVal, nullptr);
    EXPECT_FLOAT_EQ(rectPosVal->Get().x_, rectPos.x_);
}

/**
 * @tc.name: DispDistortFilterContains
 * @tc.desc: Test DispDistort filter Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, DispDistortFilterContains, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderDispDistortFilter>();
    EXPECT_TRUE(filter->Contains<DispDistortFactorRenderTag>());
}

/**
 * @tc.name: DirectionLightFilterContains
 * @tc.desc: Test DirectionLight filter Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, DirectionLightFilterContains, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderDirectionLightFilter>();
    EXPECT_TRUE(filter->Contains<DirectionLightFactorRenderTag>());
    EXPECT_TRUE(filter->Contains<DirectionLightDirectionRenderTag>());
    EXPECT_TRUE(filter->Contains<DirectionLightColorRenderTag>());
    EXPECT_TRUE(filter->Contains<DirectionLightIntensityRenderTag>());
}

/**
 * @tc.name: MaskTransitionFilterContains
 * @tc.desc: Test MaskTransition filter Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, MaskTransitionFilterContains, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderMaskTransitionFilter>();
    EXPECT_TRUE(filter->Contains<MaskTransitionFactorRenderTag>());
    EXPECT_TRUE(filter->Contains<MaskTransitionInverseRenderTag>());
}

/**
 * @tc.name: VariableRadiusBlurFilterContains
 * @tc.desc: Test VariableRadiusBlur filter Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, VariableRadiusBlurFilterContains, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderVariableRadiusBlurFilter>();
    EXPECT_TRUE(filter->Contains<VariableRadiusBlurRadiusRenderTag>());
}

/**
 * @tc.name: GridWarpFilterContains
 * @tc.desc: Test GridWarp filter Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, GridWarpFilterContains, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderGridWarpFilter>();
    EXPECT_TRUE(filter->Contains<GridWarpGridPoint0RenderTag>());
    EXPECT_TRUE(filter->Contains<GridWarpGridPoint1RenderTag>());
    EXPECT_TRUE(filter->Contains<GridWarpRotationAngle0RenderTag>());
}

/**
 * @tc.name: RadialGradientMaskContains
 * @tc.desc: Test RadialGradientMask Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, RadialGradientMaskContains, TestSize.Level1)
{
    auto mask = std::make_shared<RSNGRenderRadialGradientMask>();
    EXPECT_TRUE(mask->Contains<RadialGradientMaskCenterRenderTag>());
    EXPECT_TRUE(mask->Contains<RadialGradientMaskRadiusXRenderTag>());
    EXPECT_TRUE(mask->Contains<RadialGradientMaskRadiusYRenderTag>());
    EXPECT_TRUE(mask->Contains<RadialGradientMaskColorsRenderTag>());
    EXPECT_TRUE(mask->Contains<RadialGradientMaskPositionsRenderTag>());
}

/**
 * @tc.name: WaveGradientMaskContains
 * @tc.desc: Test WaveGradientMask Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, WaveGradientMaskContains, TestSize.Level1)
{
    auto mask = std::make_shared<RSNGRenderWaveGradientMask>();
    EXPECT_TRUE(mask->Contains<WaveGradientMaskWaveCenterRenderTag>());
    EXPECT_TRUE(mask->Contains<WaveGradientMaskWaveWidthRenderTag>());
    EXPECT_TRUE(mask->Contains<WaveGradientMaskPropagationRadiusRenderTag>());
    EXPECT_TRUE(mask->Contains<WaveGradientMaskTurbulenceStrengthRenderTag>());
}

/**
 * @tc.name: FrameGradientMaskContains
 * @tc.desc: Test FrameGradientMask Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, FrameGradientMaskContains, TestSize.Level1)
{
    auto mask = std::make_shared<RSNGRenderFrameGradientMask>();
    EXPECT_TRUE(mask->Contains<FrameGradientMaskInnerBezierRenderTag>());
    EXPECT_TRUE(mask->Contains<FrameGradientMaskOuterBezierRenderTag>());
    EXPECT_TRUE(mask->Contains<FrameGradientMaskCornerRadiusRenderTag>());
    EXPECT_TRUE(mask->Contains<FrameGradientMaskInnerFrameWidthRenderTag>());
    EXPECT_TRUE(mask->Contains<FrameGradientMaskOuterFrameWidthRenderTag>());
    EXPECT_TRUE(mask->Contains<FrameGradientMaskRectWHRenderTag>());
    EXPECT_TRUE(mask->Contains<FrameGradientMaskRectPosRenderTag>());
}

/**
 * @tc.name: DupoliNoiseMaskCreate
 * @tc.desc: Test creating DupoliNoiseMask and verify type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, DupoliNoiseMaskCreate, TestSize.Level1)
{
    auto mask = RSNGRenderMaskBase::Create(RSNGEffectType::DUPOLI_NOISE_MASK);
    ASSERT_NE(mask, nullptr);
    EXPECT_EQ(mask->GetType(), RSNGEffectType::DUPOLI_NOISE_MASK);
}

/**
 * @tc.name: DupoliNoiseMaskSetterGetter
 * @tc.desc: Test DupoliNoiseMask Setter and Getter for properties
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, DupoliNoiseMaskSetterGetter, TestSize.Level1)
{
    auto mask = std::make_shared<RSNGRenderDupoliNoiseMask>();

    float progress = 0.5f;
    mask->Setter<DupoliNoiseMaskProgressRenderTag>(progress);
    auto progressVal = mask->Getter<DupoliNoiseMaskProgressRenderTag>();
    ASSERT_NE(progressVal, nullptr);
    EXPECT_FLOAT_EQ(progressVal->Get(), progress);

    float granularity = 0.3f;
    mask->Setter<DupoliNoiseMaskGranularityRenderTag>(granularity);
    auto granularityVal = mask->Getter<DupoliNoiseMaskGranularityRenderTag>();
    ASSERT_NE(granularityVal, nullptr);
    EXPECT_FLOAT_EQ(granularityVal->Get(), granularity);

    float verticalMoveDistance = 10.0f;
    mask->Setter<DupoliNoiseMaskVerticalMoveDistanceRenderTag>(verticalMoveDistance);
    auto verticalMoveDistanceVal = mask->Getter<DupoliNoiseMaskVerticalMoveDistanceRenderTag>();
    ASSERT_NE(verticalMoveDistanceVal, nullptr);
    EXPECT_FLOAT_EQ(verticalMoveDistanceVal->Get(), verticalMoveDistance);
}

/**
 * @tc.name: DupoliNoiseMaskContains
 * @tc.desc: Test DupoliNoiseMask Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, DupoliNoiseMaskContains, TestSize.Level1)
{
    auto mask = std::make_shared<RSNGRenderDupoliNoiseMask>();
    EXPECT_TRUE(mask->Contains<DupoliNoiseMaskProgressRenderTag>());
    EXPECT_TRUE(mask->Contains<DupoliNoiseMaskGranularityRenderTag>());
    EXPECT_TRUE(mask->Contains<DupoliNoiseMaskVerticalMoveDistanceRenderTag>());
}

/**
 * @tc.name: NoisyFrameGradientMaskCreate
 * @tc.desc: Test creating NoisyFrameGradientMask and verify type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, NoisyFrameGradientMaskCreate, TestSize.Level1)
{
    auto mask = RSNGRenderMaskBase::Create(RSNGEffectType::NOISY_FRAME_GRADIENT_MASK);
    ASSERT_NE(mask, nullptr);
    EXPECT_EQ(mask->GetType(), RSNGEffectType::NOISY_FRAME_GRADIENT_MASK);
}

/**
 * @tc.name: NoisyFrameGradientMaskSetterGetter
 * @tc.desc: Test NoisyFrameGradientMask Setter and Getter for properties
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, NoisyFrameGradientMaskSetterGetter, TestSize.Level1)
{
    auto mask = std::make_shared<RSNGRenderNoisyFrameGradientMask>();

    Vector4f gradientBezierControlPoints { 0.0f, 0.0f, 1.0f, 1.0f };
    mask->Setter<NoisyFrameGradientMaskGradientBezierControlPointsRenderTag>(gradientBezierControlPoints);
    auto gradientBezierControlPointsVal = mask->Getter<NoisyFrameGradientMaskGradientBezierControlPointsRenderTag>();
    ASSERT_NE(gradientBezierControlPointsVal, nullptr);
    EXPECT_FLOAT_EQ(gradientBezierControlPointsVal->Get().x_, gradientBezierControlPoints.x_);

    float cornerRadius = 20.0f;
    mask->Setter<NoisyFrameGradientMaskCornerRadiusRenderTag>(cornerRadius);
    auto cornerRadiusVal = mask->Getter<NoisyFrameGradientMaskCornerRadiusRenderTag>();
    ASSERT_NE(cornerRadiusVal, nullptr);
    EXPECT_FLOAT_EQ(cornerRadiusVal->Get(), cornerRadius);

    Vector2f innerFrameWidth { 10.0f, 10.0f };
    mask->Setter<NoisyFrameGradientMaskInnerFrameWidthRenderTag>(innerFrameWidth);
    auto innerFrameWidthVal = mask->Getter<NoisyFrameGradientMaskInnerFrameWidthRenderTag>();
    ASSERT_NE(innerFrameWidthVal, nullptr);
    EXPECT_FLOAT_EQ(innerFrameWidthVal->Get().x_, innerFrameWidth.x_);

    Vector2f rRectWH { 100.0f, 100.0f };
    mask->Setter<NoisyFrameGradientMaskRRectWHRenderTag>(rRectWH);
    auto rRectWHVal = mask->Getter<NoisyFrameGradientMaskRRectWHRenderTag>();
    ASSERT_NE(rRectWHVal, nullptr);
    EXPECT_FLOAT_EQ(rRectWHVal->Get().x_, rRectWH.x_);

    float slope = 0.5f;
    mask->Setter<NoisyFrameGradientMaskSlopeRenderTag>(slope);
    auto slopeVal = mask->Getter<NoisyFrameGradientMaskSlopeRenderTag>();
    ASSERT_NE(slopeVal, nullptr);
    EXPECT_FLOAT_EQ(slopeVal->Get(), slope);

    float progress = 0.5f;
    mask->Setter<NoisyFrameGradientMaskProgressRenderTag>(progress);
    auto progressVal = mask->Getter<NoisyFrameGradientMaskProgressRenderTag>();
    ASSERT_NE(progressVal, nullptr);
    EXPECT_FLOAT_EQ(progressVal->Get(), progress);
}

/**
 * @tc.name: NoisyFrameGradientMaskContains
 * @tc.desc: Test NoisyFrameGradientMask Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, NoisyFrameGradientMaskContains, TestSize.Level1)
{
    auto mask = std::make_shared<RSNGRenderNoisyFrameGradientMask>();
    EXPECT_TRUE(mask->Contains<NoisyFrameGradientMaskGradientBezierControlPointsRenderTag>());
    EXPECT_TRUE(mask->Contains<NoisyFrameGradientMaskCornerRadiusRenderTag>());
    EXPECT_TRUE(mask->Contains<NoisyFrameGradientMaskInnerFrameWidthRenderTag>());
    EXPECT_TRUE(mask->Contains<NoisyFrameGradientMaskMiddleFrameWidthRenderTag>());
    EXPECT_TRUE(mask->Contains<NoisyFrameGradientMaskOutsideFrameWidthRenderTag>());
    EXPECT_TRUE(mask->Contains<NoisyFrameGradientMaskRRectWHRenderTag>());
    EXPECT_TRUE(mask->Contains<NoisyFrameGradientMaskRRectPosRenderTag>());
    EXPECT_TRUE(mask->Contains<NoisyFrameGradientMaskSlopeRenderTag>());
    EXPECT_TRUE(mask->Contains<NoisyFrameGradientMaskProgressRenderTag>());
}

/**
 * @tc.name: ContainsTypeForFilter
 * @tc.desc: Test ContainsType method for effect chain
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, ContainsTypeForFilter, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderBlurFilter>();
    EXPECT_TRUE(filter->ContainsType(RSNGEffectType::BLUR));
    EXPECT_FALSE(filter->ContainsType(RSNGEffectType::DISPLACEMENT_DISTORT));
    EXPECT_FALSE(filter->ContainsType(RSNGEffectType::SOUND_WAVE));
}

/**
 * @tc.name: ContainsTypeForMask
 * @tc.desc: Test ContainsType method for mask
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, ContainsTypeForMask, TestSize.Level1)
{
    auto mask = std::make_shared<RSNGRenderRippleMask>();
    EXPECT_TRUE(mask->ContainsType(RSNGEffectType::RIPPLE_MASK));
    EXPECT_FALSE(mask->ContainsType(RSNGEffectType::BLUR));
    EXPECT_FALSE(mask->ContainsType(RSNGEffectType::RADIAL_GRADIENT_MASK));
}

/**
 * @tc.name: SoundWaveFilterCreate
 * @tc.desc: Test creating SoundWave filter and verify type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, SoundWaveFilterCreate, TestSize.Level1)
{
    auto filter = RSNGRenderFilterBase::Create(RSNGEffectType::SOUND_WAVE);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->GetType(), RSNGEffectType::SOUND_WAVE);
}

/**
 * @tc.name: SoundWaveFilterSetterGetter
 * @tc.desc: Test SoundWave filter Setter and Getter for properties
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, SoundWaveFilterSetterGetter, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderSoundWaveFilter>();

    Vector4f colorA { 1.0f, 0.0f, 0.0f, 1.0f };
    filter->Setter<SoundWaveColorARenderTag>(colorA);
    auto colorAVal = filter->Getter<SoundWaveColorARenderTag>();
    ASSERT_NE(colorAVal, nullptr);
    EXPECT_FLOAT_EQ(colorAVal->Get().x_, colorA.x_);

    Vector4f colorB { 0.0f, 1.0f, 0.0f, 1.0f };
    filter->Setter<SoundWaveColorBRenderTag>(colorB);
    auto colorBVal = filter->Getter<SoundWaveColorBRenderTag>();
    ASSERT_NE(colorBVal, nullptr);
    EXPECT_FLOAT_EQ(colorBVal->Get().y_, colorB.y_);

    Vector4f colorC { 0.0f, 0.0f, 1.0f, 1.0f };
    filter->Setter<SoundWaveColorCRenderTag>(colorC);
    auto colorCVal = filter->Getter<SoundWaveColorCRenderTag>();
    ASSERT_NE(colorCVal, nullptr);
    EXPECT_FLOAT_EQ(colorCVal->Get().z_, colorC.z_);

    float intensity = 0.8f;
    filter->Setter<SoundWaveIntensityRenderTag>(intensity);
    auto intensityVal = filter->Getter<SoundWaveIntensityRenderTag>();
    ASSERT_NE(intensityVal, nullptr);
    EXPECT_FLOAT_EQ(intensityVal->Get(), intensity);

    float totalAlpha = 1.0f;
    filter->Setter<SoundWaveTotalAlphaRenderTag>(totalAlpha);
    auto totalAlphaVal = filter->Getter<SoundWaveTotalAlphaRenderTag>();
    ASSERT_NE(totalAlphaVal, nullptr);
    EXPECT_FLOAT_EQ(totalAlphaVal->Get(), totalAlpha);
}

/**
 * @tc.name: SoundWaveFilterContains
 * @tc.desc: Test SoundWave filter Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, SoundWaveFilterContains, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderSoundWaveFilter>();
    EXPECT_TRUE(filter->Contains<SoundWaveColorARenderTag>());
    EXPECT_TRUE(filter->Contains<SoundWaveColorBRenderTag>());
    EXPECT_TRUE(filter->Contains<SoundWaveColorCRenderTag>());
    EXPECT_TRUE(filter->Contains<SoundWaveIntensityRenderTag>());
    EXPECT_TRUE(filter->Contains<SoundWaveTotalAlphaRenderTag>());
}

/**
 * @tc.name: DispersionFilterCreate
 * @tc.desc: Test creating Dispersion filter and verify type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, DispersionFilterCreate, TestSize.Level1)
{
    auto filter = RSNGRenderFilterBase::Create(RSNGEffectType::DISPERSION);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->GetType(), RSNGEffectType::DISPERSION);
}

/**
 * @tc.name: DispersionFilterSetterGetter
 * @tc.desc: Test Dispersion filter Setter and Getter for properties
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, DispersionFilterSetterGetter, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderDispersionFilter>();

    float opacity = 0.8f;
    filter->Setter<DispersionOpacityRenderTag>(opacity);
    auto opacityVal = filter->Getter<DispersionOpacityRenderTag>();
    ASSERT_NE(opacityVal, nullptr);
    EXPECT_FLOAT_EQ(opacityVal->Get(), opacity);

    Vector2f redOffset { 5.0f, 0.0f };
    filter->Setter<DispersionRedOffsetRenderTag>(redOffset);
    auto redOffsetVal = filter->Getter<DispersionRedOffsetRenderTag>();
    ASSERT_NE(redOffsetVal, nullptr);
    EXPECT_FLOAT_EQ(redOffsetVal->Get().x_, redOffset.x_);

    Vector2f greenOffset { 0.0f, 5.0f };
    filter->Setter<DispersionGreenOffsetRenderTag>(greenOffset);
    auto greenOffsetVal = filter->Getter<DispersionGreenOffsetRenderTag>();
    ASSERT_NE(greenOffsetVal, nullptr);
    EXPECT_FLOAT_EQ(greenOffsetVal->Get().y_, greenOffset.y_);

    Vector2f blueOffset { -5.0f, -5.0f };
    filter->Setter<DispersionBlueOffsetRenderTag>(blueOffset);
    auto blueOffsetVal = filter->Getter<DispersionBlueOffsetRenderTag>();
    ASSERT_NE(blueOffsetVal, nullptr);
    EXPECT_FLOAT_EQ(blueOffsetVal->Get().x_, blueOffset.x_);
}

/**
 * @tc.name: DispersionFilterContains
 * @tc.desc: Test Dispersion filter Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, DispersionFilterContains, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderDispersionFilter>();
    EXPECT_TRUE(filter->Contains<DispersionOpacityRenderTag>());
    EXPECT_TRUE(filter->Contains<DispersionRedOffsetRenderTag>());
    EXPECT_TRUE(filter->Contains<DispersionGreenOffsetRenderTag>());
    EXPECT_TRUE(filter->Contains<DispersionBlueOffsetRenderTag>());
}

/**
 * @tc.name: RippleMaskSetterGetter
 * @tc.desc: Test RippleMask Setter and Getter for properties
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, RippleMaskSetterGetter, TestSize.Level1)
{
    auto mask = std::make_shared<RSNGRenderRippleMask>();

    Vector2f center { 0.5f, 0.5f };
    mask->Setter<RippleMaskCenterRenderTag>(center);
    auto centerVal = mask->Getter<RippleMaskCenterRenderTag>();
    ASSERT_NE(centerVal, nullptr);
    EXPECT_FLOAT_EQ(centerVal->Get().x_, center.x_);
    EXPECT_FLOAT_EQ(centerVal->Get().y_, center.y_);

    float radius = 50.0f;
    mask->Setter<RippleMaskRadiusRenderTag>(radius);
    auto radiusVal = mask->Getter<RippleMaskRadiusRenderTag>();
    ASSERT_NE(radiusVal, nullptr);
    EXPECT_FLOAT_EQ(radiusVal->Get(), radius);

    float width = 10.0f;
    mask->Setter<RippleMaskWidthRenderTag>(width);
    auto widthVal = mask->Getter<RippleMaskWidthRenderTag>();
    ASSERT_NE(widthVal, nullptr);
    EXPECT_FLOAT_EQ(widthVal->Get(), width);

    float offset = 0.0f;
    mask->Setter<RippleMaskOffsetRenderTag>(offset);
    auto offsetVal = mask->Getter<RippleMaskOffsetRenderTag>();
    ASSERT_NE(offsetVal, nullptr);
    EXPECT_FLOAT_EQ(offsetVal->Get(), offset);
}

/**
 * @tc.name: RippleMaskContains
 * @tc.desc: Test RippleMask Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, RippleMaskContains, TestSize.Level1)
{
    auto mask = std::make_shared<RSNGRenderRippleMask>();
    EXPECT_TRUE(mask->Contains<RippleMaskCenterRenderTag>());
    EXPECT_TRUE(mask->Contains<RippleMaskRadiusRenderTag>());
    EXPECT_TRUE(mask->Contains<RippleMaskWidthRenderTag>());
    EXPECT_TRUE(mask->Contains<RippleMaskOffsetRenderTag>());
}

/**
 * @tc.name: PixelMapMaskCreate
 * @tc.desc: Test creating PixelMapMask and verify type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, PixelMapMaskCreate, TestSize.Level1)
{
    auto mask = RSNGRenderMaskBase::Create(RSNGEffectType::PIXEL_MAP_MASK);
    ASSERT_NE(mask, nullptr);
    EXPECT_EQ(mask->GetType(), RSNGEffectType::PIXEL_MAP_MASK);
}

/**
 * @tc.name: PixelMapMaskSetterGetter
 * @tc.desc: Test PixelMapMask Setter and Getter for properties
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, PixelMapMaskSetterGetter, TestSize.Level1)
{
    auto mask = std::make_shared<RSNGRenderPixelMapMask>();

    Vector4f src { 0.0f, 0.0f, 1.0f, 1.0f };
    mask->Setter<PixelMapMaskSrcRenderTag>(src);
    auto srcVal = mask->Getter<PixelMapMaskSrcRenderTag>();
    ASSERT_NE(srcVal, nullptr);
    EXPECT_FLOAT_EQ(srcVal->Get().x_, src.x_);

    Vector4f dst { 0.0f, 0.0f, 100.0f, 100.0f };
    mask->Setter<PixelMapMaskDstRenderTag>(dst);
    auto dstVal = mask->Getter<PixelMapMaskDstRenderTag>();
    ASSERT_NE(dstVal, nullptr);
    EXPECT_FLOAT_EQ(dstVal->Get().z_, dst.z_);

    Vector4f fillColor { 1.0f, 1.0f, 1.0f, 1.0f };
    mask->Setter<PixelMapMaskFillColorRenderTag>(fillColor);
    auto fillColorVal = mask->Getter<PixelMapMaskFillColorRenderTag>();
    ASSERT_NE(fillColorVal, nullptr);
    EXPECT_FLOAT_EQ(fillColorVal->Get().x_, fillColor.x_);
}

/**
 * @tc.name: PixelMapMaskContains
 * @tc.desc: Test PixelMapMask Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, PixelMapMaskContains, TestSize.Level1)
{
    auto mask = std::make_shared<RSNGRenderPixelMapMask>();
    EXPECT_TRUE(mask->Contains<PixelMapMaskSrcRenderTag>());
    EXPECT_TRUE(mask->Contains<PixelMapMaskDstRenderTag>());
    EXPECT_TRUE(mask->Contains<PixelMapMaskFillColorRenderTag>());
}

/**
 * @tc.name: ColorGradientFilterCreate
 * @tc.desc: Test creating ColorGradient filter and verify type
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, ColorGradientFilterCreate, TestSize.Level1)
{
    auto filter = RSNGRenderFilterBase::Create(RSNGEffectType::COLOR_GRADIENT);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->GetType(), RSNGEffectType::COLOR_GRADIENT);
}

/**
 * @tc.name: ColorGradientFilterSetterGetter
 * @tc.desc: Test ColorGradient filter Setter and Getter for properties
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, ColorGradientFilterSetterGetter, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderColorGradientFilter>();

    std::vector<float> colors = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f };
    filter->Setter<ColorGradientColorsRenderTag>(colors);
    auto colorsVal = filter->Getter<ColorGradientColorsRenderTag>();
    ASSERT_NE(colorsVal, nullptr);
    EXPECT_EQ(colorsVal->Get().size(), colors.size());

    std::vector<float> positions = { 0.0f, 1.0f };
    filter->Setter<ColorGradientPositionsRenderTag>(positions);
    auto positionsVal = filter->Getter<ColorGradientPositionsRenderTag>();
    ASSERT_NE(positionsVal, nullptr);
    EXPECT_EQ(positionsVal->Get().size(), positions.size());
    EXPECT_FLOAT_EQ(positionsVal->Get()[0], positions[0]);

    std::vector<float> strengths = { 0.5f, 0.8f };
    filter->Setter<ColorGradientStrengthsRenderTag>(strengths);
    auto strengthsVal = filter->Getter<ColorGradientStrengthsRenderTag>();
    ASSERT_NE(strengthsVal, nullptr);
    EXPECT_EQ(strengthsVal->Get().size(), strengths.size());
    EXPECT_FLOAT_EQ(strengthsVal->Get()[0], strengths[0]);
}

/**
 * @tc.name: ColorGradientFilterContains
 * @tc.desc: Test ColorGradient filter Contains template interface
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderFilterBaseTest, ColorGradientFilterContains, TestSize.Level1)
{
    auto filter = std::make_shared<RSNGRenderColorGradientFilter>();
    EXPECT_TRUE(filter->Contains<ColorGradientColorsRenderTag>());
    EXPECT_TRUE(filter->Contains<ColorGradientPositionsRenderTag>());
    EXPECT_TRUE(filter->Contains<ColorGradientStrengthsRenderTag>());
    EXPECT_TRUE(filter->Contains<ColorGradientMaskRenderTag>());
}
} // namespace OHOS::Rosen