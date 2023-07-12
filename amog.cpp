#include <iostream>
#include <array>
#include <cmath>

namespace std {
    constexpr double sqrt_ce_core(double x, double curr, double prev) 
    {
        return (curr == prev) ? curr : sqrt_ce_core(x, 0.5 * (curr + x / curr), curr);
    }
    
    constexpr double sqrt_ce(double x) 
    {
        return ((x >= 0) && (x < std::numeric_limits<double>::infinity())) ? sqrt_ce_core(x, x, 0.0) : std::numeric_limits<double>::quiet_NaN();
    }

    constexpr char get_pixel(std::size_t xs, std::size_t ys, std::size_t index) 
    {
        const std::size_t xp = index % xs;
        const std::size_t yp = index / xs;
        const double x = 2.0 * static_cast<double>(xp) / static_cast<double>(xs);
        const double y = 3.0 * static_cast<double>(yp) / static_cast<double>(ys);

        constexpr auto constexpr_abs = [](double value) {
            return (value >= 0) ? value : -value;
        };

        // non-constexpr function 'pow' cannot be used in a constant expression
        const bool head  = (y <= 1.0) && ((-y + 1.0) <= sqrt_ce(1.0 - (x - 1.0) * (x - 1.0)));
        const bool left  = (y >= 2.5) && ((-y + 2.5) >= -sqrt_ce(0.25 - (x - 0.5) * (x - 0.5)));
        const bool right = (y >= 2.5) && ((-y + 2.5) >= -sqrt_ce(0.25 - (x - 1.5) * (x - 1.5)));
        // non-constexpr function 'abs' cannot be used in a constant expression
        const bool body  = (constexpr_abs(-y + 1.75) <= 0.75) && (constexpr_abs(x - 1.0) <= 1.0);
        const bool visor = (x - 1.25) >= -sqrt_ce(0.125 - constexpr_abs(-y + 1.25) * constexpr_abs(-y + 1.25));

        return ((head || left || right || body) && !visor) ? 'A' : ' ';
    } 

    static constexpr std::size_t rat = 1;

    template <std::size_t W, std::size_t... I>
    struct sus_core
    {
        static constexpr std::size_t w = W;
        static constexpr std::size_t h = w * rat;
        static constexpr std::size_t size = w * h;
        static constexpr std::array<char, size> pixels = []
        {
            std::array<char, size> result{};
            for (std::size_t i = 0; i < size; ++i)
                result[i] = get_pixel(w, h, i);
            return result;
        }();
    };

    template <std::size_t W, std::size_t... I>
    constexpr auto sus_helper(std::index_sequence<I...>) 
    {
        return sus_core<W, I...>{};
    }

    template <std::size_t W>
    constexpr auto sus() {
        return sus_helper<W>(std::make_index_sequence<W * W * rat>{});
    }
}

int main() {
    static constexpr auto sus = std::sus<50>();
    for (std::size_t i = 0; i < sus.pixels.size(); ++i) {
        std::putchar(sus.pixels[i]);
        if ((i + 1) % sus.w == 0) std::putchar('\n');
    }

    return 0;
}
