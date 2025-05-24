#include <float.h>

#define DEFINE_FPA_HELPERS(T, sfx, mantissa_digits)                                                              \
                                                                                                                 \
    typedef unsigned _BitInt(8 * sizeof(T)) bits_##sfx##_t;                                                      \
                                                                                                                 \
    constexpr int bit_count_##sfx = 8 * sizeof(T);                                                               \
    constexpr int fraction_bit_count_##sfx = mantissa_digits - 1;                                                \
    constexpr int exponent_bit_count_##sfx = bit_count_##sfx - 1 - fraction_bit_count_##sfx;                     \
                                                                                                                 \
    constexpr bits_##sfx##_t sign_bit_mask_##sfx = (bits_##sfx##_t)1 << (bit_count_##sfx - 1);                   \
    constexpr bits_##sfx##_t fraction_bit_mask_##sfx = (~(bits_##sfx##_t)0 >> (exponent_bit_count_##sfx + 1));   \
    constexpr bits_##sfx##_t exponent_bit_mask_##sfx = ~(sign_bit_mask_##sfx | fraction_bit_mask_##sfx);         \
                                                                                                                 \
    /* Converts a floating-point bit pattern from sign-and-magnitude format to a biased representation, where
     * the ordering of bit patterns matches the ordering of the actual values.
	 *
	 * Specifically:
	 *   - Negative numbers (sign bit set) are bit-inverted and incremented (~x + 1)
	 *     to place them before positive numbers in unsigned ordering.
	 *   - Positive numbers (sign bit clear) are offset by setting the sign bit,
	 *     shifting them above the midpoint.
	 *
	 * This allows signed floating-point values to be compared using unsigned integer comparisons.
	 * This transformation ensures that -0 and +0 map to the same biased representation.
	 */                                                                                                          \
    [[nodiscard]]                                                                                                \
    static bits_##sfx##_t sign_and_magnitude_to_biased_##sfx(bits_##sfx##_t sam) [[unsequenced]] {               \
        return (sam & sign_bit_mask_##sfx) ? ~sam + 1 : sign_bit_mask_##sfx | sam;                               \
    }                                                                                                            \
                                                                                                                 \
    /*
     * Returns the number of representable steps (ULPs) between two IEEE-754 floats by first mapping their raw
     * bit patterns into a representation where unsigned integer ordering matches numerical ordering.
     *
     * In IEEE-754, adjacent bit patterns (when interpreted as this biased unsigned integer) always differ
     * by exactly 1 ULP (Unit in the Last Place).
     *
     * Comparing two floats by "how many representable floats apart" they are instead of their absolute
     * difference is usually a more robust approach.
     *
     * Note: ULP size varies with magnitude, i.e., adjacent floats do not differ by a constant value.
     */                                                                                                          \
    [[nodiscard]]                                                                                                \
    static bits_##sfx##_t distance_between_sam_##sfx(bits_##sfx##_t sam1, bits_##sfx##_t sam2) [[unsequenced]] { \
        const bits_##sfx##_t biased1 = sign_and_magnitude_to_biased_##sfx(sam1);                                 \
        const bits_##sfx##_t biased2 = sign_and_magnitude_to_biased_##sfx(sam2);                                 \
        return (biased1 >= biased2) ? (biased1 - biased2) : (biased2 - biased1);                                 \
    }                                                                                                            \
                                                                                                                 \
    [[nodiscard]]                                                                                                \
    static bool is_nan_##sfx(bits_##sfx##_t bits) [[unsequenced]] {                                              \
        return ((bits & exponent_bit_mask_##sfx) == exponent_bit_mask_##sfx) &&                                  \
               ((bits & fraction_bit_mask_##sfx) != 0);                                                          \
    }                                                                                                            \
                                                                                                                 \
    [[nodiscard]]                                                                                                \
    extern bool cutest_fpa_almost_equals_##sfx(T lhs, T rhs, int max_ulps) [[reproducible]] {                    \
        /* Strict Aliasing Rule workaround */                                                                    \
        const union { T value; bits_##sfx##_t bits; } l = { .value = lhs };                                      \
        const union { T value; bits_##sfx##_t bits; } r = { .value = rhs };                                      \
        /* The IEEE standard says that any comparison operation involving a NAN must return false. */            \
        return !is_nan_##sfx(l.bits) &&                                                                          \
               !is_nan_##sfx(r.bits) &&                                                                          \
               distance_between_sam_##sfx(l.bits, r.bits) <= (bits_##sfx##_t)max_ulps;                           \
    }

DEFINE_FPA_HELPERS(float, f, FLT_MANT_DIG)
DEFINE_FPA_HELPERS(double, d, DBL_MANT_DIG)
DEFINE_FPA_HELPERS(long double, ld, LDBL_MANT_DIG)
