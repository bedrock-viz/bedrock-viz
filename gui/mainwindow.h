#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  private slots:
    void on_btnMCPEWorld_clicked();

    void on_btnOutputDirectory_clicked();

    void on_btnGo_clicked();

    void on_btnWebApp_clicked();

    void on_btnCheckUpdate_clicked();

    void on_action_About_triggered();

    void processStandardOutput();
    void processStandardError();
    void processFinished(int exitCode,  QProcess::ExitStatus exitStatus);

    void parseCheckUpdate(QNetworkReply* pReply);
    void parseChangeLog(QNetworkReply* pReply);
    
 private:
    Ui::MainWindow *ui;
    QProcess *mcpeVizProcess;
    QString m_sSettingsFile;
    std::string newVersion;
    
    QNetworkAccessManager* m_naManager;
    QNetworkAccessManager* m_naManager2;
    
    int getCommandLine(std::string &cmd);
    int checkNetworkError(QNetworkReply* pReply);
    void saveSettings();
    void loadSettings();
};

#endif // MAINWINDOW_H
