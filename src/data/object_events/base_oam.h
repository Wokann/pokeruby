//

//

#ifndef POKERUBY_BASE_OAM_H
#define POKERUBY_BASE_OAM_H

const struct OamData gObjectEventBaseOam_8x8 = {
    .shape = 0,
    .size = 0,
    .priority = 2
};

const struct OamData gObjectEventBaseOam_16x8 = {
    .shape = 1,
    .size = 0,
    .priority = 2
};

const struct OamData gObjectEventBaseOam_16x16 = {
    .shape = 0,
    .size = 1,
    .priority = 2
};

const struct OamData gObjectEventBaseOam_32x8 = {
    .shape = 1,
    .size = 1,
    .priority = 2
};

const struct OamData gObjectEventBaseOam_64x32 = {
    .shape = 1,
    .size = 3,
    .priority = 2
};

const struct OamData gObjectEventBaseOam_16x32 = {
    .shape = 2,
    .size = 2,
    .priority = 2
};

const struct OamData gObjectEventBaseOam_32x32 = {
    .shape = 0,
    .size = 2,
    .priority = 2
};

const struct OamData gObjectEventBaseOam_64x64 = {
    .shape = 0,
    .size = 3,
    .priority = 2
};

#endif //POKERUBY_BASE_OAM_H
