/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     zccrs <zccrs@live.com>
 *
 * Maintainer: zccrs <zhangjide@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "chameleonstyle.h"
#include "common.h"

#include <DNativeSettings>
#include <DStyleOption>
#include <DApplication>
#include <DPlatformWindowHandle>

#include <QVariant>
#include <QDebug>
#include <QApplication>
#include <QPushButton>
#include <QComboBox>
#include <QScrollBar>
#include <QCheckBox>
#include <QRadioButton>
#include <QToolButton>
#include <QLineEdit>
#include <QAction>
#include <QMenu>
#include <QPainter>
#include <QPaintEngine>
#include <QAbstractItemView>

#include <qpa/qplatformwindow.h>

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

namespace chameleon {

static QColor light_qpalette[QPalette::NColorRoles] {
    QColor("#414d68"),                  //WindowText
    QColor("#e5e5e5"),                  //Button
    QColor("#e6e6e6"),                  //Light
    QColor("#e5e5e5"),                  //Midlight
    QColor("#e3e3e3"),                  //Dark
    QColor("#e4e4e4"),                  //Mid
    QColor("#414d68"),                  //Text
    QColor("#3768ff"),                  //BrightText
    QColor("#414d68"),                  //ButtonText
    Qt::white,                          //Base
    QColor("#f8f8f8"),                  //Window
    QColor(0, 0, 0, 0.7 * 255),         //Shadow
    QColor("#0081ff"),                  //Highlight
    Qt::white,                          //HighlightedText
    QColor("#0082fa"),                  //Link
    QColor("#ad4579"),                  //LinkVisited
    QColor(0, 0, 0, 0.03 * 255),        //AlternateBase
    Qt::white,                          //NoRole
    QColor(255, 255, 255, 0.8 * 255),   //ToolTipBase
    Qt::black                           //ToolTipText
};

static QColor dark_qpalette[QPalette::NColorRoles] {
    QColor("#c0c6d4"),                  //WindowText
    QColor("#444444"),                  //Button
    QColor("#484848"),                  //Light
    QColor("#474747"),                  //Midlight
    QColor("#414141"),                  //Dark
    QColor("#434343"),                  //Mid
    QColor("#c0c6d4"),                  //Text
    QColor("#3768ff"),                  //BrightText
    QColor("#c0c6d4"),                  //ButtonText
    QColor("#454545"),                  //Base
    QColor("#252525"),                  //Window
    QColor(0, 0, 0, 0.05 * 255),        //Shadow
    QColor("#0081ff"),                  //Highlight
    QColor("#b8d3ff"),                  //HighlightedText
    QColor("#0082fa"),                  //Link
    QColor("#ad4579"),                  //LinkVisited
    QColor(0, 0, 0, 0.05 * 255),        //AlternateBase
    Qt::black,                          //NoRole
    QColor(45, 45, 45, 0.8 * 255),      //ToolTipBase
    QColor("#c0c6d4")                   //ToolTipText
};

static QColor light_dpalette[DPalette::NColorTypes] {
    QColor(0, 0, 0, 255 * 0.03),    //ItemBackground
    QColor("#001A2E"),              //TextTitle
    QColor("#526A7F"),              //TextTips
    QColor("#FF5736"),              //TextWarning
    QColor("#0082FA"),              //TextLively
    QColor("#25b7ff"),              //LightLively
    QColor("#0098ff"),              //DarkLively
    QColor(0, 0, 0, 0.03 * 255)     //FrameBorder
};

static QColor dark_dpalette[DPalette::NColorTypes] {
    QColor(255, 255, 255, 255 * 0.05),  //ItemBackground
    QColor("#C0C6D4"),                  //TextTitle
    QColor("#6D7C88"),                  //TextTips
    QColor("#FF5736"),                  //TextWarning
    QColor("#0082FA"),                  //TextLively
    QColor("#0056c1"),                  //LightLively
    QColor("#004c9c"),                  //DarkLively
    QColor(0, 0, 0, 0.05 * 255)         //FrameBorder
};

static void initDisablePalette(QPalette &pa)
{
    for (int i = 0; i < QPalette::NColorRoles; ++i) {
        QPalette::ColorRole role = static_cast<QPalette::ColorRole>(i);

        if (role == QPalette::Window) {
            continue;
        }

        QColor color = pa.color(QPalette::Normal, role);

        color.setAlpha(color.alpha() * 0.6);
        pa.setColor(QPalette::Disabled, role, color);
    }
}

static void initInactivePalette(QPalette &pa)
{
    for (int i = 0; i < QPalette::NColorRoles; ++i) {
        QPalette::ColorRole role = static_cast<QPalette::ColorRole>(i);

        if (role == QPalette::Window) {
            continue;
        }

        QColor color = pa.color(QPalette::Normal, role);

        color.setAlpha(color.alpha() * 0.4);
        pa.setColor(QPalette::Inactive, role, color);
    }
}

static void initDisablePalette(DPalette &pa)
{
    for (int i = 0; i < DPalette::NColorTypes; ++i) {
        DPalette::ColorType role = static_cast<DPalette::ColorType>(i);
        QColor color = pa.color(DPalette::Normal, role);

        color.setAlpha(color.alpha() * 0.6);
        pa.setColor(QPalette::Disabled, role, color);
    }
}

static void initInactivePalette(DPalette &pa)
{
    for (int i = 0; i < DPalette::NColorTypes; ++i) {
        DPalette::ColorType role = static_cast<DPalette::ColorType>(i);
        QColor color = pa.color(DPalette::Normal, role);

        color.setAlpha(color.alpha() * 0.4);
        pa.setColor(QPalette::Inactive, role, color);
    }
}

ChameleonStyle::ChameleonStyle()
    : DStyle()
{

}

void ChameleonStyle::drawPrimitive(QStyle::PrimitiveElement pe, const QStyleOption *opt,
                                   QPainter *p, const QWidget *w) const
{
    switch (static_cast<int>(pe)) {
    case PE_PanelButtonCommand: {
        const QMargins &margins = frameExtentMargins();

        drawShadow(p, opt->rect + margins, getColor(opt, QPalette::Shadow));
        // 初始化button的渐变背景色
        QLinearGradient lg(QPointF(0, opt->rect.top()),
                           QPointF(0, opt->rect.bottom()));
        lg.setColorAt(0, getColor(opt, QPalette::Light));
        lg.setColorAt(1, getColor(opt, QPalette::Dark));

        p->setPen(QPen(getColor(opt, DPalette::FrameBorder, w), Metrics::Painter_PenWidth));
        p->setBrush(lg);
        p->setRenderHint(QPainter::Antialiasing);

        int frame_radius = DStyle::pixelMetric(PM_FrameRadius, opt, w);

        p->drawRoundedRect(opt->rect - margins, frame_radius, frame_radius);

        return;
    }
    case PE_FrameFocusRect: {
        drawBorder(p, opt->rect, getColor(opt, QPalette::Highlight));
        return;
    }
    case PE_PanelItemViewItem: {
        if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(opt)) {
            bool valid_type = false;
            Dtk::ItemBackgroundType type = static_cast<Dtk::ItemBackgroundType>(vopt->index.data(Dtk::BackgroundTypeRole).toInt(&valid_type));

            int frame_radius = DStyle::pixelMetric(PM_FrameRadius, opt, w);

            if (valid_type && Q_LIKELY(type <= Dtk::RoundedBackground)) {
                if (type == Dtk::ClipCornerBackground) {
                    QPainterPath path;
                    path.addRoundedRect(w->rect(), frame_radius, frame_radius);
                    p->setClipPath(path);
                }

                if (!vopt->showDecorationSelected || !vopt->state.testFlag(QStyle::State_Selected)) {
                    DStyleOptionBackgroundGroup option;
                    option.state = vopt->state;
                    option.position = DStyleOptionBackgroundGroup::ItemBackgroundPosition(vopt->viewItemPosition);
                    option.dpalette = DPalette::get(w);

                    if (type != Dtk::RoundedBackground) {
                        option.directions = Qt::Vertical;
                        option.rect = vopt->rect;
                    } else {
                        option.rect = vopt->rect - frameExtentMargins();
                    }

                    DStyle::drawPrimitive(PE_ItemBackground, &option, p, w);
                }
            }

            if (vopt->state & QStyle::State_Selected) {
                QRect select_rect = opt->rect;

                if (!vopt->showDecorationSelected) {
                    select_rect = proxy()->subElementRect(QStyle::SE_ItemViewItemText,  opt, w);
                }

                p->setPen(Qt::NoPen);
                p->setBrush(getColor(opt, QPalette::Highlight));
                p->setRenderHint(QPainter::Antialiasing);
                p->drawRoundedRect(select_rect, frame_radius, frame_radius);
            }

            return;
        }
        break;
    }
    case PE_PanelLineEdit: {
        p->setBrush(getColor(opt, QPalette::Button));
        p->setPen(Qt::NoPen);
        int frame_radius = DStyle::pixelMetric(PM_FrameRadius, opt, w);
        p->drawRoundedRect(opt->rect - frameExtentMargins(), frame_radius, frame_radius);

        if (opt->state.testFlag(QStyle::State_HasFocus)) {
            proxy()->drawPrimitive(PE_FrameFocusRect, opt, p, w);
        }

        return;
    }
    case PE_IndicatorRadioButton: {
        QRect standard = opt->rect;

        p->setRenderHint(QPainter::Antialiasing, true);

        if (opt->state & State_On) {  //Qt::Checked
            double padding = standard.width() / 2.0 / 2.0;
            QPainterPath path;

            path.addEllipse(standard);
            path.addEllipse(standard.adjusted(padding, padding, -padding, -padding));

            p->fillPath(path, getColor(opt, DPalette::Highlight));
        } else if (opt->state & State_Off) {
            p->setPen(QPen(getColor(opt, DPalette::WindowText), 1));
            p->drawEllipse(standard.adjusted(1, 1, -1, -1));
        }

        return;
    }
    case PE_IndicatorCheckBox: {
        QRectF standard = opt->rect;
        DDrawUtils::drawBorder(p, standard, getColor(opt, DPalette::WindowText), 1, 2);

        if (opt->state & State_NoChange) {  //Qt::PartiallyChecked
            QRectF lineRect(0, 0, standard.width() / 2.0, 2);
            lineRect.moveCenter(standard.center());
            p->fillRect(lineRect, getColor(opt, DPalette::TextTitle, w));
        } else if (opt->state & State_On) {  //Qt::Checked
            p->setPen(Qt::NoPen);
            p->setBrush(getColor(opt, DPalette::Highlight));
            p->drawRoundedRect(standard.adjusted(1, 1, -1, -1), 1, 2);

            DDrawUtils::drawMark(p, standard.adjusted(2, 0, 0, -2), getColor(opt, DPalette::Window), getColor(opt, DPalette::Highlight), 2);
        }

        return;
    }
    default:
        break;
    }

    DStyle::drawPrimitive(pe, opt, p, w);
}

void ChameleonStyle::drawControl(QStyle::ControlElement element, const QStyleOption *opt,
                                 QPainter *p, const QWidget *w) const
{
    switch (element) {
    case CE_RadioButton:
    case CE_CheckBox:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            bool isRadio = (element == CE_RadioButton);
            QStyleOptionButton subopt = *btn;
            subopt.rect = subElementRect(isRadio ? SE_RadioButtonIndicator : SE_CheckBoxIndicator, btn, w);
            proxy()->drawPrimitive(isRadio ? PE_IndicatorRadioButton : PE_IndicatorCheckBox, &subopt, p, w);

            subopt.rect = subElementRect(SE_CheckBoxContents, btn, w);
            proxy()->drawControl(CE_CheckBoxLabel, &subopt, p, w);

            if (btn->state & State_HasFocus) {
                QRect rect(subElementRect(isRadio ? SE_RadioButtonFocusRect : SE_CheckBoxFocusRect, btn, w));

                if (isRadio) {
                    p->setPen(QPen(getColor(opt, DPalette::Highlight), DStyle::pixelMetric(PM_FocusBorderWidth)));
                    p->drawEllipse(rect.adjusted(1, 1, -1, -1));
                } else {
                    DDrawUtils::drawBorder(p, rect, getColor(opt, DPalette::Highlight),
                                          DStyle::pixelMetric(PM_FocusBorderWidth), DStyle::pixelMetric(PM_FocusBorderSpacing) + 2);
                }
            }

        }
        return;
    case CE_ScrollBarSlider: {
        p->setBrush(getColor(opt, QPalette::Highlight));
        p->setPen(Qt::NoPen);
        p->setRenderHint(QPainter::Antialiasing);
        QRectF rect = opt->rect;

        if (opt->state & QStyle::State_Horizontal) {
            p->drawRoundedRect(rect, rect.height() / 2.0, rect.height() / 2.0);

            QRectF rectArrow(0, 0, rect.height() / 2, rect.height() / 2);
            rectArrow.moveCenter(rect.center());
            rectArrow.moveLeft(rect.left() + rect.height() / 2);
            DDrawUtils::drawArrow(p, rectArrow, getColor(opt, QPalette::HighlightedText), Qt::LeftArrow);
            rectArrow.moveCenter(rect.center());
            rectArrow.moveRight(rect.right() - rect.height() / 2);
            DDrawUtils::drawArrow(p, rectArrow, getColor(opt, QPalette::HighlightedText), Qt::RightArrow);
        } else {
            p->drawRoundedRect(rect, rect.width() / 2.0, rect.width() / 2.0);

            QRectF rectArrow(0, 0, rect.width() / 2.0, rect.width() / 2.0);
            rectArrow.moveCenter(rect.center());
            rectArrow.moveTop(rect.top() + rect.width() / 2.0);
            DDrawUtils::drawArrow(p, rectArrow, getColor(opt, QPalette::HighlightedText), Qt::UpArrow);
            rectArrow.moveCenter(rect.center());
            rectArrow.moveBottom(rect.bottom() - rect.width() / 2);
            DDrawUtils::drawArrow(p, rectArrow, getColor(opt, QPalette::HighlightedText), Qt::DownArrow);
        }
        break;
    }
    case CE_MenuBarItem: {
        if (const QStyleOptionMenuItem *mbi = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
            p->save();
            QRect rect = mbi->rect;
            drawMenuBarItem(mbi, rect, p, w);
            p->restore();
            return ;
        }
    }
    break;
    case CE_MenuBarEmptyArea: {
        p->save();
        QRect menubarRect = opt->rect - frameExtentMargins();
        p->setPen(Qt::NoPen);
        p->setBrush(getColor(opt, QPalette::Window));
        p->drawRect(menubarRect);
        p->restore();
        return;
    }
    default:
        break;
    }

    DStyle::drawControl(element, opt, p, w);
}

bool ChameleonStyle::drawMenuBarItem(const QStyleOptionMenuItem *option, QRect &rect, QPainter *painter, const QWidget *widget) const
{
    bool enabled(option->state & QStyle::State_Enabled);
    bool mouseOver((option->state & QStyle::State_MouseOver) && enabled);
    bool sunken((option->state & QStyle::State_Sunken) && enabled);

    if (mouseOver || sunken) {
        QBrush background(getColor(option, QPalette::Highlight));
        qreal radius = DStyle::pixelMetric(DStyle::PM_FrameRadius);

        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(background);
        painter->drawRoundedRect(rect, radius, radius);
    }

    if (option) {
        int alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;

        if (!proxy()->styleHint(SH_UnderlineShortcut, option, widget))
            alignment |= Qt::TextHideMnemonic;

        int iconExtent = proxy()->pixelMetric(PM_SmallIconSize);
        QPixmap pix = option->icon.pixmap(widget ? widget->window()->windowHandle() : nullptr, QSize(iconExtent, iconExtent), (enabled) ? (mouseOver ? QIcon::Active : QIcon::Normal) : QIcon::Disabled);

        if (!pix.isNull()) {
            proxy()->drawItemPixmap(painter, option->rect, alignment, pix);
        } else {
            QStyleOptionMenuItem itemOption = *option;

            if (sunken)
                itemOption.palette.setBrush(QPalette::ButtonText, itemOption.palette.highlightedText());

            proxy()->drawItemText(painter, itemOption.rect, alignment, itemOption.palette, enabled,
                                  itemOption.text, QPalette::ButtonText);
        }
    }

    return true;
}

QRect ChameleonStyle::subElementRect(QStyle::SubElement r, const QStyleOption *opt,
                                     const QWidget *widget) const
{
    switch (r) {
    case SE_PushButtonFocusRect:
    case SE_ItemViewItemFocusRect:
        return opt->rect;
    case SE_ItemViewItemCheckIndicator: Q_FALLTHROUGH();
    case SE_ItemViewItemDecoration: Q_FALLTHROUGH();
    case SE_ItemViewItemText:
        if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(opt)) {
            int frame_radius = DStyle::pixelMetric(PM_FrameRadius, opt, widget);
            QStyleOptionViewItem option(*vopt);
            option.rect = opt->rect.adjusted(frame_radius, 0, -frame_radius, 0);

            if (vopt->features & DStyleOptionViewItem::UseDStyleLayout) {
                QRect checkRect, decorationRect, displayRect;
                viewItemLayout(&option, &decorationRect, &displayRect, &checkRect, false);

                switch (r) {
                case SE_ItemViewItemCheckIndicator:
                    return checkRect;
                case SE_ItemViewItemDecoration:
                    return decorationRect;
                case SE_ItemViewItemText:
                    return displayRect;
                default:
                    break;
                }
            } else {
                // 默认把checkbox放置在右边，因此使用QCommonStyle的Item布局时先移除HasCheckIndicator标志
                option.features &= ~QStyleOptionViewItem::HasCheckIndicator;

                if (r == SE_ItemViewItemDecoration) {
                    return DStyle::subElementRect(r, &option, widget);
                }

                QRect text_rect = DStyle::subElementRect(SE_ItemViewItemText, &option, widget);
                int indicator_width = proxy()->pixelMetric(PM_IndicatorWidth, &option, widget);
                int indicator_height = proxy()->pixelMetric(PM_IndicatorHeight, &option, widget);

                const QRect indicator_rect = alignedRect(opt->direction, Qt::AlignRight | Qt::AlignVCenter,
                                                         QSize(indicator_width, indicator_height), text_rect);

                int margin = proxy()->pixelMetric(QStyle::PM_FocusFrameHMargin, opt, widget);
                text_rect.setRight(qMin(text_rect.right(), indicator_rect.left() - margin));

                return r == SE_ItemViewItemText ? text_rect : indicator_rect;
            }
        }
        break;
    case SE_LineEditContents: {
        int frame_margins = DStyle::pixelMetric(PM_FrameMargins, opt, widget);
        return opt->rect.adjusted(frame_margins * 2, 0, -frame_margins * 2, 0);
    }
    case SE_RadioButtonFocusRect:
    case SE_CheckBoxFocusRect: {
        QRect re;
        re = subElementRect(SE_CheckBoxIndicator, opt, widget);
        int margin = DStyle::pixelMetric(PM_FocusBorderWidth) + DStyle::pixelMetric(PM_FocusBorderSpacing);
        re.adjust(-margin, -margin, margin, margin);
        return re;
    }
    case SE_RadioButtonIndicator:
    case SE_RadioButtonContents:
    case SE_RadioButtonClickRect:
    case SE_CheckBoxContents:
    case SE_CheckBoxClickRect:
    case SE_CheckBoxIndicator:
        if (const QStyleOptionButton *vopt = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            QStyleOptionButton option(*vopt);
            option.rect = opt->rect.marginsRemoved(frameExtentMargins());
            return DStyle::subElementRect(r, &option, widget);
        }
        break;
    case SE_PushButtonContents:
        if (const QStyleOptionButton *vopt = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            QRect buttonContentRect = vopt->rect;
            int buttonIconMargin = proxy()->pixelMetric(QStyle::PM_ButtonMargin, opt, widget) ;
            buttonContentRect.adjust(buttonIconMargin, buttonIconMargin, -buttonIconMargin, -buttonIconMargin);
            return buttonContentRect.marginsRemoved(frameExtentMargins());
        }
        break;
    default:
        break;
    }

    return DStyle::subElementRect(r, opt, widget);
}

void ChameleonStyle::drawComplexControl(QStyle::ComplexControl cc, const QStyleOptionComplex *opt,
                                        QPainter *p, const QWidget *w) const
{
    switch (cc) {
    case CC_ScrollBar: {
        p->setBrush(getColor(opt, QPalette::Button));
        p->setPen(Qt::NoPen);
        QRectF rect = opt->rect;

        if (opt->state & QStyle::State_Horizontal) {
            rect.setHeight(rect.height() / 4);
            rect.moveCenter(QRectF(opt->rect).center());
            p->drawRoundedRect(rect, rect.height() / 2.0, rect.height() / 2.0);
        } else {
            rect.setWidth(rect.width() / 4);
            rect.moveCenter(QRectF(opt->rect).center());
            p->drawRoundedRect(rect, rect.width() / 2.0, rect.width() / 2.0);
        }
    }
    break;
#if QT_CONFIG(spinbox)
    case CC_SpinBox:
        if (const QStyleOptionSpinBox *option = qstyleoption_cast<const QStyleOptionSpinBox *>(opt))
            if (drawSpinBox(option, p, w))
                return;
        break;
#endif
    case CC_Slider : {
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            //各个使用的矩形大小和位置
            QRectF rect = opt->rect;                                                                            //Slider控件最大的矩形(包含如下三个)
            QRectF rectHandle = proxy()->subControlRect(CC_Slider, opt, SC_SliderHandle, w);                    //滑块矩形
            QRectF rectSliderTickmarks = proxy()->subControlRect(CC_Slider, opt, SC_SliderTickmarks, w);        //刻度的矩形
            QRect rectGroove = proxy()->subControlRect(CC_Slider, opt, SC_SliderGroove, w);                     //滑槽的矩形

//            //测试(保留不删)
//            p->fillRect(rect, Qt::gray);
//            p->fillRect(rectSliderTickmarks, Qt::blue);
//            p->fillRect(rectGroove, Qt::red);
//            p->fillRect(rectHandle, Qt::green);
//            qDebug()<<"---rect:"<<rect<<"  rectHandle:"<<rectHandle<<"   rectSliderTickmarks:"<<rectSliderTickmarks<<"   rectGroove:"<<rectGroove;

            //预期准备工作
            float sliCentX = (slider->sliderValue * 1.0 / (slider->maximum - slider->minimum)) * rect.width();  //为绘画滑槽(线)的计算准备
            QPen pen;

            //绘画 滑槽(线)
            if (opt->subControls & SC_SliderGroove) {
                pen.setStyle(Qt::DotLine);
                pen.setWidthF(3);
                pen.setColor(getColor(opt, QPalette::Highlight));
                p->setPen(pen);
                p->setRenderHint(QPainter::Antialiasing);
                p->drawLine(QPointF(rectGroove.left(), rectHandle.top() + rectHandle.height() / 2.0), QPointF(sliCentX, rectHandle.top() + rectHandle.height() / 2.0));
                pen.setColor(getColor(opt, QPalette::Foreground));
                p->setPen(pen);
                p->drawLine(QPointF(rectGroove.right(), rectHandle.top() + rectHandle.height() / 2.0), QPointF(sliCentX, rectHandle.top() + rectHandle.height() / 2.0));
            }

            //绘画 滑块
            if (opt->subControls & SC_SliderHandle) {
                pen.setStyle(Qt::SolidLine);
                p->setPen(Qt::NoPen);
                p->setBrush(getColor(opt, QPalette::Highlight));
                p->drawRoundedRect(rectHandle, DStyle::pixelMetric(DStyle::PM_FrameRadius), DStyle::pixelMetric(DStyle::PM_FrameRadius));
            }

            //绘画 刻度,绘画方式了参考qfusionstyle.cpp
            if (opt->subControls & SC_SliderTickmarks) {     //需要绘画刻度
                p->setPen(opt->palette.foreground().color());
                int tickSize = proxy()->pixelMetric(PM_SliderTickmarkOffset, opt, w);
                int available = proxy()->pixelMetric(PM_SliderSpaceAvailable, slider, w);
                int interval = slider->tickInterval;
                int ticks = slider->tickPosition;
                if (interval <= 0) {
                    interval = slider->singleStep;
                    if (QStyle::sliderPositionFromValue(slider->minimum, slider->maximum, interval,
                                                        available)
                            - QStyle::sliderPositionFromValue(slider->minimum, slider->maximum,
                                                              0, available) < 3)
                        interval = slider->pageStep;
                }
                if (interval <= 0)
                    interval = 1;

                int v = slider->minimum;
                int len = proxy()->pixelMetric(PM_SliderLength, slider, w);
                while (v <= slider->maximum + 1) {
                    if (v == slider->maximum + 1 && interval == 1)
                        break;
                    const int v_ = qMin(v, slider->maximum);
                    int pos = sliderPositionFromValue(slider->minimum, slider->maximum, v_, available) + len / 2;
                    int extra = 2 - ((v_ == slider->minimum || v_ == slider->maximum) ? 1 : 0);

                    if (slider->orientation == Qt::Horizontal) {
                        if (ticks & QSlider::TicksAbove) {
                            p->drawLine(pos, slider->rect.top() + extra,
                                              pos, slider->rect.top() + tickSize);
                        }
                        if (ticks & QSlider::TicksBelow) {
                            p->drawLine(pos, slider->rect.bottom() - extra,
                                              pos, slider->rect.bottom() - tickSize);
                        }
                    } else {
                        if (ticks & QSlider::TicksAbove) {
                            p->drawLine(slider->rect.left() + extra, pos,
                                              slider->rect.left() + tickSize, pos);
                        }
                        if (ticks & QSlider::TicksBelow) {
                            p->drawLine(slider->rect.right() - extra, pos,
                                              slider->rect.right() - tickSize, pos);
                        }
                    }
                    // in the case where maximum is max int
                    int nextInterval = v + interval;
                    if (nextInterval < v)
                        break;
                    v = nextInterval;
                }
            }

        }
        break;
    }
    default:
        break;
    }

    DStyle::drawComplexControl(cc, opt, p, w);
}

bool ChameleonStyle::drawSpinBox(const QStyleOptionSpinBox *opt,
                                 QPainter *painter, const QWidget *widget) const
{
    if (opt->frame && (opt->subControls & SC_SpinBoxFrame)) {
        qreal borderRadius = DStyle::pixelMetric(DStyle::PM_FrameRadius);
        QRect frameRect = proxy()->subControlRect(CC_SpinBox, opt, SC_SpinBoxFrame, widget);
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(Qt::NoPen);
        painter->setBrush(getColor(opt, QPalette::Button));
        painter->drawRoundedRect(frameRect - frameExtentMargins(), borderRadius, borderRadius);
    }

    if (opt->subControls & SC_SpinBoxUp) {
        bool upIsActive = opt->activeSubControls == SC_SpinBoxUp;
        bool upIsEnabled = opt->stepEnabled & QAbstractSpinBox::StepUpEnabled && opt->state.testFlag(State_Enabled);
        QRect subRect = proxy()->subControlRect(CC_SpinBox, opt, SC_SpinBoxUp, widget);
        QStyleOptionButton buttonOpt;
        buttonOpt.rect = subRect;
        updateSpinBoxButtonState(opt, buttonOpt, upIsActive, upIsEnabled);
        proxy()->drawPrimitive(PE_PanelButtonCommand, &buttonOpt, painter, widget);

        if (opt->buttonSymbols & QAbstractSpinBox::PlusMinus) {
            QRectF plusRect = proxy()->subElementRect(SE_PushButtonContents, &buttonOpt, widget);
            qreal lineWidth = qMax(2.0, static_cast<qreal>(Metrics::SpinBox_ButtonIconWidth));
            DDrawUtils::drawPlus(painter, plusRect, getColor(opt, QPalette::ButtonText), lineWidth);
        } else {
            QRect arrowRect = proxy()->subElementRect(SE_PushButtonContents, &buttonOpt, widget);
            DDrawUtils::drawArrow(painter, arrowRect, getColor(opt, QPalette::ButtonText), Qt::ArrowType::UpArrow);
        }
    }

    if (opt->subControls & SC_SpinBoxDown) {
        bool downIsActive = opt->activeSubControls == SC_SpinBoxDown;
        bool downIsEnabled = opt->stepEnabled & QAbstractSpinBox::StepDownEnabled && opt->state.testFlag(State_Enabled);
        QRect subRect = proxy()->subControlRect(CC_SpinBox, opt, SC_SpinBoxDown, widget);
        QStyleOptionButton buttonOpt;
        buttonOpt.rect = subRect;
        updateSpinBoxButtonState(opt, buttonOpt, downIsActive, downIsEnabled);
        proxy()->drawPrimitive(PE_PanelButtonCommand, &buttonOpt, painter, widget);

        if (opt->buttonSymbols & QAbstractSpinBox::PlusMinus) {
            QRectF subtractRect = proxy()->subElementRect(SE_PushButtonContents, &buttonOpt, widget);
            qreal lineWidth = qMax(2.0, static_cast<qreal>(Metrics::SpinBox_ButtonIconWidth));
            DDrawUtils::drawSubtract(painter, subtractRect, getColor(opt, QPalette::ButtonText), lineWidth);
        } else {
            QRect arrowRect = proxy()->subElementRect(SE_PushButtonContents, &buttonOpt, widget);
            DDrawUtils::drawArrow(painter, arrowRect, getColor(opt, QPalette::ButtonText), Qt::ArrowType::DownArrow);
        }
    }

    return true;
}

void ChameleonStyle::updateSpinBoxButtonState(const QStyleOptionSpinBox *opt, QStyleOptionButton &buttonOpt, bool isActive, bool isEnabled) const
{
    State buttonState = opt->state;

    if (!isActive) {
        buttonState &= ~State_MouseOver;
        buttonState &= ~State_Sunken;
    }

    if (!isEnabled) {
        buttonState &= ~State_Enabled;
        buttonState &= ~State_MouseOver;
        buttonState &= ~State_Sunken;
    }

    buttonOpt.state = buttonState;
}

QStyle::SubControl ChameleonStyle::hitTestComplexControl(QStyle::ComplexControl cc, const QStyleOptionComplex *opt,
                                                         const QPoint &pt, const QWidget *w) const
{
    return DStyle::hitTestComplexControl(cc, opt, pt, w);
}

QRect ChameleonStyle::subControlRect(QStyle::ComplexControl cc, const QStyleOptionComplex *opt,
                                     QStyle::SubControl sc, const QWidget *w) const
{
    switch (cc) {
    case CC_SpinBox: {
        if (const QStyleOptionSpinBox *option = qstyleoption_cast<const QStyleOptionSpinBox *>(opt)) {

            if (option->buttonSymbols & QAbstractSpinBox::PlusMinus) {
                switch (sc) {
                case SC_SpinBoxEditField: {
                    QRect spinboxRect = option->rect;
                    QRect dButtonRect = proxy()->subControlRect(CC_SpinBox, opt, SC_SpinBoxDown, w);
                    QRect uButtonRect = proxy()->subControlRect(CC_SpinBox, opt, SC_SpinBoxUp, w);
                    spinboxRect.setLeft(dButtonRect.right());
                    spinboxRect.setRight(uButtonRect.left());
                    return spinboxRect;
                }
                case SC_SpinBoxUp: {
                    QRect buttonRect(option->rect.topLeft(), QSize(option->rect.height(), option->rect.height()));
                    buttonRect.moveRight(option->rect.right());
                    return buttonRect.marginsRemoved(frameExtentMargins());
                }
                case SC_SpinBoxDown: {
                    QRect buttonRect(option->rect.topLeft(), QSize(option->rect.height(), option->rect.height()));
                    buttonRect.moveLeft(option->rect.left());
                    return buttonRect.marginsRemoved(frameExtentMargins());
                }
                default:
                    break;
                }
            }
        }
    }
    case CC_Slider: {
        if (const QStyleOptionSlider *option = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            QRectF rect = option->rect;                                                    //Slider控件总的大小矩形
            int slider_size = proxy()->pixelMetric(PM_SliderControlThickness, opt, w);     //滑块的高度
            int tick_size = proxy()->pixelMetric(PM_SliderTickmarkOffset, opt, w);         //刻度的高度
            QRectF slider_handle_rect = rect;                                              //滑块和滑漕的的最小公共矩形 (后面被用作临时且被改变的)

            if (option->orientation == Qt::Horizontal) {
                slider_handle_rect.setHeight(slider_size);
                if (option->tickPosition == QSlider::TicksAbove) slider_handle_rect.moveBottom(rect.bottom());
                if (option->tickPosition == QSlider::TicksBelow) slider_handle_rect.moveTop(rect.top());
            } else {
                slider_handle_rect.setWidth(slider_size);
                if (option->tickPosition == QSlider::TicksRight)  slider_handle_rect.moveLeft(rect.left());
                if (option->tickPosition == QSlider::TicksLeft)   slider_handle_rect.moveRight(rect.right());
            }

            QRectF rectStatic =  slider_handle_rect;   //rectStatic作为 滑块和滑漕的的最小公共矩形(不改变)

            switch (sc) {
            case SC_SliderGroove: {  //滑漕
                qreal groove_size = slider_size / 4.0;
                QRectF groove_rect;

                if (option->orientation == Qt::Horizontal) {
                    groove_rect.setWidth(slider_handle_rect.width());
                    groove_rect.setHeight(groove_size);
                } else {
                    groove_rect.setWidth(groove_size);
                    groove_rect.setHeight(slider_handle_rect.height());
                }

                groove_rect.moveCenter(slider_handle_rect.center());
                return groove_rect.toRect();
            }
            case SC_SliderHandle: {  //滑块
                int sliderPos = 0;
                int len = proxy()->pixelMetric(PM_SliderLength, option, w);
                bool horizontal = option->orientation == Qt::Horizontal;
                sliderPos = sliderPositionFromValue(option->minimum, option->maximum, option->sliderPosition,
                                                    (horizontal ? slider_handle_rect.width() : slider_handle_rect.height()) - len, option->upsideDown);
                if (horizontal) {
                    slider_handle_rect.moveLeft(slider_handle_rect.left() + sliderPos);
                    slider_handle_rect.setWidth(len);
                    slider_handle_rect.moveTop(rectStatic.top());
                } else {
                    slider_handle_rect.moveTop(slider_handle_rect.top() + sliderPos);
                    slider_handle_rect.setHeight(len);
                    slider_handle_rect.moveLeft(rectStatic.left());
                }

                return slider_handle_rect.toRect();
            }
            case SC_SliderTickmarks: {  //刻度的矩形
                QRectF tick_rect = rect;

                if (option->orientation == Qt::Horizontal) {
                    tick_rect.setHeight(rect.height() - slider_handle_rect.height());

                    if (option->tickPosition == QSlider::TicksAbove) {
                        tick_rect.moveTop(rect.top());
                    } else if (option->tickPosition == QSlider::TicksBelow){
                        tick_rect.moveBottom(rect.bottom());
                    }
                } else {
                    tick_rect.setWidth(rect.width() - slider_handle_rect.width());

                    if (option->tickPosition == QSlider::TicksLeft) {
                        tick_rect.moveLeft(rect.left());
                    } else if (option->tickPosition == QSlider::TicksRight) {
                        tick_rect.moveRight(rect.right());
                    }
                }

                return tick_rect.toRect();
            }
            default:
                break;
            }
        }
    }
    break;
    default:
        break;
    }
    return DStyle::subControlRect(cc, opt, sc, w);
}

QSize ChameleonStyle::sizeFromContents(QStyle::ContentsType ct, const QStyleOption *opt,
                                       const QSize &contentsSize, const QWidget *widget) const
{
    QSize size = DStyle::sizeFromContents(ct, opt, contentsSize, widget);

    switch (ct) {
    case CT_LineEdit: {
        int button_margin = proxy()->pixelMetric(QStyle::PM_ButtonMargin, opt, widget);
        size += QSize(button_margin, button_margin);
        Q_FALLTHROUGH();
    }
    case CT_PushButton: {
        int frame_margins = DStyle::pixelMetric(PM_FrameMargins, opt, widget);
        size += QSize(frame_margins * 2, frame_margins * 2);
        break;
    }
    case CT_ItemViewItem: {
        if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(opt)) {
            const QMargins &item_margins = qvariant_cast<QMargins>(vopt->index.data(Dtk::MarginsRole));

            if (!item_margins.isNull()) {
                //加上Item自定义的margins
                size = QRect(QPoint(0, 0), size).marginsAdded(item_margins).size();
            }

            bool valid_type = false;
            Dtk::ItemBackgroundType type = static_cast<Dtk::ItemBackgroundType>(vopt->index.data(Dtk::BackgroundTypeRole).toInt(&valid_type));

            if (!valid_type || type != Dtk::RoundedBackground) {
                return size;
            }

            int frame_margins = DStyle::pixelMetric(PM_FrameMargins, opt, widget);
            size += QSize(frame_margins * 2, frame_margins * 2);
        }
        break;
    }
    case CT_SpinBox: {
        int frame_margins = DStyle::pixelMetric(PM_FrameMargins, opt, widget);
        size += QSize(frame_margins * 2, frame_margins * 2);
        break;
    }
    case CT_Slider: {
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            int height = proxy()->pixelMetric(PM_SliderControlThickness, opt, widget) + Metrics::Slider_TickmarkOffset;
            size.setHeight(qMax(size.height(), height));
            break;
        }
    }
    case CT_MenuBarItem: {
        int frame_margins = DStyle::pixelMetric(PM_FrameMargins, opt, widget);
        size += QSize(frame_margins * 2, frame_margins * 2);
        break;
    }
    default:
        break;
    }

    return size;
}

int ChameleonStyle::pixelMetric(QStyle::PixelMetric m, const QStyleOption *opt,
                                const QWidget *widget) const
{
    switch (static_cast<int>(m)) {
    case PM_ButtonDefaultIndicator:
    case PM_ButtonShiftHorizontal:
    case PM_ButtonShiftVertical:
    case PM_FocusFrameVMargin:
    case PM_FocusFrameHMargin:
        return 0;
    case PM_ButtonMargin:
        return Metrics::Button_MarginWidth;
    case PM_MenuBarItemSpacing:
        return Metrics::MenuBar_ItemSpacing;
    case PM_IndicatorWidth:
    case PM_IndicatorHeight:
    case PM_ExclusiveIndicatorHeight:
    case PM_ExclusiveIndicatorWidth:
        return Metrics::CheckBox_FrameWidth;
    case PM_ScrollBarSliderMin:
        return ScrollBar_SliderMinWidget;
    case PM_SpinBoxFrameWidth:
        return Metrics::SpinBox_FrameWidth;
    case PM_SliderLength:
        return Metrics::Slider_TickLength;        //滑块的长度
    case PM_SliderControlThickness:
        return Metrics::Slider_ControlThickness;
    case PM_MenuBarHMargin:
        return Metrics::MenuBarItem_MarginWidth;
    case PM_MenuBarVMargin:
        return Metrics::MenuBarItem_MarginHeight;
        return Metrics::Slider_ControlThickness;  //滑块高度
    case PM_SliderTickmarkOffset:
        return Slider_TickmarkOffset;             //刻度的高度
    default:
        break;
    }

    return DStyle::pixelMetric(m, opt, widget);
}

int ChameleonStyle::styleHint(QStyle::StyleHint sh, const QStyleOption *opt,
                              const QWidget *w, QStyleHintReturn *shret) const
{
    switch (sh) {
    case SH_ItemView_ShowDecorationSelected:
    case SH_ScrollBar_Transient:
        return true;
    default:
        break;
    }

    return DStyle::styleHint(sh, opt, w, shret);
}

QIcon ChameleonStyle::standardIcon(QStyle::StandardPixmap standardIcon, const QStyleOption *opt,
                                   const QWidget *widget) const
{
    switch (standardIcon) {
    case SP_LineEditClearButton: {
        QRect rect(0, 0, Metrics::Icon_Size, Metrics::Icon_Size);
        qreal sclae = widget ? widget->devicePixelRatioF() : qApp->devicePixelRatio();
        QImage image(rect.size() * sclae, QImage::Format_ARGB32_Premultiplied);

        rect.adjust(Metrics::Icon_Margins, Metrics::Icon_Margins, -Metrics::Icon_Margins, -Metrics::Icon_Margins);
        image.fill(Qt::transparent);
        image.setDevicePixelRatio(sclae);

        QPainter pan;
        pan.begin(&image);
        pan.setBrush(getColor(opt, QPalette::Base));
        pan.setPen(Qt::NoPen);
        pan.setRenderHint(QPainter::Antialiasing, true);
        pan.drawEllipse(rect);

        QRectF contentRect(0, 0, rect.width() / 2.5, rect.height() / 2.5);
        contentRect.moveCenter(QRectF(rect).center());
        DDrawUtils::drawFork(&pan, contentRect, getColor(opt, QPalette::Text), 2);
        pan.end();

        QPixmap pix = QPixmap::fromImage(image);
        pix.setDevicePixelRatio(image.devicePixelRatio());

        return QIcon(pix);
    }
    break;
    default:
        break;
    }
    return DStyle::standardIcon(standardIcon, opt, widget);
}

QPixmap ChameleonStyle::standardPixmap(QStyle::StandardPixmap sp, const QStyleOption *opt,
                                       const QWidget *widget) const
{
    return DStyle::standardPixmap(sp, opt, widget);
}

QPixmap ChameleonStyle::generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
                                            const QStyleOption *opt) const
{
    return DStyle::generatedIconPixmap(iconMode, pixmap, opt);
}

int ChameleonStyle::layoutSpacing(QSizePolicy::ControlType control1, QSizePolicy::ControlType control2,
                                  Qt::Orientation orientation, const QStyleOption *option, const QWidget *widget) const
{
    return DStyle::layoutSpacing(control1, control2, orientation, option, widget);
}

QPalette ChameleonStyle::standardPalette() const
{
    QPalette pa;
    const QColor *color_list = isDrakStyle() ? dark_qpalette : light_qpalette;

    for (int i = 0; i < QPalette::NColorRoles; ++i) {
        QPalette::ColorRole role = static_cast<QPalette::ColorRole>(i);

        pa.setColor(DPalette::Active, role, color_list[i]);
    }

    initDisablePalette(pa);
    initInactivePalette(pa);

    return pa;
}

void ChameleonStyle::polish(QPalette &pa)
{
    DStyle::polish(pa);
}

void ChameleonStyle::polish(QApplication *app)
{
    DPalette pa = proxy()->standardPalette();
    const QColor *color_list = isDrakStyle() ? dark_dpalette : light_dpalette;

    for (int i = 0; i < DPalette::NColorTypes; ++i) {
        DPalette::ColorType type = static_cast<DPalette::ColorType>(i);

        pa.setColor(DPalette::Active, type, color_list[i]);
    }

    initDisablePalette(pa);
    initInactivePalette(pa);

    DPalette::setGeneric(pa);
    app->setPalette(pa);

    DStyle::polish(app);
}

void ChameleonStyle::polish(QWidget *w)
{
    DStyle::polish(w);

    if (qobject_cast<QAbstractButton *>(w)
            || qobject_cast<QComboBox *>(w)
            || qobject_cast<QScrollBar *>(w)
            || qobject_cast<QCheckBox *>(w)
            || qobject_cast<QAbstractSpinBox *>(w)
            || qobject_cast<QTabBar *>(w)
            || qobject_cast<QCheckBox *>(w)) {
        w->setAttribute(Qt::WA_Hover, true);
    }

    if (auto view = qobject_cast<QAbstractItemView *>(w)) {
        view->viewport()->setAttribute(Qt::WA_Hover, true);
    }

    if (auto scrollbar = qobject_cast<QScrollBar *>(w)) {
        scrollbar->setAttribute(Qt::WA_OpaquePaintEvent, false);
    }

    if (DApplication::isDXcbPlatform()) {
        bool is_menu = qobject_cast<QMenu *>(w);
        bool is_tip = w->inherits("QTipLabel");

        // 当窗口已经创建对应的native窗口，要判断当前是否已经设置了窗口背景透明
        // Bug: https://github.com/linuxdeepin/internal-discussion/issues/323
        if (is_menu && w->windowHandle()) {
            if (const QPlatformWindow *handle = w->windowHandle()->handle()) {
                if (!w->testAttribute(Qt::WA_TranslucentBackground) && !handle->isExposed()) {
                    // 销毁现有的native窗口，否则设置Qt::WA_TranslucentBackground不会生效
                    class DQWidget : public QWidget {public: using QWidget::destroy;};
                    reinterpret_cast<DQWidget *>(w)->destroy(true, false);
                }
            }
        }

        if (is_menu) {
            DPlatformWindowHandle handle(w);

            if (DPlatformWindowHandle::isEnabledDXcb(w)) {
                handle.setEnableBlurWindow(true);
                w->setAttribute(Qt::WA_TranslucentBackground);
            }
        } else if (is_tip) {
            DPlatformWindowHandle::enableDXcbForWindow(w);
        }
    }
}

void ChameleonStyle::unpolish(QWidget *w)
{
    DStyle::unpolish(w);

    if (qobject_cast<QAbstractButton *>(w)
            || qobject_cast<QComboBox *>(w)
            || qobject_cast<QScrollBar *>(w)
            || qobject_cast<QCheckBox *>(w)
            || qobject_cast<QAbstractSpinBox *>(w)
            || qobject_cast<QTabBar *>(w)
            || qobject_cast<QCheckBox *>(w)) {
        w->setAttribute(Qt::WA_Hover, false);
    }

    if (auto view = qobject_cast<QAbstractItemView *>(w)) {
        view->viewport()->setAttribute(Qt::WA_Hover, false);
    }

    if (auto scrollbar = qobject_cast<QScrollBar *>(w)) {
        scrollbar->setAttribute(Qt::WA_OpaquePaintEvent, true);
    }
}

void ChameleonStyle::unpolish(QApplication *application)
{
    DStyle::unpolish(application);
}

bool ChameleonStyle::isDrakStyle() const
{
    DNativeSettings theme_settings(0);

    return theme_settings.isValid() && theme_settings.getSetting("Net/ThemeName").toByteArray().contains("dark");
}

void ChameleonStyle::drawShadow(QPainter *p, const QRect &rect, const QColor &color) const
{
    int frame_radius = DStyle::pixelMetric(PM_FrameRadius);
    int shadow_radius = DStyle::pixelMetric(PM_ShadowRadius);
    int shadow_xoffset = DStyle::pixelMetric(PM_ShadowHOffset);
    int shadow_yoffset = DStyle::pixelMetric(PM_ShadowVOffset);

    DDrawUtils::drawShadow(p, rect, frame_radius, frame_radius, color, shadow_radius,
                          QPoint(shadow_xoffset, shadow_yoffset));
}

void ChameleonStyle::drawBorder(QPainter *p, const QRect &rect, const QBrush &brush) const
{
    int border_width = DStyle::pixelMetric(PM_FocusBorderWidth);
    int border_spacing = DStyle::pixelMetric(PM_FocusBorderSpacing);
    int frame_radis = DStyle::pixelMetric(PM_FrameRadius) + border_spacing;

    DDrawUtils::drawBorder(p, rect, brush, border_width, frame_radis);
}

QBrush ChameleonStyle::generatedBrush(StateFlags flags, const QBrush &base, QPalette::ColorGroup cg, QPalette::ColorRole role, const QStyleOption *option) const
{
    Q_UNUSED(cg)

    QColor colorNew = base.color();

    if (!colorNew.isValid())
        return base;

    if ((flags & StyleState_Mask)  == SS_HoverState) {
        switch (role) {
        case QPalette::Light:
        case QPalette::Dark:
            colorNew = DStyle::adjustColor(colorNew, 0, 0, -10, 0, 0, 0, 0);
            break;
        case QPalette::Highlight:
            colorNew = DStyle::adjustColor(colorNew, 0, 0, +20);
            break;
        default:
            break;
        }

        return colorNew;
    } else if ((flags & StyleState_Mask) == SS_PressState) {
        QColor hightColor = option->palette.highlight().color();
        hightColor.setAlphaF(0.1);

        switch (role) {
        case QPalette::Light: {
            colorNew = DStyle::adjustColor(colorNew, 0, 0, -20, 0, 0, +20, 0);
            colorNew = DStyle::blendColor(colorNew, hightColor);
            break;
        }
        case QPalette::Dark: {
            colorNew = DStyle::adjustColor(colorNew, 0, 0, -15, 0, 0, +20, 0);
            colorNew = DStyle::blendColor(colorNew, hightColor);
            break;
        }
        case QPalette::Highlight:
            colorNew = DStyle::adjustColor(colorNew, 0, 0, -10);
            break;
        default:
            break;
        }

        return colorNew;
    }

    return base;
}

QBrush ChameleonStyle::generatedBrush(DStyle::StateFlags flags, const QBrush &base, QPalette::ColorGroup cg, DPalette::ColorType type, const QStyleOption *option) const
{
    Q_UNUSED(cg)
    Q_UNUSED(option)

    QColor colorNew = base.color();

    if (!colorNew.isValid())
        return base;

    if ((flags & StyleState_Mask)  == SS_HoverState) {
        switch (type) {
        case DPalette::LightLively:
            colorNew = DStyle::adjustColor(colorNew, 0, 0, +30, 0, 0, 0, 0);
            break;
        case DPalette::DarkLively:
            colorNew = DStyle::adjustColor(colorNew, 0, 0, +10, 0, 0, 0, 0);
            break;
        case DPalette::ItemBackground:
            colorNew = DStyle::adjustColor(colorNew, 0, 0, 0, 0, 0, 0, +10);
            break;
        default:
            break;
        }

        return colorNew;
    } else if ((flags & StyleState_Mask) == SS_PressState) {
        switch (type) {
        case DPalette::LightLively:
            colorNew = DStyle::adjustColor(colorNew, 0, 0, -30, 0, 0, 0, 0);
            break;
        case DPalette::DarkLively:
            colorNew = DStyle::adjustColor(colorNew, 0, 0, -20, 0, 0, 0, 0);
            break;
        default:
            break;
        }

        return colorNew;
    } else if ((flags & StyleState_Mask) == SS_NormalState) {
        switch (type) {
        case DPalette::LightLively:
            colorNew = DStyle::adjustColor(colorNew, 0, 0, +40, 0, 0, 0, 0);
            break;
        case DPalette::DarkLively:
            colorNew = DStyle::adjustColor(colorNew, 0, 0, +20, 0, 0, 0, 0);
            break;
        default:
            break;
        }

        return colorNew;
    }

    return base;
}

QColor ChameleonStyle::getColor(const QStyleOption *option, QPalette::ColorRole role) const
{
    return DStyle::generatedBrush(option, option->palette.brush(role), option->palette.currentColorGroup(), role).color();
}

QColor ChameleonStyle::getColor(const QStyleOption *option, DPalette::ColorType type, const QWidget *widget) const
{
    const DPalette &pa = DPalette::get(widget, option->palette);

    return DStyle::generatedBrush(option, pa.brush(type), pa.currentColorGroup(), type).color();
}

QMargins ChameleonStyle::frameExtentMargins() const
{
    int margins = DStyle::pixelMetric(PM_FrameMargins);

    return QMargins(margins, margins, margins, margins);
}

} // namespace chameleon
