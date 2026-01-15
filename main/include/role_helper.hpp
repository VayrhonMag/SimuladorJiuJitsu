// En fighter_state.hpp o nuevo archivo
#ifndef ROLE_HELPER_HPP
#define ROLE_HELPER_HPP

#include <string>

// Convierte un estado con roles "A" (Atacante) y "O" (Oponente)
static std::string convertRoles(const std::string &state_with_roles,
                                const std::string &actor_A,
                                const std::string &actor_O)
{
    std::string result = state_with_roles;

    // Normalizar
    size_t pos = 0;
    while ((pos = result.find(" ", pos)) != std::string::npos)
    {
        result.erase(pos, 1);
    }

    // Reemplazar A->actor_A
    pos = 0;
    while ((pos = result.find("A:", pos)) != std::string::npos)
    {
        result.replace(pos, 2, actor_A + ":");
        pos += actor_A.length() + 2;
    }

    // Reemplazar O->actor_O
    pos = 0;
    while ((pos = result.find("O:", pos)) != std::string::npos)
    {
        result.replace(pos, 2, actor_O + ":");
        pos += actor_O.length() + 2;
    }

    return result;
}

// Extrae MI posición de un estado donde ya se han convertido los roles
static std::string extractMyPosition(const std::string &state,
                                     const std::string &my_name)
{
    // Si es un estado de sumisión
    if (state.find("Estado(Sumision:") == 0 ||
        state.find("Estado(Sumisión:") == 0)
    {
        return "Sumision";
    }
    std::string key = my_name + ":";
    size_t start = state.find(key);

    if (start == std::string::npos)
    {
        if (my_name == "fighterA")
        {
            key = "A:";
        }
        else if (my_name == "fighterB")
        {
            key = "O:"; 
        }
        start = state.find(key);

        if (start == std::string::npos && my_name == "fighterB")
        {
            // Intentar como A: también
            key = "A:";
            start = state.find(key);
        }

        if (start == std::string::npos)
        {
            std::cout << "[WARN][extractMyPosition] No encontré '" << key
                      << "' para " << my_name << " en: " << state << std::endl;
            return "";
        }
    }

    start += key.length();
    size_t end = state.find(",", start);
    if (end == std::string::npos)
        end = state.find(")", start);

    if (end == std::string::npos)
    {
        std::cout << "[WARN][extractMyPosition] Formato inválido: " << state << std::endl;
        return "";
    }

    std::string position = state.substr(start, end - start);

    // Limpiar posibles espacios
    while (!position.empty() && position[0] == ' ')
    {
        position.erase(0, 1);
    }
    while (!position.empty() && position.back() == ' ')
    {
        position.pop_back();
    }

    return position;
}

#endif