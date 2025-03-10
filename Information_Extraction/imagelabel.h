#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QObject>
#include <QLabel>
#include <QMenu>

class imagelabel : public QLabel
{
    Q_OBJECT
public:
    explicit imagelabel(QWidget *parent = nullptr);

    void setPhoto(QString);  //设置图片

    void clearShow();   //清空显示
    QImage getImage() const;
    QRect getImageRect() const;
    QPoint getImageOffset() const;
    double getZoomValue() const;
    QRect getVisibleImageRect() const;
    QSize getVisibleImageSize() const;

    QImage getOriginalImage() const;


    void setImage(const QImage &image);  // 设置 QLabel 中的图像

    QImage getVisibleImage();//获取 QLabel 中显示的部分图片

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;   //右键菜单
    void paintEvent(QPaintEvent *event);                        //QPaint画图
    void wheelEvent(QWheelEvent *event) override;               //鼠标滚轮滚动
    void mousePressEvent(QMouseEvent *event) override;          //鼠标摁下
    void mouseMoveEvent(QMouseEvent *event) override;           //鼠标松开
    void mouseReleaseEvent(QMouseEvent *event) override;        //鼠标发射事件

private slots:
    void initWidget();      //初始化
    void onSelectImage();   //选择打开图片
    void onZoomInImage();   //图片放大
    void onZoomOutImage();  //图片缩小
    void onPresetImage();   //图片还原
    void binarizeImage(int threshold);

private:
    QImage m_image;           //显示的图片
    qreal m_zoomValue = 1.0;  //鼠标缩放值
    int m_xPtInterval = 0;    //平移X轴的值
    int m_yPtInterval = 0;    //平移Y轴的值
    QPoint m_oldPos;          //旧的鼠标位置
    bool m_pressed = false;   //鼠标是否被摁压
    QString m_localFileName;  //文件名称
    QMenu *m_menu;            //右键菜单
};

#endif
