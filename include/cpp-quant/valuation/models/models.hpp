#pragma once 
#include <iostream>
#include "../../../../include/cpp-quant/errors.hpp"
#include "../../../../include/cpp-quant/tools/scheduler.hpp"

class ValuationModel
{
    public: 
        ValuationModel(const DateTime& referenceTime); 
        virtual ~ValuationModel() = 0; 

        DateTime getReferenceTime() const;  
    
    private: 
        DateTime referenceTime_; 
        
};

