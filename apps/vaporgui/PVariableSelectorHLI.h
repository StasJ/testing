#pragma once

#include "PWidgetHLI.h"
#include "PVariableSelector.h"

//! \class PVariableSelectorHLI
//! Allows the user to select variables. Automatically switches between 2D and 3D
//! based on the currently selected variable.
//!
//! Designed to be used with a RenderParams object.

template<class P>
class PVariableSelectorHLI : 
    public PVariableSelector,
    public PWidgetHLI<P> 
{
public:
    
    PVariableSelectorHLI(
        const std::string &label,
        typename PWidgetHLIBase<P, std::string>::GetterType getter,
        typename PWidgetHLIBase<P, std::string>::SetterType setter
    ) : PVariableSelector<P>( "", label ),
        PWidgetHLIBase<P, std::string> (
            (PWidget*)this,
            getter,
            setter
        ) {}
};
