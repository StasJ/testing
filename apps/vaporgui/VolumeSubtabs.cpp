#include "VolumeSubtabs.h"
#include "TFEditorVolume.h"
#include "QSliderEdit.h"
#include "PGroup.h"
#include "PSection.h"
#include "PTFEditor.h"
#include "PCheckbox.h"
#include "PStringDropdownHLI.h"
#include "PEnumDropdown.h"
#include "PSliderEdit.h"
#include "PVariableSelector.h"
#include <vapor/VolumeOSPRay.h>


using namespace VAPoR;

void VolumeVariablesSubtab::Update(DataMgr *dataMgr, ParamsMgr *paramsMgr, RenderParams *params)
{
    VolumeParams *vp = dynamic_cast<VolumeParams*>( params );
    _volumeParams = vp;
    VAssert(vp);
    
    _variablesWidget->Update(dataMgr, paramsMgr, params);
    _ps->Update(params, paramsMgr, dataMgr);
}

VolumeAppearanceSubtab::VolumeAppearanceSubtab(QWidget* parent) {
    setupUi(this);
    
    verticalLayout->insertWidget(0, _pg = new PGroup);
    _pg->Add((new PTFEditor(RenderParams::_variableNameTag, {PTFEditor::Default}, "Transfer Function"))->ShowColormapBasedOnParam(VolumeParams::UseColormapVariableTag, false));
    
    PSection *rtp = new PSection("Volume Parameters");
    _pg->Add(rtp);
    rtp->Add(new PStringDropdownHLI<VolumeParams>("Raytracing Algorithm", VolumeParams::GetAlgorithmNames(VolumeParams::Type::DVR), &VolumeParams::GetAlgorithm, &VolumeParams::SetAlgorithmByUser));
    
    PGroup *gpu = new PGroup;
    _pg->Add(gpu);
    gpu->ShowBasedOnParam(VolumeParams::_algorithmTag, VolumeOSPRay::GetName(), true);
    
    rtp = new PSection("GPU Volume Parameters");
    gpu->Add(rtp);
    rtp->Add(new PEnumDropdown(VolumeParams::SamplingRateMultiplierTag, {"1x", "2x", "4x", "8x", "16x"}, {1, 2, 4, 8, 16}, "Sampling Rate Multiplier"));
    rtp->Add((new PDoubleSliderEdit(VolumeParams::VolumeDensityTag, "Volume Density"))->EnableDynamicUpdate()->SetTooltip("Changes the overall density or 'opacity' of the volume allowing for finer tuning of the transfer function."));
    rtp->Add(new PCheckbox(VolumeParams::UseColormapVariableTag, "Color by other variable"));
    rtp->Add((new PVariableSelector3D(RenderParams::_colorMapVariableNameTag))->ShowBasedOnParam(VolumeParams::UseColormapVariableTag));
    
    gpu->Add((new PTFEditor(RenderParams::_colorMapVariableNameTag, {PTFEditor::Histogram, PTFEditor::Colormap}, "Colormap Transfer Function"))->ShowBasedOnParam(VolumeParams::UseColormapVariableTag));
    
    PSection *lp = new PSection("GPU Lighting Parameters");
    gpu->Add(lp);
    lp->Add(new PCheckbox(VolumeParams::LightingEnabledTag));
    lp->Add((new PDoubleSliderEdit(VolumeParams::PhongAmbientTag,   "Ambient" ))->EnableDynamicUpdate());
    lp->Add((new PDoubleSliderEdit(VolumeParams::PhongDiffuseTag,   "Diffuse" ))->EnableDynamicUpdate());
    lp->Add((new PDoubleSliderEdit(VolumeParams::PhongSpecularTag,  "Specular"))->EnableDynamicUpdate());
    lp->Add((new PDoubleSliderEdit(VolumeParams::PhongShininessTag, "Specular"))->SetRange(1, 100)->EnableDynamicUpdate());
    
    
    PSection *op = new PSection("OSPRay Parameters");
    _pg->Add(op);
    op->ShowBasedOnParam(VolumeParams::_algorithmTag, VolumeOSPRay::GetName());
    op->Add((new PDoubleSliderEdit(VolumeParams::OSPDensity, "Density"))->SetRange(0, 3)->EnableDynamicUpdate());
    op->Add((new PIntegerSliderEdit("osp_spp", "Samples Per Pixel"))->SetRange(1, 10));
    op->Add((new PDoubleInput(VolumeParams::OSPSampleRateScalar, "Volume Sample Rate Scalar"))->EnableBasedOnParam("osp_usePT", false));
    
#ifndef NDEBUG
    op = new PSection("OSPRay DEBUG");
    _pg->Add(op);
    op->ShowBasedOnParam(VolumeParams::_algorithmTag, VolumeOSPRay::GetName());
    op->Add(new PCheckbox("osp_usePT", "Pathtracing"));
    op->Add((new PCheckbox("osp_enable_clipping", "Enable Clipping Planes"))->SetTooltip("Breaks depth buffer due to a bug in OSPRay"));
    op->Add(new PCheckbox("osp_useBackplate", "Backplate"));
    op->Add((new PDoubleSliderEdit("osp_light_intensity", "Light Intensity"))->SetRange(0, 10)->EnableDynamicUpdate()->EnableBasedOnParam("osp_usePT"));
    op->Add(new PCheckbox("osp_force_regular", "Force Regular"));
    op->Add(new PCheckbox("osp_test_volume", "Test Volume"));
    op->Add(new PCheckbox("osp_draw_test_cell_faces", "Test Faces"));
    op->Add(new PIntegerInput("osp_max_cells", "Max Cells"));
    op->Add(new PIntegerInput("osp_test_cells", "Test Cell ID"));
    op->Add(new PCheckbox("osp_opaque_bg", "Opaque Background"));
#endif
}

void VolumeAppearanceSubtab::Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *rParams)
{
    _pg->Update(rParams, paramsMgr, dataMgr);
}
