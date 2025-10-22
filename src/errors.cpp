#include "../include/cpp-quant/errors.hpp"

const char* QuantLibraryError::what() const noexcept
{
    if (cachedMessage_.empty()) {
            cachedMessage_ = getErrorMessage();  
        }
    return cachedMessage_.c_str();
}

namespace QuantErrorRegistry
{
    std::string NegativeYearFractionError::getErrorMessage() const {return "The year fraction cannot be negative.";}
    std::string NegativeForwardYearFractionError::getErrorMessage() const {return "The end year fraction must be greater or equal to the start year fraction.";}

    namespace Valuation 
    {
        namespace MarketData
        {
            std::string EmptyOvernightAverageRateError::getErrorMessage() const {return "The Overnight average rate object cannot be initialized with empty data. At least one point is required.";}

            namespace TermStructure 
            {
                namespace DiscountCurve 
                {
                    std::string MismatchTenorBumpSizeError::getErrorMessage() const {return "The vector of basis point bump must be the same size of the tenor lists.";}
                }
            }

        }
    }

}