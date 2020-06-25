/*
  Se trata básicamente de diseñar una aplicación que mediante una máquina de
  estados controle el tráfico de personas a través de la puerta de un aula
  tomando como base los datos adquiridos a través de las ímagenes de video
  de una cámara cenita.
  Se propone de entrada utilizar un aproximación centrada en determinar
  las diferencias entre una imagen base inicial neutra y las sucesivas imágenes del
  tren de video y a partir de ellas seguir el tracking del centroide considerando
  dos barreras virtuales para la contrucción de la máquina de estados.
  */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qdebug.h>
#include <iostream>   // std::cout
#include <string>
#include "opencv2/videoio.hpp"


vector<vector<Point> > contours;
vector<Vec4i> hierarchy;
int numDentro;
bool dentro,fuera,entrando,saliendo;
using namespace std;

// Constructor member
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Object video creation
    cap= new VideoCapture();
    cap->open("video.wmv"); // original video
    // settings sizes
    cap->set(CV_CAP_PROP_FRAME_WIDTH,320); // fix width
    cap->set(CV_CAP_PROP_FRAME_HEIGHT,240); // fix heigth

    // Barriers positions
    raw1=128;raw2=180;



    // Qimages for viewers

    Q_base_image = new QImage(320,240, QImage::Format_Indexed8);
    Q_current_imagemia = new QImage(320,240, QImage::Format_Indexed8);
    Q_current_imagedif = new QImage(320,240, QImage::Format_Indexed8);
    Q_current_imagedifumb = new QImage(320,240, QImage::Format_Indexed8);

    // Viewers
    viewer_miooo = new RCDraw(320,240, Q_current_imagemia, ui->viewer_original);
    viewer_base = new RCDraw(320,240, Q_base_image, ui->viewer_base);
    viewer_dif = new RCDraw(320,240,Q_current_imagedif,ui->viewer_diferencia);
    viewer_difumb = new RCDraw(320,240,Q_current_imagedifumb,ui->viewer_diferenciaumb);

    // Run connect and start timer
    connect(&timer,SIGNAL(timeout()),this,SLOT(compute()));
    timer.start(100);


    if(!cap->isOpened())
        exit(-1);

    *cap >> base;


    cvtColor(base, base, CV_RGB2GRAY,1);

    memcpy(Q_base_image->bits(),base.data, base.rows*base.cols*sizeof(uchar) );
    viewer_base->update();

    //Inicializamos la maquina de estados.
    numDentro=1;
    dentro=true;
    fuera=false;
    entrando=false;
    saliendo=false;
    ui->cont->display(numDentro);
}

// Destructor member
MainWindow::~MainWindow()
{
    delete ui;
    delete cap;
    delete Q_current_imagemia;
    delete Q_base_image;
    delete Q_current_imagedif;
    delete Q_current_imagedifumb;

 }

//Metodo para calcular el centroide
Point MainWindow::calcularCentroide(Mat difumb){
    float sumx=0, sumy=0;
        float num_pixel = 0;
        for(int x=0; x<difumb.cols; x++) {
            for(int y=0; y<difumb.rows; y++) {
                int val = difumb.at<uchar>(y,x);
                if( val >= 50) {
                    sumx += x;
                    sumy += y;
                    num_pixel++;
                }
            }
        }
        Point p(sumx/num_pixel, sumy/num_pixel);
        return p;
}

//Metodo de actualizacion de estados
void MainWindow::gestionMaquinaEstados(Point ubicacion){
    if(ubicacion.y<raw1){//Si la persona esta por debajo de la linea 1
        dentro=true; //Esta dentro
        if(entrando==true){ //Si estaba entrando
            fuera=false; // No esta fuera
            numDentro++; //Se incrementa el numero de personas dentro
            ui->cont->display(numDentro);//actualiza la interfaz
            entrando=false; //No esta entrando
            saliendo=false; //No esta saliendo
        }
    }
    if(ubicacion.y>raw2){//Si la persona esta por encima de la linea 2
        fuera=true; //Esta fuera
        if(saliendo==true){ //Si estaba saliendo
            dentro=false; // No esta dentro
            numDentro--; //Se decrementa el numero de personas dentro
            ui->cont->display(numDentro); //actualiza la interfaz
            saliendo=false; //No esta saliendo
            entrando=false; //No esta entrando
        }
    }
    if(ubicacion.y>raw1 && ubicacion.y<raw2){//Si la persona esta entre las 2 lineas
            if(dentro==true){ //Si estaba dentro
                saliendo=true; //Esta saliendo
                entrando=false; //No esta entrando

            }
            if(fuera==true){ //Si estaba fuera
                entrando=true; //Esta entrando
                saliendo=false; //No esta saliendo

            }

    }
}

// Process loop implementation on SLOT
void MainWindow::compute()
{
   ////////////////// CAMERA  //////////////////////////
   if(!cap->isOpened())  // check if we succeeded
       exit(-1);
   // Get a image from video
   *cap >> Current_Image;


   //Draw barrier lines and copy to current image viewer
   cv::line(Current_Image,Point(0,raw1),Point(Current_Image.cols-1,raw1),CV_RGB(0,255,255),1);
   cv::line(Current_Image,Point(0,raw2),Point(Current_Image.cols-1,raw2),CV_RGB(0,255,255),1);

   //////////

   cvtColor(Current_Image, Current_Image, CV_RGB2GRAY,1); //Pasa la imagen actual a gris
   GaussianBlur(Current_Image, Current_Image,Size(7,7),1.5, 1.5); //Aplica un filtro gausiano

   //////////

   memcpy(Q_current_imagemia->bits(),Current_Image.data, Current_Image.rows*Current_Image.cols*sizeof(uchar) );
   viewer_miooo->update();


   /////////////



    absdiff(base,Current_Image,image_diferencia); //Calculamos las diferencias entre las imagenes
    memcpy(Q_current_imagedif->bits(),image_diferencia.data, image_diferencia.rows*image_diferencia.cols*sizeof(uchar) );
    viewer_dif->update();

    threshold(image_diferencia,difumb,100,255,THRESH_TOZERO);//Umbraliza la diferencias
    memcpy(Q_current_imagedifumb->bits(),difumb.data, difumb.rows*difumb.cols*sizeof(uchar) );
    viewer_difumb->update();

    Point ubicacion=calcularCentroide(difumb); //actualizamos la ubicacion de la persona

    gestionMaquinaEstados(ubicacion); //actualizamos la maquina de estados


}

