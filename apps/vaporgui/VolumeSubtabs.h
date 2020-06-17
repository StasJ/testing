#pragma once

#include "ui_VolumeAppearanceGUI.h"
#include "ui_VolumeVariablesGUI.h"
#include "ui_VolumeGeometryGUI.h"
#include "ui_VolumeAnnotationGUI.h"
#include "Flags.h"
#include <vapor/MapperFunction.h>
#include <vapor/VolumeParams.h>

namespace VAPoR {
	class ControlExec;
	class RenderParams;
	class ParamsMgr;
	class DataMgr;
}

class TFEditorVolume;
class QSliderEdit;
class PGroup;

#include "PSection.h"
#include "PCheckbox.h"
#include "PSliderEdit.h"
#include "PIntegerInput.h"
#include "PDoubleInput.h"

class VolumeVariablesSubtab : public QWidget, public Ui_VolumeVariablesGUI {

    Q_OBJECT

public:
	VolumeVariablesSubtab(QWidget* parent) {
		setupUi(this);
		_variablesWidget->Reinit(
			(VariableFlags)(SCALAR | COLOR),
			(DimFlags)(THREED)
		);
        
        _ps = new PSection("OSPray");
        ((QVBoxLayout*)layout())->insertWidget(1, _ps);
        
        _ps->Add(new PCheckbox("osp_enable", "Enable"));
        _ps->Add(new PCheckbox("osp_usePT", "Pathtracing"));
        _ps->Add(new PCheckbox("osp_force_regular", "Force Regular"));
        _ps->Add((new PDoubleSliderEdit("osp_density", "Density"))->SetRange(0, 100)->EnableDynamicUpdate());
        _ps->Add(new PCheckbox("osp_test_volume", "Test Volume"));
//        _ps->Add(new PIntegerInput("osp_max_cells", "Max Unstruct Cells"));
        _ps->Add(new PIntegerInput("osp_test_cells", "Test Cell ID"));
        _ps->Add((new PDoubleInput("osp_volumeSamplingRate", "Volume Sample Rate Scalar"))->EnableBasedOnParam("osp_usePT", false));
        _ps->Add((new PDoubleSliderEdit("osp_light_intensity", "Light Intensity"))->SetRange(0, 10000)->EnableDynamicUpdate()->EnableBasedOnParam("osp_usePT"));
        _ps->Add((new PDoubleSliderEdit("osp_light_brightness", "Light Brightness"))->SetRange(0, 10)->EnableDynamicUpdate()->EnableBasedOnParam("osp_usePT"));
        _ps->Add((new PIntegerSliderEdit("osp_spp", "Samples Per Pixel"))->SetRange(1, 10));
	}

	void Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *rParams);
    
private slots:
    
private:
    VAPoR::VolumeParams *_volumeParams;
    PSection *_ps;
};

class VolumeAppearanceSubtab : public QWidget, public Ui_VolumeAppearanceGUI {
	Q_OBJECT

public:
    VolumeAppearanceSubtab(QWidget* parent);
	void Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *rParams);
    
private:
    PGroup *_pg;
};

class VolumeGeometrySubtab : public QWidget, public Ui_VolumeGeometryGUI {

	Q_OBJECT

public:
	VolumeGeometrySubtab(QWidget* parent) {
		setupUi(this);
		_geometryWidget->Reinit(
			(DimFlags)THREED,
			(VariableFlags)SCALAR
		);
	}
	
	void Update(
		VAPoR::ParamsMgr *paramsMgr,
		VAPoR::DataMgr *dataMgr,
		VAPoR::RenderParams *rParams
	) {
		_geometryWidget->Update(paramsMgr, dataMgr, rParams);
		_copyRegionWidget->Update(paramsMgr, rParams);
		_transformTable->Update(rParams->GetTransform());
	}
};

class VolumeAnnotationSubtab : public QWidget, public Ui_VolumeAnnotationGUI {

	Q_OBJECT

public:
	VolumeAnnotationSubtab(QWidget* parent) {
		setupUi(this);
	}
	
	void Update(
		VAPoR::ParamsMgr *paramsMgr,
		VAPoR::DataMgr *dataMgr,
		VAPoR::RenderParams *rParams
	) {
		_colorbarWidget->Update(dataMgr, paramsMgr, rParams);
	}
};
