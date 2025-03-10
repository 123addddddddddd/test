#ifndef CUSTOMGRAPHICSVIEW_H
#define CUSTOMGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QPainter>
#include <QPaintEvent>
#include <QWidget>
#include <QPoint>
#include <QVector>
#include <QKeyEvent>


class customGraphicsview : public QGraphicsView
{
    Q_OBJECT
public:
    explicit  customGraphicsview(QWidget *parent = nullptr);
    ~customGraphicsview();

    // 添加文字
    void addText(const QString &text,const QString &fontName, int fontSize);
    void keyPressEvent(QKeyEvent *event);
    void captureTextBitmap();
    void extractTextAsBinary(const QImage &image);

    //绘制文字
    void drawTextItems(QPainter &painter);
    void saveViewportAsImage(const QString &filePath);

    //绘制图像
    void setBinarizedImage(const QImage &image);

protected:
    // 重写 paintEvent
    void paintEvent(QPaintEvent *event)override;
    // 重写鼠标事件
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    // 绘制水平标尺
    void DrawHorizonRuler(QPainter &painter);
    // 绘制垂直标尺
    void DrawVerticalRuler(QPainter &painter);
    // 计算缩放比例和步长
    void calculateScaleAndStep();

    float m_scale;   // 缩放比例
    int m_step;      // 步长
    bool m_needRecalculateScale; // 是否需要重新计算缩放比例和步长


    struct TextItem {
        QString text;    // 文字内容
        QPoint position; // 文字位置
        QString fontName ;  // 字体名称
        int fontSize ;          // 字体大小
    };


    QImage m_binarizedImage;  // 存储二值化图像
    QPoint m_imageOffset = QPoint(0, 0);  // 记录图片的偏移
    QPoint m_lastMousePos;  // 记录鼠标的上一次位置
    bool m_draggingImage = false;  // 标记是否在拖动图片

    QVector<TextItem> m_textItems; // 所有文字列
    int m_draggingIndex = -1;      // 当前被拖动文字索引
    QPoint m_dragStartPos;         // 鼠标拖动起点
};

#endif // CUSTOMGRAPHICSVIEW_H
