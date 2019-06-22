#ifndef BAND_H
#define BAND_H

#include <QtCore>

class Band
{
public:
    Band() { }
    Band(QString name, float start, float end) : _name(name), _start(start), _end(end) { }

    QString& name() { return _name; }
    float start() { return _start; }
    float end() { return _end; }

private:
    QString _name;
    float _start;
    float _end;
};

#endif // BAND_H
