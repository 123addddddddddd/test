#include "customgraphicsview.h"
#include <QScrollBar>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QPainter>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDebug>

customGraphicsview::customGraphicsview(QWidget *parent)
    : QGraphicsView(parent), m_scale(1.0), m_step(100), m_needRecalculateScale(true)
{
    // 初始化，设置视图的基本属性
    setRenderHint(QPainter::Antialiasing); // 启用抗锯齿

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);  // 始终显示水平滚动条

    QScrollBar *hScrollBar = horizontalScrollBar();
    hScrollBar->setSingleStep(10);  // 每次点击滚动条时，滚动10个单位
    hScrollBar->setPageStep(100);   // 每次点击滑块时，滚动100个单位
}

customGraphicsview::~customGraphicsview()
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    QPalette palette = this->palette();
    palette.setColor(QPalette::Background, Qt::white);
    this->setAutoFillBackground(true);  // 确保填充背景
    this->setPalette(palette);

}

//添加文本
void customGraphicsview::addText(const QString &text, const QString &fontName, int fontSize)
{
    // 添加新文字，初始位置在绘画框中心
    TextItem item;
    item.text = text;
    item.fontName = fontName;
    item.fontSize = fontSize;
    m_textItems.append(item);
    viewport()->update(); // 刷新绘画框
}

//绘制文本
void customGraphicsview::drawTextItems(QPainter &painter)
{
    for (const auto &item : m_textItems) {
        QFont itemFont(item.fontName, item.fontSize);
        painter.setFont(itemFont);

        QFontMetrics metrics(itemFont);
        QRect textRect = metrics.boundingRect(item.text);

        int textX = item.position.x();
        int textY = item.position.y() + metrics.ascent(); // 上对齐

        painter.setPen(Qt::black);
        painter.drawText(textX, textY, item.text);

        // 绘制文本的边界矩形（红色框）
        painter.setPen(Qt::red);
        textRect.moveTopLeft(item.position);
        // painter.drawRect(textRect);
    }

}

//设置图像
void customGraphicsview::setBinarizedImage(const QImage &image)
{
    m_binarizedImage = image;
    // update();  // 刷新视图，触发重新绘制
    this->viewport()->update();//强制刷新视图
}

//绘制事件
void customGraphicsview::paintEvent(QPaintEvent *event)
{
    // 先绘制场景内容
    QGraphicsView::paintEvent(event);

    // 使用viewport进行绘制，确保自定义内容不会被遮挡
    QPainter painter(this->viewport());

    // 计算缩放比例和步长（只在需要时重新计算）
    if (m_needRecalculateScale) {
        calculateScaleAndStep();
        m_needRecalculateScale = false; // 标记已经计算过了
    }

    // 绘制自定义内容
    DrawHorizonRuler(painter);   // 上边-X轴标尺
    DrawVerticalRuler(painter);   // 左边-Y轴标尺

    if (!m_binarizedImage.isNull()) {
        // 这里的(0, 0)是图像的绘制位置
        // painter.drawImage(0, 0, m_binarizedImage);
        painter.drawImage(m_imageOffset,m_binarizedImage);
    }


    QPainter painter_text(this->viewport());
    qDebug()<<painter_text.isActive()<<endl;

    //绘制所有文本项
    drawTextItems(painter);
}

//鼠标按压事件
void customGraphicsview::mousePressEvent(QMouseEvent *event)
{

    for (int i = 0; i < m_textItems.size(); ++i) {
        const auto &item = m_textItems[i];

        QFont currentFont(item.fontName, item.fontSize);
        QFontMetrics metrics(currentFont);
        // 计算文本的边界矩形，不考虑起始位置
        QRect textRect = metrics.boundingRect(item.text);
        // 将矩形调整到文本的实际位置
        textRect.moveTopLeft(item.position);
        // 输出 textRect 和鼠标点击坐标
        qDebug() << "textRect: (" << textRect.x() << ", " << textRect.y() << ", "
                 << textRect.width() << ", " << textRect.height() << ")";
        qDebug() << "Mouse position: (" << event->pos().x() << ", " << event->pos().y() << ")";


        if (textRect.contains(event->pos())) {
            m_draggingIndex = i; // 找到被拖动的文字
            m_dragStartPos = event->pos() - item.position;
            break;
        }else{
            qDebug()<<"fail"<<endl;
        }
    }

    //
    QPoint mousePos = event->pos();
    // 计算图片的矩形区域
    QRect imageRect(m_imageOffset, m_binarizedImage.size());

    if (imageRect.contains(mousePos)) {
        m_draggingImage = true;  // 开始拖动
        m_lastMousePos = mousePos;
    } else {
        m_draggingImage = false;
    }

}

//鼠标移动事件
void customGraphicsview::mouseMoveEvent(QMouseEvent *event)
{
    if (m_draggingIndex != -1) {
        m_textItems[m_draggingIndex].position = event->pos() - m_dragStartPos;
        viewport()->update(); // 刷新
    }

    if (m_draggingImage) {
        // 计算鼠标移动的偏移量
        QPoint delta = event->pos() - m_lastMousePos;
        m_imageOffset += delta;
        m_lastMousePos = event->pos();
        viewport()->update();  // 刷新
    }
}

//释放鼠标事件
void customGraphicsview::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_draggingIndex = -1; // 停止拖动
    m_draggingImage = false;
}

//键盘事件
void customGraphicsview::keyPressEvent(QKeyEvent *event)
{
    // 如果按下的是 Delete 键，并且有文字被选中
    if (event->key() == Qt::Key_Delete && m_draggingIndex != -1) {
        // 删除选中的文字
        m_textItems.remove(m_draggingIndex);
        m_draggingIndex = -1;  // 清除选中的文字索引
        viewport()->update();  // 刷新绘画框
    }
}

// 绘制水平标尺
void customGraphicsview::DrawHorizonRuler(QPainter &painter)
{

    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);

    // 主刻度线（10 像素高）和次刻度线（5 像素高）绘制
    for (int i = 0; i < this->width(); i += m_step)
    {
        // 绘制主刻度线（10像素高）
        painter.drawLine(i, 0, i, 10);  // 10 像素高的主刻度线
        // 绘制数字
        painter.drawText(i + 2, 20, QString::number(i));  // 数字位置偏移2像素

        // 绘制次刻度线（5像素高）
        for (int j = 1; j < m_step / 10; ++j) // 在主刻度线之间绘制次刻度线
        {
            int x = i + j * (m_step / 10);
            painter.drawLine(x, 0, x, 5);  // 5 像素高的次刻度线
        }
    }

}

// 绘制垂直标尺
void customGraphicsview::DrawVerticalRuler(QPainter &painter)
{

    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);

    // 主刻度线（10 像素长）和次刻度线（5 像素长）绘制
    for (int i = 0; i < this->height(); i += m_step)
    {
        // 绘制主刻度线（10像素长）
        painter.drawLine(0, i, 10, i);  // 10 像素长的主刻度线
        // 绘制数字
        painter.drawText(20, i + 2, QString::number(i));  // 数字位置偏移 2 像素

        // 绘制次刻度线（5像素长）
        for (int j = 1; j < m_step / 10; ++j) // 在主刻度线之间绘制次刻度线
        {
            int y = i + j * (m_step / 10);
            painter.drawLine(0, y, 5, y);  // 5 像素长的次刻度线
        }
    }
}

void customGraphicsview::calculateScaleAndStep()
{
    // 获取视图的宽度和场景的实际尺寸
    QPointF leftTop = mapToScene(QPoint(0, 0));
    QPointF rightTop = mapToScene(QPoint(this->width(), 0));

    // 计算缩放比例
    m_scale = (rightTop.x() - leftTop.x()) / this->width();

    // 计算步长
    m_step = 100;
    m_step = ((1.0 / m_scale * 100) / 10) * 10;

    if (m_step > 50 && m_step < 150)
    {
        m_step = 100;
    }
    else if (m_step >= 150 && m_step < 200)
    {
        m_step = 200;
    }

}

//提取为二进制
void customGraphicsview::captureTextBitmap()
{
    // 创建一个 QImage，尺寸与视口相同
    QImage image(viewport()->size(), QImage::Format_ARGB32);
    image.fill(Qt::white);  // 背景颜色可根据需要设置

    // 创建一个 QPainter 将内容绘制到 QImage 上
    QPainter painter(&image);

    // 先渲染视口内容到 QImage，但不包括标尺
    QGraphicsView::render(&painter);  // 渲染视口内容到 QImage

    // 绘制自定义的文本项
    drawTextItems(painter);

    // 提取文本为二进制
    extractTextAsBinary(image);
}

//二进制
void customGraphicsview::extractTextAsBinary(const QImage &image)
{
    // 确保 image 的像素格式是黑白的（1 bit per pixel）
    QImage monoImage = image.convertToFormat(QImage::Format_Mono);

    // 获取图像的宽度和高度
    int width = monoImage.width();
    int height = monoImage.height();
    qDebug() << "Image size: " << monoImage.size();

    // 每列有 152 个像素点（按照你的要求），因此每列会产生 19 个字节（8 个像素点表示 1 个字节）
    int pixelsPerColumn = 152;
    int bytesPerColumn = pixelsPerColumn / 8;  // 每 8 个像素点对应 1 个字节
    int totalColumns = 220;  // 图像的列数是 220
    int totalBytes = bytesPerColumn * totalColumns;  // 总字节数

    uchar* hziCode = new uchar[totalBytes];

    // 按列遍历图像
    for (int x = 0; x < totalColumns; ++x) {
        // 每列的字节数
        for (int y = 0; y < pixelsPerColumn; y += 8) {
            uchar byte = 0;
            // 处理每 8 个像素点
            for (int i = 0; i < 8 && y + i < pixelsPerColumn; ++i) {
                QColor pixelColor = monoImage.pixel(x, y + i);
                bool isBlack = (pixelColor == Qt::black);  // 黑色为 1
                byte |= (isBlack << (7 - i));  // 每个像素占 1 位，从高位开始
            }
            // 存储合并后的字节
            int index = (x * bytesPerColumn) + (y / 8);
            hziCode[index] = byte;
        }
    }

    // 输出字模数据（以十六进制格式输出，且每列 19 个字节换行）
    QFile file("./hzi_code.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for writing.";
        delete[] hziCode;  // 释放内存
        return;
    }

    // 创建 QTextStream 用于写入文件
    QTextStream out(&file);

    // 输出每列的 19 个字节
    for (int x = 0; x < totalColumns; ++x) {
        out << "{";
        for (int y = 0; y < bytesPerColumn; ++y) {
            if (y > 0) {
                out << ", ";
            }

            // 输出为 0x 格式，不加引号
            out << QString("0x%1").arg(hziCode[x * bytesPerColumn + y], 2, 16, QLatin1Char('0'));
        }
        out << "},\n";  // 每列后换行并加上逗号
    }

    delete[] hziCode;  // 释放内存
    file.close();
}

//保存
void customGraphicsview::saveViewportAsImage(const QString &filePath)
{
    // 获取视口大小
    QSize viewportSize = viewport()->size();

    // 创建一个 QImage，尺寸与视口相同
    QImage image(viewport()->size(), QImage::Format_ARGB32);
    image.fill(Qt::white);

    // 创建一个 QPainter 将内容绘制到 QImage 上
    QPainter painter(&image);

    // 先绘制场景内容
 //   QGraphicsView::render(&painter);  // 渲染视口内容到 QImage，但不包含标尺

    if (!m_binarizedImage.isNull()) {
           painter.drawImage(m_imageOffset, m_binarizedImage);
       }

    // 绘制自定义内容（只包括文本）
    drawTextItems(painter);

    // 保存图像
    image.save(filePath);
}
