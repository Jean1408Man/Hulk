#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

namespace hulk::common {

class DiagnosticRepository {
private:
    json messages;
    std::string default_msg = "Error desconocido (ID: {0} no encontrado).";

public:
    // Carga el archivo JSON
    bool load(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            return false;
        }
        try {
            file >> messages;
            return true;
        } catch (const json::parse_error& e) {
            std::cerr << "Error parseando JSON de diagnósticos: " << e.what() << std::endl;
            return false;
        }
    }

    std::string get_template(const std::string& id) const {
        if (messages.contains(id)) {
            return messages[id].get<std::string>();
        }
        return "Error: [ID " + id + " no definido]";
    }
};

}