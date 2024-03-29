#ifndef __GOLDRUSH_PIXIE_PETE_H__
#define __GOLDRUSH_PIXIE_PETE_H__

#include "cards/card_effect.h"

namespace banggame {

    struct equip_pixie_pete : event_equip {
        void on_enable(card *target_card, player *target);
    };

    DEFINE_EQUIP(pixie_pete, equip_pixie_pete)
}

#endif