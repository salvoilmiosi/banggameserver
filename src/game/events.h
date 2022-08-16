#ifndef __EVENTS_H__
#define __EVENTS_H__

#include <functional>
#include <map>
#include <set>

#include "player.h"

namespace banggame {

    struct request_bang;

    #define EVENT(name, ...) (name, std::function<void(__VA_ARGS__)>)
    
    DEFINE_ENUM_TYPES(event_type,
        EVENT(apply_sign_modifier,              player *origin, card_sign &value)
        EVENT(apply_beer_modifier,              player *origin, int &value)
        EVENT(apply_maxcards_modifier,          player *origin, int &value)
        EVENT(apply_volcanic_modifier,          player *origin, bool &value)
        EVENT(apply_immunity_modifier,          card *origin_card, player *origin, const player *target, effect_flags flags, bool &value)
        EVENT(apply_escapable_modifier,         card *origin_card, player *origin, const player *target, effect_flags flags, bool &value)
        EVENT(apply_initial_cards_modifier,     player *origin, int &value)
        EVENT(apply_bang_modifier,              player *origin, request_bang *req)

        // verifica per gli effetti che rubano carte in alcune condizioni
        EVENT(verify_card_taker, player *target, equip_type type, bool &value)

        // verifica l'effetto ha un bersaglio unico
        EVENT(verify_target_unique, card *origin_card, player *origin, player *target, bool &valid)

        // verifica se sei obbligato a giocare una carta prima di passare
        EVENT(verify_mandatory_card, player *origin, card* &value)

        // viene chiamato quando si sta cercando il prossimo giocatore di turno
        EVENT(verify_revivers, player *origin)
        
        // viene chiamato quando scarti una carta a fine turno
        EVENT(on_discard_pass, player *origin, card *target_card)

        // viene chiamata quando un giocatore deve estrarre prima di pescare
        EVENT(on_predraw_check, player *origin, card *target_card)

        // viene chiamata quando estrai una carta nel momento che viene pescata
        EVENT(on_draw_check, player *origin, card *target_card)

        // viene chiamato quando estrai una carta nel momento che viene scelta
        EVENT(on_draw_check_select, player *origin, card *origin_card, card *drawn_card)

        // viene chiamato quando si scarta VOLONTARIAMENTE una carta (si gioca cat balou o panico contro una carta)
        EVENT(on_discard_card, player *origin, player *target, card *target_card)

        // viene chiamato quando una carta arancione viene scartata perche' sono finiti i cubetti
        EVENT(on_discard_orange_card, player *target, card *target_card)

        // viene chiamato quando un giocatore viene colpito
        EVENT(on_hit, card *origin_card, player *origin, player *target, int damage, bool is_bang)

        // viene chiamato quando un giocatore gioca mancato
        EVENT(on_missed, card *origin_card, player *origin, player *target, bool is_bang)

        // viene chiamato quando un giocatore clicca su prendi danno quando muore
        EVENT(on_player_death_resolve, player *target, bool tried_save)

        // viene chiamato quando un giocatore muore
        EVENT(on_player_death, player *origin, player *target)

        // viene chiamato quando un giocatore equipaggia una carta
        EVENT(on_equip_card, player *origin, player *target, card *target_card)

        // viene chiamato quando un giocatore gioca una carta dalla mano
        EVENT(on_play_hand_card, player *origin, card *target_card)

        // viene chiamato dopo la fine di un effetto
        EVENT(on_effect_end, player *origin, card *target_card)

        // viene chiamato quando un giocatore gioca birra
        EVENT(on_play_beer, player *origin)

        // viene chiamato prima dell'inizio del turno, prima delle estrazioni
        EVENT(pre_turn_start, player *origin)

        // viene chiamato all'inizio del turno, prima di pescare
        EVENT(on_turn_start, player *origin)

        // viene chiamato all'inizio del turno, prima di attivare fase di pesca
        EVENT(phase_one_override, player *origin)

        // viene chiamato quando si clicca sul mazzo per pescare in fase di pesca
        EVENT(on_draw_from_deck, player *origin)

        // viene chiamato quando si pesca una carta in fase di pesca
        EVENT(on_card_drawn, player *origin, card *target_card, bool &reveal)

        // viene chiamato dopo che un giocatore finisce la fase di pesca
        EVENT(post_draw_cards, player *origin)
        
        // viene chiamato alla fine del turno
        EVENT(on_turn_end, player *origin)

        // viene chiamato dopo la fine del turno, prima dei turni extra
        EVENT(post_turn_end, player *origin)
    )

    struct event_card_key {
        card *target_card;
        int priority;

        event_card_key(card *target_card, int priority = 0)
            : target_card(target_card)
            , priority(priority) {}

        bool operator == (const event_card_key &other) const = default;

        auto operator <=> (const event_card_key &other) const {
            return target_card == other.target_card ?
                priority <=> other.priority :
                target_card <=> other.target_card;
        }

        auto operator <=> (card *other) const {
            return target_card <=> other;
        }

        bool priority_greater(const event_card_key &other) const {
            return priority == other.priority ?
                target_card < other.target_card :
                priority > other.priority;
        }
    };

    template<typename T>
    concept priority_key = requires (T value) {
        { value.priority_greater(value) } -> std::convertible_to<bool>;
    };

    template<priority_key Key, enums::reflected_enum EnumType>
    struct priority_double_map {
    private:
        enum value_status : uint8_t {
            inactive, active, erased
        };

        struct value_variant {
            enums::enum_variant<EnumType> value;
            value_status status = inactive;

            template<typename ... Ts>
            value_variant(Ts && ... args) : value{FWD(args)...} {}
        };
        
        struct iterator_compare {
            template<typename T>
            bool operator()(const T &lhs, const T &rhs) const {
                return lhs->first.priority_greater(rhs->first)
                    || !rhs->first.priority_greater(lhs->first)
                    && (&*lhs < &*rhs);
            }
        };

        using container_map = std::multimap<Key, value_variant, std::less<>>;
        using container_iterator = typename container_map::iterator;
        using iterator_set = std::set<container_iterator, iterator_compare>;
        using iterator_table = std::array<iterator_set, enums::num_members_v<EnumType>>;
        using iterator_vector = std::vector<container_iterator>;

        container_map m_map;
        iterator_table m_table;
        iterator_vector m_changes;

    public:
        template<EnumType E, typename ... Ts>
        void add(Key key, Ts && ... args) {
            auto it = m_map.emplace(std::piecewise_construct,
                std::make_tuple(std::move(key)),
                std::make_tuple(enums::enum_tag<E>, FWD(args) ...));
            m_table[enums::indexof(E)].emplace(it);
            m_changes.push_back(it);
        }

        template<typename T>
        void erase(const T &key) {
            auto [low, high] = m_map.equal_range(key);
            for (; low != high; ++low) {
                low->second.status = erased;
                m_changes.push_back(low);
            }
        }

        void commit_changes() {
            for (auto it : m_changes) {
                switch (it->second.status) {
                case inactive:
                    it->second.status = active;
                    break;
                case erased:
                    m_table[it->second.value.index()].erase(it);
                    m_map.erase(it);
                }
            }
            m_changes.clear();
        }

        template<EnumType E>
        auto get_table() {
            return m_table[enums::indexof(E)]
                | std::views::filter([](auto it) {
                    return it->second.status == active;
                })
                | std::views::transform([](auto it) {
                    return it->second.value.template get<E>();
                });
        }
    };

    template<typename T, typename U> struct same_function_args {};

    template<typename Function, typename RetType, typename ... Ts>
    struct same_function_args<Function, std::function<RetType(Ts...)>> : std::is_invocable_r<RetType, Function, Ts...> {};

    template<typename T, event_type E>
    concept invocable_for_event = same_function_args<T, enums::enum_type_t<E>>::value;

    template<typename Function> struct function_argument;
    template<typename T, typename Arg> struct function_argument<void (T::*) (Arg)> : std::type_identity<Arg> {};
    template<typename T, typename Arg> struct function_argument<void (T::*) (Arg) const> : std::type_identity<Arg> {};
    template<typename Function> struct deduce_event_args : function_argument<decltype(&Function::operator())> {};

    class listener_map {
    private:
        priority_double_map<event_card_key, event_type> m_listeners;
        bool m_lock = false;

    public:
        template<event_type E, invocable_for_event<E> Function>
        void add_listener(event_card_key key, Function &&fun) {
            m_listeners.add<E>(key, std::forward<Function>(fun));
            if (!m_lock) {
                m_listeners.commit_changes();
            }
        }

        void remove_listeners(auto key) {
            m_listeners.erase(key);
            if (!m_lock) {
                m_listeners.commit_changes();
            }
        }

        template<event_type E, typename ... Ts>
        void call_event(Ts && ... args) {
            bool prev_lock = m_lock;
            m_lock = true;
            for (const auto &fun : m_listeners.get_table<E>()) {
                std::invoke(fun, args ...);
            }
            m_lock = prev_lock;
            if (!m_lock) {
                m_listeners.commit_changes();
            }
        }
    };

}

#endif