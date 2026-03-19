#include "button.h"

namespace ui {

bool Boundary::contains(float x, float y) const {
    return x >= bounds.x && x <= bounds.x + bounds.w
      && y >= bounds.y && y <= bounds.y + bounds.h;
}

} // namespace ui
