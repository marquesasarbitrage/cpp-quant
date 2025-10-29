#pragma once 
#include <iostream>
#include <exception>

class QuantLibraryError: public std::exception 
{
    public:
        const char* what() const noexcept override;
        explicit QuantLibraryError(){};
        virtual ~QuantLibraryError() = default;
    protected: 
        virtual std::string getErrorMessage() const = 0; 
    private:
        mutable std::string cachedMessage_;  // must be mutable to modify in const what()
};

namespace QuantErrorRegistry
{
    class NegativeYearFractionError final: public QuantLibraryError {protected: std::string getErrorMessage() const override; };
    class NegativeForwardYearFractionError final: public QuantLibraryError {protected: std::string getErrorMessage() const override; };

    namespace Valuation 
    {
        namespace MarketData
        {
            class EmptyOvernightAverageRateError final:public QuantLibraryError {protected: std::string getErrorMessage() const override; };

            namespace TermStructure 
            {
                namespace DiscountCurve 
                {
                    class MismatchTenorBumpSizeError final:public QuantLibraryError {protected: std::string getErrorMessage() const override; };
                }
            }

        }
    }

    namespace Instruments
    {
        class NegativeBidAskSpreadError final: public QuantLibraryError {protected: std::string getErrorMessage() const override; };

        class SettledInstrumentError final: public QuantLibraryError {protected: std::string getErrorMessage() const override; };

        class IncorrectModelError final: public QuantLibraryError {protected: std::string getErrorMessage() const override; };

        class NegativeNotionalError final : public QuantLibraryError {protected: std::string getErrorMessage() const override; };

        namespace Bond
        {
            class InvalidRedemptionMapError final: public QuantLibraryError {protected: std::string getErrorMessage() const override; };

            class InvalidCouponDateError final: public QuantLibraryError {protected: std::string getErrorMessage() const override; };
        }

        
    }

    
}