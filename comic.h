#include <jsoncons/json.hpp>

struct Comic {
    enum Quality {
        NONE,
        MINT,
        NEAR_MINT,
        VERY_FINE,
        FINE,
        VERY_GOOD,
        GOOD,
        FAIR,
        POOR,
    };

    unsigned int number;
    Quality quality;

    Comic(unsigned int number)
        : number(number)
        , quality(NONE)
    {
    }

    Comic(unsigned int number, Quality quality)
        : number(number)
        , quality(quality)
    {
    }
};

JSONCONS_ALL_MEMBER_TRAITS(Comic, number, quality)