#ifndef HEAT_HEAT_HH
#define HEAT_HEAT_HH

#include "include/state.hh"

namespace heat {

void initialize(state &s);

void advance(state &s);

void analyse(const state &s);

void finalize(const state &s);

} // namespace heat

#endif // HEAT_INCLUDE_HH
