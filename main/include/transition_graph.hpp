#pragma once
#include <map>
#include <vector>
#include <string>

class TransitionGraph {
public:
    std::map<std::string, std::vector<std::string>> edges;

    TransitionGraph() {
        edges["Guardia_Cerrada_Inf"] = {
            "Media_Guardia_Inf",
            "Guardia_Abierta_Inf"
        };

        edges["Guardia_Abierta_Inf"] = {
            "Media_Guardia_Inf",
            "Tortuga_Inf"
        };

        edges["Media_Guardia_Inf"] = {
            "Guardia_Cerrada_Sup",
            "Media_Guardia_Sup"
        };

        edges["Tortuga_Inf"] = {
            "Media_Guardia_Inf",
            "Guardia_Ambas"
        };

        edges["Media_Guardia_Sup"] = {
            "Montada_Sup"
        };

        edges["Montada_Sup"] = {
            "Control_Lateral_Sup"
        };

        edges["Control_Lateral_Sup"] = {
            "Norte_Sur_Sup",
            "Montada_Sup"
        };
    }

    bool can_transition(const std::string& pos, const std::string& next) {
        auto& v = edges[pos];
        return std::find(v.begin(), v.end(), next) != v.end();
    }
};