#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItem>
#include <QStandardItemModel>
#include <QGraphicsTextItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()<<"Symbol"<<"Liczba"<<"Kod ASCII"<<"Kod Huffmana");
    ui->tableWidget->setColumnWidth(0,60);
    ui->tableWidget->setColumnWidth(1,60);
    ui->tableWidget->setColumnWidth(2,60);
    ui->tableWidget->setColumnWidth(3,150);
    ui->progressBar->setVisible(false);
    ui->pushButton_SaveImage->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_Code_clicked()
{
    if(ui->textEdit_InputData->toPlainText().isEmpty()) {


        // wstawia dene testowe do pola "Dane wejsciowe".
        ui->textEdit_InputData->setText("Peter Piper picked a peck of pickled peppers. "
                                        /*"Did Peter Piper pick a peck of pickled peppers? "
                                        "If Peter Piper Picked a peck of pickled peppers, "
                                        "Where's the peck of pickled peppers Peter Piper picked? "
                                        "She sells seashells by the seashore. "
                                        "The shells she sells are surely seashells. "
                                        "So if she sells shells on the seashore, "
                                        "I'm sure she sells seashore shells. "*/
                                        /*"Red lorry, yellow lorry. "
                                        "Which wristwatches are Swiss wristwatches? "
                                        "How much wood would a woodchuck chuck "
                                        "If a woodchuck could chuck wood? "
                                        "He would chuck, he would, as much as he could, "
                                        "And chuck as much as a woodchuck would "
                                        "If a woodchuck could chuck wood."*/);
    }
    scene->clear();
    ui->progressBar->setValue(0);
    ui->progressBar->setVisible(true);
    ui->progressBar->setValue(20);
    shortSymbolsToVector();
    setRoot();
    ui->progressBar->setValue(40);
    drawNodes(0, 0, rootElement, TypeNode::Root, 0, countLevel(rootElement,0));
    ui->progressBar->setValue(60);
    setSymbolsToTable();
    ui->progressBar->setValue(80);
    isData = true;
    setDataOut();
    ui->pushButton_SaveImage->setVisible(true);
    ui->progressBar->setVisible(false);
}



void MainWindow::on_pushButton_Clear_clicked()
{
    ui->progressBar->setValue(0);
    ui->progressBar->setVisible(true);
    ui->progressBar->setValue(20);
    scene->clear();
    if (!isData){
         ui->progressBar->setVisible(false);
        return;
    }
    ui->tableWidget->clear();
    while (ui->tableWidget->rowCount()!= 0)
        ui->tableWidget->removeRow(0);
    ui->progressBar->setValue(40);
    ui->lineEdit_AveregeWordLength->clear();
    ui->lineEdit_CompreseBits->clear();
    ui->lineEdit_CompreseBytes->clear();
    ui->lineEdit_Entropy->clear();
    ui->lineEdit_InputBits->clear();
    ui->lineEdit_InputBytes->clear();
    ui->lineEdit_OutpputBytes->clear();
    ui->lineEdit_OutputBits->clear();
    ui->textEdit_InputData->clear();
    ui->textEdit_OutputData->clear();
    ui->progressBar->setValue(60);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()<<"Symbol"<<"Liczba"<<"Kod ASCII"<<"Kod Huffmana");
    elements.clear();
    ui->progressBar->setValue(80);
    rootElement = NULL;
    isData = false;
    ui->pushButton_SaveImage->setVisible(false);
    bytesInput = 0;
    bytesOutput = 0;
    bitsInput = 0;
    bitsOutput = 0;
    dataOutput.clear();

    ui->progressBar->setVisible(false);
}

void MainWindow::shortSymbolsToVector() {

    QString dataiInput = ui->textEdit_InputData->toPlainText();
    for (uint16_t i = 0; i < dataiInput.length(); ++i) {
        bool isSymbol = false;

        for (uint8_t j = 0; j < elements.size(); ++j)
            if (dataiInput[i] == elements[j]->symbol.getSymbol()){
                elements[j]->symbol.incrementNumberSymbol();
                isSymbol = true;
                break;
            }
        if (!isSymbol)
            elements.push_back(new Element(Symbol(dataiInput[i], 1)));
    }

    for (uint8_t i = 0; i < elements.size(); ++i) {
        uint16_t numSym = 0;
        for (uint16_t j = 0; j < dataiInput.length(); j++)
            if (elements[i]->symbol.getSymbol() == dataiInput[j])
                ++numSym;
        if(numSym)
            elements[i]->symbol.setSymbolNumber(numSym);
        else
            elements.remove(i);
    }

    for (uint8_t i = 0; i < elements.size(); ++i)
        for (uint8_t j = 1; j < elements.size() - i; ++j)
           if (elements[j-1]->symbol < elements[j]->symbol){
               Element * tempElem = elements[j-1];
               elements[j-1] = elements[j];
               elements[j] = tempElem;

           }
}

void MainWindow::setSymbolsToTable() {
    ui->tableWidget->clear();
    while (ui->tableWidget->rowCount()!= 0)
        ui->tableWidget->removeRow(0);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()<<"Symbol"<<"Liczba"<<"Kod ASCII"<<"Kod Huffmana");
    ui->tableWidget->setRowCount(elements.size());

    for (uint8_t i = 0; i < elements.size(); ++i){
       QTableWidgetItem * item;
       if (elements[i]->symbol.getSymbol() != ' ')
            item = new QTableWidgetItem(QString(elements[i]->symbol.getSymbol()));
       else
           item = new QTableWidgetItem("Space");
       ui->tableWidget->setItem(i, 0, item);
       item = new QTableWidgetItem(QString::number(elements[i]->symbol.getSymbolNumber()));
       ui->tableWidget->setItem(i, 1, item);
       item = new QTableWidgetItem(QString::number(elements[i]->symbol.getSymbol().toLatin1()));
       ui->tableWidget->setItem(i, 2, item);
       item = new QTableWidgetItem(elements[i]->huffmanCode);
       ui->tableWidget->setItem(i, 3, item);
    }
}

void MainWindow::setRoot() {
    QVector<Element*> tempEl;
    for (uint8_t i = 0; i < elements.size(); i++)
        tempEl.push_back(new Element(elements[i]->symbol, elements[i]->symbol.getSymbolNumber()));
    for (uint8_t i = tempEl.size() - 1; i > 0;  i--) {
        Element * tempEle = new Element(tempEl[i-1], tempEl[i], tempEl[i-1]->numberSymbolsNode + tempEl[i]->numberSymbolsNode);
        tempEl[i-1]->parentNode = tempEle;
        tempEl[i]->parentNode = tempEle;
        tempEl[i-1] = tempEle;
        for (uint8_t j = i-1; j > 0; j--) {
            if (tempEl[j]->numberSymbolsNode > tempEl[j-1]->numberSymbolsNode) {
                Element *temp1 = tempEl[j-1];
                tempEl[j-1] = tempEl[j];;
                tempEl[j] = temp1;
            }
            else
                break;
        }
    }
    rootElement = tempEl[0];
}

void MainWindow::drawNodes(int16_t xPosition, int16_t yPosition, Element *node, TypeNode typeNode, int8_t level, uint8_t levels) {
    int16_t yLine = 100, y = 150, x = pow(2, levels) * 28 ;
    int8_t lev = level + 1;
    if (typeNode == Root && node->leftNode == NULL && node->rifgtNode == NULL) {
        scene->addEllipse(xPosition,yPosition,50,50, QPen(QBrush(Qt::red),2), QBrush(Qt::NoBrush));
        QGraphicsTextItem * textItem = new QGraphicsTextItem();
        textItem->setPos(xPosition+18 - getXPosition(node->numberSymbolsNode),yPosition +13);
        textItem->setPlainText(QString::number(node->numberSymbolsNode));
        scene->addItem(textItem);
        node->huffmanCode = "0";
        xPosition -= 100; yPosition += y;
        scene->addLine(xPosition - 25, yPosition, xPosition + 50  - 25, yPosition, QPen(Qt::blue, 2, Qt::SolidLine,Qt::RoundCap));
        scene->addLine(xPosition - 25, yPosition + 50, xPosition + 50  - 25, yPosition + 50, QPen(Qt::blue, 2, Qt::SolidLine,Qt::RoundCap));
        scene->addLine(xPosition + 50 - 25, yPosition, xPosition + 50 - 25, yPosition + 50, QPen(Qt::blue, 2, Qt::SolidLine,Qt::RoundCap));
        scene->addLine(xPosition - 25, yPosition, xPosition - 25, yPosition + 50, QPen(Qt::blue, 2, Qt::SolidLine,Qt::RoundCap));
        QGraphicsTextItem * textItem1 = new QGraphicsTextItem();
        if (node->symbol.getSymbol() != ' ') {
            textItem1->setPos(xPosition+3 - 25,yPosition +15);
            textItem1->setPlainText("Znak: '" + QString(node->symbol.getSymbol()) + "'");
        }
        else {
            textItem1->setPos(xPosition - 25,yPosition +15);
            textItem1->setPlainText("z: Space");
        }
        scene->addItem(textItem1);
        QGraphicsTextItem * textItem2 = new QGraphicsTextItem();
        textItem2->setPos(xPosition+10 - 25 - getXPosition(node->numberSymbolsNode),yPosition );
        textItem2->setPlainText("W: " + QString::number(node->numberSymbolsNode));
        scene->addItem(textItem2);
        QGraphicsTextItem * textItem3 = new QGraphicsTextItem();
        textItem3->setPos(xPosition - 3.5 - 3 * node->huffmanCode.length(),yPosition + 30);
        textItem3->setPlainText(node->huffmanCode);
        scene->addItem(textItem3);
        QGraphicsTextItem * textItem4 = new QGraphicsTextItem();
        textItem4->setPos(xPosition + 125 / 2 - 25  ,yPosition - 70);
        textItem4->setPlainText("0");
        scene->addItem(textItem4);
        scene->addLine(xPosition + 125, yPosition - 100, xPosition, yPosition, QPen(Qt::green, 2, Qt::SolidLine,Qt::RoundCap));
        setCodeHuffmanaToElement(node->symbol.getSymbol(), node->huffmanCode);
          //drawNodes(xPosition + 25 - x, yPosition + y, node, Left, 0, levels);
        return;
    }
    else if (typeNode == Root && node->leftNode != NULL && node->rifgtNode != NULL) {
        scene->addEllipse(xPosition,yPosition,50,50, QPen(QBrush(Qt::red),2), QBrush(Qt::NoBrush));
        QGraphicsTextItem * textItem = new QGraphicsTextItem();
        textItem->setPos(xPosition+18 - getXPosition(node->numberSymbolsNode),yPosition +13);
        textItem->setPlainText(QString::number(node->numberSymbolsNode));
        scene->addItem(textItem);
        node->leftNode->huffmanCode = "0";
        node->rifgtNode->huffmanCode = "1";
        drawNodes(xPosition + 25 - x , yPosition + y, node->leftNode, Left, lev, levels);
        drawNodes(xPosition + 25 + x, yPosition + y, node->rifgtNode, Right ,lev, levels);
    }
    else if (typeNode == Left && node->leftNode != NULL && node->rifgtNode != NULL) {
        scene->addEllipse(xPosition - 25,yPosition,50,50, QPen(QBrush(Qt::red),2), QBrush(Qt::NoBrush));
        QGraphicsTextItem * textItem = new QGraphicsTextItem();
        textItem->setPos(xPosition+18 - 25 - getXPosition(node->numberSymbolsNode),yPosition +13);
        textItem->setPlainText(QString::number(node->numberSymbolsNode));
        scene->addItem(textItem);
        if (level != 1)
            scene->addLine(xPosition + (x / pow(2, level -1)), yPosition - yLine, xPosition, yPosition, QPen(Qt::green, 2, Qt::SolidLine,Qt::RoundCap));
        else
            scene->addLine(xPosition + x, yPosition - yLine, xPosition, yPosition, QPen(Qt::green, 2, Qt::SolidLine,Qt::RoundCap));
        uint8_t mLev = 0;
        node->leftNode->huffmanCode = node->huffmanCode + "0";
        node->rifgtNode->huffmanCode = node->huffmanCode + "1";
        QGraphicsTextItem * textItem4 = new QGraphicsTextItem();
        textItem4->setPos(xPosition + (x / pow(2, level -1)) / 2 - 25  ,yPosition - 70);
        textItem4->setPlainText("0");
        scene->addItem(textItem4);
        drawNodes(xPosition - (x / pow(2, level - mLev)), yPosition + y, node->leftNode, Left, lev - mLev, levels);
        drawNodes(xPosition + (x / pow(2, level - mLev)), yPosition + y, node->rifgtNode, Right ,lev - mLev, levels);
    }
    else if (typeNode == Right && node->leftNode != NULL && node->rifgtNode != NULL) {
        scene->addEllipse(xPosition - 25,yPosition,50,50, QPen(QBrush(Qt::red),2), QBrush(Qt::NoBrush));
        QGraphicsTextItem * textItem = new QGraphicsTextItem();
        textItem->setPos(xPosition+18 -25 - getXPosition(node->numberSymbolsNode),yPosition +13);
        textItem->setPlainText(QString::number(node->numberSymbolsNode));
        scene->addItem(textItem);
        if (level != 1)
            scene->addLine(xPosition - (x / pow(2, level -1)), yPosition - yLine, xPosition, yPosition, QPen(Qt::green, 2, Qt::SolidLine,Qt::RoundCap));
        else
            scene->addLine(xPosition - x, yPosition - yLine, xPosition, yPosition, QPen(Qt::green, 2, Qt::SolidLine,Qt::RoundCap));
        uint8_t mLev = 0;
        node->leftNode->huffmanCode = node->huffmanCode  + "0";
        node->rifgtNode->huffmanCode = node->huffmanCode + "1";
        QGraphicsTextItem * textItem4 = new QGraphicsTextItem();
        textItem4->setPos(xPosition - (x / pow(2, level -1)) / 2 + 10  ,yPosition - 70);
        textItem4->setPlainText("1");
        scene->addItem(textItem4);
        drawNodes(xPosition - (x / pow(2, level - mLev)), yPosition + y, node->leftNode, Left, lev - mLev, levels);
        drawNodes(xPosition + (x / pow(2, level - mLev)), yPosition + y, node->rifgtNode, Right ,lev  - mLev, levels);
    }
    else if (typeNode == Left && node->leftNode == NULL && node->rifgtNode == NULL) {
        scene->addLine(xPosition - 25, yPosition, xPosition + 50  - 25, yPosition, QPen(Qt::blue, 2, Qt::SolidLine,Qt::RoundCap));
        scene->addLine(xPosition - 25, yPosition + 50, xPosition + 50  - 25, yPosition + 50, QPen(Qt::blue, 2, Qt::SolidLine,Qt::RoundCap));
        scene->addLine(xPosition + 50 - 25, yPosition, xPosition + 50 - 25, yPosition + 50, QPen(Qt::blue, 2, Qt::SolidLine,Qt::RoundCap));
        scene->addLine(xPosition - 25, yPosition, xPosition - 25, yPosition + 50, QPen(Qt::blue, 2, Qt::SolidLine,Qt::RoundCap));QGraphicsTextItem * textItem = new QGraphicsTextItem();
        if (node->symbol.getSymbol() != ' ') {
            textItem->setPos(xPosition+3 - 25,yPosition +15);
            textItem->setPlainText("Znak: '" + QString(node->symbol.getSymbol()) + "'");
        }
        else {
            textItem->setPos(xPosition - 25,yPosition +15);
            textItem->setPlainText("z: Space");
        }
        scene->addItem(textItem);
        QGraphicsTextItem * textItem2 = new QGraphicsTextItem();
        textItem2->setPos(xPosition+10 - 25 - getXPosition(node->numberSymbolsNode),yPosition );
        textItem2->setPlainText("W: " + QString::number(node->numberSymbolsNode));
        scene->addItem(textItem2);
        QGraphicsTextItem * textItem3 = new QGraphicsTextItem();
        textItem3->setPos(xPosition - 3.5 - 3 * node->huffmanCode.length(),yPosition + 30);
        textItem3->setPlainText(node->huffmanCode);
        scene->addItem(textItem3);
        QGraphicsTextItem * textItem4 = new QGraphicsTextItem();
        textItem4->setPos(xPosition + (x / pow(2, level -1)) / 2 - 25  ,yPosition - 70);
        textItem4->setPlainText("0");
        scene->addItem(textItem4);
        scene->addLine(xPosition + (x / pow(2, level -1)), yPosition - 100, xPosition, yPosition, QPen(Qt::green, 2, Qt::SolidLine,Qt::RoundCap));
        setCodeHuffmanaToElement(node->symbol.getSymbol(), node->huffmanCode);
    }
    else if (typeNode == Right && node->leftNode == NULL && node->rifgtNode == NULL) {
        scene->addLine(xPosition - 25, yPosition, xPosition + 50  - 25, yPosition, QPen(Qt::blue, 2, Qt::SolidLine,Qt::RoundCap));
        scene->addLine(xPosition - 25, yPosition + 50, xPosition + 50  - 25, yPosition + 50, QPen(Qt::blue, 2, Qt::SolidLine,Qt::RoundCap));
        scene->addLine(xPosition + 50 - 25, yPosition, xPosition + 50 - 25, yPosition + 50, QPen(Qt::blue, 2, Qt::SolidLine,Qt::RoundCap));
        scene->addLine(xPosition - 25, yPosition, xPosition - 25, yPosition + 50, QPen(Qt::blue, 2, Qt::SolidLine,Qt::RoundCap));
        QGraphicsTextItem * textItem = new QGraphicsTextItem();
        if (node->symbol.getSymbol() != ' ') {
            textItem->setPos(xPosition+3 - 25,yPosition +15);
            textItem->setPlainText("Znak: '" + QString(node->symbol.getSymbol()) + "'");
        }
        else {
            textItem->setPos(xPosition+ - 25,yPosition +15);
            textItem->setPlainText("z: Space");
        }
        scene->addItem(textItem);
         QGraphicsTextItem * textItem2 = new QGraphicsTextItem();
        textItem2->setPos(xPosition+10 -25 - getXPosition(node->numberSymbolsNode),yPosition);
        textItem2->setPlainText("W: " + QString::number(node->numberSymbolsNode));
        scene->addItem(textItem2);
        QGraphicsTextItem * textItem3 = new QGraphicsTextItem();
        textItem3->setPos(xPosition - 3.5 - 3 * node->huffmanCode.length() ,yPosition + 30);
        textItem3->setPlainText(node->huffmanCode);
        scene->addItem(textItem3);
        QGraphicsTextItem * textItem4 = new QGraphicsTextItem();
        textItem4->setPos(xPosition - (x / pow(2, level -1)) / 2 + 10  ,yPosition - 70);
        textItem4->setPlainText("1");
        scene->addItem(textItem4);
        scene->addLine(xPosition - (x / pow(2, level -1)), yPosition - 100, xPosition, yPosition, QPen(Qt::green, 2, Qt::SolidLine,Qt::RoundCap));
        setCodeHuffmanaToElement(node->symbol.getSymbol(), node->huffmanCode);
    }
}
/*

uint8_t MainWindow::countLevel(uint8_t numberSymbols) {
    uint8_t n = numberSymbols;
    uint8_t count = 0;
    if (numberSymbols < 2)
        return 0;
    numberSymbols--;
    while (numberSymbols > 0) {
        numberSymbols /= 2;
        count++;
    }
    if (rootElement->numberSymbolsNode/n >10)
        count++;
    if (rootElement->numberSymbolsNode/n >15)
        count++;
    if (rootElement->numberSymbolsNode/n >20)
        count++;
    if (rootElement->numberSymbolsNode/n >5)
        count++;
    if (rootElement->numberSymbolsNode/n >8)
        count++;
    if (rootElement->numberSymbolsNode/n >13)
        count++;

    return count - 1;

}
*/
uint8_t MainWindow::countLevel(Element *node, uint8_t numberSymbols) {
    if (node->leftNode == NULL && node->rifgtNode == NULL)
        return numberSymbols - 1;
    else {
        uint8_t leftN = countLevel(node->leftNode, numberSymbols + 1);
        uint8_t rightN = countLevel(node->rifgtNode, numberSymbols +1);
        if (leftN > rightN)
            return  leftN;
        return rightN;
    }
    return numberSymbols;
}


void MainWindow::setCodeHuffmanaToElement(QChar _char, QString codeHuff){
    for (uint8_t i = 0; i < elements.size(); i++)
        if (elements[i]->symbol.getSymbol() == _char){
            elements[i]->huffmanCode = codeHuff;
            return;
        }
}

void MainWindow::on_pushButton_SaveImage_clicked()
{
    QString fileName= QFileDialog::getSaveFileName(this, "Save image", QCoreApplication::applicationDirPath(), "BMP Files (*.bmp);;JPEG (*.JPEG);;PNG (*.png)" );
    if (!fileName.isNull()) {
        QPixmap pixMap = this->ui->graphicsView->grab();
        pixMap.save(fileName);
    }
}

uint16_t MainWindow::getXPosition(uint16_t n) {
    uint8_t xPos = 0;
    if (n >9)
        xPos += 3;
    if (n >99)
        xPos += 3;
    if (n >999)
        xPos += 3;
    return xPos;
}

void MainWindow::setDataOut() {
   QString dataInput = ui->textEdit_InputData->toPlainText();
   bytesInput = dataInput.length();
   bitsInput = bytesInput * 8;
   dataOutput.clear();
   bytesOutput = 0;
   bitsOutput = 0;

   for (uint16_t i = 0; i < bytesInput; i++) {
       for (uint8_t j = 0; j < elements.size(); j++) {
           if (dataInput[i] == elements[j]->symbol.getSymbol()){
               dataOutput += elements[j]->huffmanCode;
               if (i < bytesInput -1)
                   dataOutput += " ";
               bitsOutput += elements[j]->huffmanCode.length();
               break;
           }
       }
   }
   bytesOutput = bitsOutput / 8;
   if (bitsOutput % 8 != 0)
       bytesOutput++;
   ui->textEdit_OutputData->setText(dataOutput);
   ui->lineEdit_InputBits->setText(QString::number(bitsInput));
   ui->lineEdit_OutputBits->setText(QString::number(bitsOutput));
   ui->lineEdit_InputBytes->setText(QString::number(bytesInput));
   ui->lineEdit_OutpputBytes->setText(QString::number(bytesOutput));
   ui->lineEdit_CompreseBytes->setText(QString::number((1 - (((float) bytesOutput) / ((float) bytesInput)))*100,'f',2) + "%");
   ui->lineEdit_CompreseBits->setText(QString::number((1 - (((float) bitsOutput) / ((float) bitsInput)))*100,'f',2) + "%");
   ui->lineEdit_AveregeWordLength->setText(QString::number( ((float)bitsOutput) / bytesInput,'f',4));
   ui->lineEdit_Entropy->setText(QString::number(countEntropy(rootElement, rootElement->numberSymbolsNode) ,'f',6));
}

double MainWindow::countEntropy(Element *node, uint16_t n) {
    if (node->leftNode == NULL && node->rifgtNode == NULL) {
        double prob = (double)node->numberSymbolsNode / n;
        return prob * log(1/prob)/log(2);
    }
    return countEntropy(node->leftNode, n) + countEntropy(node->rifgtNode, n);
}
