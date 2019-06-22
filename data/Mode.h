#ifndef MODE_H
#define MODE_H

#include <QtCore>

class Mode
{
public:
    Mode() {}
    Mode(QString name) : _name(name) { }

    QString& defaultRst() { return _defaultRst; }
    void setDefaultRst(QString newDefaultRst) { _defaultRst = newDefaultRst; }

    QString& name() { return _name; }

    QStringList& submodes() { return _submodes; }

    void addSubmode(QString submode) {
        _submodes.append(submode);
    }

    void addSubmodes(QStringList submodes) {
        _submodes.append(submodes);
    }

private:
    QString _name;
    QString _defaultRst;
    QStringList _submodes;
};

#endif // MODE_H
