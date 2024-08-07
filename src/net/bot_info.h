#ifndef __BOT_INFO_H__
#define __BOT_INFO_H__

#include "messages.h"

#include "utils/fixed_string.h"

namespace banggame {
    
    struct playable_card_info;
    using card_node = const playable_card_info *;

    using bot_rule = std::function<bool(card_node)>;

    template<utils::fixed_string Name>
    struct bot_rule_function_map;

    #define DEFINE_BOT_RULE(name, function, ...) \
        bot_rule function(__VA_ARGS__); \
        template<> struct bot_rule_function_map<#name> { \
            static constexpr auto value = function; \
        };

    #define BUILD_BOT_RULE(name, ...) \
        bot_rule_function_map<#name>::value(__VA_ARGS__)

    struct bot_settings {
        bool allow_timer_no_action;
        int max_random_tries;
        int bypass_prompt_after;
        std::vector<bot_rule> response_rules;
        std::vector<bot_rule> in_play_rules;
    };

    struct bot_info_t {
        std::vector<std::string> names;
        std::vector<utils::image_pixels> propics;
        bot_settings settings;
    };

    extern const bot_info_t bot_info;

    DEFINE_BOT_RULE(pocket, rule_filter_by_pocket, pocket_type)
    DEFINE_BOT_RULE(repeat, rule_repeat)

}

#endif