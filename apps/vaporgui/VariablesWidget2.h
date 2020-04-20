#pragma once

#include "PSection.h"
#include "Flags.h"

class PVariableSelector;

class VariablesWidget2 : public PSection {

    Q_OBJECT

public:

    VariablesWidget2();
    void Reinit(
        VariableFlags varFlags,
        DimFlags      dimFlags
    );

    int GetActiveDimension() const;
    DimFlags GetDimFlags() const;

    void Configure2DFieldVars();
    void Configure3DFieldVars();

private:



};
