#pragma once
#include <thread>
#include <map>
#include <mutex>
#include <string>

namespace LightControl {
    class LightControlDiscover
    { 
        public:
            /**
             * @brief Constructor
             */
            LightControlDiscover();

            /**
             * @brief Destructor
             */
            virtual ~LightControlDiscover();

            /**
             * @brief Remove all discovered devices from the list
             */
            void clearDevices();

            /**
             * @brief Add a new device to the list of discovered devices
             */
            void addDevice(std::string name, std::string address);

            /**
             * @brief Get a list of discovered devices
             * 
             * @return Map of discovered devices
             */
            std::map<std::string, std::string> getDevices();

        private:
            
            LightControlDiscover(LightControlDiscover const&) = delete;
            void operator=(LightControlDiscover const&)       = delete;

            /**
             * @brief Run needed initialisation code
             */
            void init();

            /**
             * @brief Discovery thread loop
             */
            void run();
            
            bool shouldStop;
            std::thread discover_thread;
            std::map<std::string, std::string> _devices;
            std::mutex _deviceLock;
    };
}