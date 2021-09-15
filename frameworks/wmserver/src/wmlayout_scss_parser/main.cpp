#include "driver.h"

using namespace OHOS;

void PrintCSSBlock(const struct Driver::CSSBlock &block)
{
    for (const auto &[k, v] : block.declares) {
        printf("%s: %s;\n", k.c_str(), v.c_str());
    }
    for (const auto &[k, v] : block.blocks) {
        printf("%s {\n", k.c_str());
        PrintCSSBlock(v);
        printf("} %s\n", k.c_str());
    }
}

int main(int argc, char const *argv[])
{
    Driver driver;
    driver.parse();
    PrintCSSBlock(driver.global);
    return 0;
}
