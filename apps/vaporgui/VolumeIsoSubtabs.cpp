#include "VolumeIsoSubtabs.h"
#include "VolumeSubtabs.h"
#include "PGroup.h"
#include "PSection.h"
#include "PTFEditor.h"
#include "PCheckbox.h"
#include "PStringDropdownHLI.h"
#include "PEnumDropdown.h"
#include "PSliderEdit.h"
#include "PVariableSelector.h"
#include "PColorSelector.h"
#include <vapor/VolumeOSPRay.h>

using namespace VAPoR;

void VolumeIsoVariablesSubtab::Update(DataMgr *dataMgr, ParamsMgr *paramsMgr, RenderParams *params)
{
    VolumeIsoParams *vp = dynamic_cast<VolumeIsoParams*>( params );
    _isoParams = vp;
    VAssert(vp);
    // TODO volume
    // long mode = _isoParams->GetCastingMode();
    // _castingModeComboBox->setCurrentIndex( mode - 1 );
    
    _variablesWidget->Update(dataMgr, paramsMgr, params);
}

VolumeIsoAppearanceSubtab::VolumeIsoAppearanceSubtab(QWidget* parent) 
{
    setupUi(this);
    
    _pg = new PGroup;
    
    verticalLayout->insertWidget(0, _pg);
    _pg->Add((new PTFEditor(RenderParams::_variableNameTag, {PTFEditor::Histogram, PTFEditor::IsoValues}, "Transfer Function")));
    _pg->Add(((new PGroup)->Add((new PTFEditor(RenderParams::_colorMapVariableNameTag, {PTFEditor::Histogram, PTFEditor::Colormap}, "Colormap Transfer Function"))->ShowBasedOnParam(VolumeParams::UseColormapVariableTag))->ShowBasedOnParam(VolumeParams::_algorithmTag, VolumeOSPRayIso::GetName(), true)));
    
    PSection *rtp = new PSection("Isosurface Parameters");
    _pg->Add(rtp);
    rtp->Add(new PStringDropdownHLI<VolumeIsoParams>("Raytracing Algorithm", VolumeIsoParams::GetAlgorithmNames(VolumeParams::Type::Iso), &VolumeIsoParams::GetAlgorithm, &VolumeIsoParams::SetAlgorithmByUser));
    rtp->Add((new PEnumDropdown(VolumeIsoParams::SamplingRateMultiplierTag, {"1x", "2x", "4x", "8x", "16x"}, {1, 2, 4, 8, 16}, "Sampling Rate Multiplier"))->EnableBasedOnParam(VolumeParams::_algorithmTag, VolumeOSPRayIso::GetName(), true));
    rtp->Add((new PCheckbox(VolumeIsoParams::UseColormapVariableTag, "Color by other variable"))->EnableBasedOnParam(VolumeParams::_algorithmTag, VolumeOSPRayIso::GetName(), true));
    rtp->Add((new PColorSelector(RenderParams::_constantColorTag, "Color"))->ShowBasedOnParam(VolumeIsoParams::UseColormapVariableTag, false));
    rtp->Add((new PVariableSelector3D(RenderParams::_colorMapVariableNameTag))->ShowBasedOnParam(VolumeIsoParams::UseColormapVariableTag));

    PSection *lp = new PSection("GPU Lighting Parameters");
    lp->ShowBasedOnParam(VolumeParams::_algorithmTag, VolumeOSPRayIso::GetName(), true);
    _pg->Add(lp);
    lp->Add(new PCheckbox(VolumeIsoParams::LightingEnabledTag));
    lp->Add((new PDoubleSliderEdit(VolumeIsoParams::PhongAmbientTag,   "Ambient" ))->EnableDynamicUpdate());
    lp->Add((new PDoubleSliderEdit(VolumeIsoParams::PhongDiffuseTag,   "Diffuse" ))->EnableDynamicUpdate());
    lp->Add((new PDoubleSliderEdit(VolumeIsoParams::PhongSpecularTag,  "Specular"))->EnableDynamicUpdate());
    lp->Add((new PDoubleSliderEdit(VolumeIsoParams::PhongShininessTag, "Specular"))->SetRange(1, 100)->EnableDynamicUpdate());
}


void VolumeIsoAppearanceSubtab::Update( VAPoR::DataMgr      *dataMgr,
                                         VAPoR::ParamsMgr    *paramsMgr,
                                         VAPoR::RenderParams *params ) 
{
    _pg->Update(params, paramsMgr, dataMgr);
}
