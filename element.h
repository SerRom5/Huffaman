#ifndef ELEMENT_H
#define ELEMENT_H
#include <QString>

using namespace std;

class Symbol {
    QChar symbol;
    uint16_t numberSymbols;

public:
    Symbol(QChar _symbol, uint16_t _numSym) : symbol(_symbol), numberSymbols(_numSym) {}
    Symbol() : symbol(0), numberSymbols(0) {}
    void setSymbol(const Symbol &_symbol) {
        symbol = _symbol.symbol;
        numberSymbols = _symbol.numberSymbols;
    }

    void incrementNumberSymbol() {   ++numberSymbols;    }
    void decrementNumberSymbol() {   --numberSymbols;    }

    const QChar &getSymbol() {   return symbol;    }
    void setSymbolNumber(const int16_t &_symbolNumber) {    numberSymbols = _symbolNumber;   }
    int16_t getSymbolNumber() {   return numberSymbols;   }

    friend bool operator<(const Symbol &sym1, const Symbol &sym2) {   return  sym1.numberSymbols < sym2.numberSymbols;  }
};


// struktura elementu drzewa  binarnego
class Element
{
public:
    Element * parentNode = NULL;
    Element * leftNode = NULL;
    Element * rifgtNode = NULL;
    Symbol symbol;
    uint16_t numberSymbolsNode;
    QString huffmanCode = "";

    Element(Element *_leftNode, Element *_rightNode, const  uint16_t _numSymNode = 0) : leftNode(_leftNode), rifgtNode(_rightNode), numberSymbolsNode(_numSymNode) {}
    Element(const Symbol _sym, uint16_t _numberSymNoe) : symbol(_sym), numberSymbolsNode(_numberSymNoe) {}
    Element(const Symbol _sym) : symbol(_sym) {}
    Element();
};

#endif // ELEMENT_H
