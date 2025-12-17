#ifndef TECHNIQUE_HPP
#define TECHNIQUE_HPP

#include <string>
#include <iostream>

struct Technique {
    std::string id;
    std::string name;
    std::string category;
    
    // Estados con roles: "A"=Atacante, "O"=Oponente
    std::string from_state;          // "Pos(A:De_Pie, O:De_Pie)"
    std::string to_state_success;    // Estado si éxito
    std::string to_state_fail;       // Estado si fallo
    
    // Descripciones (opcionales, para debugging)
    std::string desc_init;
    std::string desc_success;
    std::string desc_fail;
    
    Technique() = default;
    
    Technique(const std::string& i, const std::string& n, const std::string& cat,
              const std::string& from, const std::string& to_succ, const std::string& to_fail,
              const std::string& dinit = "", const std::string& dsucc = "", const std::string& dfail = "")
        : id(i), name(n), category(cat), 
          from_state(from), to_state_success(to_succ), to_state_fail(to_fail),
          desc_init(dinit), desc_success(dsucc), desc_fail(dfail) {}
};

inline std::ostream& operator<<(std::ostream& os, const Technique& t) {
    os << "[Tech " << t.id << ": " << t.name 
       << " from=" << t.from_state 
       << " to_succ=" << t.to_state_success 
       << " to_fail=" << t.to_state_fail << "]";
    return os;
}

#endif