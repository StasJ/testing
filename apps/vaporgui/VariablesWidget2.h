#pragma once

#include "VSection.h"
#include "Flags.h"

namespace VAPoR {
    class DataMgr;
    class ParamsMgr;
    class RenderParams;
}

//#define BREAK_FOO = true
#define BREAK_HLI = true

#if defined( BREAK_FOO )
    class Foo;

    // do not break HLI
    //template<typename T> class PVariableSelectorHLI;
    template<typename T> class PEnumDropdownHLI;

#elif defined( BREAK_HLI )
    //class PVariableSelectorHLI;
    class PEnumDropdownHLI;

    // do not break foo 
    template<typename T> class Foo;

#else
    template<typename T> class Foo;
    template<typename T> class PEnumDropdownHLI;
#endif



class PGroup;
class VComboBox;
class VLineItem;
class FidelityWidget2;

class VariablesWidget2 : public VSection {

    Q_OBJECT

public:

    VariablesWidget2();
    void Reinit(
        VariableFlags varFlags,
        DimFlags      dimFlags
    );

    virtual void Update(
        //const VAPoR::DataMgr *dataMgr,
        VAPoR::DataMgr *dataMgr,
        VAPoR::ParamsMgr *paramsMgr,
        VAPoR::RenderParams *rParams
    );

    int GetActiveDimension() const;
    DimFlags GetDimFlags() const;

    void Configure2DFieldVars();
    void Configure3DFieldVars();

private:

#if defined( BREAK_FOO )
    Foo* _foo;

    // do not break HLI
    PEnumDropdownHLI<VAPoR::RenderParams>* _enumHLI;

#elif defined( BREAK_HLI )
    PEnumDropdownHLI* _enumHLI;
   
    // do not break foo
    Foo<int>* _foo;

#else
    Foo<int>* _foo;
    PEnumDropdownHLI<VAPoR::RenderParams>* _enumHLI;
#endif

    size_t _activeDim;
    bool   _initialized;

    VariableFlags _variableFlags;
    DimFlags      _dimFlags;

    VComboBox* _dimCombo;
    VLineItem* _dimLineItem;

    PGroup* _pg;

    
    /*PVariableSelector* _scalarCombo;
    PVariableSelector* _xFieldCombo;
    PVariableSelector* _yFieldCombo;
    PVariableSelector* _zFieldCombo;
    PVariableSelector* _colorCombo;
    PVariableSelector* _heightCombo;*/
    

    FidelityWidget2* _fidelityWidget;

    static const std::string _sectionTitle;

    /*
    /Users/pearse/VAPOR/apps/vaporgui/PVariableSelectorHLI.h:13:7: 
    error: redefinition of 'PVariableSelectorHLI' as different kind of symbol

    class PVariableSelectorHLI :
      ^
    /Users/pearse/VAPOR/apps/vaporgui/VariablesWidget2.h:13:7: 
    note: previous definition is here
    class PVariableSelectorHLI;
    */

    //
    //PVariableSelectorHLI* _scalarCombo;
    //

    //const VAPoR::DataMgr* _dataMgr;
    VAPoR::DataMgr* _dataMgr;
    VAPoR::ParamsMgr* _paramsMgr;
    VAPoR::RenderParams* _rParams;

private slots:
    void _dimChanged();
};
