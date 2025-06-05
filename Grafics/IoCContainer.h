#ifndef IOCCONTAINER_H
#define IOCCONTAINER_H

#include <memory>
#include <map>
#include <string>
#include <stdexcept>
#include <any>
#include <QDebug>

class IoCContainer {
public:
    static IoCContainer& instance() {
        static IoCContainer container;
        return container;
    }

    template<typename T>
    void registerService(const std::string& key, std::shared_ptr<T> service) {
        m_services[key] = service;
        qDebug() << "Service registered:" << QString::fromStdString(key);
    }

    template<typename T>
    std::shared_ptr<T> resolve(const std::string& key) {
        auto it = m_services.find(key);
        if (it == m_services.end()) {
            throw std::runtime_error("Service not found: " + key);
        }
        try {
            return std::any_cast<std::shared_ptr<T>>(it->second);
        } catch (const std::bad_any_cast& e) {
            throw std::runtime_error("Service type mismatch for key: " + key + " (" + e.what() + ")");
        }
    }

private:
    IoCContainer() = default;
    ~IoCContainer() = default;
    IoCContainer(const IoCContainer&) = delete;
    IoCContainer& operator=(const IoCContainer&) = delete;

    std::map<std::string, std::any> m_services;
};

#endif // IOCCONTAINER_H
