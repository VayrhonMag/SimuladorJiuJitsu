#include "cadmium/simulation/rt_root_coordinator.hpp"
#include "cadmium/simulation/root_coordinator.hpp"
#include <limits>
#include "include/top.hpp"

#ifdef RT_ESP32
    #include <cadmium/simulation/rt_clock/ESPclock.hpp>
#else
    #include <cadmium/simulation/rt_clock/chrono.hpp>
#endif

#ifndef NO_LOGGING
    #include "cadmium/simulation/logger/stdout.hpp"
    #include "cadmium/simulation/logger/csv.hpp"
#endif

using namespace cadmium;

extern "C" {
    #ifdef RT_ESP32
        void app_main()
    #else
        int main()
    #endif
    {
        // 👇 IMPORTANTE: el tipo debe llamarse igual que en top.hpp
        std::shared_ptr<topSystem> model = std::make_shared<topSystem>("topSystem");

        #ifdef RT_ESP32
            cadmium::ESPclock clock;
            auto rootCoordinator = cadmium::RealTimeRootCoordinator<cadmium::ESPclock<double>>(model, clock);
        #else
            cadmium::ChronoClock clock;
            auto rootCoordinator =
                cadmium::RealTimeRootCoordinator<cadmium::ChronoClock<std::chrono::steady_clock>>(model, clock);
            // O podrías usar solo:
            // auto rootCoordinator = cadmium::RootCoordinator(model);
        #endif

        #ifndef NO_LOGGING
        // Por ahora stdout; si quieres CSV descomenta la línea de abajo
        rootCoordinator.setLogger<cadmium::STDOUTLogger>(";");
        // rootCoordinator.setLogger<cadmium::CSVLogger>("fight_log.csv", ";");
        #endif

        rootCoordinator.start();

        // ⏱ aquí definimos duración: 6 minutos = 360 segundos
        #ifdef RT_ESP32
        rootCoordinator.simulate(std::numeric_limits<double>::infinity());
        #else
        rootCoordinator.simulate(360.0);
        #endif

        rootCoordinator.stop();

        #ifndef RT_ESP32
        return 0;
        #endif
    }
}