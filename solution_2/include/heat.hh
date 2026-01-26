#ifndef HEAT_HEAT_HH
#define HEAT_HEAT_HH

#include "spec/control.hh"

namespace heat {

void initialize(spec::control_policy &cp);

void advance(spec::control_policy &cp);

void analyze(spec::control_policy &cp);

void finalize(spec::control_policy &cp);

} // namespace heat

#endif // HEAT_INCLUDE_HH
