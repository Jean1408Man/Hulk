#include "backend.h"

#include <iostream>
#include <string>

namespace {

void print_usage() {
    std::cerr << "Uso: hulk_backend <archivo.hulk> [-o salida] [--emit-ir] [--emit-cpp] [--keep-temp]\n";
}

} // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    Hulk::Backend::BackendOptions options;
    options.input_path = argv[1];

    for (int i = 2; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "-o") {
            if (i + 1 >= argc) {
                print_usage();
                return 1;
            }
            options.output_path = argv[++i];
        } else if (arg == "--emit-cpp") {
            options.emit_cpp = true;
        } else if (arg == "--emit-ir") {
            options.emit_ir = true;
        } else if (arg == "--keep-temp") {
            options.keep_temp = true;
        } else {
            std::cerr << "Opcion desconocida: " << arg << "\n";
            print_usage();
            return 1;
        }
    }

    if (options.emit_ir && options.emit_cpp) {
        std::cerr << "Use solo una opcion de emision: --emit-ir o --emit-cpp.\n";
        print_usage();
        return 1;
    }

    const Hulk::Backend::BackendResult result = Hulk::Backend::run_backend(options);
    return result.ok ? 0 : 1;
}
