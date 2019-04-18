#include "FlowSubtabs.h"
#include "VariablesWidget.h"
#include "TFWidget.h"
#include "GeometryWidget.h"
#include "CopyRegionWidget.h"
#include "TransformTable.h"
#include "ColorbarWidget.h"
#include "VaporWidgets.h"

QVaporSubtab::QVaporSubtab(QWidget* parent) : QWidget(parent)
{
    _layout = new QVBoxLayout(this);
    _layout->setContentsMargins(0,0,0,0);
    _layout->insertSpacing(-1, 20);
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Maximum);
}


//
//================================
//
FlowVariablesSubtab::FlowVariablesSubtab(QWidget* parent) : QVaporSubtab(parent)
{
    _variablesWidget = new VariablesWidget(this);
    _variablesWidget->Reinit(   (VariableFlags)(VECTOR | COLOR),
                                (DimFlags)(THREED) );
    _layout->addWidget( _variablesWidget, 0, 0 );

    _velocityMltp = new QLineEdit( this );
    _layout->addWidget( _velocityMltp );

    _steady = new VCheckBox( this, "Use Steady Flow" );
    _layout->addWidget( _steady );

    _steadyNumOfSteps = new QLineEdit( this );
    _layout->addWidget( _steadyNumOfSteps);

    connect( _steady,           SIGNAL( _checkboxClicked() ), this, SLOT( _steadyGotClicked() ) );
    connect( _velocityMltp,     SIGNAL( editingFinished() ),  this, SLOT( _velocityMultiplierChanged() ) );
    connect( _steadyNumOfSteps, SIGNAL( editingFinished() ),  this, SLOT( _steadyNumOfStepsChanged() ) );
}

void 
FlowVariablesSubtab::Update( VAPoR::DataMgr      *dataMgr,
                             VAPoR::ParamsMgr    *paramsMgr,
                             VAPoR::RenderParams *rParams) 
{
    _params = dynamic_cast<VAPoR::FlowParams*>(rParams);
    assert(_params);
    _variablesWidget->Update(dataMgr, paramsMgr, rParams);

    // Update custom widgets
    bool isSteady = _params->GetIsSteady();
    _steady->SetCheckState( isSteady );

    auto mltp = _params->GetVelocityMultiplier();
    _velocityMltp->setText( QString::number( mltp, 'f', 3 ) );

    int numOfSteps = _params->GetSteadyNumOfSteps();
    _steadyNumOfSteps->setText( QString::number( numOfSteps ) );
}

void
FlowVariablesSubtab::_steadyGotClicked()
{
    bool userInput = _steady->GetCheckState();
    _params->SetIsSteady( userInput );
}

void
FlowVariablesSubtab::_velocityMultiplierChanged()
{
    bool ok;
    double d = _velocityMltp->text().toDouble( &ok );
    if( ok )    // We don't need this verification once the line edit has its own validator
        _params->SetVelocityMultiplier( d );
}

void 
FlowVariablesSubtab::_steadyNumOfStepsChanged()
{
    bool ok;
    int i = _steadyNumOfSteps->text().toInt( &ok );
    if( ok )    // We don't need this verification once the line edit has its own validator
        _params->SetSteadyNumOfSteps( i );
}

//
//================================
//
FlowAppearanceSubtab::FlowAppearanceSubtab(QWidget* parent) : QVaporSubtab(parent)
{
    _TFWidget = new TFWidget(this);
    _TFWidget->Reinit((TFFlags)(SAMPLING | CONSTANT_COLOR));

    _layout->addWidget( _TFWidget, 0, 0 );

    _params = NULL;
}

void FlowAppearanceSubtab::Update(  VAPoR::DataMgr *dataMgr,
                                    VAPoR::ParamsMgr *paramsMgr,
                                    VAPoR::RenderParams *rParams) 
{
    _params = dynamic_cast<VAPoR::FlowParams*>(rParams);
    assert(_params);

    _TFWidget->Update(dataMgr, paramsMgr, rParams);
}

//
//================================
//
FlowIntegrationSubtab::FlowIntegrationSubtab(QWidget* parent) : QVaporSubtab(parent)
{
    _integrationSettingsTab = new VTabWidget( this, "Flow Integration Settings" );

    _integrationTypeCombo = new VComboBox( this, "Integration type" );
    _integrationTypeCombo->AddOption( "Steady", 0 );
    _integrationTypeCombo->AddOption( "Unsteady", 1 );
    _integrationSettingsTab->AddWidget( _integrationTypeCombo );
    connect( _integrationTypeCombo, SIGNAL( _indexChanged(int) ),
        this, SLOT( _configureIntegrationOptions() ));

    _directionCombo = new VComboBox( this, "Integration direction" );
    _directionCombo->AddOption( "Forward", 0 );
    _directionCombo->AddOption( "Backward", 1 );
    _integrationSettingsTab->AddWidget( _directionCombo );

    _startSpinBox = new VSpinBox( this, "Integration start time" );
    _integrationSettingsTab->AddWidget( _startSpinBox );
    _endSpinBox = new VSpinBox( this, "Integration end time" );
    _integrationSettingsTab->AddWidget( _endSpinBox );
    _intervalSpinBox = new VSpinBox( this, "Seed injection interval" );
    _integrationSettingsTab->AddWidget( _intervalSpinBox );
    
    _periodicBoundaryComboX = new VCheckBox( this, "X axis periodicity" );
    _integrationSettingsTab->AddWidget( _periodicBoundaryComboX );
    _periodicBoundaryComboY = new VCheckBox( this, "Y axis periodicity" );
    _integrationSettingsTab->AddWidget( _periodicBoundaryComboY );
    _periodicBoundaryComboZ = new VCheckBox( this, "Z axis periodicity" );
    _integrationSettingsTab->AddWidget( _periodicBoundaryComboZ );

    _multiplierLineEdit = new VLineEdit( this, "Vector field multiplier" );
    _integrationSettingsTab->AddWidget( _multiplierLineEdit );

    _configureIntegrationOptions();
    _layout->addWidget( _integrationSettingsTab );

}

void FlowIntegrationSubtab::_configureIntegrationOptions() {
    string seedType = _integrationTypeCombo->GetCurrentText();
    if ( seedType == "Steady" ) {
        _startSpinBox->hide();
        _endSpinBox->hide();
        _intervalSpinBox->hide();
    }
    else {
        _startSpinBox->show();
        _endSpinBox->show();
        _intervalSpinBox->show();
    }
}

void FlowIntegrationSubtab::Update(
        VAPoR::DataMgr *dataMgr,
        VAPoR::ParamsMgr *paramsMgr,
        VAPoR::RenderParams *rParams
    )
{

}

//
//================================
//
FlowSeedingSubtab::FlowSeedingSubtab(QWidget* parent) : QVaporSubtab(parent)
{
    _seedSettingsTab = new VTabWidget( this, "Seed Distribution Settings" );
    _distributionCombo = new VComboBox( this, "Seed distribution type" );
    _distributionCombo->AddOption( "Gridded", 0 );
    _distributionCombo->AddOption( "Random", 1 );
    _distributionCombo->AddOption( "List of points", 2 );
    _seedSettingsTab->AddWidget( _distributionCombo );
    connect( _distributionCombo, SIGNAL( _indexChanged(int) ),
        this, SLOT( _configureRakeOptions() ));

    // Random rake options
    _randomCountSpinBox = new VSpinBox( this, "Number of random seeds", 64 );
    _seedSettingsTab->AddWidget( _randomCountSpinBox );
    _biasVariableCombo = new VComboBox( this, "Random distribution bias variable" );
    _seedSettingsTab->AddWidget( _biasVariableCombo );
    _biasSliderEdit = new QSliderEdit( this );
    _biasSliderEdit->SetLabel( "Bias weight" );
    _seedSettingsTab->AddWidget( _biasSliderEdit );

    // Gridded rake options
    _xDistributionSpinBox = new VSpinBox( this, "X axis seeds", 8 );
    _seedSettingsTab->AddWidget( _xDistributionSpinBox );
    _yDistributionSpinBox = new VSpinBox( this, "Y axis seeds", 8 );
    _seedSettingsTab->AddWidget( _yDistributionSpinBox );
    _zDistributionSpinBox = new VSpinBox( this, "Z axis seeds", 8 );
    _seedSettingsTab->AddWidget( _zDistributionSpinBox );

    // List of seed file picker
    _fileReader = new VFileReader( this, "Seed File" );
    _seedSettingsTab->AddWidget( _fileReader );

    _configureRakeOptions();
    _layout->addWidget( _seedSettingsTab );
   
    // Rake region selector 
    _geometryWidget   = new GeometryWidget(this);
    _geometryWidget->Reinit( 
        (DimFlags)THREED,
        (VariableFlags)VECTOR
    );
    _layout->addWidget( _geometryWidget );

}

void FlowSeedingSubtab::Update(
        VAPoR::DataMgr *dataMgr,
        VAPoR::ParamsMgr *paramsMgr,
        VAPoR::RenderParams *rParams
    )
{
    //VAPoR::Box* rakeBox = rParams->GetRakeBox();
    //_geometryWidget->Update(paramsMgr, dataMgr, rParams, rakeBox);
    _geometryWidget->Update(paramsMgr, dataMgr, rParams );
}

void FlowSeedingSubtab::_configureRakeOptions() {
    string seedType = _distributionCombo->GetCurrentText();
    if ( seedType == "Random" ) {
        _randomCountSpinBox->show();
        _biasVariableCombo->show();
        _biasSliderEdit->show();

        _xDistributionSpinBox->hide();
        _yDistributionSpinBox->hide();
        _zDistributionSpinBox->hide();

        _fileReader->hide();
    }
    else if ( seedType == "Gridded" ) {
        _randomCountSpinBox->hide();
        _biasVariableCombo->hide();
        _biasSliderEdit->hide();

        _xDistributionSpinBox->show();
        _yDistributionSpinBox->show();
        _zDistributionSpinBox->show();

        _fileReader->hide();

    }
    else { // ( seedType == "List of points" )
        _randomCountSpinBox->hide();
        _biasVariableCombo->hide();
        _biasSliderEdit->hide();

        _xDistributionSpinBox->hide();
        _yDistributionSpinBox->hide();
        _zDistributionSpinBox->hide();

        _fileReader->show();

    }
}

void FlowSeedingSubtab::_pushTestPressed() 
{
    cout << "Push button pressed" << endl;
}

void FlowSeedingSubtab::_comboBoxSelected( int index ) 
{
    string option = "*** Need to turn on _comboTest at FlowSubtabs.cpp:107";
    cout << "Combo selected at index " << index << " for option " << option << endl;
}

void FlowSeedingSubtab::_checkBoxSelected() 
{
    bool checked = 0;//_checkboxTest->GetCheckState();
    cout << "Checkbox is checked? " << checked << endl;
}


//
//================================
//
FlowGeometrySubtab::FlowGeometrySubtab(QWidget* parent) : QVaporSubtab(parent)
{
    _geometryWidget   = new GeometryWidget(this);
    _copyRegionWidget = new CopyRegionWidget(this);
    _transformTable   = new TransformTable(this);
    _geometryWidget->Reinit( 
        (DimFlags)THREED,
        (VariableFlags)VECTOR
    );

    _layout->addWidget( _geometryWidget, 0 ,0 );
    _layout->addWidget( _copyRegionWidget, 0 ,0 );
    _layout->addWidget( _transformTable, 0 ,0 );

    _params = NULL;
}

void FlowGeometrySubtab::Update( VAPoR::ParamsMgr *paramsMgr,
                                 VAPoR::DataMgr *dataMgr,
                                 VAPoR::RenderParams *rParams) 
{
    _params = dynamic_cast<VAPoR::FlowParams*>(rParams);
    assert(_params);

    _geometryWidget->Update(paramsMgr, dataMgr, rParams);
    _copyRegionWidget->Update(paramsMgr, rParams);
    _transformTable->Update(rParams->GetTransform());
}


//
//================================
//
FlowAnnotationSubtab::FlowAnnotationSubtab(QWidget* parent) : QVaporSubtab(parent)
{
    _colorbarWidget = new ColorbarWidget(this);
    _layout->addWidget(_colorbarWidget, 0, 0);
}

void FlowAnnotationSubtab::Update(  VAPoR::ParamsMgr *paramsMgr,
                                    VAPoR::DataMgr *dataMgr,
                                    VAPoR::RenderParams *rParams) 
{
    _colorbarWidget->Update(dataMgr, paramsMgr, rParams);
}
