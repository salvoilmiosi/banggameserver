#ifndef __ARMEDANDDANGEROUS_DOUBLEBARREL_H__
#define __ARMEDANDDANGEROUS_DOUBLEBARREL_H__

#include "../card_effect.h"

namespace banggame {

    struct effect_doublebarrel {
        void on_play(card *origin_card, player *origin);
    };
}

#endif