#include <clang/Tooling/Tooling.h>
#include <frontend-action/frontend_action.h>
#include <memory>

int main(int argc, char **argv) {
    if (argc > 1) {
        clang::tooling::runToolOnCode(std::make_unique<frontend_action>(), argv[1]);
    }
}