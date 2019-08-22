#include "TFControlPointWidget.h"
#include <QBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QDoubleValidator>
#include <vapor/RenderParams.h>

TFControlPointWidget::TFControlPointWidget()
{
    QBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    this->setLayout(layout);
    layout->addWidget(_valueEdit = new QLineEdit, 30);
    layout->addWidget(_valueEditType = new QComboBox, 20);
    layout->addStretch(20);
    
    _valueEdit->setValidator(new QDoubleValidator);
    
    _valueEditType->blockSignals(true);
    _valueEditType->setMinimumSize(30, 10);
    _valueEditType->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    _valueEditType->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    _valueEditType->addItem("data");
    _valueEditType->addItem("%");
    _valueEditType->blockSignals(false);
    
    connect(_valueEditType, SIGNAL(currentIndexChanged(int)), this, SLOT(valueEditTypeChanged(int)));
    connect(_valueEdit, SIGNAL(returnPressed()), this, SLOT(valueEditChanged()));
    
    this->setDisabled(true);
}

void TFControlPointWidget::Update(VAPoR::RenderParams *rParams)
{
    if (!rParams)
        return;
    
    _min = rParams->GetMapperFunc(rParams->GetVariableName())->getMinMapValue();
    _max = rParams->GetMapperFunc(rParams->GetVariableName())->getMaxMapValue();
    
    ((QDoubleValidator*)_valueEdit->validator())->setRange(_min, _max);
}

//void TFControlPointWidget::SelectOpacityControlPoint(int index)
//{
//    this->setEnabled(true);
//    _opacityId = index;
//    _colorId = -1;
//    Update(_params);
//}

//void TFControlPointWidget::SelectColorControlPoint(int index)
//{
//    this->setEnabled(true);
//    _opacityId = -1;
//    _colorId = index;
//    Update(_params);
//}

void TFControlPointWidget::DeselectControlPoint()
{
    this->setDisabled(true);
//    _opacityId = -1;
//    _colorId = -1;
    _valueEdit->clear();
}

void TFControlPointWidget::SetNormalizedValue(float value)
{
    _value = value;
    updateValue();
}

void TFControlPointWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    
    QWidget::paintEvent(event);
}

void TFControlPointWidget::updateValue()
{
    if (!isEnabled())
        return;
    
    float value = _value;
    if (isUsingMappedValue())
        value = toMappedValue(value);
    else
        value *= 100;
    _valueEdit->setText(QString::number(value));
}

bool TFControlPointWidget::isUsingNormalizedValue() const
{
    return _valueEditType->currentIndex() == ValueFormat::Percent;
}

bool TFControlPointWidget::isUsingMappedValue() const
{
    return _valueEditType->currentIndex() == ValueFormat::Mapped;
}

float TFControlPointWidget::toMappedValue(float normalized) const
{
    return normalized * (_max - _min) + _min;
}

float TFControlPointWidget::toNormalizedValue(float mapped) const
{
    if (_max == _min)
        return 0;
    
    return (mapped - _min) / (_max - _min);
}

float TFControlPointWidget::getValueFromEdit() const
{
    float value = _valueEdit->text().toFloat();
    if (isUsingMappedValue())
        return toNormalizedValue(value);
    else
        return value / 100.f;
}

void TFControlPointWidget::valueEditTypeChanged(int)
{
    updateValue();
}

void TFControlPointWidget::valueEditChanged()
{
    _value = getValueFromEdit();
    controlPointChanged();
}
