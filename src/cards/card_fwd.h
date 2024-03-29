#ifndef __CARD_FWD_H__
#define __CARD_FWD_H__

#include "utils/enum_variant.h"
#include "utils/reflector.h"
#include "utils/small_pod.h"
#include "utils/utils.h"

namespace banggame {
    
    struct game;
    struct card;
    struct player;
    
    struct effect_vtable;
    struct equip_vtable;
    struct modifier_vtable;
    struct mth_vtable;

    enum class target_player_filter;
    enum class target_card_filter;
    enum class tag_type;
    
    enum class effect_flags;
    enum class game_flags;
    enum class player_flags;
    enum class discard_all_reason;

}

namespace banggame::serial {

    using opt_card = banggame::card *;
    using card = not_null<opt_card>;
    using opt_player = banggame::player *;
    using player = not_null<opt_player>;
    using int_list = small_int_set;
    
    using player_list = std::vector<player>;
    using card_list = std::vector<card>;
    
}

#endif