#pragma once

#include <functional>
#include <array>
#include <cstring>
#include <algorithm>
#include <type_traits>

#ifndef MAX_EVENT_TYPES
#define MAX_EVENT_TYPES 46
#endif

#ifndef MAX_LISTENERS_PER_EVENT
#define MAX_LISTENERS_PER_EVENT 8
#endif

enum class EventPriority {
    FIRST,
    NORMAL,
    LAST,
    COUNT
};

namespace EmbeddedEvent {

struct Event {};
template<typename T>
struct TypeId {
private:
    static int id;
public:
    static int get() { return id; }
};
template<typename T> int TypeId<T>::id = [](){ 
    static int counter = 0; 
    return counter++; 
}();

template<typename T, typename Class>
struct MemberFunctionWrapper {
    using Callback = void (Class::*)(T&);
    Class* instance;
    Callback callback;

    static void wrapper(T& event) {
        auto& wrapper = getInstance();
        if (wrapper.instance && wrapper.callback) {
            (wrapper.instance->*wrapper.callback)(event);
        }
    }

    static MemberFunctionWrapper& getInstance() {
        static MemberFunctionWrapper instance;
        return instance;
    }

    static void set(Class* inst, Callback cb) {
        auto& wrapper = getInstance();
        wrapper.instance = inst;
        wrapper.callback = cb;
    }
};

class Dispatcher {
private:
    template<typename T>
    struct Listener {
        using FuncCallback = void(*)(T&);
        FuncCallback callback;
        EventPriority priority;
        void* memberInstance = nullptr;
        void* memberCallback = nullptr;
    };

    template<typename T>
    struct ListenerList {
        std::array<Listener<T>, MAX_LISTENERS_PER_EVENT> listeners;
        size_t count = 0;
    };

    struct EventEntry {
        int typeId;
        void* listeners;
    };

    std::array<EventEntry, MAX_EVENT_TYPES> m_eventMap;
    size_t m_eventCount = 0;

    template<typename T>
    ListenerList<T>* findListeners() {
        static_assert(std::is_base_of_v<Event, T>, "事件必须继承自EmbeddedEvent::Event");
        int typeId = TypeId<T>::get();
        for (size_t i = 0; i < m_eventCount; ++i) {
            if (m_eventMap[i].typeId == typeId) {
                return static_cast<ListenerList<T>*>(m_eventMap[i].listeners);
            }
        }
        return nullptr;
    }

    // 按优先级排序监听器
    template<typename T>
    void sortListeners(ListenerList<T>* list) {
        if (!list) return;
        std::sort(list->listeners.begin(), list->listeners.begin() + list->count,
            [](const Listener<T>& a, const Listener<T>& b) {
                return a.priority < b.priority;
            });
    }

public:
    Dispatcher() {
        std::memset(m_eventMap.data(), 0, sizeof(m_eventMap));
    }

    Dispatcher(const Dispatcher&) = delete;
    Dispatcher& operator=(const Dispatcher&) = delete;

    // 注册普通函数监听器
    template<typename T>
    bool registerListener(void(*callback)(T&), EventPriority priority = EventPriority::NORMAL) {
        auto listeners = findListeners<T>();
        if (!listeners) {
            if (m_eventCount >= MAX_EVENT_TYPES) {
                return false; 
            }
            
            static ListenerList<T> newListeners;
            m_eventMap[m_eventCount] = {TypeId<T>::get(), &newListeners};
            listeners = &newListeners;
            m_eventCount++;
        }

        // 添加监听器（不超过最大数量）
        if (listeners->count < MAX_LISTENERS_PER_EVENT) {
            listeners->listeners[listeners->count++] = {callback, priority, nullptr, nullptr};
            sortListeners(listeners);
            return true;
        }
        return false;
    }

    // 注册成员函数监听器
    template<typename T, typename Class>
    bool registerListener(Class* instance, void (Class::*callback)(T&), EventPriority priority = EventPriority::NORMAL) {
        using Wrapper = MemberFunctionWrapper<T, Class>;
        Wrapper::set(instance, callback);
        
        // 注册包装器的静态函数
        bool result = registerListener<T>(&Wrapper::wrapper, priority);
        if (result) {
            // 记录成员函数信息用于注销
            auto listeners = findListeners<T>();
            if (listeners && listeners->count > 0) {
                auto& last = listeners->listeners[listeners->count - 1];
                last.memberInstance = instance;
                last.memberCallback = reinterpret_cast<void*>(callback);
            }
        }
        return result;
    }

    // 注销普通函数监听器
    template<typename T>
    bool unregisterListener(void(*callback)(T&)) {
        auto listeners = findListeners<T>();
        if (!listeners) return false;

        for (size_t i = 0; i < listeners->count; ++i) {
            if (listeners->listeners[i].callback == callback && 
                !listeners->listeners[i].memberInstance) {
                // 移动后续元素填补空缺
                std::move(listeners->listeners.begin() + i + 1,
                          listeners->listeners.begin() + listeners->count,
                          listeners->listeners.begin() + i);
                listeners->count--;
                return true;
            }
        }
        return false;
    }

    // 注销成员函数监听器
    template<typename T, typename Class>
    bool unregisterListener(Class* instance, void (Class::*callback)(T&)) {
        using Wrapper = MemberFunctionWrapper<T, Class>;
        auto listeners = findListeners<T>();
        if (!listeners) return false;

        for (size_t i = 0; i < listeners->count; ++i) {
            if (listeners->listeners[i].callback == &Wrapper::wrapper &&
                listeners->listeners[i].memberInstance == instance &&
                listeners->listeners[i].memberCallback == reinterpret_cast<void*>(callback)) {
                std::move(listeners->listeners.begin() + i + 1,
                          listeners->listeners.begin() + listeners->count,
                          listeners->listeners.begin() + i);
                listeners->count--;
                return true;
            }
        }
        return false;
    }

    // 触发事件
    template<typename T>
    void trigger(T& event) {
        auto listeners = findListeners<T>();
        if (!listeners) return;
        // 保存当前计数以防止迭代中修改
        auto currentCount = listeners->count;
        for (size_t i = 0; i < currentCount; ++i) {
            if (listeners->listeners[i].callback) {
                listeners->listeners[i].callback(event);
            }
        }
    }

    // 清除特定事件类型的所有监听器
    template<typename T>
    void clearListeners() {
        auto listeners = findListeners<T>();
        if (listeners) {
            listeners->count = 0;
        }
    }

    // 清除所有事件的监听器
    void clearAllListeners() {
        for (size_t i = 0; i < m_eventCount; ++i) {
            m_eventMap[i].typeId = 0;
            m_eventMap[i].listeners = nullptr;
        }
        m_eventCount = 0;
    }
};

// 作用域监听器 - 自动注册/注销
template<typename T>
class ScopedListener {
private:
    Dispatcher& m_dispatcher;
    void(*m_funcCallback)(T&) = nullptr;
    void* m_memberInstance = nullptr;
    void* m_memberCallback = nullptr;
    bool m_isMember = false;

public:
    // 普通函数构造函数
    ScopedListener(Dispatcher& dispatcher, void(*callback)(T&), EventPriority priority = EventPriority::NORMAL)
        : m_dispatcher(dispatcher), m_funcCallback(callback) {
        m_dispatcher.registerListener<T>(callback, priority);
    }

    // 成员函数构造函数
    template<typename Class>
    ScopedListener(Dispatcher& dispatcher, Class* instance, void (Class::*callback)(T&), EventPriority priority = EventPriority::NORMAL)
        : m_dispatcher(dispatcher), m_memberInstance(instance), m_memberCallback(reinterpret_cast<void*>(callback)), m_isMember(true) {
        m_dispatcher.registerListener<T, Class>(instance, callback, priority);
    }

    ~ScopedListener() {
        if (m_isMember) {
            using Class = typename std::remove_pointer<decltype(m_memberInstance)>::type;
            m_dispatcher.unregisterListener<T, Class>(
                static_cast<Class*>(m_memberInstance),
                reinterpret_cast<void(Class::*)(T&)>(m_memberCallback)
            );
        } else if (m_funcCallback) {
            m_dispatcher.unregisterListener<T>(m_funcCallback);
        }
    }

    ScopedListener(const ScopedListener&) = delete;
    ScopedListener& operator=(const ScopedListener&) = delete;
};

// 事件持有者 - 方便创建和传递事件
template<typename T>
struct EventHolder {
    static_assert(std::is_base_of_v<Event, T>, "事件必须继承自EmbeddedEvent::Event");
    T event;

    template<typename... Args>
    explicit EventHolder(Args&&... args) : event(std::forward<Args>(args)...) {}

    T* get() { return &event; }
    T& ref() { return event; }
    T* operator->() { return get(); }
};

// 创建事件持有者的辅助函数
template<typename T, typename... Args>
EventHolder<T> makeEventHolder(Args&&... args) {
    return EventHolder<T>(std::forward<Args>(args)...);
}

}