#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include "../third_party/nlohmann/json.hpp"

using json = nlohmann::json;

namespace hulk::common {

class DiagnosticRepository {
private:
    json messages_;
    static constexpr const char* kUnknownTemplate = "Error desconocido (ID: {0} no encontrado).";

public:
    bool load(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            return false;
        }
        try {
            file >> messages_;
            return true;
        } catch (const json::parse_error& e) {
            std::cerr << "Error parseando JSON de diagnósticos: " << e.what() << "\n";
            return false;
        }
    }

    [[nodiscard]] std::string get_template(const std::string& id) const {
        if (messages_.contains(id)) {
            return messages_[id].get<std::string>();
        }
        return "Error: [ID \"" + id + "\" no definido en el repositorio]";
    }
};

} // namespace hulk::common
