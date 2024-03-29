#include "event_map.h"

namespace banggame {
    
    listener_map::iterator_map_iterator listener_map::do_add_listener(std::type_index type, event_card_key key, event_listener_fun &&fun) {
        auto listener = m_listeners.emplace(type, key, std::move(fun));
        return m_map.emplace(key, listener);
    }

    void listener_map::do_remove_listeners(iterator_map_range range) {
        if (range.empty()) return;

        for (auto &[key, listener] : range) {
            if (listener->active) {
                if (m_lock) {
                    m_to_remove.push_back(listener);
                    listener->active = false;
                } else {
                    m_listeners.erase(listener);
                }
            }
        }
        m_map.erase(range.begin(), range.end());
    }

    void listener_map::do_call_event(std::type_index type, const void *tuple) {
        auto [low, high] = m_listeners.equal_range(type);
        rn::subrange range(low, high);
        if (range.empty()) return;

        ++m_lock;
        for (const event_listener &listener : range) {
            if (listener.type == type && listener.active) {
                std::invoke(listener.fun, tuple);
            }
        }
        --m_lock;
        
        if (!m_lock && !m_to_remove.empty()) {
            for (listener_iterator listener : m_to_remove) {
                m_listeners.erase(listener);
            }
            m_to_remove.clear();
        }
    }
}