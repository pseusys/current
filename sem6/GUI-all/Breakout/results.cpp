#include <QLayout>
#include <QLabel>
#include <QScrollArea>
#include <fstream>
#include <iostream>
#include <sstream>
#include "results.h"

void parseResults(std::vector<QString>* results) {
    std::stringstream resultsStream;

    try {
        std::ifstream fileStream;
        fileStream.open("./results.400s");
        resultsStream << fileStream.rdbuf();
        fileStream.close();
    } catch(std::ifstream::failure& e) {
        std::cout << "read not ok" << std::endl;
    }

    std::string date, res;
    while ((resultsStream >> date) && (resultsStream >> res)) {
        results->push_back(QString(date.c_str()) + QString(": ") + QString(res.c_str()));
    }
}

Results::Results(QWidget *parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout();

    QLabel* label = new QLabel();
    label->resize(200, 50);
    label->setFont(QFont("Courier", 15, QFont::DemiBold));
    label->setText("Scores:");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);

    QScrollArea* scroll = new QScrollArea();
    scroll->resize(200, 200);

    QWidget* resultsParent = new QWidget();
    QVBoxLayout* resultsLayout = new QVBoxLayout();

    std::vector<QString>* results = new std::vector<QString>();
    parseResults(results);
    for (unsigned long long i = 0; i < results->size(); i++) {
        QLabel* result = new QLabel();
        result->resize(200, 50);
        result->setFont(QFont("Courier", 9, QFont::Medium));
        result->setText(QString::number(i + 1) + QString(" ") + results->at(i));
        resultsLayout->addWidget(result);
    }

    resultsParent->setLayout(resultsLayout);

    scroll->setWidget(resultsParent);
    layout->addWidget(scroll);

    QPushButton* exit = new QPushButton();
    exit->resize(100, 25);
    exit->setText("Return");
    connect(exit, SIGNAL(released()), this, SIGNAL(finish()));
    layout->addWidget(exit);

    setLayout(layout);
}
