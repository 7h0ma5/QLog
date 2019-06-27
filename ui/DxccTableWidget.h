#ifndef DXCCTABLEWIDGET_H
#define DXCCTABLEWIDGET_H

#include <QWidget>
#include <QTableView>

class DxccTableModel;

class DxccTableWidget : public QTableView
{
    Q_OBJECT
public:
    explicit DxccTableWidget(QWidget *parent = nullptr);

signals:

public slots:
    void clear();
    void setDxcc(int dxcc);

private:
    DxccTableModel* dxccTableModel;
};

#endif // DXCCTABLEWIDGET_H
