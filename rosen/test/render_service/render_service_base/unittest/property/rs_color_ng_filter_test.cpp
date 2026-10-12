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

#include "gtest/gtest.h"
#include "property/rs_properties.h"
#include "effect/rs_render_filter_base.h"
#include "effect/rs_render_property_tag.h"

using namespace testing;
using namespace OHOS;
using namespace OHOS::Rosen;

class RSColorNGFilterTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

HWTEST_F(RSColorNGFilterTest, CreateBinarizationFilter, TestSize.Level1)
{
    auto filter = RSNGRenderFilterBase::Create(RSNGEffectType::BINARIZATION);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->GetType(), RSNGEffectType::BINARIZATION);
}

HWTEST_F(RSColorNGFilterTest, CreateLightUpEffectFilter, TestSize.Level1)
{
    auto filter = RSNGRenderFilterBase::Create(RSNGEffectType::LIGHT_UP_EFFECT);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->GetType(), RSNGEffectType::LIGHT_UP_EFFECT);
}

HWTEST_F(RSColorNGFilterTest, CreateDynamicDimFilter, TestSize.Level1)
{
    auto filter = RSNGRenderFilterBase::Create(RSNGEffectType::DYNAMIC_DIM);
    ASSERT_NE(filter, nullptr);
    EXPECT_EQ(filter->GetType(), RSNGEffectType::DYNAMIC_DIM);
}

HWTEST_F(RSColorNGFilterTest, BinarizationFilterParamSave, TestSize.Level1)
{
    auto filter = RSNGRenderFilterBase::Create(RSNGEffectType::BINARIZATION);
    ASSERT_NE(filter, nullptr);
    auto binFilter = std::static_pointer_cast<RSNGRenderBinarizationFilter>(filter);
    binFilter->Setter<BinarizationLowRenderTag>(0.2f);
    binFilter->Setter<BinarizationHighRenderTag>(0.8f);
    binFilter->Setter<BinarizationThresholdRenderTag>(0.5f);
    binFilter->Setter<BinarizationThresholdRangeRenderTag>(0.1f);
    EXPECT_FLOAT_EQ(binFilter->Getter<BinarizationLowRenderTag>()->Get(), 0.2f);
    EXPECT_FLOAT_EQ(binFilter->Getter<BinarizationHighRenderTag>()->Get(), 0.8f);
    EXPECT_FLOAT_EQ(binFilter->Getter<BinarizationThresholdRenderTag>()->Get(), 0.5f);
    EXPECT_FLOAT_EQ(binFilter->Getter<BinarizationThresholdRangeRenderTag>()->Get(), 0.1f);
}

HWTEST_F(RSColorNGFilterTest, LightUpEffectFilterParamSave, TestSize.Level1)
{
    auto filter = RSNGRenderFilterBase::Create(RSNGEffectType::LIGHT_UP_EFFECT);
    ASSERT_NE(filter, nullptr);
    auto lightUpFilter = std::static_pointer_cast<RSNGRenderLightUpEffectFilter>(filter);
    lightUpFilter->Setter<LightUpEffectValueRenderTag>(0.7f);
    EXPECT_FLOAT_EQ(lightUpFilter->Getter<LightUpEffectValueRenderTag>()->Get(), 0.7f);
}

HWTEST_F(RSColorNGFilterTest, DynamicDimFilterParamSave, TestSize.Level1)
{
    auto filter = RSNGRenderFilterBase::Create(RSNGEffectType::DYNAMIC_DIM);
    ASSERT_NE(filter, nullptr);
    auto dimFilter = std::static_pointer_cast<RSNGRenderDynamicDimFilter>(filter);
    dimFilter->Setter<DynamicDimValueRenderTag>(0.3f);
    EXPECT_FLOAT_EQ(dimFilter->Getter<DynamicDimValueRenderTag>()->Get(), 0.3f);
}

HWTEST_F(RSColorNGFilterTest, LastSetterWinsBinarizationThenLightUp, TestSize.Level1)
{
    RSProperties properties;
    auto binFilter = RSNGRenderFilterBase::Create(RSNGEffectType::BINARIZATION);
    ASSERT_NE(binFilter, nullptr);
    properties.SetColorNGFilter(binFilter);
    EXPECT_EQ(properties.GetColorNGFilter()->GetType(), RSNGEffectType::BINARIZATION);

    auto lightUpFilter = RSNGRenderFilterBase::Create(RSNGEffectType::LIGHT_UP_EFFECT);
    ASSERT_NE(lightUpFilter, nullptr);
    properties.SetColorNGFilter(lightUpFilter);
    EXPECT_EQ(properties.GetColorNGFilter()->GetType(), RSNGEffectType::LIGHT_UP_EFFECT);
}

HWTEST_F(RSColorNGFilterTest, LastSetterWinsLightUpThenDynamicDim, TestSize.Level1)
{
    RSProperties properties;
    auto lightUpFilter = RSNGRenderFilterBase::Create(RSNGEffectType::LIGHT_UP_EFFECT);
    ASSERT_NE(lightUpFilter, nullptr);
    properties.SetColorNGFilter(lightUpFilter);
    EXPECT_EQ(properties.GetColorNGFilter()->GetType(), RSNGEffectType::LIGHT_UP_EFFECT);

    auto dimFilter = RSNGRenderFilterBase::Create(RSNGEffectType::DYNAMIC_DIM);
    ASSERT_NE(dimFilter, nullptr);
    properties.SetColorNGFilter(dimFilter);
    EXPECT_EQ(properties.GetColorNGFilter()->GetType(), RSNGEffectType::DYNAMIC_DIM);
}

HWTEST_F(RSColorNGFilterTest, LastSetterWinsDynamicDimThenBinarization, TestSize.Level1)
{
    RSProperties properties;
    auto dimFilter = RSNGRenderFilterBase::Create(RSNGEffectType::DYNAMIC_DIM);
    ASSERT_NE(dimFilter, nullptr);
    properties.SetColorNGFilter(dimFilter);
    EXPECT_EQ(properties.GetColorNGFilter()->GetType(), RSNGEffectType::DYNAMIC_DIM);

    auto binFilter = RSNGRenderFilterBase::Create(RSNGEffectType::BINARIZATION);
    ASSERT_NE(binFilter, nullptr);
    properties.SetColorNGFilter(binFilter);
    EXPECT_EQ(properties.GetColorNGFilter()->GetType(), RSNGEffectType::BINARIZATION);
}

HWTEST_F(RSColorNGFilterTest, ColorNGFilterClearToNull, TestSize.Level1)
{
    RSProperties properties;
    auto filter = RSNGRenderFilterBase::Create(RSNGEffectType::LIGHT_UP_EFFECT);
    ASSERT_NE(filter, nullptr);
    properties.SetColorNGFilter(filter);
    ASSERT_NE(properties.GetColorNGFilter(), nullptr);

    properties.SetColorNGFilter(nullptr);
    EXPECT_EQ(properties.GetColorNGFilter(), nullptr);
}

HWTEST_F(RSColorNGFilterTest, DefaultColorNGFilterIsNullptr, TestSize.Level1)
{
    RSProperties properties;
    EXPECT_EQ(properties.GetColorNGFilter(), nullptr);
}

HWTEST_F(RSColorNGFilterTest, BinarizationFilterSerialization, TestSize.Level1)
{
    auto filter = RSNGRenderFilterBase::Create(RSNGEffectType::BINARIZATION);
    ASSERT_NE(filter, nullptr);
    auto binFilter = std::static_pointer_cast<RSNGRenderBinarizationFilter>(filter);
    binFilter->Setter<BinarizationLowRenderTag>(0.1f);
    binFilter->Setter<BinarizationHighRenderTag>(0.9f);
    binFilter->Setter<BinarizationThresholdRenderTag>(0.4f);
    binFilter->Setter<BinarizationThresholdRangeRenderTag>(0.05f);

    Parcel parcel;
    ASSERT_TRUE(filter->Marshalling(parcel));
    parcel.RewindRead(0);

    std::shared_ptr<RSNGRenderFilterBase> unmarshalledFilter;
    ASSERT_TRUE(RSNGRenderFilterBase::Unmarshalling(parcel, unmarshalledFilter));
    ASSERT_NE(unmarshalledFilter, nullptr);
    EXPECT_EQ(unmarshalledFilter->GetType(), RSNGEffectType::BINARIZATION);

    auto unmarshalledBin = std::static_pointer_cast<RSNGRenderBinarizationFilter>(unmarshalledFilter);
    EXPECT_FLOAT_EQ(unmarshalledBin->Getter<BinarizationLowRenderTag>()->Get(), 0.1f);
    EXPECT_FLOAT_EQ(unmarshalledBin->Getter<BinarizationHighRenderTag>()->Get(), 0.9f);
    EXPECT_FLOAT_EQ(unmarshalledBin->Getter<BinarizationThresholdRenderTag>()->Get(), 0.4f);
    EXPECT_FLOAT_EQ(unmarshalledBin->Getter<BinarizationThresholdRangeRenderTag>()->Get(), 0.05f);
}

HWTEST_F(RSColorNGFilterTest, LightUpEffectFilterSerialization, TestSize.Level1)
{
    auto filter = RSNGRenderFilterBase::Create(RSNGEffectType::LIGHT_UP_EFFECT);
    ASSERT_NE(filter, nullptr);
    auto lightUpFilter = std::static_pointer_cast<RSNGRenderLightUpEffectFilter>(filter);
    lightUpFilter->Setter<LightUpEffectValueRenderTag>(0.65f);

    Parcel parcel;
    ASSERT_TRUE(filter->Marshalling(parcel));
    parcel.RewindRead(0);

    std::shared_ptr<RSNGRenderFilterBase> unmarshalledFilter;
    ASSERT_TRUE(RSNGRenderFilterBase::Unmarshalling(parcel, unmarshalledFilter));
    ASSERT_NE(unmarshalledFilter, nullptr);
    EXPECT_EQ(unmarshalledFilter->GetType(), RSNGEffectType::LIGHT_UP_EFFECT);

    auto unmarshalledLightUp = std::static_pointer_cast<RSNGRenderLightUpEffectFilter>(unmarshalledFilter);
    EXPECT_FLOAT_EQ(unmarshalledLightUp->Getter<LightUpEffectValueRenderTag>()->Get(), 0.65f);
}

HWTEST_F(RSColorNGFilterTest, DynamicDimFilterSerialization, TestSize.Level1)
{
    auto filter = RSNGRenderFilterBase::Create(RSNGEffectType::DYNAMIC_DIM);
    ASSERT_NE(filter, nullptr);
    auto dimFilter = std::static_pointer_cast<RSNGRenderDynamicDimFilter>(filter);
    dimFilter->Setter<DynamicDimValueRenderTag>(0.45f);

    Parcel parcel;
    ASSERT_TRUE(filter->Marshalling(parcel));
    parcel.RewindRead(0);

    std::shared_ptr<RSNGRenderFilterBase> unmarshalledFilter;
    ASSERT_TRUE(RSNGRenderFilterBase::Unmarshalling(parcel, unmarshalledFilter));
    ASSERT_NE(unmarshalledFilter, nullptr);
    EXPECT_EQ(unmarshalledFilter->GetType(), RSNGEffectType::DYNAMIC_DIM);

    auto unmarshalledDim = std::static_pointer_cast<RSNGRenderDynamicDimFilter>(unmarshalledFilter);
    EXPECT_FLOAT_EQ(unmarshalledDim->Getter<DynamicDimValueRenderTag>()->Get(), 0.45f);
}

HWTEST_F(RSColorNGFilterTest, BinarizationFilterClone, TestSize.Level1)
{
    auto filter = RSNGRenderFilterBase::Create(RSNGEffectType::BINARIZATION);
    ASSERT_NE(filter, nullptr);
    auto binFilter = std::static_pointer_cast<RSNGRenderBinarizationFilter>(filter);
    binFilter->Setter<BinarizationLowRenderTag>(0.3f);
    binFilter->Setter<BinarizationHighRenderTag>(0.7f);

    auto cloned = RSNGRenderFilterHelper::CreateRenderFilterByTuple(binFilter->properties_);
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetType(), RSNGEffectType::BINARIZATION);
    auto clonedBin = std::static_pointer_cast<RSNGRenderBinarizationFilter>(cloned);
    EXPECT_FLOAT_EQ(clonedBin->Getter<BinarizationLowRenderTag>()->Get(), 0.3f);
    EXPECT_FLOAT_EQ(clonedBin->Getter<BinarizationHighRenderTag>()->Get(), 0.7f);
}

HWTEST_F(RSColorNGFilterTest, BinarizationFilterHash, TestSize.Level1)
{
    auto filter1 = RSNGRenderFilterBase::Create(RSNGEffectType::BINARIZATION);
    ASSERT_NE(filter1, nullptr);
    auto binFilter1 = std::static_pointer_cast<RSNGRenderBinarizationFilter>(filter1);
    binFilter1->Setter<BinarizationLowRenderTag>(0.5f);
    binFilter1->Setter<BinarizationHighRenderTag>(0.5f);
    binFilter1->Setter<BinarizationThresholdRenderTag>(0.5f);
    binFilter1->Setter<BinarizationThresholdRangeRenderTag>(0.0f);
    uint32_t hash1 = filter1->CalculateHash();
    EXPECT_NE(hash1, 0u);

    auto filter2 = RSNGRenderFilterBase::Create(RSNGEffectType::BINARIZATION);
    ASSERT_NE(filter2, nullptr);
    auto binFilter2 = std::static_pointer_cast<RSNGRenderBinarizationFilter>(filter2);
    binFilter2->Setter<BinarizationLowRenderTag>(0.5f);
    binFilter2->Setter<BinarizationHighRenderTag>(0.5f);
    binFilter2->Setter<BinarizationThresholdRenderTag>(0.5f);
    binFilter2->Setter<BinarizationThresholdRangeRenderTag>(0.0f);
    uint32_t hash2 = filter2->CalculateHash();
    EXPECT_EQ(hash1, hash2);
}
