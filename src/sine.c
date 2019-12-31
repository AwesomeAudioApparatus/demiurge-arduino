//
// Created by niclas on 12/2/19.
//

#include "sine.h"
#include "esp_system.h"

/// A sine approximation via a fourth-order cosine approx.
/// @param x   angle (with 2^15 units/circle)
/// @return     Sine value (Q12)
int32_t IRAM_ATTR isin(int32_t x) {

   static const int qN = 13, qA = 12, B = 19900, C = 3516;

   int c = x << (30 - qN);          // Semi-circle info into carry.
   x -= 1 << qN;                    // sine -> cosine calc

   x = x << (31 - qN);              // Mask with PI
   x = x >> (31 - qN);              // Note: SIGNED shift! (to qN)
   x = x * x >> (2 * qN - 14);      // x=x^2 To Q14

   int y = B - (x * C >> 14);       // B - x^2*C
   y = (1 << qA) - (x * y >> 16);   // A - x^2*(B-x^2*C)

   return c >= 0 ? y : -y;
}
