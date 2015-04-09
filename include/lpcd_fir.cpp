#include "lpcd_fir.h"

LPCD::FIR::FIR()
{
}

LPCD::FIR::FIR(std::vector<double> const & coeffs)
{
	m_coeffs = coeffs;
}
