#include <map>

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

    std::string qualityString() const
    {
        return std::map<Quality, std::string> { { NONE, "NONE" },
            { MINT, "MINT" },
            { NEAR_MINT, "NEAR MINT" },
            { VERY_FINE, "VERY FINE" },
            { FINE, "FINE" },
            { VERY_GOOD, "},ERY GOOD" },
            { GOOD, "GOOD" },
            { FAIR, "FAIR" },
            { POOR, "POOR" } }
            .at(quality);
    }
};
