#include "imagelabel.h"
#include <QPainter>
#include <QDebug>
#include <QAction>
#include <QMenu>
#include <QWheelEvent>
#include <QFileDialog>
#include  <QInputDialog>

imagelabel::imagelabel(QWidget *parent):QLabel(parent)
{
    initWidget();
}

/**
* @brief imagelabel::initWidget 初始化
*/
void imagelabel::initWidget()
{
    //初始化右键菜单
    m_menu = new QMenu(this);
    QAction *loadImage = new QAction;
    loadImage->setText("选择图片");
    connect(loadImage, &QAction::triggered, this, &imagelabel::onSelectImage);
    m_menu->addAction(loadImage);
    m_menu->addSeparator();

    QAction *zoomInAction = new QAction;
    zoomInAction->setText("放大");
    connect(zoomInAction, &QAction::triggered, this, &imagelabel::onZoomInImage);
    m_menu->addAction(zoomInAction);

    QAction *zoomOutAction = new QAction;
    zoomOutAction->setText("缩小");
    connect(zoomOutAction, &QAction::triggered, this, &imagelabel::onZoomOutImage);
    m_menu->addAction(zoomOutAction);

    QAction *presetAction = new QAction;
    presetAction->setText("还原");
    connect(presetAction, &QAction::triggered, this, &imagelabel::onPresetImage);
    m_menu->addAction(presetAction);
    m_menu->addSeparator();

    QAction *clearAction = new QAction;
    clearAction->setText("清空");
    connect(clearAction, &QAction::triggered, this, &imagelabel::clearShow);
    m_menu->addAction(clearAction);

    // 二值化
    QAction *binarizeAction = new QAction;
    binarizeAction->setText("二值化");
    connect(binarizeAction, &QAction::triggered, this, [this]() {
        bool ok;
        int threshold = QInputDialog::getInt(this, "set", "set（0-255）：", 128, 0, 255, 1, &ok);
        if (ok)   binarizeImage(threshold);
    });
    m_menu->addAction(binarizeAction);
}

/**
* @brief imagelabel::setPhoto 设置要显示的图片
* @param path 图片路径
*/
void imagelabel::setPhoto(QString path)
{
    if(path.isEmpty())
    {
        return;
    }

    m_zoomValue = 1.0;
    m_xPtInterval = 0;
    m_yPtInterval = 0;

    m_localFileName = path;
    m_image.load(m_localFileName);
    update();
}

/**
* @brief imagelabel::clearShow 清空
*/
void imagelabel::clearShow()
{
    m_localFileName = "";
    m_image = QImage();
    this->clear();
}

QImage imagelabel::getImage() const
{
    return m_image;
}

/**
* @brief imagelabel::paintEvent 绘图事件
* @param event
*/
void imagelabel::paintEvent(QPaintEvent *event)
{
    if(m_image.isNull())
        return QWidget::paintEvent(event);

    QPainter painter(this);

    // 根据窗口计算应该显示的图片的大小
    int width = qMin(m_image.width(), this->width());
    int height = int(width * 1.0 / (m_image.width() * 1.0 / m_image.height()));
    height = qMin(height, this->height());
    width = int(height * 1.0 * (m_image.width() * 1.0 / m_image.height()));

    // 平移
    painter.translate(this->width() / 2 + m_xPtInterval, this->height() / 2 + m_yPtInterval);

    // 缩放
    painter.scale(m_zoomValue, m_zoomValue);

    // 绘制图像
    QRect picRect(-width / 2, -height / 2, width, height);
    painter.drawImage(picRect, m_image);

    QWidget::paintEvent(event);
}

/**
* @brief imagelabel::wheelEvent 滚轮滚动缩放图片
* @param event
*/
void imagelabel::wheelEvent(QWheelEvent *event)
{
    int value = event->delta();
    if (value > 0)  //放大
        onZoomInImage();
    else            //缩小
        onZoomOutImage();

    update();
}

/**
* @brief imagelabel::mousePressEvent 鼠标按下，为移动图片做准备
* @param event
*/
void imagelabel::mousePressEvent(QMouseEvent *event)
{
    m_oldPos = event->pos();
    m_pressed = true;
    this->setCursor(Qt::ClosedHandCursor); //设置鼠标样式
}

/**
* @brief imagelabel::mouseMoveEvent 鼠标按下后，再移动鼠标，图片随之移动
* @param event
*/
void imagelabel::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_pressed)
        return QWidget::mouseMoveEvent(event);

    QPoint pos = event->pos();
    int xPtInterval = pos.x() - m_oldPos.x();
    int yPtInterval = pos.y() - m_oldPos.y();

    m_xPtInterval += xPtInterval;
    m_yPtInterval += yPtInterval;

    m_oldPos = pos;
    update();
}

/**
* @brief imagelabel::mouseReleaseEvent 鼠标抬起，图片移动结束
*/
void imagelabel::mouseReleaseEvent(QMouseEvent */*event*/)
{
    m_pressed = false;
    this->setCursor(Qt::ArrowCursor); //设置鼠标样式
}

/**
* @brief imagelabel::contextMenuEvent 右键显示菜单栏
* @param event
*/
void imagelabel::contextMenuEvent(QContextMenuEvent *event)
{
    QPoint pos = event->pos();
    pos = this->mapToGlobal(pos);
    m_menu->exec(pos);
}

/**
* @brief imagelabel::onSelectImage 选择图片
*/
void imagelabel::onSelectImage()
{
    QString path = QFileDialog::getOpenFileName(this, "选择 要显示的图片", "./", tr("Images (*.png *.jpg *.jpeg *.bmp)"));
    if (path.isEmpty())
        return;

    setPhoto(path);
}

/**
* @brief imagelabel::onZoomInImage 图片放大
*/
void imagelabel::onZoomInImage()
{
    m_zoomValue += 0.05;
    update();
}

/**
* @brief imagelabel::onZoomOutImage 图片缩小
*/
void imagelabel::onZoomOutImage()
{
    m_zoomValue -= 0.05;
    if (m_zoomValue <= 0)
    {
        m_zoomValue = 0.05;
        return;
    }

    update();
}

/**
* @brief imagelabel::onPresetImage 图片还原
*/
void imagelabel::onPresetImage()
{
    m_zoomValue = 1.0;
    m_xPtInterval = 0;
    m_yPtInterval = 0;
    update();
}

//二值化
void imagelabel::binarizeImage(int threshold)
{
    if (m_image.isNull())
        return;
    // 将图像转换为灰度图像
    QImage grayImage = m_image.convertToFormat(QImage::Format_Grayscale8);
    // 遍历每个像素，根据阈值进行二值化
    for (int y = 0; y < grayImage.height(); ++y) {
        for (int x = 0; x < grayImage.width(); ++x) {
            int pixelValue = qGray(grayImage.pixel(x, y)); // 获取灰度值
            if (pixelValue > threshold) {
                grayImage.setPixel(x, y, qRgb(255, 255, 255)); // 白色
            } else {
                grayImage.setPixel(x, y, qRgb(0, 0, 0)); // 黑色
            }
        }
    }

    // 更新当前图片为二值化后的图像
    m_image = grayImage;
    update(); // 刷新显示
}


QRect imagelabel::getImageRect() const
{
    if (m_image.isNull())
        return QRect();

    int width = qMin(m_image.width(), this->width());
    int height = int(width * 1.0 / (m_image.width() * 1.0 / m_image.height()));
    height = qMin(height, this->height());
    width = int(height * 1.0 * (m_image.width() * 1.0 / m_image.height()));

    return QRect(-width / 2, -height / 2, width, height);
}

QPoint imagelabel::getImageOffset() const
{
    return QPoint(m_xPtInterval, m_yPtInterval);
}

double imagelabel::getZoomValue() const
{
    return m_zoomValue;
}

QRect imagelabel::getVisibleImageRect() const
{
    if (m_image.isNull())
        return QRect();

    // 计算实际显示的图像区域
    int width = qMin(m_image.width(), this->width());
    int height = int(width * 1.0 / (m_image.width() * 1.0 / m_image.height()));
    height = qMin(height, this->height());
    width = int(height * 1.0 * (m_image.width() * 1.0 / m_image.height()));

    QRect visibleRect(-width / 2, -height / 2, width, height);

    // 应用偏移量和缩放
    visibleRect.translate(m_xPtInterval, m_yPtInterval);
    visibleRect.setWidth(visibleRect.width() / m_zoomValue);
    visibleRect.setHeight(visibleRect.height() / m_zoomValue);

    return visibleRect;
}

QSize imagelabel::getVisibleImageSize() const
{
    if (m_image.isNull())
        return QSize();

    int width = qMin(m_image.width(), this->width());
    int height = int(width * 1.0 / (m_image.width() * 1.0 / m_image.height()));
    height = qMin(height, this->height());
    width = int(height * 1.0 * (m_image.width() * 1.0 / m_image.height()));

    return QSize(width, height);
}

QImage imagelabel::getOriginalImage() const
{
    return m_image; // 返回原始图像
}


QImage imagelabel::getVisibleImage()
{
    if (m_image.isNull()) return QImage(); // 确保有图片

        // 获取 QLabel 的大小（显示区域）
        QSize labelSize = this->size();

        // 计算缩放后的图片尺寸
        int scaledWidth = m_image.width() * m_zoomValue*0.45;
        int scaledHeight = m_image.height() * m_zoomValue*0.45;

        // 创建缩放后的图片
        QImage scaledImage = m_image.scaled(scaledWidth, scaledHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // 计算需要裁剪的区域
        int x = 0, y = 0, width = scaledImage.width(), height = scaledImage.height();

        if (width > labelSize.width()) {
            x = (width - labelSize.width()) / 2; // 居中裁剪
            width = labelSize.width();
        }
        if (height > labelSize.height()) {
            y = (height - labelSize.height()) / 2;
            height = labelSize.height();
        }

        // 确保裁剪区域在有效范围
        QRect cropRect(x, y, width, height);
        cropRect = cropRect.intersected(scaledImage.rect());

        return scaledImage.copy(cropRect); // 返回可视区域的图片


//        if (m_image.isNull()) return QImage(); // 确保有图片

//        // 1️⃣ 获取 QLabel 的大小（可视区域）
//        QSize labelSize = this->size();

//        // 2️⃣ 计算 **等比例缩放** 后的图片大小
//        QImage scaledImage = m_image.scaled(m_image.size() * m_zoomValue, Qt::KeepAspectRatio, Qt::SmoothTransformation);

//        // 3️⃣ 计算 QLabel 内 `scaledImage` 的实际偏移位置（用于居中）
//        int x_offset = (labelSize.width() - scaledImage.width()) / 2;
//        int y_offset = (labelSize.height() - scaledImage.height()) / 2;

//        // 4️⃣ 计算需要裁剪的区域
//        QRect cropRect(-x_offset, -y_offset, labelSize.width(), labelSize.height());

//        // 5️⃣ **确保裁剪区域不超出 `scaledImage`**
//        cropRect = cropRect.intersected(scaledImage.rect());

//        return scaledImage.copy(cropRect);


}


