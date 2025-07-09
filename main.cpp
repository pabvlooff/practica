#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QProcess>
#include <QLabel>
#include <QMessageBox>

class PingWidget : public QWidget
{
    Q_OBJECT
public:
    PingWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        // Настройка интерфейса
        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        // Панель ввода
        QHBoxLayout *inputLayout = new QHBoxLayout();
        QLabel *label = new QLabel("Адрес для пинга:");
        m_hostEdit = new QLineEdit("Введите IP-адрес или домен");
        m_pingButton = new QPushButton("Запинговать");
        m_pingButton->setFixedWidth(100);

        inputLayout->addWidget(label);
        inputLayout->addWidget(m_hostEdit);
        inputLayout->addWidget(m_pingButton);

        // Поле вывода
        m_output = new QTextEdit();
        m_output->setReadOnly(true);
        m_output->setFontFamily("Times New Roman");

        // Кнопка очистки
        QPushButton *clearButton = new QPushButton("Очистить");
        clearButton->setFixedWidth(100);

        // Добавление элементов в основной layout
        mainLayout->addLayout(inputLayout);
        mainLayout->addWidget(m_output);
        mainLayout->addWidget(clearButton, 0, Qt::AlignRight);

        // Подключение сигналов
        connect(m_pingButton, &QPushButton::clicked, this, &PingWidget::startPing);
        connect(clearButton, &QPushButton::clicked, m_output, &QTextEdit::clear);

        // Настройка окна
        setWindowTitle("Пинг");
        resize(600, 400);
    }

public slots:
    void startPing() {
        QString host = m_hostEdit->text().trimmed();

        if (host.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Введите адрес для пинга");
            return;
        }

        m_output->clear();
        m_process = new QProcess(this);

        // Настройка процесса
        connect(m_process, &QProcess::readyReadStandardOutput, [this]() {
            QString output = QString::fromLocal8Bit(m_process->readAllStandardOutput());
            m_output->moveCursor(QTextCursor::End);
            m_output->insertPlainText(output);
            m_output->moveCursor(QTextCursor::End);
        });

        connect(m_process, &QProcess::finished, [this](int exitCode) {
            m_output->append(QString("\nPing завершен с кодом: %1").arg(exitCode));
            m_pingButton->setEnabled(true);
        });

        m_pingButton->setEnabled(false);

        // Запуск ping (разные команды для Windows и других ОС)
        #ifdef Q_OS_WIN
            m_process->start("ping", QStringList() << "-n" << "4" << host);
        #else
            m_process->start("ping", QStringList() << "-c" << "4" << host);
        #endif

        if (!m_process->waitForStarted()) {
            m_output->append("Не удалось запустить ping");
            m_pingButton->setEnabled(true);
        }
    }

private:
    QLineEdit *m_hostEdit;
    QTextEdit *m_output;
    QPushButton *m_pingButton;
    QProcess *m_process;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    PingWidget widget;
    widget.show();

    return a.exec();
}

#include "main.moc"
