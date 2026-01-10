#pragma once

#include <QObject>

class MediaController : public QObject
{
public:
    Q_OBJECT
public:
    MediaController(QObject* parent = nullptr);

};