#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GL/glut.h"


#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <rcdraw.h>


#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;


#include <iostream>



namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    int raw1,raw2;

    Ui::MainWindow *ui;
    QTimer timer; // Slot Timer
    VideoCapture *cap; //

    RCDraw *viewer_original;
    RCDraw *viewer_base;
    RCDraw *viewer_miooo;
    RCDraw *viewer_dif;
     RCDraw *viewer_difumb;
    QImage *Q_current_image;
    QImage *Q_base_image;
    QImage *Q_current_imagemia;
    QImage *Q_current_imagedif;
    QImage *Q_current_imagedifumb;

    Mat  Current_Image,base,image_diferencia,difumb;


public slots:
        void compute();
       Point calcularCentroide(Mat difumb);
       void gestionMaquinaEstados(Point ubicacion);

};

#endif // MAINWINDOW_H
