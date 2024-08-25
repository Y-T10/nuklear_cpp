#include <format>
#include <cstdio>

#include "nuklear-cpp.hpp"

int main(int argc, char* argv[]) {
    std::printf("%s", std::format("sample program with fmt!\n").c_str());
    return 0;
}
