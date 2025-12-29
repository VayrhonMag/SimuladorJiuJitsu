#ifndef TECHNIQUE_HPP
#define TECHNIQUE_HPP

#include <string>
#include <iostream>

struct Technique {
    std::string id;
    std::string name;
    std::string category;
    std::string type;           // Nueva: "Ofensiva", "Defensiva", "Neutra"
    
    // Estados con roles: "A"=Atacante, "O"=Oponente
    std::string from_state;          // "Pos(A:De_Pie, O:De_Pie)"
    std::string to_state_success;    // Estado si éxito
    std::string to_state_fail;       // Estado si fallo
    
    // Nuevos campos del CSV actualizado
    int time_cost = 0;           // Tiempo en segundos (usaremos como tiempo de ejecución)
    int energy_cost = 0;         // Energía que gasta el actor
    int energy_gain = 0;         // Energía que gana (solo para técnicas defensivas)
    
    // Descripciones (opcionales, para debugging)
    std::string desc_init;
    std::string desc_success;
    std::string desc_fail;
    
    Technique() = default;
    
    Technique(const std::string& i, const std::string& n, const std::string& cat,
              const std::string& type_str, const std::string& from, const std::string& to_succ, 
              const std::string& to_fail, int tcost = 0, int ecost = 0, int egain = 0,
              const std::string& dinit = "", const std::string& dsucc = "", const std::string& dfail = "")
        : id(i), name(n), category(cat), type(type_str),
          from_state(from), to_state_success(to_succ), to_state_fail(to_fail),
          time_cost(tcost), energy_cost(ecost), energy_gain(egain),
          desc_init(dinit), desc_success(dsucc), desc_fail(dfail) {}
};

inline std::ostream& operator<<(std::ostream& os, const Technique& t) {
    os << "[Tech " << t.id << ": " << t.name 
       << " type=" << t.type
       << " from=" << t.from_state 
       << " to_succ=" << t.to_state_success 
       << " to_fail=" << t.to_state_fail
       << " time=" << t.time_cost << "s"
       << " energy_cost=" << t.energy_cost
       << " energy_gain=" << t.energy_gain << "]";
    return os;
}

#endif