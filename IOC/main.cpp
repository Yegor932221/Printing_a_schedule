#include <QApplication>

#include <functional>
#include <iostream>
#include <memory>
#include <map>
#include <string>
#include <QString>

using namespace std;

// Ваши интерфейсы и классы
class IProcessor {
public:
    virtual ~IProcessor() = default;
    virtual QString GetProcessorInfo() = 0;
};

class IComputer {
public:
    virtual ~IComputer() = default;
    virtual IProcessor* GetProcessor() = 0;
    virtual QString GetProcessorInfo() = 0;
};

enum ProcessorType { x86, x64 };

class IntelProcessor : public IProcessor {
    QString m_version;
    ProcessorType m_type;
    double m_speed;
public:
    IntelProcessor(QString version, ProcessorType type, double speed)
        : m_version(version), m_type(type), m_speed(speed) {}

    QString GetProcessorInfo() override {
        return "Processor for " + m_version + " " + QString::number(m_speed) + "GHz " +
               (m_type == x86 ? "x86" : "x64");
    }
};

class Computer : public IComputer {
    shared_ptr<IProcessor> m_processor;
public:
    Computer(shared_ptr<IProcessor> processor) : m_processor(processor) {}

    IProcessor* GetProcessor() override {
        return m_processor.get();
    }

    QString GetProcessorInfo() override {
        return m_processor->GetProcessorInfo();
    }
};

// IOC Container (как у вас было)
class IOCContainer {
    static int s_nextTypeId;

    template<typename T>
    static int GetTypeID() {
        static int typeId = s_nextTypeId++;
        return typeId;
    }

public:
    class FactoryRoot {
    public:
        virtual ~FactoryRoot() = default;
    };

    map<int, shared_ptr<FactoryRoot>> m_factories;

    template<typename T>
    class CFactory : public FactoryRoot {
        function<shared_ptr<T>()> m_functor;
    public:
        explicit CFactory(function<shared_ptr<T>()> functor) : m_functor(functor) {}
        shared_ptr<T> GetObject() { return m_functor(); }
    };

    template<typename T>
    shared_ptr<T> GetObject() {
        auto typeId = GetTypeID<T>();
        auto factoryBase = m_factories[typeId];
        auto factory = static_pointer_cast<CFactory<T>>(factoryBase);
        return factory->GetObject();
    }

    template<typename TInterface, typename... TS>
    void RegisterFunctor(function<shared_ptr<TInterface>(shared_ptr<TS>...)> functor) {
        m_factories[GetTypeID<TInterface>()] = make_shared<CFactory<TInterface>>(
            [=] { return functor(GetObject<TS>()...); });
    }

    template<typename TInterface>
    void RegisterInstance(shared_ptr<TInterface> t) {
        m_factories[GetTypeID<TInterface>()] = make_shared<CFactory<TInterface>>(
            [=] { return t; });
    }

    template<typename TInterface, typename TConcrete, typename... TArguments>
    void RegisterFactory() {
        RegisterFunctor(
            function<shared_ptr<TInterface>(shared_ptr<TArguments>...)>(
                [](shared_ptr<TArguments>... args) {
                    return make_shared<TConcrete>(forward<shared_ptr<TArguments>>(args)...);
                }));
    }

    template<typename TInterface, typename TConcrete, typename... TArguments>
    void RegisterInstance() {
        RegisterInstance<TInterface>(make_shared<TConcrete>(GetObject<TArguments>()...));
    }
};

int IOCContainer::s_nextTypeId = 115094801;

// Пример использования
int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    IOCContainer container;

    // 1. Регистрируем процессор как синглтон с параметрами
    container.RegisterInstance<IProcessor>(
        make_shared<IntelProcessor>("Intel Core i7", x64, 3.2));

    // 2. Регистрируем компьютер с зависимостью от процессора
    container.RegisterFactory<IComputer, Computer, IProcessor>();

    // 3. Получаем компьютер (процессор внедрится автоматически)
    auto computer = container.GetObject<IComputer>();

    cout << computer->GetProcessorInfo().toStdString() << endl;
    // Выведет: "Processor for Intel Core i7 3.2GHz x64"

     return a.exec();
}
