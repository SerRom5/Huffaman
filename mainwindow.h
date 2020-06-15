#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QFileDialog>
#include "element.h"
#include <QVector>
#include <math.h>

namespace Ui {
class MainWindow;
}

enum TypeNode { Root, Left, Right };

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_Code_clicked();
    void on_pushButton_Clear_clicked();
    void on_pushButton_SaveImage_clicked();

private:
    // rysuje drzewo binarne w okne
    void drawNodes(int16_t xPosition, int16_t Position, Element *node, TypeNode typeNode , int8_t level,  uint8_t levels);

    // Oblicza liczbę poziomów drzewa binarnego
    uint8_t countLevel(Element *node, uint8_t numberSymbols);

    // oblicza entropię
    double countEntropy(Element * node, uint16_t n);


    void setCodeHuffmanaToElement(QChar _char, QString codeHuff);

    // zwraca x pozycję dla wstawienia liczby symboli w kwadracie na rysunku drzewa binarnego
    uint16_t getXPosition(uint16_t n);

    //uint8_t countLevel(uint8_t numberSymbols);

    // pobiera ciąg znaków z pala "Dane wejsciowe" i sortuje je do vectora "elements"
    void shortSymbolsToVector();

    // uzupełnia tablice danymi
    void setSymbolsToTable();

    // buduje drzewo binarne w "rootElement"
    void setRoot();

    // wstawia dane końcowe do pól okna programu
    void setDataOut();

    Ui::MainWindow *ui;
    QGraphicsScene * scene;

    QVector<Symbol> symbols;
    QVector<Element*> elements;
    Element *rootElement;

    uint16_t bytesInput = 0;
    uint16_t bytesOutput = 0;
    uint16_t bitsInput = 0;
    uint16_t bitsOutput = 0;
    QString dataOutput;
    bool isData = false;

};

#endif // MAINWINDOW_H
