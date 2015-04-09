#ifndef LPCD_FIR_H
#define LPCD_FIR_H

#include <vector>

namespace LPCD
{
	class FIR
	{
	public:
		FIR();
		FIR(std::vector<double> const& coeffs);

		void setCoeffs(std::vector<double> const& coeffs) { m_coeffs = coeffs; }
		std::vector<double> process(double const& gain, std::vector<double> const& input, std::vector<double>& delayed)
		{
			std::vector<double> output(input.size(), 0.0);

			for (uint32_t i = 0; i < output.size(); i++)
			{
				output[i] = gain * input[i] + delayed[0];

				for (uint8_t j = 1; j < m_coeffs.size()-1; j++)
				{
					int x = m_coeffs.size();
					int y = delayed.size();
					delayed[j - 1] = delayed[j] - m_coeffs[j] * output[i];
				}
			}

			return output;
		}

	private:
		std::vector<double> m_coeffs;
	};
}
#endif