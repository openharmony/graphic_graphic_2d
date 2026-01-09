#include <gtest/gtest.h>
#include "rs_layer_parcel_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSLayerParcelFactoryTest : public Test {};

/**
 * Function: Register_And_Get_Function_LUT
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. register a dummy unmarshalling func
 *                  2. get it back and compare pointer
 */
HWTEST_F(RSLayerParcelFactoryTest, Register_And_Get_Function_LUT, TestSize.Level1)
{
    uint16_t key = 0x1234;
    UnmarshallingFunc func = [](OHOS::MessageParcel&) { return static_cast<RSLayerParcel*>(nullptr); };
    RSLayerParcelFactory::Instance().Register(key, func);
    auto got = RSLayerParcelFactory::Instance().GetUnmarshallingFunc(key);
    ASSERT_NE(got, nullptr);
}

/**
 * Function: Get_Unregistered_Key_Returns_Null
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. query unknown key
 *                  2. expect nullptr
 */
HWTEST_F(RSLayerParcelFactoryTest, Get_Unregistered_Key_Returns_Null, TestSize.Level1)
{
    auto got = RSLayerParcelFactory::Instance().GetUnmarshallingFunc(0xFFFF);
    EXPECT_EQ(got, nullptr);
}

/**
 * Function: Register_DuplicateKey_NotOverwritten
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. register func1 for key
 *                  2. register func2 for same key (duplicate)
 *                  3. expect LUT retains original func1
 */
HWTEST_F(RSLayerParcelFactoryTest, Register_DuplicateKey_NotOverwritten, TestSize.Level1)
{
    uint16_t key = 0x2222;
    UnmarshallingFunc func1 = [](OHOS::MessageParcel&) { return static_cast<RSLayerParcel*>(nullptr); };
    UnmarshallingFunc func2 = [](OHOS::MessageParcel&) { return reinterpret_cast<RSLayerParcel*>(0x1); };
    RSLayerParcelFactory::Instance().Register(key, func1);
    RSLayerParcelFactory::Instance().Register(key, func2); // duplicate, should be ignored
    auto got = RSLayerParcelFactory::Instance().GetUnmarshallingFunc(key);
    ASSERT_NE(got, nullptr);
    EXPECT_EQ(got, func1);
}

/**
 * Function: AutoRegister_Helper_RegistersFunction
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. use RSLayerParcelRegister helper to auto-register
 *                  2. verify factory returns the registered function
 */
RSLayerParcel* DummyUnmarshal(OHOS::MessageParcel&)
{
    return nullptr;
}
HWTEST_F(RSLayerParcelFactoryTest, AutoRegister_Helper_RegistersFunction, TestSize.Level1)
{
    constexpr uint16_t key = 0x3333;
    static RSLayerParcelRegister<key, &DummyUnmarshal> reg;
    auto got = RSLayerParcelFactory::Instance().GetUnmarshallingFunc(key);
    ASSERT_NE(got, nullptr);
    EXPECT_EQ(got, &DummyUnmarshal);
}
} // namespace OHOS::Rosen
