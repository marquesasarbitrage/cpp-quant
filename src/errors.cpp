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

    namespace TermStructure 
    {
        namespace DiscountCurve 
        {
            std::string ArbitrageError::getErrorMessage() const {return "The discount curve has arbitrage (discount bonds non-decreasing with time).";}
        }
    }
}