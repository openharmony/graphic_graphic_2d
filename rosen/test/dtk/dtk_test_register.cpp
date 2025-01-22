#include "dtk_test_register.h"

namespace OHOS {
namespace Rosen {
DtkTestRegister* DtkTestRegister::Instance()
{
    static DtkTestRegister* instance = nullptr;
    if (instance == nullptr) {
        instance = new DtkTestRegister();
    }
    return instance;
}

bool DtkTestRegister::regist(const std::string testName,
                                const std::function<std::shared_ptr<TestBase>()> testCase,
                                TestLevel testLevel, int testCaseIndex)
{
    int iTestLevel = testLevel;
    std::string key = testName + "_L" + std::to_string(iTestLevel) + "_" + std::to_string(testCaseIndex);
    testCaseMap.emplace(key, testCase);
    return true;
}

std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> DtkTestRegister::getTestCaseMap()
{
    return testCaseMap;
}
} // namespace Rosen
} // namespace OHOS