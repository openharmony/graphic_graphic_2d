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

#ifdef ROSEN_OHOS
#include <parcel.h>
#include <message_parcel.h>
#endif

#include "effect/blend_shader_obj.h"
#include "effect/shader_effect.h"
#include "effect/shader_effect_lazy.h"
#include "transaction/rs_marshalling_helper.h"
#include "utils/object_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ShaderEffectLazyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
#ifdef ROSEN_OHOS
    static std::function<bool(Parcel&, std::shared_ptr<Data>)> originalMarshallingCallback_;
    static std::function<std::shared_ptr<Data>(Parcel&)> originalUnmarshallingCallback_;
#endif
};

#ifdef ROSEN_OHOS
std::function<bool(Parcel&, std::shared_ptr<Data>)> ShaderEffectLazyTest::originalMarshallingCallback_;
std::function<std::shared_ptr<Data>(Parcel&)> ShaderEffectLazyTest::originalUnmarshallingCallback_;
#endif

void ShaderEffectLazyTest::SetUpTestCase()
{
#ifdef ROSEN_OHOS
    // Save original callbacks
    originalMarshallingCallback_ = ObjectHelper::Instance().GetDataMarshallingCallback();
    originalUnmarshallingCallback_ = ObjectHelper::Instance().GetDataUnmarshallingCallback();

    // Register Data marshalling/unmarshalling callbacks
    ObjectHelper::Instance().SetDataMarshallingCallback(
        [](Parcel& parcel, std::shared_ptr<Data> data) -> bool {
            return OHOS::Rosen::RSMarshallingHelper::Marshalling(parcel, data);
        }
    );
    ObjectHelper::Instance().SetDataUnmarshallingCallback(
        [](Parcel& parcel) -> std::shared_ptr<Data> {
            std::shared_ptr<Data> data;
            return OHOS::Rosen::RSMarshallingHelper::Unmarshalling(parcel, data) ? data : nullptr;
        }
    );
#endif
}

void ShaderEffectLazyTest::TearDownTestCase()
{
#ifdef ROSEN_OHOS
    // Restore original callbacks
    ObjectHelper::Instance().SetDataMarshallingCallback(originalMarshallingCallback_);
    ObjectHelper::Instance().SetDataUnmarshallingCallback(originalUnmarshallingCallback_);
#endif
}
void ShaderEffectLazyTest::SetUp() {}
void ShaderEffectLazyTest::TearDown() {}

/*
 * @tc.name: CreateFromShaderEffectObj001
 * @tc.desc: Test ShaderEffectLazy creation from ShaderEffectObj and GetShaderEffectObj functionality
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyTest, CreateFromShaderEffectObj001, TestSize.Level1)
{
    // Test 1: CreateFromShaderEffectObj with valid BlendShaderObj
    auto dstShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    auto srcShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto shaderObj = BlendShaderObj::Create(dstShader, srcShader, BlendMode::SRC_OVER);

    auto lazyShader = ShaderEffectLazy::CreateFromShaderEffectObj(shaderObj);
    EXPECT_TRUE(lazyShader != nullptr);
    EXPECT_TRUE(lazyShader->IsLazy());
    EXPECT_EQ(lazyShader->GetType(), ShaderEffect::ShaderEffectType::LAZY_SHADER);

    // Test GetShaderEffectObj functionality
    auto retrievedObj = lazyShader->GetShaderEffectObj();
    EXPECT_TRUE(retrievedObj != nullptr);
    EXPECT_EQ(retrievedObj->GetType(), static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(retrievedObj->GetSubType(), static_cast<int32_t>(ShaderEffect::ShaderEffectType::BLEND));

    // Verify that the original and retrieved objects are consistent
    EXPECT_EQ(shaderObj->GetType(), retrievedObj->GetType());
    EXPECT_EQ(shaderObj->GetSubType(), retrievedObj->GetSubType());
    EXPECT_EQ(shaderObj.get(), retrievedObj.get()); // Should be the same object instance

    // Test 2: CreateFromShaderEffectObj with null object
    std::shared_ptr<ShaderEffectObj> nullObj = nullptr;
    auto nullLazyShader = ShaderEffectLazy::CreateFromShaderEffectObj(nullObj);
    EXPECT_TRUE(nullLazyShader == nullptr);
}

/*
 * @tc.name: Materialize001
 * @tc.desc: Test ShaderEffectLazy::Materialize functionality
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyTest, Materialize001, TestSize.Level1)
{
    auto dstShader = ShaderEffect::CreateColorShader(0xFFFF0000);
    auto srcShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    auto lazyShader = ShaderEffectLazy::CreateBlendShader(dstShader, srcShader, BlendMode::SRC_OVER);
    EXPECT_TRUE(lazyShader != nullptr);

    auto materializedShader = lazyShader->Materialize();
    EXPECT_TRUE(materializedShader != nullptr);
    EXPECT_FALSE(materializedShader->IsLazy());
    EXPECT_EQ(materializedShader->GetType(), ShaderEffect::ShaderEffectType::BLEND);
}

/*
 * @tc.name: CreateBlendShader001
 * @tc.desc: Test ShaderEffectLazy::CreateBlendShader with various parameter scenarios
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyTest, CreateBlendShader001, TestSize.Level1)
{
    // Test 1: CreateBlendShader with valid shaders
    auto dstShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto srcShader = ShaderEffect::CreateColorShader(0xFF00FF00);
    EXPECT_TRUE(dstShader != nullptr);
    EXPECT_TRUE(srcShader != nullptr);

    auto lazyBlendShader = ShaderEffectLazy::CreateBlendShader(dstShader, srcShader, BlendMode::SRC_OVER);
    EXPECT_TRUE(lazyBlendShader != nullptr);

    // Verify it's a Lazy shader with correct type
    EXPECT_TRUE(lazyBlendShader->IsLazy());
    EXPECT_EQ(lazyBlendShader->GetType(), ShaderEffect::ShaderEffectType::LAZY_SHADER);

    // Verify the underlying object
    auto shaderObj = lazyBlendShader->GetShaderEffectObj();
    EXPECT_TRUE(shaderObj != nullptr);
    EXPECT_EQ(shaderObj->GetType(), static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(shaderObj->GetSubType(), static_cast<int32_t>(ShaderEffect::ShaderEffectType::BLEND));

    // Test 2: CreateBlendShader with null dst shader
    std::shared_ptr<ShaderEffect> nullDst = nullptr;
    auto nullDstResult = ShaderEffectLazy::CreateBlendShader(nullDst, srcShader, BlendMode::MULTIPLY);
    EXPECT_TRUE(nullDstResult == nullptr); // Should return nullptr for null dst

    // Test 3: CreateBlendShader with null src shader
    std::shared_ptr<ShaderEffect> nullSrc = nullptr;
    auto nullSrcResult = ShaderEffectLazy::CreateBlendShader(dstShader, nullSrc, BlendMode::SCREEN);
    EXPECT_TRUE(nullSrcResult == nullptr); // Should return nullptr for null src
}

/*
 * @tc.name: CreateBlendShader002
 * @tc.desc: Test ShaderEffectLazy::CreateBlendShader with Lazy shaders as input
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyTest, CreateBlendShader002, TestSize.Level1)
{
    // Create Lazy shaders as input
    auto baseShader1 = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto baseShader2 = ShaderEffect::CreateColorShader(0xFF00FF00);
    auto lazyDst = ShaderEffectLazy::CreateBlendShader(baseShader1, baseShader1, BlendMode::SRC_OVER);
    auto lazySrc = ShaderEffectLazy::CreateBlendShader(baseShader2, baseShader2, BlendMode::SRC_OVER);

    EXPECT_TRUE(lazyDst != nullptr);
    EXPECT_TRUE(lazySrc != nullptr);
    EXPECT_TRUE(lazyDst->IsLazy());
    EXPECT_TRUE(lazySrc->IsLazy());

    // CreateBlendShader should accept Lazy shaders as input
    auto lazyBlendShader = ShaderEffectLazy::CreateBlendShader(lazyDst, lazySrc, BlendMode::OVERLAY);
    EXPECT_TRUE(lazyBlendShader != nullptr);
    EXPECT_TRUE(lazyBlendShader->IsLazy());
    EXPECT_EQ(lazyBlendShader->GetType(), ShaderEffect::ShaderEffectType::LAZY_SHADER);

    // Get ShaderEffectObj and verify subType
    auto shaderEffectObj = lazyBlendShader->GetShaderEffectObj();
    EXPECT_TRUE(shaderEffectObj != nullptr);
    EXPECT_EQ(shaderEffectObj->GetType(), static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(shaderEffectObj->GetSubType(), static_cast<int32_t>(ShaderEffect::ShaderEffectType::BLEND));
}

/*
 * @tc.name: SerializeDeserialize001
 * @tc.desc: Test ShaderEffectLazy defensive Serialize/Deserialize methods
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyTest, SerializeDeserialize001, TestSize.Level1)
{
    auto dstShader = ShaderEffect::CreateColorShader(0xFF123456);
    auto srcShader = ShaderEffect::CreateColorShader(0xFF654321);
    auto lazyShader = ShaderEffectLazy::CreateBlendShader(dstShader, srcShader, BlendMode::SRC_OVER);
    EXPECT_TRUE(lazyShader != nullptr);

    // Test defensive Serialize - should return nullptr
    auto serializedData = lazyShader->Serialize();
    EXPECT_TRUE(serializedData == nullptr);

    // Test defensive Deserialize - should return false
    auto dummyData = std::make_shared<Data>();
    bool deserializeResult = lazyShader->Deserialize(dummyData);
    EXPECT_FALSE(deserializeResult);
}

/*
 * @tc.name: MaterializeWithEmptyShader001
 * @tc.desc: Test ShaderEffectLazy::Materialize with empty underlying shader
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ShaderEffectLazyTest, MaterializeWithEmptyShader001, TestSize.Level1)
{
    // Create a mock ShaderEffectObj that returns empty shader
    class MockShaderEffectObj : public ShaderEffectObj {
    public:
        MockShaderEffectObj() : ShaderEffectObj(static_cast<int32_t>(ShaderEffect::ShaderEffectType::COLOR_SHADER)) {}

        std::shared_ptr<void> GenerateBaseObject() override
        {
            // Return a shader that will have empty Serialize data
            class MockShaderEffect : public ShaderEffect {
            public:
                MockShaderEffect() : ShaderEffect(ShaderEffectType::COLOR_SHADER) {}
                std::shared_ptr<Data> Serialize() const override
                {
                    return nullptr; // Simulate failed serialization
                }
            };
            return std::static_pointer_cast<void>(std::make_shared<MockShaderEffect>());
        }

#ifdef ROSEN_OHOS
        bool Marshalling(Parcel& parcel) override { return true; }
        bool Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth) override { isValid = true; return true; }
#endif
    };

    auto mockObj = std::make_shared<MockShaderEffectObj>();
    auto lazyShader = ShaderEffectLazy::CreateFromShaderEffectObj(mockObj);
    EXPECT_NE(lazyShader, nullptr);

    // Materialize should succeed even with empty underlying shader
    auto materializedShader = lazyShader->Materialize();
    EXPECT_NE(materializedShader, nullptr);
    EXPECT_EQ(materializedShader->GetType(), ShaderEffect::ShaderEffectType::COLOR_SHADER);
}

/*
 * @tc.name: LazyContainingException001
 * @tc.desc: Test Lazy shader containing exception NoLazy shader scenarios
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ShaderEffectLazyTest, LazyContainingException001, TestSize.Level1)
{
    // Create shaders that may fail during creation/serialization
    auto baseShader = ShaderEffect::CreateColorShader(0xFF0000FF);
    auto blendShader = ShaderEffect::CreateColorShader(0xFF00FF00);

    // Create lazy shader with these base shaders
    auto lazyShader = ShaderEffectLazy::CreateBlendShader(baseShader, blendShader, BlendMode::SRC_OVER);
    EXPECT_NE(lazyShader, nullptr);
    EXPECT_TRUE(lazyShader->IsLazy());

    // Test materialization - should handle potential exceptions gracefully
    auto materializedShader = lazyShader->Materialize();
    EXPECT_NE(materializedShader, nullptr);
    EXPECT_FALSE(materializedShader->IsLazy());
    EXPECT_EQ(materializedShader->GetType(), ShaderEffect::ShaderEffectType::BLEND);

    // Test that materialized shader can be used normally
    auto secondMaterialization = lazyShader->Materialize();
    EXPECT_NE(secondMaterialization, nullptr);
    EXPECT_EQ(secondMaterialization->GetType(), materializedShader->GetType());
}

/*
 * @tc.name: UnmarshallingReadTypeFailure001
 * @tc.desc: Test ShaderEffectLazy::Unmarshalling with parcel.ReadInt32(type) failure
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ShaderEffectLazyTest, UnmarshallingReadTypeFailure001, TestSize.Level1)
{
#ifdef ROSEN_OHOS
    // Create an empty Parcel - ReadInt32 will fail due to no data
    Parcel emptyParcel;
    bool isValid = true;
    auto result = ShaderEffectLazy::Unmarshalling(emptyParcel, isValid, 0);
    EXPECT_EQ(result, nullptr);
#endif
}

/*
 * @tc.name: UnmarshallingReadSubTypeFailure001
 * @tc.desc: Test ShaderEffectLazy::Unmarshalling with parcel.ReadInt32(subType) failure
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ShaderEffectLazyTest, UnmarshallingReadSubTypeFailure001, TestSize.Level1)
{
#ifdef ROSEN_OHOS
    // Create a Parcel with only one int32 - first ReadInt32 succeeds, second fails
    Parcel parcel;
    parcel.WriteInt32(static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT));
    // No second int32 written, so second ReadInt32 will fail
    bool isValid = true;
    auto result = ShaderEffectLazy::Unmarshalling(parcel, isValid, 0);
    EXPECT_EQ(result, nullptr);
#endif
}

/*
 * @tc.name: ChainedLazyShaders001
 * @tc.desc: Test chained lazy shaders with Marshalling/Unmarshalling and serialization data comparison
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyTest, ChainedLazyShaders001, TestSize.Level1)
{
#ifdef ROSEN_OHOS
    // Create base color shaders
    auto baseShader1 = ShaderEffect::CreateColorShader(0xFF0000FF); // Blue
    auto baseShader2 = ShaderEffect::CreateColorShader(0xFF00FF00); // Green
    auto baseShader3 = ShaderEffect::CreateColorShader(0xFFFF0000); // Red
    auto baseShader4 = ShaderEffect::CreateColorShader(0xFFFFFF00); // Yellow
    ASSERT_NE(baseShader1, nullptr);
    ASSERT_NE(baseShader2, nullptr);
    ASSERT_NE(baseShader3, nullptr);
    ASSERT_NE(baseShader4, nullptr);

    // Create first level lazy shaders
    auto lazyShader1 = ShaderEffectLazy::CreateBlendShader(baseShader1, baseShader2, BlendMode::SRC_OVER);
    auto lazyShader2 = ShaderEffectLazy::CreateBlendShader(baseShader3, baseShader4, BlendMode::MULTIPLY);
    ASSERT_NE(lazyShader1, nullptr);
    ASSERT_NE(lazyShader2, nullptr);
    EXPECT_TRUE(lazyShader1->IsLazy());
    EXPECT_TRUE(lazyShader2->IsLazy());

    // Create second level lazy shader by chaining first level lazy shaders
    auto chainedLazyShader = ShaderEffectLazy::CreateBlendShader(lazyShader1, lazyShader2, BlendMode::OVERLAY);
    ASSERT_NE(chainedLazyShader, nullptr);
    EXPECT_TRUE(chainedLazyShader->IsLazy());
    EXPECT_EQ(chainedLazyShader->GetType(), ShaderEffect::ShaderEffectType::LAZY_SHADER);

    // Verify the underlying object structure
    auto shaderObj = chainedLazyShader->GetShaderEffectObj();
    ASSERT_NE(shaderObj, nullptr);
    EXPECT_EQ(shaderObj->GetType(), static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(shaderObj->GetSubType(), static_cast<int32_t>(ShaderEffect::ShaderEffectType::BLEND));

    // Test Marshalling
    Parcel parcel;
    bool marshalResult = chainedLazyShader->Marshalling(parcel);
    EXPECT_TRUE(marshalResult);

    // Test Unmarshalling
    parcel.RewindRead(0);
    bool isValid = true;
    auto unmarshalledShader = ShaderEffectLazy::Unmarshalling(parcel, isValid);
    EXPECT_TRUE(isValid);
    ASSERT_NE(unmarshalledShader, nullptr);
    EXPECT_TRUE(unmarshalledShader->IsLazy());
    EXPECT_EQ(unmarshalledShader->GetType(), ShaderEffect::ShaderEffectType::LAZY_SHADER);

    // Verify unmarshalled object structure
    auto unmarshalledObj = unmarshalledShader->GetShaderEffectObj();
    ASSERT_NE(unmarshalledObj, nullptr);
    EXPECT_EQ(unmarshalledObj->GetType(), static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT));
    EXPECT_EQ(unmarshalledObj->GetSubType(), static_cast<int32_t>(ShaderEffect::ShaderEffectType::BLEND));

    // Test Materialize for both original and unmarshalled shaders
    auto originalMaterialized = chainedLazyShader->Materialize();
    auto unmarshalledMaterialized = unmarshalledShader->Materialize();
    ASSERT_NE(originalMaterialized, nullptr);
    ASSERT_NE(unmarshalledMaterialized, nullptr);
    EXPECT_FALSE(originalMaterialized->IsLazy());
    EXPECT_FALSE(unmarshalledMaterialized->IsLazy());
    EXPECT_EQ(originalMaterialized->GetType(), ShaderEffect::ShaderEffectType::BLEND);
    EXPECT_EQ(unmarshalledMaterialized->GetType(), ShaderEffect::ShaderEffectType::BLEND);

    // Test serialization data comparison
    auto originalSerializedData = originalMaterialized->Serialize();
    auto unmarshalledSerializedData = unmarshalledMaterialized->Serialize();
    ASSERT_NE(originalSerializedData, nullptr);
    ASSERT_NE(unmarshalledSerializedData, nullptr);
    EXPECT_GT(originalSerializedData->GetSize(), 0);
    EXPECT_GT(unmarshalledSerializedData->GetSize(), 0);
    // Compare serialized data sizes
    EXPECT_EQ(originalSerializedData->GetSize(), unmarshalledSerializedData->GetSize());

    // Compare serialized data content byte by byte
    const uint8_t* originalBytes = static_cast<const uint8_t*>(originalSerializedData->GetData());
    const uint8_t* unmarshalledBytes = static_cast<const uint8_t*>(unmarshalledSerializedData->GetData());
    int memResult = memcmp(originalBytes, unmarshalledBytes, originalSerializedData->GetSize());
    EXPECT_EQ(memResult, 0);

    // Test round-trip marshalling consistency
    Parcel parcel2;
    bool marshalResult2 = unmarshalledShader->Marshalling(parcel2);
    EXPECT_TRUE(marshalResult2);

    // Compare marshalled data sizes
    size_t originalMarshalledSize = parcel.GetDataSize();
    size_t unmarshalledMarshalledSize = parcel2.GetDataSize();
    EXPECT_EQ(originalMarshalledSize, unmarshalledMarshalledSize);
    EXPECT_GT(originalMarshalledSize, 0);

    // Compare marshalled data content
    const uint8_t* originalMarshalledData = reinterpret_cast<const uint8_t*>(parcel.GetData());
    const uint8_t* unmarshalledMarshalledData = reinterpret_cast<const uint8_t*>(parcel2.GetData());
    int marshalledMemResult = memcmp(originalMarshalledData, unmarshalledMarshalledData, originalMarshalledSize);
    EXPECT_EQ(marshalledMemResult, 0);
#endif
}

/*
 * @tc.name: LazyShaderEquivalenceTest001
 * @tc.desc: Test chained lazy shaders equivalence with direct shader creation
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ShaderEffectLazyTest, LazyShaderEquivalenceTest001, TestSize.Level1)
{
    // Define common parameters - use same parameters for both methods
    ColorQuad baseColor1 = 0xFF0000FF; // Blue
    ColorQuad baseColor2 = 0xFF00FF00; // Green
    ColorQuad baseColor3 = 0xFFFF0000; // Red
    ColorQuad baseColor4 = 0xFFFFFF00; // Yellow

    // Method 1: Create chain using ShaderEffectLazy (nested blend shaders)
    auto lazyBase1 = ShaderEffect::CreateColorShader(baseColor1);
    auto lazyBase2 = ShaderEffect::CreateColorShader(baseColor2);
    auto lazyBase3 = ShaderEffect::CreateColorShader(baseColor3);
    auto lazyBase4 = ShaderEffect::CreateColorShader(baseColor4);
    ASSERT_NE(lazyBase1, nullptr);
    ASSERT_NE(lazyBase2, nullptr);
    ASSERT_NE(lazyBase3, nullptr);
    ASSERT_NE(lazyBase4, nullptr);

    // Create first level lazy blend shaders
    auto lazyBlend1 = ShaderEffectLazy::CreateBlendShader(lazyBase1, lazyBase2, BlendMode::SRC_OVER);
    auto lazyBlend2 = ShaderEffectLazy::CreateBlendShader(lazyBase3, lazyBase4, BlendMode::MULTIPLY);
    ASSERT_NE(lazyBlend1, nullptr);
    ASSERT_NE(lazyBlend2, nullptr);
    EXPECT_TRUE(lazyBlend1->IsLazy());
    EXPECT_TRUE(lazyBlend2->IsLazy());

    // Create second level lazy blend shader by chaining first level lazy shaders
    auto chainedLazyShader = ShaderEffectLazy::CreateBlendShader(lazyBlend1, lazyBlend2, BlendMode::OVERLAY);
    ASSERT_NE(chainedLazyShader, nullptr);
    EXPECT_TRUE(chainedLazyShader->IsLazy());

    // Materialize the lazy chain
    auto materializedLazy = chainedLazyShader->Materialize();
    ASSERT_NE(materializedLazy, nullptr);
    EXPECT_FALSE(materializedLazy->IsLazy());
    EXPECT_EQ(materializedLazy->GetType(), ShaderEffect::ShaderEffectType::BLEND);

    // Method 2: Create equivalent chain using direct ShaderEffect calls with SAME parameters
    auto directBase1 = ShaderEffect::CreateColorShader(baseColor1);
    auto directBase2 = ShaderEffect::CreateColorShader(baseColor2);
    auto directBase3 = ShaderEffect::CreateColorShader(baseColor3);
    auto directBase4 = ShaderEffect::CreateColorShader(baseColor4);
    ASSERT_NE(directBase1, nullptr);
    ASSERT_NE(directBase2, nullptr);
    ASSERT_NE(directBase3, nullptr);
    ASSERT_NE(directBase4, nullptr);

    // Create first level direct blend shaders
    auto directBlend1 = ShaderEffect::CreateBlendShader(*directBase1, *directBase2, BlendMode::SRC_OVER);
    auto directBlend2 = ShaderEffect::CreateBlendShader(*directBase3, *directBase4, BlendMode::MULTIPLY);
    ASSERT_NE(directBlend1, nullptr);
    ASSERT_NE(directBlend2, nullptr);
    EXPECT_FALSE(directBlend1->IsLazy());
    EXPECT_FALSE(directBlend2->IsLazy());

    // Create second level direct blend shader
    auto directChainedShader = ShaderEffect::CreateBlendShader(*directBlend1, *directBlend2, BlendMode::OVERLAY);
    ASSERT_NE(directChainedShader, nullptr);
    EXPECT_FALSE(directChainedShader->IsLazy());
    EXPECT_EQ(directChainedShader->GetType(), ShaderEffect::ShaderEffectType::BLEND);

    // Compare serialized data to verify equivalence
    auto lazyData = materializedLazy->Serialize();
    auto directData = directChainedShader->Serialize();
    ASSERT_NE(lazyData, nullptr);
    ASSERT_NE(directData, nullptr);
    EXPECT_GT(lazyData->GetSize(), 0);
    EXPECT_GT(directData->GetSize(), 0);
    // Compare data sizes
    EXPECT_EQ(lazyData->GetSize(), directData->GetSize());

    // Compare serialized content byte by byte
    const uint8_t* lazyBytes = static_cast<const uint8_t*>(lazyData->GetData());
    const uint8_t* directBytes = static_cast<const uint8_t*>(directData->GetData());
    int memResult = memcmp(lazyBytes, directBytes, lazyData->GetSize());
    EXPECT_EQ(memResult, 0);
}

/*
 * @tc.name: CreateFromShaderEffectObjInvalidType001
 * @tc.desc: Test ShaderEffectLazy::CreateFromShaderEffectObj with invalid object type
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ShaderEffectLazyTest, CreateFromShaderEffectObjInvalidType001, TestSize.Level1)
{
    // Create a mock object with wrong type by inheriting from ShaderEffectObj and modifying type
    class MockInvalidShaderEffectObj : public ShaderEffectObj {
    public:
        MockInvalidShaderEffectObj() : ShaderEffectObj(static_cast<int32_t>(ShaderEffect::ShaderEffectType::BLEND)) {
            // Override the type to be IMAGE_FILTER instead of SHADER_EFFECT
            type_ = static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER);
        }

        std::shared_ptr<void> GenerateBaseObject() override {
            return nullptr; // Not used in this test
        }

#ifdef ROSEN_OHOS
        bool Marshalling(Parcel& parcel) override { return true; }
        bool Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth) override {
            isValid = true;
            return true;
        }
#endif
    };

    // Create mock object with invalid type
    auto invalidObj = std::make_shared<MockInvalidShaderEffectObj>();
    EXPECT_EQ(invalidObj->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));

    // Test the validation branch - should return nullptr due to invalid type
    auto result = ShaderEffectLazy::CreateFromShaderEffectObj(invalidObj);
    EXPECT_EQ(result, nullptr);

    // Test with NO_TYPE
    class MockNoTypeShaderEffectObj : public ShaderEffectObj {
    public:
        MockNoTypeShaderEffectObj() :
            ShaderEffectObj(static_cast<int32_t>(ShaderEffect::ShaderEffectType::COLOR_SHADER)) {
            // Override the type to be NO_TYPE instead of SHADER_EFFECT
            type_ = static_cast<int32_t>(Object::ObjectType::NO_TYPE);
        }

        std::shared_ptr<void> GenerateBaseObject() override { return nullptr; }

#ifdef ROSEN_OHOS
        bool Marshalling(Parcel& parcel) override { return true; }
        bool Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth) override {
            isValid = true;
            return true;
        }
#endif
    };

    auto noTypeObj = std::make_shared<MockNoTypeShaderEffectObj>();
    EXPECT_EQ(noTypeObj->GetType(), static_cast<int32_t>(Object::ObjectType::NO_TYPE));

    // Test with NO_TYPE - should also return nullptr
    auto noTypeResult = ShaderEffectLazy::CreateFromShaderEffectObj(noTypeObj);
    EXPECT_EQ(noTypeResult, nullptr);
}

/*
 * @tc.name: UnmarshallingCreateLazyFailure001
 * @tc.desc: Test ShaderEffectLazy::Unmarshalling CreateFromShaderEffectObj failure branch
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 */
HWTEST_F(ShaderEffectLazyTest, UnmarshallingCreateLazyFailure001, TestSize.Level1)
{
#ifdef ROSEN_OHOS
    // Create a mock object that will cause CreateFromShaderEffectObj to fail
    class MockFailingShaderEffectObj : public ShaderEffectObj {
    public:
        MockFailingShaderEffectObj() : ShaderEffectObj(static_cast<int32_t>(ShaderEffect::ShaderEffectType::BLEND)) {
            // Override the type to be IMAGE_FILTER to make CreateFromShaderEffectObj fail
            type_ = static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER);
        }

        std::shared_ptr<void> GenerateBaseObject() override {
            return nullptr;
        }

        bool Marshalling(Parcel& parcel) override { return true; }
        bool Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth) override {
            isValid = true;
            return true;
        }
    };

    // Register a mock function that returns an object with wrong type
    auto originalFunc = ObjectHelper::Instance().GetFunc(
        static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT),
        static_cast<int32_t>(ShaderEffect::ShaderEffectType::BLEND));

    // Register a mock function that returns an object that will cause CreateFromShaderEffectObj to fail
    ObjectHelper::Instance().Register(
        static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT),
        static_cast<int32_t>(ShaderEffect::ShaderEffectType::BLEND),
        [](Parcel& parcel, bool& isValid, int32_t depth) -> std::shared_ptr<Object> {
            auto obj = std::make_shared<MockFailingShaderEffectObj>();
            return std::static_pointer_cast<Object>(obj);
        });

    // Create parcel with valid data for SHADER_EFFECT/BLEND
    Parcel parcel;
    parcel.WriteInt32(static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT));
    parcel.WriteInt32(static_cast<int32_t>(ShaderEffect::ShaderEffectType::BLEND));

    // Test Unmarshalling - should fail due to CreateFromShaderEffectObj returning nullptr
    bool isValid = true;
    auto result = ShaderEffectLazy::Unmarshalling(parcel, isValid, 0);
    EXPECT_EQ(result, nullptr);

    // Restore original function
    if (originalFunc) {
        ObjectHelper::Instance().Register(
            static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT),
            static_cast<int32_t>(ShaderEffect::ShaderEffectType::BLEND),
            originalFunc);
    }
#endif
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS