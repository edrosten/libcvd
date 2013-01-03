#ifndef GRANTA_DEBUG_HPP
#define GRANTA_DEBUG_HPP
#ifdef GRANTA_DEBUG
#define GRANTA_CHECK_INDEX(_i_, _n_) GRANTA_ASSERT(_i_ < _n_)
#define GRANTA_ASSERT(_x_) assert(_x_)
#else
#define GRANTA_CHECK_INDEX(_i_, _n_)
#define GRANTA_ASSERT(_x_) assert(_x_)
#endif
#endif
