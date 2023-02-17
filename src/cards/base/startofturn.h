#ifndef __BASE_STARTOFTURN_H__
#define __BASE_STARTOFTURN_H__

#include "cards/card_effect.h"

namespace banggame {

    struct effect_startofturn {
        game_string get_error(card *origin_card, player *origin) const;
    };
}

#endif