#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSplitter>
#include <QTextEdit>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(2);




}

MainWindow::~MainWindow()
{
    delete ui;
}



//打开按钮
void MainWindow::on_open_pushButton_clicked()
{
    // 弹出文件选择对话框，选择 BMP 文件
    QString filePath = QFileDialog::getOpenFileName(
                this,
                tr("打开图片"),
                "",
                tr("BMP 图片 (*.bmp);;所有文件 (*)")
                );

    // 检查文件路径是否为空
    if (!filePath.isEmpty()) {
        // 创建 QPixmap 对象，加载 BMP 图片
        QPixmap pixmap(filePath);

        // 检查图片是否加载成功
        if (!pixmap.isNull()) {
            // 创建 QGraphicsScene
            QGraphicsScene *scene = new QGraphicsScene();

            // 创建 QGraphicsPixmapItem 来显示图片
            QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);

            // 将图片项添加到场景中
            scene->addItem(item);

            // 设置 ui->graphicsView 的场景为新创建的场景
            ui->graphicsView->setScene(scene);

            // 可选：调整图像适配视口
            ui->graphicsView->fitInView(item, Qt::KeepAspectRatio);
        } else {
            // 如果图片加载失败，弹出警告框
            QMessageBox::warning(this, tr("fail"), tr("retry"));
        }
    }
}

//保存按钮
void MainWindow::on_save_pushButton_clicked()
{
    // 弹出文件保存对话框
    QString filePath = QFileDialog::getSaveFileName(
                this,
                tr("保存图片"),
                "",
                tr("BMP 图片 (*.bmp);;所有文件 (*)")
                );
    // 检查用户是否选择了有效的文件路径
    if (!filePath.isEmpty()) {
        // 如果文件没有后缀，则手动加上 .bmp 后缀
        if (!filePath.endsWith(".bmp", Qt::CaseInsensitive)) {
            filePath += ".bmp";
        }

        // 调用函数保存视口为图片
        ui->graphicsView->saveViewportAsImage(filePath);
    }



}

//添加文本
void MainWindow::on_text_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

//添加图片
void MainWindow::on_picture_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


//旋转
void MainWindow::on_rotate_pushButton_clicked()
{

}

//反色
void MainWindow::on_contrary_pushButton_clicked()
{

}

//删除
void MainWindow::on_delete_pushButton_clicked()
{

}

//连接喷码机
void MainWindow::on_connect_pushButton_clicked()
{

}

//发送
void MainWindow::on_send_pushButton_clicked()
{

}

//确定文本按钮
void MainWindow::on_text_confirm_pushButton_clicked()
{
    ui->graphicsView->addText(ui->text_lineEdit->text(),ui->textsytle_comboBox->currentText(),ui->textsize_comboBox->currentText().toInt());

}

//提取二进制
void MainWindow::on_extract_pushButton_clicked()
{
    ui->graphicsView->captureTextBitmap();

}

//二值化
void MainWindow::Binarization(/*const cv::Mat &image*/)
{



}


//二值化处理按钮
void MainWindow::on_binary_pushButton_clicked()
{


}

//图片确定按钮
void MainWindow::on_imageConfirm_pushButton_clicked()
{
    // 获取 QLabel 显示的最终图片
       QImage visibleImage = ui->image_label->getVisibleImage();
    if(!ui->image_label->getImage().isNull()){
        ui->graphicsView->setBinarizedImage(visibleImage);
    }

}
