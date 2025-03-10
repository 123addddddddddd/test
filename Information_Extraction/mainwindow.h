#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <opencv2/opencv.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_open_pushButton_clicked();

    void on_save_pushButton_clicked();

    void on_text_pushButton_clicked();

    void on_picture_pushButton_clicked();

    void on_rotate_pushButton_clicked();

    void on_contrary_pushButton_clicked();

    void on_delete_pushButton_clicked();

    void on_connect_pushButton_clicked();

    void on_send_pushButton_clicked();

    void on_text_confirm_pushButton_clicked();

    void on_extract_pushButton_clicked();

   void Binarization( /*const cv::Mat &image*/);

   void on_binary_pushButton_clicked();

   void on_imageConfirm_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
