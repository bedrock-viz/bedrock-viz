/*
 * MCPE Viz Helper/GUI
 * (c) Plethora777 -- 2015.11.05
 *
 * GPL'ed code - please see LICENSE
 *
 * todo
 *   optionally, play sound on success / failure?
 *
 */

#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QDesktopServices>
#include <QSettings>
#include <QUrl>
#include <QRegExp>
#include <vector>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../mcpe_viz.version.h"


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  this->setWindowTitle(tr("MCPE Viz Helper by Plethora777"));

  QLabel *label =
    new QLabel(tr(std::string("<html><a href=\"https://github.com/Plethora777/mcpe_viz\">MCPE Viz Helper</a> v" + mcpe_viz_version_short + " by <a href=\"https://twitter.com/Plethora777\">Plethora777</a></html>").c_str()));
  label->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
  label->setOpenExternalLinks(true);
  statusBar()->addPermanentWidget(label);

  mcpeVizProcess = NULL;

#ifdef Q_OS_WIN32
  m_sSettingsFile = QApplication::applicationDirPath() + "\\mcpe_viz_helper.ini";
#else
  m_sSettingsFile = QApplication::applicationDirPath() + "/mcpe_viz_helper.ini";
#endif
    
  loadSettings();

  m_naManager = NULL;
  m_naManager2 = NULL;
}

MainWindow::~MainWindow()
{
  saveSettings();
  delete ui;
  if (m_naManager) {
    delete m_naManager;
  }
  if (m_naManager2) {
    delete m_naManager2;
  }
}

void MainWindow::loadSettings() {
  QSettings settings(m_sSettingsFile, QSettings::IniFormat);
  QString sText;

  sText = settings.value("dirMCPEWorld", "").toString();
  ui->txtMCPEWorld->setText(sText);

  sText = settings.value("dirOutput", "").toString();
  ui->txtOutputDirectory->setText(sText);

  sText = settings.value("outputName", "").toString();
  ui->txtOutputName->setText(sText);

  int modeId = settings.value("mode", "0").toInt();
  ui->cbImages->setCurrentIndex(modeId);
}

void MainWindow::saveSettings() {
  QSettings settings(m_sSettingsFile, QSettings::IniFormat);
  settings.setValue("dirMCPEWorld", ui->txtMCPEWorld->text());
  settings.setValue("dirOutput", ui->txtOutputDirectory->text());
  settings.setValue("outputName", ui->txtOutputName->text());
  settings.setValue("mode", ui->cbImages->currentIndex());
}

int MainWindow::getCommandLine(std::string &cmd) {
  cmd = "";
  typedef std::vector<std::string> StringList;
  StringList cmd_args;
  StringList err_list;
  QFileInfo fi;

  // we want to tile if needed
  cmd_args.push_back("--auto-tile");
    
  // check that all is well
  fi = QFileInfo(ui->txtMCPEWorld->text());
  QString dirDb = fi.absolutePath();
  if (dirDb.length() > 0) {
    cmd_args.push_back("--db \"" + dirDb.toStdString() + "\"");
  } else {
    err_list.push_back("Invalid MCPE World directory");
  }

  fi = QFileInfo(ui->txtOutputDirectory->text());
  if ( ! fi.isDir() ) {
    err_list.push_back("Output Directory is not a directory");
  }
  if ( ! fi.isWritable() ) {
    err_list.push_back("We cannot write to Output Directory");
  }

  QString dirOutput = ui->txtOutputDirectory->text();
  QString outputName = QFileInfo(ui->txtOutputName->text()).baseName();
  if (dirOutput.length() > 0 && outputName.length() > 0) {
    cmd_args.push_back("--out \"" + dirOutput.toStdString() + "/" + outputName.toStdString() + "\"");
  } else {
    err_list.push_back("Invalid Output Directory and/or Output Name");
  }

  // make sure that dirOutput and dirDb are different
  if ( dirOutput == dirDb ) {
    err_list.push_back("Output directory cannot be the same as MCPE World directory");
  }

  // todo - super brittle way to do this
  int mode = ui->cbImages->currentIndex();
  switch (mode) {
  case 0:
    // web most
    cmd_args.push_back("--html-most");
    break;
  case 1:
    // web all
    cmd_args.push_back("--html-all");
    break;
  case 2:
    // image most
    cmd_args.push_back("--all-image");
    break;
  case 3:
    //image all
    cmd_args.push_back("--all-image --slices");
    break;
  default:
    err_list.push_back("Invalid comboBox setting");
    break;
  }

  if (err_list.size() > 0) {
    QString msg="";
    for (StringList::iterator it=err_list.begin(); it != err_list.end(); ++it) {
      msg.append(it->c_str());
      msg.append("\n");
    }
    QMessageBox::warning(this, "Error", msg);
    // todo - would be nice to highlight the fields with issues
    return -1;
  }

  // execute mcpe_viz.exe w/ the proper params and put output in txtProgress
  cmd = "\"" + QApplication::applicationDirPath().toStdString() + "/mcpe_viz\" --flush";

  for (StringList ::iterator it=cmd_args.begin(); it != cmd_args.end(); ++it) {
    cmd += " " + (*it);
  }
  return 0;
}


void MainWindow::processStandardOutput() {
  QTextCursor c = ui->txtProgress->textCursor();
  c.movePosition(QTextCursor::End);
  c.insertText(mcpeVizProcess->readAllStandardOutput());
  ui->txtProgress->ensureCursorVisible();
}

void MainWindow::processStandardError() {
  QTextCursor c = ui->txtProgress->textCursor();
  c.movePosition(QTextCursor::End);
  c.insertText(mcpeVizProcess->readAllStandardError());
  ui->txtProgress->ensureCursorVisible();
}

void MainWindow::processFinished(int exitCode,  QProcess::ExitStatus exitStatus) {
  // QApplication::restoreOverrideCursor();

  if (mcpeVizProcess->exitCode() == 0) {
    ui->txtProgress->appendHtml("<div style=\"color:green\">MCPE Viz completed successfully!<br/>You can launch the web app now.</div>");
    ui->btnWebApp->setEnabled(true);
  } else {
    QMessageBox::warning(this,"Error", "MCPE Viz failed.  See Progress window for details.  If you consistently have this problem, you should switch from mcpe_viz win32 to win64, or vice versa.");
  }

  delete mcpeVizProcess;
  mcpeVizProcess = NULL;
  ui->btnGo->setEnabled(true);
}

void MainWindow::on_btnGo_clicked()
{
  std::string cmd;
  ui->txtProgress->clear();
  ui->btnWebApp->setEnabled(false);
  ui->btnGo->setEnabled(false);
  int ret = getCommandLine(cmd);
  if ( ret == 0 ) {

    ui->txtCommandLine->setText(cmd.c_str());
      
    if (mcpeVizProcess) {
      delete mcpeVizProcess;
    }
    mcpeVizProcess = new QProcess(this);
    connect (mcpeVizProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(processStandardOutput()));  // connect process signals with your code
    connect (mcpeVizProcess, SIGNAL(readyReadStandardError()), this, SLOT(processStandardError()));  // same here
    connect (mcpeVizProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished(int,QProcess::ExitStatus)));  // same here
    //todo - signal error also?
    //QApplication::setOverrideCursor(Qt::WaitCursor);
    mcpeVizProcess->start(cmd.c_str());
    // todo - wait for start?
  } else {
    ui->btnGo->setEnabled(true);
  }
}

void MainWindow::on_btnWebApp_clicked()
{
  QString dirOutput = ui->txtOutputDirectory->text();
  QString outputName = QFileInfo(ui->txtOutputName->text()).baseName();
  if (dirOutput.length() > 0 && outputName.length() > 0) {
    std::string url = dirOutput.toStdString() + "/" + outputName.toStdString() + ".html";
    QDesktopServices::openUrl(QUrl(url.c_str()));
  } else {
    QMessageBox::warning(this,"Error", "Invalid Output Directory and/or Output Name");
  }
}

// from: http://stackoverflow.com/a/1053134
void MainWindow::on_btnCheckUpdate_clicked()
{
  QUrl url("https://raw.githubusercontent.com/Plethora777/mcpe_viz/master/mcpe_viz.version.h");

  if (m_naManager) {
    delete m_naManager;
  }
  m_naManager = new QNetworkAccessManager(this);
  connect(m_naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(parseCheckUpdate(QNetworkReply*)));
  m_naManager->get(QNetworkRequest(url));
}

int MainWindow::checkNetworkError(QNetworkReply* pReply) {
  QNetworkReply::NetworkError err = pReply->error();
  if ( err != QNetworkReply::NoError ) {
    char tmpstring[1025];
    sprintf(tmpstring,
	    "Network Error<br/>"
	    "errorCode=%d<br/>"
	    "errorString=%s<br/>"
	    "httpStatus=%d<br/>"
	    , (int)err
	    , pReply->errorString().toStdString().c_str()
	    , pReply->attribute( QNetworkRequest::HttpStatusCodeAttribute).toInt()
	    );
    QMessageBox::critical(this, "Network Error", tmpstring);
    return -1;
  }
  return 0;
}

void MainWindow::parseCheckUpdate(QNetworkReply* pReply)
{
  if ( checkNetworkError(pReply) != 0 ) {
    pReply->deleteLater();
    return;
  }
  
  QByteArray data=pReply->readAll();

  QString str(data);
  std::string origStr = str.toStdString();
  
  // parse this: mcpe_viz_version_short("X.Y.Z");
  QRegExp rx("mcpe_viz_version_short\\(\"(.+)\"\\)");
  rx.setMinimal(true);
  
  QStringList slist = str.split("\n");

  newVersion = "";
  
  for (int i=0; i < slist.size(); i++) {

    int pos = 0;
    if ((pos = rx.indexIn(slist.at(i), pos)) != -1) {
      if ( rx.cap(1) == QString(mcpe_viz_version_short.c_str()) ) {
	// no update
	QMessageBox::information(this, "No Update",
				 "No update available.<br/><br/>"
				 "You are running the most current version."
				 );
	pReply->deleteLater();
	return;
      } else {
	// update
	newVersion = rx.cap(1).toStdString();

	// we now get the changelog
	QUrl url("https://raw.githubusercontent.com/Plethora777/mcpe_viz/master/ChangeLog");

	if (m_naManager2) {
	  delete m_naManager2;
	}
	m_naManager2 = new QNetworkAccessManager(this);
	connect(m_naManager2, SIGNAL(finished(QNetworkReply*)), this, SLOT(parseChangeLog(QNetworkReply*)));
	m_naManager2->get(QNetworkRequest(url));
	  
	pReply->deleteLater();
	return;
      }
    }

  }

  // problem
  // todo - error details?
  std::string msg = "Sorry, failed to check for update.<br/><br/>" 
    "Data:<br/><code>" + origStr + "</code>"
    ;
  QMessageBox::critical(this, "Error", msg.c_str());
  pReply->deleteLater();
}


void MainWindow::parseChangeLog(QNetworkReply* pReply)
{
  std::string newVersionHighlight = "";
    
  if ( checkNetworkError(pReply) != 0 ) {

    // we did not get the changlog
    newVersionHighlight = "(Sorry, we had a problem checking the ChangeLog -- See ChangeLog on GitHub)";

  } else {
  
    QByteArray data=pReply->readAll();
    
    QString str(data);
    std::string origStr = str.toStdString();
    
    // parse this: mcpe_viz_version_short("X.Y.Z");
    QRegExp rx("^Highlight:\\s*(.+)\\s*$");
    rx.setMinimal(true);
    
    QStringList slist = str.split("\n");
    
    for (int i=0; i < slist.size(); i++) {
      int pos = 0;
      if ((pos = rx.indexIn(slist.at(i), pos)) != -1) {
	newVersionHighlight = rx.cap(1).toStdString();
      }
    }
  }
  
  std::string msg = "You are running <b>v" + mcpe_viz_version_short + "</b> and <b>v" + newVersion + "</b> is available on GitHub.<br/><br/>" +
    "New Version Highlight:<br/><b>" + newVersionHighlight + "</b><br/><br/>" +
    "<a href=\"https://github.com/Plethora777/mcpe_viz\">Click here to go to GitHub and grab the update</a>"
    ;
  QMessageBox::warning(this, "Update Available!", msg.c_str());
  
  pReply->deleteLater();
}

void MainWindow::on_action_About_triggered()
{
  QMessageBox::information(this, "About...",
			   "<html>MCPE Viz Helper by <a href=\"https://twitter.com/Plethora777\">Plethora777</a><br/><br/>"
			   "<a href=\"https://github.com/Plethora777/mcpe_viz\">MCPE Viz on GitHub</a>"
			   "</html>"
			   );
}

void MainWindow::on_btnMCPEWorld_clicked()
{
  QFileDialog dialog(this, tr("Select MCPE World"), "", tr("MCPE World (level.dat)"));
  if (dialog.exec()) {
    QStringList fileNames = dialog.selectedFiles();
    ui->txtMCPEWorld->setText(fileNames[0]);
  }
}

void MainWindow::on_btnOutputDirectory_clicked()
{
  QFileDialog dialog(this, "Select Output Directory", "", "");
  dialog.setFileMode(QFileDialog::Directory);
  if (dialog.exec()) {
    QStringList fileNames = dialog.selectedFiles();
    ui->txtOutputDirectory->setText(fileNames[0]);
  }
}
