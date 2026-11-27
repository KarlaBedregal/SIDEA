#pragma once
#include <QWidget>
#include <QListWidget>

class LogPanel : public QWidget {
    Q_OBJECT
public:
    explicit LogPanel(QWidget* parent = nullptr);

public slots:
    void agregarEvento(const QString& texto, const QString& tipo);

private:
    void configurarUI();
    QListWidget* m_lista = nullptr;
    static constexpr int MAX_ENTRADAS = 60;
};
