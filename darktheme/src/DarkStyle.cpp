#include "DarkStyle.h"
#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QDialogButtonBox>
#include <QDockWidget>
#include <QInputDialog>
#include <QListView>
#include <QMessageBox>
#include <QPixmapCache>
#include <QStyleOptionComplex>
#include <QTableWidget>
#include "NinePatch.h"
#include <math.h>

#define MBI_NORMAL                  1
#define MBI_HOT                     2
#define MBI_PUSHED                  3
#define MBI_DISABLED                4

namespace {

static const int windowsItemFrame        =  2; // menu item frame width
static const int windowsItemHMargin      =  3; // menu item hor text margin
static const int windowsItemVMargin      =  4; // menu item ver text margin
static const int windowsArrowHMargin     =  6; // arrow horizontal margin
static const int windowsRightBorder      = 15; // right border on windows


class Lighten {
private:
	int lut[256];
public:
	Lighten()
	{
		const double x = 0.75;
		for (int i = 0; i < 256; i++) {
			lut[i] = (int)(pow(i / 255.0, x) * 255);
		}
	}
	int operator [] (int i)
	{
		return lut[i];
	}
};

QImage loadImage(QString const &path, QString const &role = QString())
{
	QImage image;
	image.load(path);
	image.setText("name", path);
	image.setText("role", role);
	return image;
}

QImage generateHoverImage(QImage const &source)
{
	QImage newimage;
	int w = source.width();
	int h = source.height();
	if (w > 2 && h > 2) {
		Lighten lighten;
		newimage = source;
		newimage.setText("role", "hover");
		w -= 2;
		h -= 2;
		for (int y = 0; y < h; y++) {
			QRgb *ptr = (QRgb *)newimage.scanLine(y + 1) + 1;
			for (int x = 0; x < w; x++) {
				int v = qGray(ptr[x]);
				v = lighten[v];
				ptr[x] = qRgba(v, v, v, qAlpha(ptr[x]));
			}
		}
	}
	return newimage;
}

void drawFrame(QPainter *pr, int x, int y, int w, int h, QColor color_topleft, QColor color_bottomright)
{
	if (w < 3 || h < 3) {
		if (w > 0 && h > 0) {
			pr->fillRect(x, y, w, h, color_topleft);
		}
	} else {
		if (!color_topleft.isValid()) color_topleft = Qt::black;
		if (!color_bottomright.isValid()) color_bottomright = color_topleft;
		pr->fillRect(x, y, w - 1, 1, color_topleft);
		pr->fillRect(x, y + 1, 1, h -1, color_topleft);
		pr->fillRect(x + w - 1, y, 1, h -1, color_bottomright);
		pr->fillRect(x + 1, y + h - 1, w - 1, 1, color_bottomright);
	}
}

void drawFrame(QPainter *pr, QRect const &r, QColor color_topleft, QColor color_bottomright)
{
	return drawFrame(pr, r.x(), r.y(), r.width(), r.height(), color_topleft, color_bottomright);
}

} // end namespace

// DarkStyle

DarkStyle::DarkStyle()
{
	button_normal = loadImage(QLatin1String(":/darktheme/button/button_normal.png"), QLatin1String("normal"));
	button_press = loadImage(QLatin1String(":/darktheme/button/button_press.png"), QLatin1String("press"));

	hsb.sub_line = generateButtonImages(QLatin1String(":/darktheme/hsb/hsb_sub_line.png"));
	hsb.add_line = generateButtonImages(QLatin1String(":/darktheme/hsb/hsb_add_line.png"));
	hsb.page_bg = loadImage(QLatin1String(":/darktheme/hsb/hsb_page_bg.png"));
	hsb.slider.im_normal = loadImage(QLatin1String(":/darktheme/hsb/hsb_slider.png"));
	hsb.slider.im_hover = generateHoverImage(hsb.slider.im_normal);

	vsb.sub_line = generateButtonImages(QLatin1String(":/darktheme/vsb/vsb_sub_line.png"));
	vsb.add_line = generateButtonImages(QLatin1String(":/darktheme/vsb/vsb_add_line.png"));
	vsb.page_bg = loadImage(QLatin1String(":/darktheme/vsb/vsb_page_bg.png"));
	vsb.slider.im_normal = loadImage(QLatin1String(":/darktheme/vsb/vsb_slider.png"));
	vsb.slider.im_hover = generateHoverImage(vsb.slider.im_normal);

	progress_horz.load(QLatin1String(":/darktheme/progress/horz.png"));
	progress_vert.load(QLatin1String(":/darktheme/progress/vert.png"));
}

QString DarkStyle::pixmapkey(const QString &name, const QString &role, const QSize &size)
{
	QString key = "%1:%2:%3:%4";
	key = key.arg(name).arg(role).arg(size.width()).arg(size.height());
	return key;
}

DarkStyle::ButtonImages DarkStyle::generateButtonImages(QString const &path)
{
	QImage source = loadImage(path);
	ButtonImages buttons;
	int w = source.width();
	int h = source.height();
	if (w > 4 && h > 4) {
		Lighten lighten;
		buttons.im_normal = source;
		buttons.im_hover = source;
		w -= 4;
		h -= 4;
		for (int y = 0; y < h; y++) {
			QRgb *src1 = (QRgb *)source.scanLine(y + 1);
			QRgb *src2 = (QRgb *)source.scanLine(y + 2);
			QRgb *src3 = (QRgb *)source.scanLine(y + 3);
			QRgb *dst0 = (QRgb *)buttons.im_normal.scanLine(y + 2) + 2;
			QRgb *dst1 = (QRgb *)buttons.im_hover.scanLine(y + 2) + 2;
			for (int x = 0; x < w; x++) {
				int v = (int)qAlpha(src3[x + 3]) - (int)qAlpha(src1[x + 1]);
				v = (v + 256) / 2;
				int alpha = qAlpha(src2[x + 2]);
				dst0[x] = qRgba(v, v, v, alpha);
				v = lighten[v];
				dst1[x] = qRgba(v, v, v, alpha);
			}
		}
		buttons.im_normal.setText("name", source.text("name"));
		buttons.im_hover.setText("name", source.text("name"));
		buttons.im_normal.setText("role", "normal");
		buttons.im_hover.setText("role", "hover");
	}
	return buttons;
}

QPixmap DarkStyle::pixmapFromImage(const QImage &image, QSize size) const
{
	QString key = pixmapkey(image.text("name"), image.text("role"), size);

	QPixmap *pm = QPixmapCache::find(key);
	if (pm) return *pm;

	TextureCacheItem t;
	t.key = key;
	t.pm = QPixmap::fromImage(image.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
	QPixmapCache::insert(key, t.pm);
	return t.pm;
}

QColor DarkStyle::colorForSelectedFrame(const QStyleOption *opt) const
{
	(void)opt;
	return QColor(128, 192, 255);

}

QColor DarkStyle::colorForItemView(QStyleOption const *opt) const
{
	return opt->palette.color(QPalette::Dark);
//	return opt->palette.color(QPalette::Base); // これじゃない
}

void DarkStyle::drawNinePatchImage(QPainter *p, const QImage &image, const QRect &r, int w, int h) const
{
	QImage im = createImageFromNinePatchImage(image, w, h);
	im.setText("name", image.text("name"));
	im.setText("role", image.text("role"));
	QPixmap pm = pixmapFromImage(im, r.size());
	p->drawPixmap(r.x(), r.y(), pm);
}

void DarkStyle::polish(QPalette &palette)
{
	palette = QPalette(QColor(64, 64, 64));
}

void DarkStyle::drawGutter(QPainter *p, const QRect &r) const
{
	int x = r.x();
	int y = r.y();
	int w = r.width();
	int h = r.height();
	QColor dark = QColor(32, 32, 32);
	QColor lite = QColor(128, 128, 128);
	if (w < h) {
		x += (w - 1) / 2;
		p->fillRect(x, y, 1, h, dark);
		p->fillRect(x + 1, y, 1, h, lite);
	} else if (w > h) {
		fprintf(stderr, "%d %d\n", y, h);
		y += (h - 1) / 2;
		p->fillRect(x, y, w, 1, dark);
		p->fillRect(x, y + 1, w, 1, lite);
	}
}

void DarkStyle::drawSelectedMenuFrame(const QStyleOption *option, QPainter *p, QRect rect, const QWidget *widget, bool deep) const
{
	(void)widget;
	QColor color = colorForSelectedFrame(option);

	int x, y, w, h;
	x = rect.x();
	y = rect.y();
	w = rect.width();
	h = rect.height();

	auto SetAlpha = [&](QColor *color, int alpha){
		if (deep) {
			alpha = alpha * 3 / 2;
		}
		color->setAlpha(alpha);
	};

	QString key;
	{
		char tmp[100];
		sprintf(tmp, "selection_frame:%02x%02x%02x:%dx%d", color.red(), color.green(), color.blue(), w, h);
		key = tmp;
	}

	QPixmap pixmap;
	if (!QPixmapCache::find(key, &pixmap)) {
		pixmap = QPixmap(w, h);
		pixmap.fill(Qt::transparent);
		QPainter pr(&pixmap);
		pr.setRenderHint(QPainter::Antialiasing);

		QColor pencolor = color;
		SetAlpha(&pencolor, 128);
		pr.setPen(pencolor);
		pr.setBrush(Qt::NoBrush);

		QPainterPath path;
		path.addRoundedRect(1.5, 1.5, w - 1.5, h - 1.5, 3, 3);

		pr.drawPath(path);

		pr.setClipPath(path);

		int a = color.alpha();
		QColor color0 = color;
		QColor color1 = color;
		SetAlpha(&color0, 96 * a / 255);
		SetAlpha(&color1, 32 * a / 255);
		QLinearGradient gr(QPointF(0, 0), QPointF(0, h));
		gr.setColorAt(0, color0);
		gr.setColorAt(1, color1);
		QBrush br(gr);
		pr.fillRect(0, 0, w, h, br);

		pr.end();
		QPixmapCache::insert(key, pixmap);
	}

	p->drawPixmap(x, y, w, h, pixmap);
}

void DarkStyle::drawButton(QPainter *p, const QStyleOption *option) const
{
	int w = option->rect.width();
	int h = option->rect.height();
	bool pressed = (option->state & (State_Sunken | State_On));
	bool hover = (option->state & State_MouseOver);

	if (pressed) {
		drawNinePatchImage(p, button_press, option->rect, w, h);
	} else {
		drawNinePatchImage(p, button_normal, option->rect, w, h);
	}
	{
		QPainterPath path;
		path.addRoundedRect(option->rect, 6, 6); // 角丸四角形のパス
		p->save();
		p->setRenderHint(QPainter::Antialiasing);
		p->setClipPath(path);

		int x = option->rect.x();
		int y = option->rect.y();
		int w = option->rect.width();
		int h = option->rect.height();
		QColor color0, color1;
#ifdef Q_OS_MAC
		if (pressed) {
			color0 = Qt::black;
			color1 = Qt::black;
			color0.setAlpha(48);
			color1.setAlpha(144);
		} else {
			color0 = Qt::black;
			color1 = Qt::black;
			color0.setAlpha(32);
			color1.setAlpha(128);
		}
#else
		if (pressed) {
			color0 = Qt::black;
			color1 = Qt::black;
			color0.setAlpha(32);
			color1.setAlpha(128);
		} else if (hover) {
			color0 = Qt::black;
			color1 = Qt::black;
			color0.setAlpha(16);
			color1.setAlpha(96);
		} else {
			color0 = Qt::black;
			color1 = Qt::black;
			color0.setAlpha(32);
			color1.setAlpha(128);
		}
#endif
		QLinearGradient gr(QPointF(x, y), QPointF(x, y + h));
		gr.setColorAt(0, color0);
		gr.setColorAt(1, color1);
		QBrush br(gr);
		p->fillRect(x, y, w, h, br);

		p->restore();
	}
}

void DarkStyle::drawToolButton(QPainter *p, const QStyleOption *option) const
{
	p->save();

	bool hover = (option->state & State_MouseOver);
	bool pressed = (option->state & State_Sunken);
#ifdef Q_OS_MAC
	hover = false;
#endif

	int x = option->rect.x();
	int y = option->rect.y();
	int w = option->rect.width();
	int h = option->rect.height();
	QColor color0, color1;
#ifdef Q_OS_MAC
	if (pressed) {
		color0 = QColor(72, 72, 72);
		color1 = QColor(40, 40, 40);
	} else {
		color0 = QColor(80, 80, 80);
		color1 = QColor(48, 48, 48);
	}
#else
	if (pressed) {
		color0 = QColor(80, 80, 80);
		color1 = QColor(48, 48, 48);
	} else if (hover) {
		color0 = QColor(96, 96, 96);
		color1 = QColor(64, 64, 64);
	} else {
		color0 = QColor(80, 80, 80);
		color1 = QColor(48, 48, 48);
	}
#endif
	QLinearGradient gr(QPointF(x, y), QPointF(x, y + h));
	gr.setColorAt(0, color0);
	gr.setColorAt(1, color1);
	QBrush br(gr);
	p->fillRect(x, y, w, h, br);

	if (option->state & State_Raised) {
		drawFrame(p, option->rect, QColor(96, 96, 96), QColor(32, 32, 32));
	} else if (option->state & State_Sunken) {
		drawFrame(p, option->rect, QColor(48, 48, 48), QColor(48, 48, 48));
	}

	p->restore();
}

void DarkStyle::drawRaisedFrame(QPainter *p, QRect const &rect, QPalette const &palette)
{
	p->save();
	int x = rect.x();
	int y = rect.y();
	int w = rect.width();
	int h = rect.height();
	p->setClipRect(x, y, w, h);
	p->fillRect(x, y, w, h, palette.color(QPalette::Window));
	p->fillRect(x, y, w - 1, 1, palette.color(QPalette::Light));
	p->fillRect(x, y, 1, h - 1, palette.color(QPalette::Light));
	p->fillRect(x + 1, y + h - 2, w - 2, 1, palette.color(QPalette::Dark));
	p->fillRect(x + w - 2, y + 1, 1, h - 2, palette.color(QPalette::Dark));
	p->fillRect(x, y + h - 1, w, 1, palette.color(QPalette::Shadow));
	p->fillRect(x + w - 1, y, 1, h, palette.color(QPalette::Shadow));
	p->restore();
}

void DarkStyle::drawMenuBarBG(QPainter *p, const QStyleOption *option, QWidget const *widget) const
{
	int x = option->rect.x();
	int y = widget->y();
	int w = option->rect.width();
	int h = widget->height();
	QLinearGradient gradient;
	gradient.setStart(x, y);
	gradient.setFinalStop(x, y + h / 2);
	gradient.setColorAt(0, option->palette.color(QPalette::Light));
	gradient.setColorAt(1, option->palette.color(QPalette::Window));
	p->fillRect(x, y, w, h, gradient);
	p->fillRect(x, y + h - 1, w, 1, option->palette.color(QPalette::Dark));
}

int DarkStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
	if (metric == PM_SliderLength) {
		return std::min(widget->width(), widget->height());
	}
	return QProxyStyle::pixelMetric(metric, option, widget);
}

QRect DarkStyle::subControlRect(ComplexControl cc, const QStyleOptionComplex *option, SubControl sc, const QWidget *widget) const
{
	if (cc == CC_Slider && sc == SC_SliderGroove) {
		return widget->rect();
	}
	if (cc == CC_Slider && sc == SC_SliderHandle) {
		if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
			QRect rect;
			int extent = std::min(widget->width(), widget->height());
			int span = pixelMetric(PM_SliderLength, slider, widget);
			bool horizontal = slider->orientation == Qt::Horizontal;
			int sliderPos = sliderPositionFromValue(slider->minimum, slider->maximum,
													slider->sliderPosition,
													(horizontal ? slider->rect.width()
																: slider->rect.height()) - span,
													slider->upsideDown);
			if (horizontal) {
				rect.setRect(slider->rect.x() + sliderPos, slider->rect.y(), span, extent);
			} else {
				rect.setRect(slider->rect.x(), slider->rect.y() + sliderPos, extent, span);
			}
			return rect;
		}
	}
	if (cc == CC_GroupBox) {
		QRect rect;
		if (const QStyleOptionGroupBox *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(option)) {
			switch (sc) {
			case SC_GroupBoxFrame:
				// FALL THROUGH
			case SC_GroupBoxContents:
				{
					int topMargin = 0;
					int topHeight = 0;
					int bottomMargin = 0;
					int noLabelMargin = 0;
					QRect frameRect = groupBox->rect;
					int verticalAlignment = styleHint(SH_GroupBox_TextLabelVerticalAlignment, groupBox, widget);
					if (groupBox->text.size()) {
						topHeight = groupBox->fontMetrics.height();
						if (verticalAlignment & Qt::AlignVCenter) {
							topMargin = topHeight / 2;
						} else if (verticalAlignment & Qt::AlignTop) {
							topMargin = -topHeight/2;
						}
					} else {
						topHeight = groupBox->fontMetrics.height();
						noLabelMargin = topHeight / 2;
						if (verticalAlignment & Qt::AlignVCenter) {
							topMargin = topHeight / 4 - 4;
							bottomMargin = topHeight / 4 - 4;
						} else if (verticalAlignment & Qt::AlignTop) {
							topMargin = topHeight/2 - 4;
							bottomMargin = topHeight/2 - 4;
						}
					}

					if (sc == SC_GroupBoxFrame) {
						frameRect.setTop(topMargin);
						frameRect.setBottom(frameRect.height() + bottomMargin);
						rect = frameRect;
						break;
					}

					int frameWidth = 0;
					if ((groupBox->features & QStyleOptionFrameV2::Flat) == 0) {
						frameWidth = pixelMetric(PM_DefaultFrameWidth, groupBox, widget);
					}
					rect = frameRect.adjusted(frameWidth, frameWidth + topHeight, -frameWidth, -frameWidth - noLabelMargin);
					break;
				}
			case SC_GroupBoxCheckBox:
				// FALL THROUGH
			case SC_GroupBoxLabel:
				{
					QFontMetrics fontMetrics = groupBox->fontMetrics;
					int h = fontMetrics.height();
					int tw = fontMetrics.size(Qt::TextShowMnemonic, groupBox->text + QLatin1Char(' ')).width();
					int marg = (groupBox->features & QStyleOptionFrameV2::Flat) ? 0 : 8;
					rect = groupBox->rect.adjusted(marg, 0, -marg, 0);
					rect.setHeight(h);

					int indicatorWidth = pixelMetric(PM_IndicatorWidth, option, widget);
					int indicatorSpace = pixelMetric(PM_CheckBoxLabelSpacing, option, widget) - 1;
					bool hasCheckBox = groupBox->subControls & QStyle::SC_GroupBoxCheckBox;
					int checkBoxSize = hasCheckBox ? (indicatorWidth + indicatorSpace) : 0;

					// Adjusted rect for label + indicatorWidth + indicatorSpace
					QRect totalRect = alignedRect(groupBox->direction, groupBox->textAlignment, QSize(tw + checkBoxSize, h), rect);
					// Adjust totalRect if checkbox is set
					if (hasCheckBox) {
						bool ltr = groupBox->direction == Qt::LeftToRight;
						int left = 0;
						// Adjust for check box
						if (sc == SC_GroupBoxCheckBox) {
							int indicatorHeight = pixelMetric(PM_IndicatorHeight, option, widget);
							left = ltr ? totalRect.left() : (totalRect.right() - indicatorWidth);
							int top = totalRect.top() + (fontMetrics.height() - indicatorHeight) / 2;
							totalRect.setRect(left, top, indicatorWidth, indicatorHeight);
							// Adjust for label
						} else {
							left = ltr ? (totalRect.left() + checkBoxSize - 2) : totalRect.left();
							totalRect.setRect(left, totalRect.top(), totalRect.width() - checkBoxSize, totalRect.height());
						}
					}
					rect = totalRect;
					break;
				}
			default:
				break;
			}
		}
		return rect;
	}
	return QProxyStyle::subControlRect(cc, option, sc, widget);
}

void DarkStyle::drawPrimitive(PrimitiveElement pe, const QStyleOption *option, QPainter *p, const QWidget *widget) const
{
#ifdef Q_OS_LINUX
	if (pe == PE_FrameFocusRect) {
//		QColor color = option->palette.color(QPalette::Light);
		QColor color(64, 128, 255);
		drawFrame(p, option->rect, color, color);
		return;
	}
#endif
	if (pe == PE_IndicatorArrowDown) {
		//		p->fillRect(option->rect, Qt::red);
		switch (pe) {
		case PE_IndicatorArrowUp:
		case PE_IndicatorArrowDown:
		case PE_IndicatorArrowRight:
		case PE_IndicatorArrowLeft:
			{
				if (option->rect.width() <= 1 || option->rect.height() <= 1)
					break;
				QRect r = option->rect.adjusted(-10, -10, 0, 10);
				int size = qMin(r.height(), r.width());
				QPixmap pixmap;
//				QString pixmapName = QStyleHelper::uniqueName(QLatin1String("$qt_ia-")
//															  % QLatin1String(metaObject()->className()), option, QSize(size, size))
//						% HexString<uint>(pe);
				if (1) {//if (!QPixmapCache::find(pixmapName, pixmap)) {
					int border = size/5;
					int sqsize = 2*(size/2);
					QImage image(sqsize, sqsize, QImage::Format_ARGB32_Premultiplied);
					image.fill(0);
					QPainter imagePainter(&image);

					QPolygon a;
					switch (pe) {
					case PE_IndicatorArrowUp:
						a.setPoints(3, border, sqsize/2,  sqsize/2, border,  sqsize - border, sqsize/2);
						break;
					case PE_IndicatorArrowDown:
						a.setPoints(3, border, sqsize/2,  sqsize/2, sqsize - border,  sqsize - border, sqsize/2);
						break;
					case PE_IndicatorArrowRight:
						a.setPoints(3, sqsize - border, sqsize/2,  sqsize/2, border,  sqsize/2, sqsize - border);
						break;
					case PE_IndicatorArrowLeft:
						a.setPoints(3, border, sqsize/2,  sqsize/2, border,  sqsize/2, sqsize - border);
						break;
					default:
						break;
					}

					int bsx = 0;
					int bsy = 0;

					if (option->state & State_Sunken) {
						bsx = pixelMetric(PM_ButtonShiftHorizontal, option, widget);
						bsy = pixelMetric(PM_ButtonShiftVertical, option, widget);
					}

					QRect bounds = a.boundingRect();
					int sx = sqsize / 2 - bounds.center().x() - 1;
					int sy = sqsize / 2 - bounds.center().y() - 1;
					imagePainter.translate(sx + bsx, sy + bsy);
					imagePainter.setPen(option->palette.buttonText().color());
					imagePainter.setBrush(option->palette.buttonText());
					imagePainter.setRenderHint(QPainter::Qt4CompatiblePainting);

					if (!(option->state & State_Enabled)) {
						imagePainter.translate(1, 1);
						imagePainter.setBrush(option->palette.light().color());
						imagePainter.setPen(option->palette.light().color());
						imagePainter.drawPolygon(a);
						imagePainter.translate(-1, -1);
						imagePainter.setBrush(option->palette.mid().color());
						imagePainter.setPen(option->palette.mid().color());
					}

					imagePainter.drawPolygon(a);
					imagePainter.end();
					pixmap = QPixmap::fromImage(image);
//					QPixmapCache::insert(pixmapName, pixmap);
				}
				int xOffset = r.x() + (r.width() - size)/2;
				int yOffset = r.y() + (r.height() - size)/2;
				p->drawPixmap(xOffset, yOffset, pixmap);
			}
		}
		return;
	}
	if (pe == PE_PanelMenu) {
		QRect r = option->rect;
		drawFrame(p, r, Qt::black, Qt::black);
		r = r.adjusted(1, 1, -1, -1);
		drawFrame(p, r, QColor(128, 128, 128), QColor(64, 64, 64));
		r = r.adjusted(1, 1, -1, -1);
		p->fillRect(r, QColor(80, 80, 80));
		return;
	}
	if (pe == PE_FrameMenu) {
		return;
	}
	if (pe == PE_PanelButtonBevel) {
		drawButton(p, option);
		return;
	}
	if (pe == PE_PanelStatusBar) {
		p->fillRect(option->rect, option->palette.color(QPalette::Window));
		return;
	}
	if (pe == PE_FrameTabWidget) {
		drawRaisedFrame(p, option->rect, option->palette);
		return;
	}
	if (pe == PE_PanelLineEdit) {
		if (const QStyleOptionFrame *panel = qstyleoption_cast<const QStyleOptionFrame *>(option)) {
//			p->fillRect(option->rect, option->palette.color(QPalette::Dark));
			p->fillRect(option->rect, colorForItemView(option));
			if (panel->lineWidth > 0) {
				drawFrame(p, option->rect, option->palette.color(QPalette::Shadow), option->palette.color(QPalette::Light));
			}

//		option->
//				if (qobject_cast<QComboBox const *>(widget->parent())) {
//			// nop
//		} else {
//		}
		}
		return;
	}
	if (pe == PE_FrameGroupBox) {
#if 0
		QRect r = option->rect;
		r = r.adjusted(1, 1, -1, -1);
		drawFrame(p, r, option->palette.color(QPalette::Light), option->palette.color(QPalette::Light));
		r = r.adjusted(-1, -1, -1, -1);
		drawFrame(p, r, option->palette.color(QPalette::Dark), option->palette.color(QPalette::Dark));
#else
		p->save();

		p->setRenderHint(QPainter::Antialiasing);
		QRectF r = option->rect;
		r = r.adjusted(1.5, 1.5, -0.5, -0.5);
		p->setPen(option->palette.color(QPalette::Light));
		p->drawRoundedRect(r, 5, 5);
		r = r.adjusted(-1, -1, -1, -1);
		p->setPen(option->palette.color(QPalette::Dark));
		p->drawRoundedRect(r, 5, 5);
		p->restore();
#endif
		return;
	}
	if (pe == PE_PanelItemViewRow) {
#ifdef Q_OS_WIN
		// thru
#else
		return;
#endif
	}
	if (pe == PE_PanelItemViewItem) {
//		p->fillRect(option->rect, colorForItemView(option)); // 選択枠を透過描画させるので背景は描かない
		if (QTableView const *tableview = qobject_cast<QTableView const *>(widget)) {
			Qt::PenStyle grid_pen_style = Qt::NoPen;
			if (tableview->showGrid()) {
				grid_pen_style = tableview->gridStyle();
			}
			QAbstractItemView::SelectionBehavior selection_behavior = tableview->selectionBehavior();
			if (grid_pen_style != Qt::NoPen) {
				int x = option->rect.x();
				int y = option->rect.y();
				int w = option->rect.width();
				int h = option->rect.height();
				p->fillRect(x + w - 1, y, 1, h, option->palette.color(QPalette::Dark));
				p->fillRect(x, y + h - 1, w, 1, option->palette.color(QPalette::Dark));
			}
			if (option->state & State_Selected) {
				p->save();
				p->setClipRect(option->rect);
				QRect r = widget->rect();
				if (selection_behavior == QAbstractItemView::SelectionBehavior::SelectRows) {
					r = QRect(r.x(), option->rect.y(), r.width(), option->rect.height());
				} else if (selection_behavior == QAbstractItemView::SelectionBehavior::SelectRows) {
					r = QRect(option->rect.x(), r.y(), option->rect.y(), r.height());
				}
				drawSelectedMenuFrame(option, p, r, widget, false);
				p->restore();
			}
		} else {
			int n = 0;
			if (option->state & State_Selected) {
				n++;
			}
//			if (option->state & State_MouseOver) {
//				n++;
//			}
			if (n > 0) {
				drawSelectedMenuFrame(option, p, option->rect, widget, n > 1);
			}
		}
		return;
	}
	if (pe == QStyle::PE_IndicatorBranch) {
		QColor bg = option->palette.color(QPalette::Base);
		p->fillRect(option->rect, bg);

		if (legacy_windows_.drawPrimitive(pe, option, p, widget)) {
			return;
		}
	}
	if (pe == QStyle::PE_Widget) { // bg for messagebox
		const QDialogButtonBox *buttonBox = 0;

		if (qobject_cast<const QMessageBox *> (widget))
			buttonBox = widget->findChild<const QDialogButtonBox *>(QLatin1String("qt_msgbox_buttonbox"));
#ifndef QT_NO_INPUTDIALOG
		else if (qobject_cast<const QInputDialog *> (widget))
			buttonBox = widget->findChild<const QDialogButtonBox *>(QLatin1String("qt_inputdlg_buttonbox"));
#endif // QT_NO_INPUTDIALOG

		if (buttonBox) {
			int y = buttonBox->geometry().top();
			QRect r(option->rect.x(), y, option->rect.width(), 1);
			p->fillRect(r, option->palette.color(QPalette::Light));
			r.translate(0, -1);
			p->fillRect(r, option->palette.color(QPalette::Dark));
		}

		return;
	}
//	if (pe == QStyle::PE_PanelItemViewRow) {

//	}
	qDebug() << pe;
	QProxyStyle::drawPrimitive(pe, option, p, widget);
}

void DarkStyle::drawControl(ControlElement ce, const QStyleOption *option, QPainter *p, const QWidget *widget) const
{
	bool disabled = !(option->state & State_Enabled);
#ifdef Q_OS_MAC
    if (ce == CE_ToolBar) {
		int x = option->rect.x();
		int y = option->rect.y();
		int w = option->rect.width();
		int h = option->rect.height();
		p->fillRect(x, y + h - 1, w, 1, QColor(32, 32, 32));
		return;
	}
//  if (ce == CE_ToolButtonLabel) {
//		return;
//	}
#endif
#ifdef Q_OS_LINUX
	if (ce == CE_ToolBar) {
		int x = option->rect.x();
		int y = option->rect.y();
		int w = option->rect.width();
		int h = option->rect.height();
		QColor color = option->palette.color(QPalette::Window);
		p->fillRect(x, y + h - 1, w, 1, color);
		return;
	}
	if (ce == CE_PushButtonLabel) {
		 if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
			 QRect ir = button->rect;
			 uint tf = Qt::AlignVCenter | Qt::TextShowMnemonic;
			 QPoint buttonShift;

			 if (option->state & State_Sunken) {
				 buttonShift = QPoint(pixelMetric(PM_ButtonShiftHorizontal, option, widget), proxy()->pixelMetric(PM_ButtonShiftVertical, option, widget));
			 }

			 if (proxy()->styleHint(SH_UnderlineShortcut, button, widget)) {
				 tf |= Qt::TextShowMnemonic;
			 } else {
				 tf |= Qt::TextHideMnemonic;
			 }

			 if (!button->icon.isNull()) {
				 //Center both icon and text
				 QPoint point;

				 QIcon::Mode mode = button->state & State_Enabled ? QIcon::Normal : QIcon::Disabled;
				 if (mode == QIcon::Normal && button->state & State_HasFocus) {
					 mode = QIcon::Active;
				 }

				 QIcon::State state = QIcon::Off;

				 if (button->state & State_On) {
					 state = QIcon::On;
				 }

				 QPixmap pixmap = button->icon.pixmap(button->iconSize, mode, state);
				 int w = pixmap.width();
				 int h = pixmap.height();

				 if (!button->text.isEmpty()) {
					 w += button->fontMetrics.boundingRect(option->rect, tf, button->text).width() + 4;
				 }

				 point = QPoint(ir.x() + ir.width() / 2 - w / 2, ir.y() + ir.height() / 2 - h / 2);

				 if (button->direction == Qt::RightToLeft) {
					 point.rx() += pixmap.width();
				 }

				 p->drawPixmap(visualPos(button->direction, button->rect, point + buttonShift), pixmap);

				 if (button->direction == Qt::RightToLeft) {
					 ir.translate(-point.x() - 2, 0);
				 } else {
					 ir.translate(point.x() + pixmap.width() + 2, 0);
				 }

				 // left-align text if there is
				 if (!button->text.isEmpty()) {
					 tf |= Qt::AlignLeft;
				 }

			 } else {
				 tf |= Qt::AlignHCenter;
			 }

			 ir.translate(buttonShift);

			 if (button->features & QStyleOptionButton::HasMenu) {
				 ir = ir.adjusted(0, 0, -pixelMetric(PM_MenuButtonIndicator, button, widget), 0);
			 }

			 drawItemText(p, ir, tf, option->palette, (button->state & State_Enabled), button->text, QPalette::ButtonText);
			 return;
		 }
	}
	if (ce == CE_RadioButton || ce == CE_CheckBox) {
		if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {
			bool isRadio = (ce == CE_RadioButton);

			QStyleOptionButton subopt = *btn;
			subopt.rect = subElementRect(isRadio ? SE_RadioButtonIndicator : SE_CheckBoxIndicator, btn, widget);
			proxy()->drawPrimitive(isRadio ? PE_IndicatorRadioButton : PE_IndicatorCheckBox, &subopt, p, widget);
			subopt.rect = subElementRect(isRadio ? SE_RadioButtonContents : SE_CheckBoxContents, btn, widget);

			drawControl(isRadio ? CE_RadioButtonLabel : CE_CheckBoxLabel, &subopt, p, widget);

			if (btn->state & State_HasFocus) {
				QStyleOptionFocusRect fropt;
				fropt.QStyleOption::operator=(*btn);
				fropt.rect = subElementRect(isRadio ? SE_RadioButtonFocusRect : SE_CheckBoxFocusRect, btn, widget);
				drawPrimitive(PE_FrameFocusRect, &fropt, p, widget);
			}
			return;
		}
	}
	if (ce == CE_RadioButtonLabel || ce == CE_CheckBoxLabel) {
		if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {
			uint alignment = visualAlignment(btn->direction, Qt::AlignLeft | Qt::AlignVCenter);

			if (!styleHint(SH_UnderlineShortcut, btn, widget)) {
				alignment |= Qt::TextHideMnemonic;
			}
			QPixmap pix;
			QRect textRect = btn->rect;
			if (!btn->icon.isNull()) {
				pix = btn->icon.pixmap(btn->iconSize, btn->state & State_Enabled ? QIcon::Normal : QIcon::Disabled);
				drawItemPixmap(p, btn->rect, alignment, pix);
				if (btn->direction == Qt::RightToLeft) {
					textRect.setRight(textRect.right() - btn->iconSize.width() - 4);
				} else {
					textRect.setLeft(textRect.left() + btn->iconSize.width() + 4);
				}
			}
			if (!btn->text.isEmpty()){
				drawItemText(p, textRect, alignment | Qt::TextShowMnemonic, btn->palette, btn->state & State_Enabled, btn->text, QPalette::ButtonText);
			}
			return;
		}
	}
  #endif
	if (ce == CE_ShapedFrame) {
		if (qobject_cast<QAbstractItemView const *>(widget)) {
			p->fillRect(option->rect, option->palette.color(QPalette::Window));
			drawFrame(p, option->rect, option->palette.color(QPalette::Dark), option->palette.color(QPalette::Light));
		}
		return;
	}
	if (ce == CE_PushButtonBevel) {
		enum PartID {
			BP_PUSHBUTTON,
			BP_COMMANDLINK,
		};

		enum StateID {
			PBS_DISABLED,
			PBS_PRESSED,
			PBS_HOT,
			PBS_DEFAULTED,
			PBS_NORMAL,
		};

		if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {
			int state = option->state;
			State flags = option->state;
			int partId = BP_PUSHBUTTON;
			int stateId = 0;
			if (btn->features & QStyleOptionButton::CommandLinkButton) {
				partId = BP_COMMANDLINK;
			}
			if (!(flags & State_Enabled) && !(btn->features & QStyleOptionButton::Flat)) {
				stateId = PBS_DISABLED;
			} else if (flags & (State_Sunken | State_On)) {
				stateId = PBS_PRESSED;
			} else if (flags & State_MouseOver) {
				stateId = PBS_HOT;
			} else if (btn->features & QStyleOptionButton::DefaultButton && (state & State_Active)) {
				stateId = PBS_DEFAULTED;
			} else {
				stateId = PBS_NORMAL;
			}

			drawButton(p, option);

			if (btn->features & QStyleOptionButton::HasMenu) {
				int mbiw = 0, mbih = 0;
				QRect r = subElementRect(SE_PushButtonContents, option, 0);
				QStyleOptionButton newBtn = *btn;
				r = QRect(r.right() - mbiw - 2, option->rect.top() + (option->rect.height()/2) - (mbih/2), mbiw + 1, mbih + 1);
				newBtn.rect = QStyle::visualRect(option->direction, option->rect, r);
				drawPrimitive(PE_IndicatorArrowDown, &newBtn, p, widget);
			}
		}
		return;
	}
	if (ce == CE_MenuBarEmptyArea) {
		drawMenuBarBG(p, option, widget);
		return;
	}
	if (ce == CE_MenuBarItem) {
		drawMenuBarBG(p, option, widget);
		if (option->state & State_Selected) {
			drawSelectedMenuFrame(option, p, option->rect, widget, false);
		}
		if (const QStyleOptionMenuItem *mbi = qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
			QPalette::ColorRole textRole = disabled ? QPalette::Text : QPalette::ButtonText;
			QPixmap pix = mbi->icon.pixmap(pixelMetric(PM_SmallIconSize, option, widget), QIcon::Normal);
			uint alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
			if (!styleHint(SH_UnderlineShortcut, mbi, widget)) {
				alignment |= Qt::TextHideMnemonic;
			}
			if (!pix.isNull()) {
				drawItemPixmap(p, mbi->rect, alignment, pix);
			} else {
				drawItemText(p, mbi->rect, alignment, mbi->palette, mbi->state & State_Enabled, mbi->text, textRole);
			}
		}
		return;
	}
	if (ce == CE_MenuEmptyArea) {
		return;
	}
	if (ce == CE_MenuItem) {
		if (const QStyleOptionMenuItem *menuitem = qstyleoption_cast<const QStyleOptionMenuItem *>(option)) {
			int stateId = 0;
			// windows always has a check column, regardless whether we have an icon or not
			int checkcol = 25;// / QWindowsXPStylePrivate::devicePixelRatio(widget);
			const int gutterWidth = 3;// / QWindowsXPStylePrivate::devicePixelRatio(widget);
			QRect rect = option->rect;

			//draw vertical menu line
			if (option->direction == Qt::LeftToRight) {
				checkcol += rect.x();
			}

			int x, y, w, h;
			menuitem->rect.getRect(&x, &y, &w, &h);
			int tab = menuitem->tabWidth;
			bool dis = !(menuitem->state & State_Enabled);
			bool checked = menuitem->checkType != QStyleOptionMenuItem::NotCheckable
					? menuitem->checked : false;
			bool act = menuitem->state & State_Selected;

			if (menuitem->menuItemType == QStyleOptionMenuItem::Separator) {
				QRect r = option->rect.adjusted(2, 0, -2, 0);
				drawGutter(p, r);
				return;
			}

			QRect vCheckRect = visualRect(option->direction, menuitem->rect, QRect(menuitem->rect.x(), menuitem->rect.y(), checkcol - (gutterWidth + menuitem->rect.x()), menuitem->rect.height()));

			if (act) {
				stateId = dis ? MBI_DISABLED : MBI_HOT;
				drawSelectedMenuFrame(option, p, option->rect, widget, false);
			}

			if (checked) {
			}

			if (!menuitem->icon.isNull()) {
				QIcon::Mode mode = dis ? QIcon::Disabled : QIcon::Normal;
				if (act && !dis) {
					mode = QIcon::Active;
				}
				QPixmap pixmap;
				if (checked) {
					pixmap = menuitem->icon.pixmap(pixelMetric(PM_SmallIconSize, option, widget), mode, QIcon::On);
				} else {
					pixmap = menuitem->icon.pixmap(pixelMetric(PM_SmallIconSize, option, widget), mode);
				}
				const int pixw = pixmap.width() / pixmap.devicePixelRatio();
				const int pixh = pixmap.height() / pixmap.devicePixelRatio();
				QRect pmr(0, 0, pixw, pixh);
				pmr.moveCenter(vCheckRect.center());
				p->setPen(menuitem->palette.text().color());
				p->drawPixmap(pmr.topLeft(), pixmap);
			}

			p->setPen(menuitem->palette.buttonText().color());

			const QColor textColor = menuitem->palette.text().color();
			if (dis) {
				p->setPen(textColor);
			}

			int xm = windowsItemFrame + checkcol + windowsItemHMargin + (gutterWidth - menuitem->rect.x()) - 1;
			int xpos = menuitem->rect.x() + xm;
			QRect textRect(xpos, y + windowsItemVMargin, w - xm - windowsRightBorder - tab + 1, h - 2 * windowsItemVMargin);
			QRect vTextRect = visualRect(option->direction, menuitem->rect, textRect);
			QString s = menuitem->text;
			if (!s.isEmpty()) {    // draw text
				p->save();
				int t = s.indexOf(QLatin1Char('\t'));
				int text_flags = Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
				if (!styleHint(SH_UnderlineShortcut, menuitem, widget))
					text_flags |= Qt::TextHideMnemonic;
				text_flags |= Qt::AlignLeft;
				if (t >= 0) {
					QRect vShortcutRect = visualRect(option->direction, menuitem->rect,
													 QRect(textRect.topRight(), QPoint(menuitem->rect.right(), textRect.bottom())));
					p->drawText(vShortcutRect, text_flags, s.mid(t + 1));
					s = s.left(t);
				}
				QFont font = menuitem->font;
				if (menuitem->menuItemType == QStyleOptionMenuItem::DefaultItem)
					font.setBold(true);
				p->setFont(font);
				p->setPen(textColor);
				p->drawText(vTextRect, text_flags, s.left(t));
				p->restore();
			}
			if (menuitem->menuItemType == QStyleOptionMenuItem::SubMenu) {// draw sub menu arrow
				int dim = (h - 2 * windowsItemFrame) / 2;
				PrimitiveElement arrow;
				arrow = (option->direction == Qt::RightToLeft) ? PE_IndicatorArrowLeft : PE_IndicatorArrowRight;
				xpos = x + w - windowsArrowHMargin - windowsItemFrame - dim;
				QRect  vSubMenuRect = visualRect(option->direction, menuitem->rect, QRect(xpos, y + h / 2 - dim / 2, dim, dim));
				QStyleOptionMenuItem newMI = *menuitem;
				newMI.rect = vSubMenuRect;
				newMI.state = dis ? State_None : State_Enabled;
				drawPrimitive(arrow, &newMI, p, widget);
			}
		}
		return;
	}
	if (ce == CE_TabBarTabShape) {
		if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
			bool rtlHorTabs = (tab->direction == Qt::RightToLeft && (tab->shape == QTabBar::RoundedNorth || tab->shape == QTabBar::RoundedSouth));
			bool selected = tab->state & State_Selected;
			bool lastTab = ((!rtlHorTabs && tab->position == QStyleOptionTab::End) || (rtlHorTabs && tab->position == QStyleOptionTab::Beginning));
			bool firstTab = ((!rtlHorTabs && tab->position == QStyleOptionTab::Beginning) || (rtlHorTabs && tab->position == QStyleOptionTab::End));
			bool onlyOne = tab->position == QStyleOptionTab::OnlyOneTab;
			bool previousSelected = ((!rtlHorTabs && tab->selectedPosition == QStyleOptionTab::PreviousIsSelected) || (rtlHorTabs && tab->selectedPosition == QStyleOptionTab::NextIsSelected));
			bool nextSelected = ((!rtlHorTabs && tab->selectedPosition == QStyleOptionTab::NextIsSelected) || (rtlHorTabs && tab->selectedPosition == QStyleOptionTab::PreviousIsSelected));
			int tabBarAlignment = styleHint(SH_TabBar_Alignment, tab, widget);
			bool leftAligned = (!rtlHorTabs && tabBarAlignment == Qt::AlignLeft) || (rtlHorTabs && tabBarAlignment == Qt::AlignRight);
			bool rightAligned = (!rtlHorTabs && tabBarAlignment == Qt::AlignRight) || (rtlHorTabs && tabBarAlignment == Qt::AlignLeft);

			QColor light = tab->palette.light().color();
			QColor dark = tab->palette.dark().color();
			QColor shadow = tab->palette.shadow().color();
			int borderThinkness = pixelMetric(PM_TabBarBaseOverlap, tab, widget);
			if (selected) {
				borderThinkness /= 2;
			}
			QRect r2(option->rect);
			int x1 = r2.left();
			int x2 = r2.right();
			int y1 = r2.top();
			int y2 = r2.bottom();
			switch (tab->shape) {
			default:
				{
					p->save();

					QRect rect(tab->rect);
					int tabOverlap = onlyOne ? 0 : pixelMetric(PM_TabBarTabOverlap, option, widget);

					if (!selected) {
						switch (tab->shape) {
						case QTabBar::TriangularNorth:
							rect.adjust(0, 0, 0, -tabOverlap);
							if(!selected) rect.adjust(1, 1, -1, 0);
							break;
						case QTabBar::TriangularSouth:
							rect.adjust(0, tabOverlap, 0, 0);
							if(!selected) rect.adjust(1, 0, -1, -1);
							break;
						case QTabBar::TriangularEast:
							rect.adjust(tabOverlap, 0, 0, 0);
							if(!selected) rect.adjust(0, 1, -1, -1);
							break;
						case QTabBar::TriangularWest:
							rect.adjust(0, 0, -tabOverlap, 0);
							if(!selected) rect.adjust(1, 1, 0, -1);
							break;
						default:
							break;
						}
					}

					p->setPen(QPen(tab->palette.foreground(), 0));
					if (selected) {
						p->setBrush(tab->palette.background());
					} else {
						if (widget && widget->parentWidget()) {
							p->setBrush(widget->parentWidget()->palette().shadow());
						} else {
							p->setBrush(tab->palette.shadow());
						}
					}

					int y;
					int x;
					QPolygon a(10);
					switch (tab->shape) {
					case QTabBar::TriangularNorth:
					case QTabBar::TriangularSouth:
						{
							a.setPoint(0, 0, -1);
							a.setPoint(1, 0, 0);
							y = rect.height() - 2;
							x = y / 3;
							a.setPoint(2, x++, y - 1);
							++x;
							a.setPoint(3, x++, y++);
							a.setPoint(4, x, y);

							int i;
							int right = rect.width() - 1;
							for (i = 0; i < 5; ++i) {
								a.setPoint(9 - i, right - a.point(i).x(), a.point(i).y());
							}
							if (tab->shape == QTabBar::TriangularNorth) {
								for (i = 0; i < 10; ++i) {
									a.setPoint(i, a.point(i).x(), rect.height() - 1 - a.point(i).y());
								}
							}

							a.translate(rect.left(), rect.top());
							p->setRenderHint(QPainter::Antialiasing);
							p->translate(0, 0.5);

							QPainterPath path;
							path.addPolygon(a);
							p->drawPath(path);
							break;
						}
					case QTabBar::TriangularEast:
					case QTabBar::TriangularWest:
						{
							a.setPoint(0, -1, 0);
							a.setPoint(1, 0, 0);
							x = rect.width() - 2;
							y = x / 3;
							a.setPoint(2, x - 1, y++);
							++y;
							a.setPoint(3, x++, y++);
							a.setPoint(4, x, y);
							int i;
							int bottom = rect.height() - 1;
							for (i = 0; i < 5; ++i) {
								a.setPoint(9 - i, a.point(i).x(), bottom - a.point(i).y());
							}
							if (tab->shape == QTabBar::TriangularWest) {
								for (i = 0; i < 10; ++i) {
									a.setPoint(i, rect.width() - 1 - a.point(i).x(), a.point(i).y());
								}
							}
							a.translate(rect.left(), rect.top());
							p->setRenderHint(QPainter::Antialiasing);
							p->translate(0.5, 0);
							QPainterPath path;
							path.addPolygon(a);
							p->drawPath(path);
							break;
						}
					default:
						break;
					}
					p->restore();
				}
				break;
			case QTabBar::RoundedNorth:
				{
					if (!selected) {
						y1 += 2;
						x1 += onlyOne || firstTab ? borderThinkness : 0;
						x2 -= onlyOne || lastTab ? borderThinkness : 0;
					}

					p->fillRect(QRect(x1 + 1, y1 + 1, (x2 - x1) - 1, (y2 - y1) - 2), tab->palette.background());

					// Delete border
					if (selected) {
						p->fillRect(QRect(x1,y2-1,x2-x1,1), tab->palette.background());
						p->fillRect(QRect(x1,y2,x2-x1,1), tab->palette.background());
					}
					// Left
					if (firstTab || selected || onlyOne || !previousSelected) {
						p->setPen(light);
						p->drawLine(x1, y1 + 2, x1, y2 - ((onlyOne || firstTab) && selected && leftAligned ? 0 : borderThinkness));
						p->drawPoint(x1 + 1, y1 + 1);
					}
					// Top
					{
						int beg = x1 + (previousSelected ? 0 : 2);
						int end = x2 - (nextSelected ? 0 : 2);
						p->setPen(light);
						p->drawLine(beg, y1, end, y1);
					}
					// Right
					if (lastTab || selected || onlyOne || !nextSelected) {
						p->setPen(shadow);
						p->drawLine(x2, y1 + 2, x2, y2 - ((onlyOne || lastTab) && selected && rightAligned ? 0 : borderThinkness));
						p->drawPoint(x2 - 1, y1 + 1);
						p->setPen(dark);
						p->drawLine(x2 - 1, y1 + 2, x2 - 1, y2 - ((onlyOne || lastTab) && selected && rightAligned ? 0 : borderThinkness));
					}
					break;
				}
			case QTabBar::RoundedSouth:
				{
					if (!selected) {
						y2 -= 2;
						x1 += firstTab ? borderThinkness : 0;
						x2 -= lastTab ? borderThinkness : 0;
					}

					p->fillRect(QRect(x1 + 1, y1 + 2, (x2 - x1) - 1, (y2 - y1) - 1), tab->palette.background());

					// Delete border
					if (selected) {
						p->fillRect(QRect(x1, y1 + 1, (x2 - 1)-x1, 1), tab->palette.background());
						p->fillRect(QRect(x1, y1, (x2 - 1)-x1, 1), tab->palette.background());
					}
					// Left
					if (firstTab || selected || onlyOne || !previousSelected) {
						p->setPen(light);
						p->drawLine(x1, y2 - 2, x1, y1 + ((onlyOne || firstTab) && selected && leftAligned ? 0 : borderThinkness));
						p->drawPoint(x1 + 1, y2 - 1);
					}
					// Bottom
					{
						int beg = x1 + (previousSelected ? 0 : 2);
						int end = x2 - (nextSelected ? 0 : 2);
						p->setPen(shadow);
						p->drawLine(beg, y2, end, y2);
						p->setPen(dark);
						p->drawLine(beg, y2 - 1, end, y2 - 1);
					}
					// Right
					if (lastTab || selected || onlyOne || !nextSelected) {
						p->setPen(shadow);
						p->drawLine(x2, y2 - 2, x2, y1 + ((onlyOne || lastTab) && selected && rightAligned ? 0 : borderThinkness));
						p->drawPoint(x2 - 1, y2 - 1);
						p->setPen(dark);
						p->drawLine(x2 - 1, y2 - 2, x2 - 1, y1 + ((onlyOne || lastTab) && selected && rightAligned ? 0 : borderThinkness));
					}
					break;
				}
			case QTabBar::RoundedWest:
				{
					if (!selected) {
						x1 += 2;
						y1 += firstTab ? borderThinkness : 0;
						y2 -= lastTab ? borderThinkness : 0;
					}

					p->fillRect(QRect(x1 + 1, y1 + 1, (x2 - x1) - 2, (y2 - y1) - 1), tab->palette.background());

					// Delete border
					if (selected) {
						p->fillRect(QRect(x2 - 1, y1, 1, y2-y1), tab->palette.background());
						p->fillRect(QRect(x2, y1, 1, y2-y1), tab->palette.background());
					}
					// Top
					if (firstTab || selected || onlyOne || !previousSelected) {
						p->setPen(light);
						p->drawLine(x1 + 2, y1, x2 - ((onlyOne || firstTab) && selected && leftAligned ? 0 : borderThinkness), y1);
						p->drawPoint(x1 + 1, y1 + 1);
					}
					// Left
					{
						int beg = y1 + (previousSelected ? 0 : 2);
						int end = y2 - (nextSelected ? 0 : 2);
						p->setPen(light);
						p->drawLine(x1, beg, x1, end);
					}
					// Bottom
					if (lastTab || selected || onlyOne || !nextSelected) {
						p->setPen(shadow);
						p->drawLine(x1 + 3, y2, x2 - ((onlyOne || lastTab) && selected && rightAligned ? 0 : borderThinkness), y2);
						p->drawPoint(x1 + 2, y2 - 1);
						p->setPen(dark);
						p->drawLine(x1 + 3, y2 - 1, x2 - ((onlyOne || lastTab) && selected && rightAligned ? 0 : borderThinkness), y2 - 1);
						p->drawPoint(x1 + 1, y2 - 1);
						p->drawPoint(x1 + 2, y2);
					}
					break;
				}
			case QTabBar::RoundedEast:
				{
					if (!selected) {
						x2 -= 2;
						y1 += firstTab ? borderThinkness : 0;
						y2 -= lastTab ? borderThinkness : 0;
					}

					p->fillRect(QRect(x1 + 2, y1 + 1, (x2 - x1) - 1, (y2 - y1) - 1), tab->palette.background());

					// Delete border
					if (selected) {
						p->fillRect(QRect(x1 + 1, y1, 1, (y2 - 1)-y1),tab->palette.background());
						p->fillRect(QRect(x1, y1, 1, (y2-1)-y1), tab->palette.background());
					}
					// Top
					if (firstTab || selected || onlyOne || !previousSelected) {
						p->setPen(light);
						p->drawLine(x2 - 2, y1, x1 + ((onlyOne || firstTab) && selected && leftAligned ? 0 : borderThinkness), y1);
						p->drawPoint(x2 - 1, y1 + 1);
					}
					// Right
					{
						int beg = y1 + (previousSelected ? 0 : 2);
						int end = y2 - (nextSelected ? 0 : 2);
						p->setPen(shadow);
						p->drawLine(x2, beg, x2, end);
						p->setPen(dark);
						p->drawLine(x2 - 1, beg, x2 - 1, end);
					}
					// Bottom
					if (lastTab || selected || onlyOne || !nextSelected) {
						p->setPen(shadow);
						p->drawLine(x2 - 2, y2, x1 + ((onlyOne || lastTab) && selected && rightAligned ? 0 : borderThinkness), y2);
						p->drawPoint(x2 - 1, y2 - 1);
						p->setPen(dark);
						p->drawLine(x2 - 2, y2 - 1, x1 + ((onlyOne || lastTab) && selected && rightAligned ? 0 : borderThinkness), y2 - 1);
					}
					break;
				}
			}
		}
		return;
	}
	if (ce == CE_ProgressBarGroove || ce == CE_ProgressBarContents) {
		if (const QStyleOptionProgressBar *bar = qstyleoption_cast<const QStyleOptionProgressBar *>(option)) {

			QColor color(0, 128, 255);

			QPixmap const *pixmap = &progress_horz;

			QString key = "horz";

			bool horz = true;
			if (bar->orientation == Qt::Vertical) {
				pixmap = &progress_vert;
				key = "vert";
				horz = false;
			}

			p->save();

			int x = option->rect.x();
			int y = option->rect.y();
			int w = option->rect.width();
			int h = option->rect.height();
			if (ce == CE_ProgressBarContents) {
				int len = bar->progress - bar->minimum;
				int div = bar->maximum - bar->minimum;
				bool inv = bar->invertedAppearance;
				if (horz) {
					int v = div > 0 ? (w * len / div) : 0;
					if (inv) {
						x = x + w - v;
					}
					w = v;
				} else {
					int v = div > 0 ? (h * len / div) : 0;
					if (!inv) {
						y = y + h - v;
					}
					h = v;
				}
				QRect r(x, y, w, h);
				p->setClipRect(r);
				p->fillRect(option->rect, color);
			}
			QPixmap pm;
			QSize size(w, h);
			key = pixmapkey("progress_bar", key, size);
			if (!QPixmapCache::find(key, &pm)) {
				pm = pixmap->scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
				QPixmapCache::insert(key, pm);
			}
			p->setOpacity(0.5);
			p->drawPixmap(option->rect, pm, pm.rect());

			p->restore();
		}
		return;
	}
	if (ce == CE_HeaderSection || ce == CE_HeaderEmptyArea) {
		int x = option->rect.x();
		int y = option->rect.y();
		int w = option->rect.width();
		int h = option->rect.height();
		QLinearGradient gradient;
		gradient.setStart(x, y);
		gradient.setFinalStop(x, y + h / 4);
		gradient.setColorAt(0, option->palette.color(QPalette::Light));
		gradient.setColorAt(1, option->palette.color(QPalette::Window));
		p->fillRect(x, y, w, h, gradient);
		p->fillRect(x, y + h - 1, w, 1, option->palette.color(QPalette::Dark));
		if (ce == CE_HeaderSection) {
			if (option->state & QStyle::State_MouseOver) {
				p->save();
				p->fillRect(x, y, w, h, QColor(255, 255, 255, 32));
				p->restore();
			}
		}
		return;
	}
#ifdef Q_OS_MAC
	if (ce == CE_DockWidgetTitle) {
		if (const QStyleOptionDockWidget *dwOpt = qstyleoption_cast<const QStyleOptionDockWidget *>(option)) {
			const QDockWidget *dockWidget = qobject_cast<const QDockWidget *>(widget);
			QRect rect = option->rect;
			if (dockWidget && dockWidget->isFloating()) {
				QProxyStyle::drawControl(ce, option, p, widget);
				return;
			}

			const QStyleOptionDockWidgetV2 *v2 = qstyleoption_cast<const QStyleOptionDockWidgetV2*>(dwOpt);
			bool verticalTitleBar = v2 == 0 ? false : v2->verticalTitleBar;

			if (verticalTitleBar) {
				rect.setSize(rect.size().transposed());

				p->translate(rect.left() - 1, rect.top() + rect.width());
				p->rotate(-90);
				p->translate(-rect.left() + 1, -rect.top());
			}

			p->setBrush(option->palette.background().color().darker(110));
			p->setPen(option->palette.background().color().darker(130));
			p->drawRect(rect.adjusted(0, 1, -1, -3));

			int buttonMargin = 4;
			int mw = proxy()->pixelMetric(QStyle::PM_DockWidgetTitleMargin, dwOpt, widget);
			int fw = proxy()->pixelMetric(PM_DockWidgetFrameWidth, dwOpt, widget);
			const QDockWidget *dw = qobject_cast<const QDockWidget *>(widget);
			bool isFloating = dw != 0 && dw->isFloating();

			QRect r = option->rect.adjusted(0, 2, -1, -3);
			QRect titleRect = r;

			if (dwOpt->closable) {
				QSize sz = proxy()->standardIcon(QStyle::SP_TitleBarCloseButton, dwOpt, widget).actualSize(QSize(10, 10));
				titleRect.adjust(0, 0, -sz.width() - mw - buttonMargin, 0);
			}

			if (dwOpt->floatable) {
				QSize sz = proxy()->standardIcon(QStyle::SP_TitleBarMaxButton, dwOpt, widget).actualSize(QSize(10, 10));
				titleRect.adjust(0, 0, -sz.width() - mw - buttonMargin, 0);
			}

			if (isFloating) {
				titleRect.adjust(0, -fw, 0, 0);
				if (widget != 0 && widget->windowIcon().cacheKey() != QApplication::windowIcon().cacheKey())
					titleRect.adjust(titleRect.height() + mw, 0, 0, 0);
			} else {
				titleRect.adjust(mw, 0, 0, 0);
				if (!dwOpt->floatable && !dwOpt->closable)
					titleRect.adjust(0, 0, -mw, 0);
			}
			if (!verticalTitleBar)
				titleRect = visualRect(dwOpt->direction, r, titleRect);

			if (!dwOpt->title.isEmpty()) {
				QString titleText = p->fontMetrics().elidedText(dwOpt->title, Qt::ElideRight, verticalTitleBar ? titleRect.height() : titleRect.width());
				const int indent = 4;
				int align = Qt::AlignRight | Qt::AlignVCenter;
				drawItemText(p, rect.adjusted(indent + 1, 1, -indent - 1, -1),
							 align, dwOpt->palette,
							 dwOpt->state & State_Enabled, titleText,
							 QPalette::WindowText);
			}
		}
		return;
	}
#endif // Q_OS_MAC
#ifdef Q_OS_LINUX
	if (ce == CE_Splitter) {
		p->fillRect(option->rect, option->palette.color(QPalette::Window));
		return;
	}
#endif
//	qDebug() << ce;
	QProxyStyle::drawControl(ce, option, p, widget);
}

void DarkStyle::drawComplexControl(ComplexControl cc, const QStyleOptionComplex *option, QPainter *p, const QWidget *widget) const
{
	if (cc == QStyle::CC_ComboBox) {
		if (const QStyleOptionComboBox *cmb = qstyleoption_cast<const QStyleOptionComboBox *>(option)) {
			SubControls sub = option->subControls;
			if (cmb->editable) {
				int partId = 0;
//				int stateId = 0;
#define EP_EDITBORDER_NOSCROLL      6
#define EP_EDITBORDER_HVSCROLL      9
				State flags = option->state;
				if (sub & SC_ComboBoxEditField) {
					partId = EP_EDITBORDER_NOSCROLL;
//					if (!(flags & State_Enabled))
//						stateId = ETS_DISABLED;
//					else if (flags & State_MouseOver)
//						stateId = ETS_HOT;
//					else if (flags & State_HasFocus)
//						stateId = ETS_FOCUSED;
//					else
//						stateId = ETS_NORMAL;

//					XPThemeData theme(widget, painter,
//									  QWindowsXPStylePrivate::EditTheme,
//									  partId, stateId, r);

//					d->drawBackground(theme);
					p->fillRect(option->rect, option->palette.color(QPalette::Dark));
					drawFrame(p, option->rect, option->palette.color(QPalette::Shadow), option->palette.color(QPalette::Light));
//					drawFrame(p, option->rect, option->palette.color(QPalette::Dark), option->palette.color(QPalette::Light));
				}
				if (sub & SC_ComboBoxArrow) {
					QRect subRect = subControlRect(CC_ComboBox, option, SC_ComboBoxArrow, widget);
//					XPThemeData theme(widget, painter, QWindowsXPStylePrivate::ComboboxTheme);
//					theme.rect = subRect;
//					partId = option->direction == Qt::RightToLeft ? CP_DROPDOWNBUTTONLEFT : CP_DROPDOWNBUTTONRIGHT;

//					if (!(cmb->state & State_Enabled))
//						stateId = CBXS_DISABLED;
//					else if (cmb->state & State_Sunken || cmb->state & State_On)
//						stateId = CBXS_PRESSED;
//					else if (cmb->state & State_MouseOver && option->activeSubControls & SC_ComboBoxArrow)
//						stateId = CBXS_HOT;
//					else
//						stateId = CBXS_NORMAL;

//					theme.partId = partId;
//					theme.stateId = stateId;
//					d->drawBackground(theme);
					QStyleOptionComplex opt = *option;
//					p->fillRect(subRect, Qt::blue);
					opt.rect = subRect;
					drawToolButton(p, &opt);
					opt.rect = QRect(subRect.right() - 4, subRect.y() + subRect.height() / 2 - 1, 2, 2);
					drawPrimitive(PE_IndicatorArrowDown, &opt, p, widget);
				}

			} else {
				if (sub & SC_ComboBoxFrame) {
					QStyleOptionButton btn;
					btn.QStyleOption::operator=(*option);
					btn.rect = option->rect.adjusted(-1, -1, 1, 1);
					if (sub & SC_ComboBoxArrow) {
						btn.features = QStyleOptionButton::HasMenu;
					}
					drawControl(QStyle::CE_PushButton, &btn, p, widget);
				}
			}
		}
		return;
	}
	if (cc == QStyle::CC_ToolButton) {
		QStyle::State flags = option->state;
		if (const QStyleOptionToolButton *toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(option)) {
			QRect button, menuarea;
			button = subControlRect(cc, toolbutton, SC_ToolButton, widget);
			menuarea = subControlRect(cc, toolbutton, SC_ToolButtonMenu, widget);

			State bflags = toolbutton->state & ~State_Sunken;
			State mflags = bflags;
			bool autoRaise = flags & State_AutoRaise;
			if (autoRaise) {
				if (!(bflags & State_MouseOver) || !(bflags & State_Enabled)) {
					bflags &= ~State_Raised;
				}
			}

			if (toolbutton->state & State_Sunken) {
				if (toolbutton->activeSubControls & SC_ToolButton) {
					bflags |= State_Sunken;
					mflags |= State_MouseOver | State_Sunken;
				} else if (toolbutton->activeSubControls & SC_ToolButtonMenu) {
					mflags |= State_Sunken;
					bflags |= State_MouseOver;
				}
			}

			QStyleOption tool(0);
			tool.palette = toolbutton->palette;
			if (toolbutton->subControls & SC_ToolButton) {
				if (flags & (State_Sunken | State_On | State_Raised) || !autoRaise) {
					tool.rect = option->rect;
					tool.state = bflags;
					drawToolButton(p, &tool);
				}
			}

			if (toolbutton->state & State_HasFocus) {
				QStyleOptionFocusRect fr;
				fr.QStyleOption::operator=(*toolbutton);
				fr.rect.adjust(3, 3, -3, -3);
				if (toolbutton->features & QStyleOptionToolButton::MenuButtonPopup) {
					fr.rect.adjust(0, 0, -pixelMetric(QStyle::PM_MenuButtonIndicator, toolbutton, widget), 0);
				}
				drawPrimitive(PE_FrameFocusRect, &fr, p, widget);
			}
			QStyleOptionToolButton label = *toolbutton;
			label.state = bflags;
			int fw = 2;
			if (!autoRaise)
				label.state &= ~State_Sunken;
			label.rect = button.adjusted(fw, fw, -fw, -fw);
			drawControl(CE_ToolButtonLabel, &label, p, widget);

			if (toolbutton->subControls & SC_ToolButtonMenu) {
				tool.rect = menuarea;
				tool.state = mflags;
				if (autoRaise) {
					drawPrimitive(PE_IndicatorButtonDropDown, &tool, p, widget);
				} else {
					tool.state = mflags;
					menuarea.adjust(-2, 0, 0, 0);
					// Draw menu button
					if ((bflags & State_Sunken) != (mflags & State_Sunken)){
						p->save();
						p->setClipRect(menuarea);
						tool.rect = option->rect;
						drawToolButton(p, &tool);
						p->restore();
					}
					// Draw arrow
					p->save();
					p->setPen(option->palette.dark().color());
					p->drawLine(menuarea.left(), menuarea.top() + 3, menuarea.left(), menuarea.bottom() - 3);
					p->setPen(option->palette.light().color());
					p->drawLine(menuarea.left() - 1, menuarea.top() + 3, menuarea.left() - 1, menuarea.bottom() - 3);

					tool.rect = menuarea.adjusted(2, 3, -2, -1);
					drawPrimitive(PE_IndicatorArrowDown, &tool, p, widget);
					p->restore();
				}
			} else if (toolbutton->features & QStyleOptionToolButton::HasMenu) {
				int mbi = pixelMetric(PM_MenuButtonIndicator, toolbutton, widget);
				QRect ir = toolbutton->rect;
				QStyleOptionToolButton newBtn = *toolbutton;
				newBtn.rect = QRect(ir.right() + 4 - mbi, ir.height() - mbi + 4, mbi - 5, mbi - 5);
				drawPrimitive(PE_IndicatorArrowDown, &newBtn, p, widget);
			}
		}
		return;
	}
	if (cc == QStyle::CC_ScrollBar) {
		bool ishorz = (option->state & State_Horizontal);
		ScrollBarTextures *tx = const_cast<ScrollBarTextures *>(ishorz ? &hsb : &vsb);

		int extent = (ishorz ? tx->slider.im_normal.height() : tx->slider.im_normal.width()) - 2;

		auto DrawNinePatchImage2 = [&](QImage const &image, QRect const &r){
			int w, h;
			if (ishorz) {
				h = extent;
				int d = r.height();
				w = (d == 0) ? 0 : (h * r.width() / d);
			} else {
				w = extent;
				int d = r.width();
				h = (d == 0) ? 0 : (w * r.height() / d);
			}
			drawNinePatchImage(p, image, r, w, h);
		};

		p->fillRect(option->rect, QColor(64, 64, 64));
		p->setRenderHint(QPainter::Antialiasing);

		{
#ifdef Q_OS_MAC
			QRect r = option->rect;
#else
			QRect r1 = subControlRect(CC_ScrollBar, option, SC_ScrollBarAddPage, widget);
			QRect r2 = subControlRect(CC_ScrollBar, option, SC_ScrollBarSubPage, widget);
			QRect r = r1.united(r2);
#endif
			DrawNinePatchImage2(tx->page_bg, r);
		}

#ifndef Q_OS_MAC // macOSのスクロールバーには、ラインステップボタンが無い
		{
			QRect r = subControlRect(CC_ScrollBar, option, SC_ScrollBarSubLine, widget);
			DrawNinePatchImage2(tx->page_bg, r);
		}
		{
			QRect r = subControlRect(CC_ScrollBar, option, SC_ScrollBarAddLine, widget);
			DrawNinePatchImage2(tx->page_bg, r);
		}

		auto DrawAddSubButton = [&](ButtonImages const &ims, SubControl subctl){
			QRect r = subControlRect(CC_ScrollBar, option, subctl, widget);
			QPixmap pm;;
			if (!(option->activeSubControls & subctl)) {
				pm = pixmapFromImage(ims.im_normal, r.size());
				p->drawPixmap(r.topLeft(), pm);
			} else {
				pm = pixmapFromImage(ims.im_hover, r.size());
				p->drawPixmap(r.topLeft(), pm);
			}
		};
		DrawAddSubButton(tx->sub_line, SC_ScrollBarSubLine);
		DrawAddSubButton(tx->add_line, SC_ScrollBarAddLine);
#endif

		{
			QRect r = subControlRect(CC_ScrollBar, option, SC_ScrollBarSlider, widget);
			int w, h;
			if (ishorz) {
				h = extent;
				int d = r.height();
				w = (d == 0) ? 0 : (h * r.width() / d);
			} else {
				w = extent;
				int d = r.width();
				h = (d == 0) ? 0 : (w * r.height() / d);
			}
#ifdef Q_OS_MAC // macだとズレて見えるので調整する
			if (ishorz) {
				r = r.translated(1, 0);
			} else {
				r = r.translated(0, 1);
			}
#endif
			if (!(option->activeSubControls & SC_ScrollBarSlider)) {
				DrawNinePatchImage2(tx->slider.im_normal, r);
			} else {
				DrawNinePatchImage2(tx->slider.im_hover, r);
			}
		}

		return;
	}
	if (cc == CC_GroupBox) {
		if (const QStyleOptionGroupBox *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(option)) {
			QRect textRect = subControlRect(CC_GroupBox, option, SC_GroupBoxLabel, widget);
			QRect checkBoxRect = subControlRect(CC_GroupBox, option, SC_GroupBoxCheckBox, widget);
			if (groupBox->subControls & QStyle::SC_GroupBoxFrame) {
				QStyleOptionFrame frame;
				frame.QStyleOption::operator=(*groupBox);
				frame.features = groupBox->features;
				frame.lineWidth = groupBox->lineWidth;
				frame.midLineWidth = groupBox->midLineWidth;
				frame.rect = subControlRect(CC_GroupBox, option, SC_GroupBoxFrame, widget);
				p->save();
				QRegion region(groupBox->rect);
				if (!groupBox->text.isEmpty()) {
					bool ltr = groupBox->direction == Qt::LeftToRight;
					QRect finalRect;
					if (groupBox->subControls & QStyle::SC_GroupBoxCheckBox) {
						finalRect = checkBoxRect.united(textRect);
						finalRect.adjust(ltr ? -4 : 0, 0, ltr ? 0 : 4, 0);
					} else {
						finalRect = textRect;
					}
					region -= finalRect;
				}
				p->setClipRegion(region);
				drawPrimitive(PE_FrameGroupBox, &frame, p, widget);
				p->restore();
			}
			// Draw title
			if ((groupBox->subControls & QStyle::SC_GroupBoxLabel) && !groupBox->text.isEmpty()) {
				QColor textColor = groupBox->textColor;
				if (textColor.isValid()) {
					p->setPen(textColor);
				}
				int alignment = int(groupBox->textAlignment);
				if (!styleHint(QStyle::SH_UnderlineShortcut, option, widget)) {
					alignment |= Qt::TextHideMnemonic;
				}

				drawItemText(p, textRect,  Qt::TextShowMnemonic | Qt::AlignHCenter | alignment, groupBox->palette, groupBox->state & State_Enabled, groupBox->text, QPalette::WindowText);

				if (groupBox->state & State_HasFocus) {
					QStyleOptionFocusRect fropt;
					fropt.QStyleOption::operator=(*groupBox);
					fropt.rect = textRect;
					drawPrimitive(PE_FrameFocusRect, &fropt, p, widget);
				}
			}

			// Draw checkbox
			if (groupBox->subControls & SC_GroupBoxCheckBox) {
				QStyleOptionButton box;
				box.QStyleOption::operator=(*groupBox);
				box.rect = checkBoxRect;
				drawPrimitive(PE_IndicatorCheckBox, &box, p, widget);
			}
		}
		return;
	}
	if (cc == CC_Slider) {
		if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
			if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
				QRect groove = subControlRect(CC_Slider, option, SC_SliderGroove, widget);
				QRect handle = subControlRect(CC_Slider, option, SC_SliderHandle, widget);

				bool horizontal = slider->orientation == Qt::Horizontal;
				bool ticksAbove = slider->tickPosition & QSlider::TicksAbove;
				bool ticksBelow = slider->tickPosition & QSlider::TicksBelow;
				QPixmap cache;
				QBrush oldBrush = p->brush();
				QPen oldPen = p->pen();
				QColor shadowAlpha(Qt::black);
				shadowAlpha.setAlpha(10);
				if (option->state & State_HasFocus && option->state & State_KeyboardFocusChange) {
				}

				if ((option->subControls & SC_SliderGroove) && groove.isValid()) {
					QRect rect(0, 0, groove.width(), groove.height());
					QString key = pixmapkey("slider_groove", horizontal ? "horz" : "vert", rect.size());

					QRectF grooveRect;
					double r = std::min(groove.width(), groove.height()) / 2;
					{
						double n = r * 3 / 4;
						grooveRect = rect.adjusted(n, n, -n, -n);
						r -= n;
					}

					// draw background groove
					QPixmap cache;
					if (!QPixmapCache::find(key, cache)) {
						cache = QPixmap(rect.size());
						cache.fill(Qt::transparent);
						QPainter groovePainter(&cache);
						groovePainter.setRenderHint(QPainter::Antialiasing, true);
						groovePainter.translate(0.5, 0.5);
						QLinearGradient gradient;
						if (horizontal) {
							gradient.setStart(grooveRect.center().x(), grooveRect.top());
							gradient.setFinalStop(grooveRect.center().x(), grooveRect.bottom());
						} else {
							gradient.setStart(grooveRect.left(), grooveRect.center().y());
							gradient.setFinalStop(grooveRect.right(), grooveRect.center().y());
						}
						groovePainter.setPen(Qt::NoPen);
						gradient.setColorAt(0, QColor(32, 32, 32));
						gradient.setColorAt(1, QColor(96, 96, 96));
						groovePainter.setBrush(gradient);
						groovePainter.drawRoundedRect(grooveRect, r, r);
						groovePainter.end();
						QPixmapCache::insert(key, cache);
					}
					p->drawPixmap(groove.topLeft(), cache);
				}

				if (option->subControls & SC_SliderTickmarks) {
					int tickSize = pixelMetric(PM_SliderTickmarkOffset, option, widget);
					int available = pixelMetric(PM_SliderSpaceAvailable, slider, widget);
					int interval = slider->tickInterval;
					if (interval <= 0) {
						interval = slider->singleStep;
						if (QStyle::sliderPositionFromValue(slider->minimum, slider->maximum, interval, available) - QStyle::sliderPositionFromValue(slider->minimum, slider->maximum, 0, available) < 3) {
							interval = slider->pageStep;
						}
					}
					if (interval <= 0) {
						interval = 1;
					}

					int v = slider->minimum;
					int len = pixelMetric(PM_SliderLength, slider, widget);
					while (v <= slider->maximum + 1) {
						if (v == slider->maximum + 1 && interval == 1) break;
						const int v_ = qMin(v, slider->maximum);
						int pos = sliderPositionFromValue(slider->minimum, slider->maximum,
														  v_, (horizontal
															   ? slider->rect.width()
															   : slider->rect.height()) - len,
														  slider->upsideDown) + len / 2;
						int extra = 2 - ((v_ == slider->minimum || v_ == slider->maximum) ? 1 : 0);

						if (horizontal) {
							if (ticksAbove) {
								p->setPen(QColor(128, 128, 128));
								p->drawLine(pos, slider->rect.top() + extra, pos, slider->rect.top() + tickSize);
							}
							if (ticksBelow) {
								p->setPen(QColor(0, 0, 0));
								p->drawLine(pos, slider->rect.bottom() - extra, pos, slider->rect.bottom() - tickSize);
							}
						} else {
							if (ticksAbove) {
								p->setPen(QColor(128, 128, 128));
								p->drawLine(slider->rect.left() + extra, pos, slider->rect.left() + tickSize, pos);
							}
							if (ticksBelow) {
								p->setPen(QColor(0, 0, 0));
								p->drawLine(slider->rect.right() - extra, pos, slider->rect.right() - tickSize, pos);
							}
						}
						// in the case where maximum is max int
						int nextInterval = v + interval;
						if (nextInterval < v) break;
						v = nextInterval;
					}
				}
				// draw handle
				if ((option->subControls & SC_SliderHandle) ) {
					QRect pixmapRect(0, 0, handle.width(), handle.height());
					QRect r = pixmapRect;
					QPainterPath path;
					path.addEllipse(r);
					QString handlePixmapName = pixmapkey(QLatin1String("slider_handle"), "", handle.size());
					if (!QPixmapCache::find(handlePixmapName, cache)) {
						cache = QPixmap(handle.size());
						cache.fill(Qt::transparent);
						QPainter handlePainter(&cache);

						handlePainter.setRenderHint(QPainter::Antialiasing, true);
						handlePainter.translate(0.5, 0.5);

						QLinearGradient gradient;
						gradient.setStart(pixmapRect.topLeft());
						gradient.setFinalStop(pixmapRect.bottomRight());
						gradient.setColorAt(0, QColor(128, 128, 128));
						gradient.setColorAt(1, QColor(0, 0, 0));
						handlePainter.save();
						handlePainter.setClipPath(path);
						handlePainter.fillRect(r, gradient);

						handlePainter.setPen(QPen(QColor(128, 128, 128), 2));
						handlePainter.setBrush(Qt::NoBrush);
						handlePainter.drawEllipse(r.adjusted(0, 0, 2, 2));
						handlePainter.setPen(QPen(QColor(0, 0, 0), 2));
						handlePainter.setBrush(Qt::NoBrush);
						handlePainter.drawEllipse(r.adjusted(-2, -2, 0, 0));

						handlePainter.restore();
						handlePainter.end();
						QPixmapCache::insert(handlePixmapName, cache);
					}

					QPoint topleft = handle.topLeft();
					p->drawPixmap(topleft, cache);

					if ((option->state & State_HasFocus) && (option->state & State_KeyboardFocusChange)) {
						p->save();
						p->setClipPath(path.translated(topleft));
						p->fillRect(handle, QColor(255, 255, 255, 32));
						p->restore();
					}
				}
				p->setBrush(oldBrush);
				p->setPen(oldPen);
			}
		}
		return;
	}
//	qDebug() << cc;
	QProxyStyle::drawComplexControl(cc, option, p, widget);
}
