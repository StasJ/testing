#include "TFHistogramWidget.h"
#include "TFHistogramInfoWidget.h"
#include <QPaintEvent>
#include <vapor/DataMgr.h>
#include <QPainter>
#include <glm/glm.hpp>
#include <Histo.h>
#include "ErrorReporter.h"
#include <vapor/RenderParams.h>
#include <vapor/ParamsMgr.h>

using namespace VAPoR;
using std::vector;
using glm::vec2;
using glm::clamp;

static vec2 qvec2(const QPoint &qp)  { return vec2(qp.x(), qp.y()); }
static vec2 qvec2(const QPointF &qp) { return vec2(qp.x(), qp.y()); }
static QPointF qvec2(const vec2 &v) { return QPointF(v.x, v.y); }


TFHistogramWidget::TFHistogramWidget()
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    this->setFrameStyle(QFrame::Box);
}

void TFHistogramWidget::Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *rp)
{
    _renderParams = rp;
    _dataMgr = dataMgr;
    
    if (_histo.getNumBins() != width())
        _histo.reset(width());
    if (_histo.PopulateIfNeeded(dataMgr, rp) < 0)
        MSG_ERR("Failed to populate histogram");
}

QSize TFHistogramWidget::minimumSizeHint() const
{
    return QSize(100, 40);
}

TFInfoWidget *TFHistogramWidget::createInfoWidget()
{
    TFHistogramInfoWidget *info = new TFHistogramInfoWidget;
    
    connect(this, SIGNAL(UpdateInfo(float)), info, SLOT(SetControlPoint(float)));
//    connect(this, SIGNAL(InfoDeselected()), info, SLOT(Deselect()));
    
    return info;
}

#define CONTROL_POINT_RADIUS (4.0f)
#define PADDING (CONTROL_POINT_RADIUS + 1.0f)

void TFHistogramWidget::paintEvent(QPaintEvent* event)
{
    QFrame::paintEvent(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    
    p.fillRect(rect(), Qt::gray);
    
//    QMatrix m;
//    m.translate(PADDING, height());
//    m.scale((width()-PADDING*2)/(float)_histo.getNumBins(), -1);
//    p.setMatrix(m);
    
    QPolygonF graph;
    graph.push_back(NDCToQPixel(0,0));
    
    int nBins = _histo.getNumBins();
    for (int i = 0; i < nBins; i++) {
        float bin = _histo.getBinSizeNormalized(i);
        graph.push_back(NDCToQPixel(i/(float)nBins, bin));
        graph.push_back(NDCToQPixel(i/(float)nBins, bin));
    }
    
    graph.push_back(NDCToQPixel(1,0));
    
    p.setBrush(QBrush(Qt::black));
    p.drawPolygon(graph);
}

void TFHistogramWidget::mousePressEvent(QMouseEvent *event)
{
    emit Activated(this);
    emit UpdateInfo(PixelToNDC(event->pos()).x);
}

void TFHistogramWidget::mouseReleaseEvent(QMouseEvent *event)
{
//    emit InfoDeselected();
}

void TFHistogramWidget::mouseMoveEvent(QMouseEvent *event)
{
    emit UpdateInfo(PixelToNDC(event->pos()).x);
}

//void TFHistogramWidget::mouseDoubleClickEvent(QMouseEvent *event)

glm::vec2 TFHistogramWidget::NDCToPixel(const glm::vec2 &v) const
{
    return vec2(PADDING + v.x * (width()-2*PADDING), PADDING + (1.0f - v.y) * (height()-2*PADDING));
}

QPointF TFHistogramWidget::NDCToQPixel(const glm::vec2 &v) const
{
    const vec2 p = NDCToPixel(v);
    return QPointF(p.x, p.y);
}

QPointF TFHistogramWidget::NDCToQPixel(float x, float y) const
{
    return NDCToQPixel(vec2(x, y));
}

glm::vec2 TFHistogramWidget::PixelToNDC(const glm::vec2 &p) const
{
    float width = QWidget::width();
    float height = QWidget::height();
    VAssert(width != 0 && height != 0);
    
    return vec2((p.x - PADDING) / (width-2*PADDING), 1.0f - (p.y - PADDING) / (height-2*PADDING));
}

glm::vec2 TFHistogramWidget::PixelToNDC(const QPointF &p) const
{
    return PixelToNDC(vec2(p.x(), p.y()));
}