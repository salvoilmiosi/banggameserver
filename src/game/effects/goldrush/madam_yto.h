#ifndef __GOLDRUSH_MADAM_YTO_H__
#define __GOLDRUSH_MADAM_YTO_H__

#include "../card_effect.h"

namespace banggame {

    struct effect_madam_yto : event_based_effect {
        void on_enable(card *target_card, player *origin);
    };
}

#endif