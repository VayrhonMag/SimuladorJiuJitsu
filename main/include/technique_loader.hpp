#ifndef TECHNIQUE_LOADER_HPP
#define TECHNIQUE_LOADER_HPP

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include "technique.hpp"

std::vector<Technique> loadTechniqueCSV(const std::string& filename) {
    std::vector<Technique> techniques;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir el archivo: " << filename << std::endl;
        return techniques;
    }
    
    std::string line;
    bool first_line = true;
    
    while (std::getline(file, line)) {
        // Saltar línea de encabezado
        if (first_line) {
            first_line = false;
            continue;
        }
        
        // Saltar líneas vacías
        if (line.empty() || line.find_first_not_of(' ') == std::string::npos) {
            continue;
        }
        
        std::stringstream ss(line);
        std::string token;
        Technique t;
        
        try {
            // ID (columna 0)
            if (!std::getline(ss, token, ';')) continue;
            t.id = token;
            
            // Nombre (columna 1)
            if (!std::getline(ss, token, ';')) continue;
            t.name = token;
            
            // Categoría (columna 2)
            if (!std::getline(ss, token, ';')) continue;
            t.category = token;
            
            // Tipo (columna 3) - NUEVO
            if (!std::getline(ss, token, ';')) continue;
            t.type = token;
            
            // Estado Inicial (columna 4)
            if (!std::getline(ss, token, ';')) continue;
            t.from_state = token;
            
            // Estado Final Exitoso (columna 5)
            if (!std::getline(ss, token, ';')) continue;
            t.to_state_success = token;
            
            // Estado Final Fallido (columna 6)
            if (!std::getline(ss, token, ';')) continue;
            t.to_state_fail = token;
            
            // Tiempo (s) (columna 7) - NUEVO
            if (!std::getline(ss, token, ';')) continue;
            t.time_cost = std::stoi(token);
            
            // Costo de Energía (columna 8) - NUEVO
            if (!std::getline(ss, token, ';')) continue;
            t.energy_cost = std::stoi(token);
            
            // Ganancia de Energía (columna 9) - NUEVO
            if (!std::getline(ss, token, ';')) continue;
            t.energy_gain = std::stoi(token);
            
            // Normalizar strings (remover espacios)
            auto normalize = [](std::string& str) {
                str.erase(0, str.find_first_not_of(' '));
                str.erase(str.find_last_not_of(' ') + 1);
            };
            
            normalize(t.from_state);
            normalize(t.to_state_success);
            normalize(t.to_state_fail);
            
            techniques.push_back(t);
            
            //std::cout << "[DEBUG] Cargada técnica: " << t.id << " - " << t.name 
                      //<< " type: " << t.type 
                      //<< " time: " << t.time_cost
                      //<< " energy_cost: " << t.energy_cost
                      //<< " energy_gain: " << t.energy_gain << std::endl;
                      
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Error procesando línea: " << line << std::endl;
            std::cerr << "Excepción: " << e.what() << std::endl;
        }
    }
    
    file.close();
    std::cout << "[INFO] Cargadas " << techniques.size() << " técnicas desde " 
              << filename << std::endl;
    
    return techniques;
}

#endif