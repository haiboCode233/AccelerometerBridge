#include "apiwindow.h"
#include "ui_apiwindow.h"
#include "module/api/api.h"

APIWindow::APIWindow(QWidget *parent)
    : QWidget{parent}
{
    ui = new Ui::centralwidget();
    ui->setupUi(this);
    setWindowTitle("API Test");
}

void APIWindow::on_btn_init_clicked()
{
    if(api_init())
    {
        ui->textBrowser->insertPlainText("初始化成功，开启UDP端口");
        ui->textBrowser->insertPlainText("\r\n");
        ui->textBrowser->insertPlainText("\r\n");
    }
    else
    {
        ui->textBrowser->insertPlainText("已完成初始化");
        ui->textBrowser->insertPlainText("\r\n");
        ui->textBrowser->insertPlainText("\r\n");
    }

}

void APIWindow::on_btn_deinit_clicked()
{
    if(api_deinit())
    {
        ui->textBrowser->insertPlainText("关闭UDP端口成功");
        ui->textBrowser->insertPlainText("\r\n");
        ui->textBrowser->insertPlainText("\r\n");
    }
    else
    {
        ui->textBrowser->insertPlainText("UDP端口已关闭");
        ui->textBrowser->insertPlainText("\r\n");
        ui->textBrowser->insertPlainText("\r\n");
    }

}

void APIWindow::on_btn_clearTextBox_clicked()
{
    ui->textBrowser->clear();

}

void APIWindow::on_btn_getData_clicked()
{
    if(api_getData_once())
    {
        ui->textBrowser->insertPlainText("请先完成初始化");
        ui->textBrowser->insertPlainText("\r\n");
        ui->textBrowser->insertPlainText("\r\n");
    }
    else
    {
        ui->textBrowser->insertPlainText("已成功收集数据");
        ui->textBrowser->insertPlainText("\r\n");
        ui->textBrowser->insertPlainText("\r\n");
    }
}


void APIWindow::on_btn_showData_clicked()
{
    std::ostringstream output;  // Create a stringstream to hold the output
    if(api_show_data())
    {
        output << std::endl <<"请先初始化" << std::endl<< std::endl;
    }
    else
    {
        int row = 0;
        int col = 0;

        for(row=0;row<DATA_ROWS;row++)
        {
            for (row = 0; row < DATA_ROWS; row++) {
                output << std::endl << "Channel: " << row + 1 << std::endl;
                for (col = 0; col < DATA_COLS; col++) {
                    output << std::setiosflags(std::ios::scientific) << std::setprecision(5) << api_data->readFromArray()[row][col] << " \t";
                    if (((col + 1) % 4 == 0 && col + 1 != DATA_COLS) || (row + 1 == DATA_ROWS && col + 1 == DATA_COLS)) {
                        output<<std::endl;
                    }
                }
            }
        }
    }
    std::string result = output.str();  // Get the content of the stringstream as a string
    QString q_str = QString::fromStdString(result);
    ui->textBrowser->insertPlainText(q_str);
}


void APIWindow::on_btn_getShow_clicked()
{
    std::ostringstream output;  // Create a stringstream to hold the output
    if(api_getShow())
    {
        output << std::endl <<"请先初始化" << std::endl<< std::endl;
    }
    else
    {
        int row = 0;
        int col = 0;

        for(row=0;row<DATA_ROWS;row++)
        {
            for (row = 0; row < DATA_ROWS; row++) {
                output << std::endl << "Channel: " << row + 1 << std::endl;
                for (col = 0; col < DATA_COLS; col++) {
                    output << std::setiosflags(std::ios::scientific) << std::setprecision(5) << api_data->readFromArray()[row][col] << " \t";
                    if (((col + 1) % 4 == 0 && col + 1 != DATA_COLS) || (row + 1 == DATA_ROWS && col + 1 == DATA_COLS)) {
                        output<<std::endl;
                    }
                }
            }
        }
    }
    std::string result = output.str();  // Get the content of the stringstream as a string
    QString q_str = QString::fromStdString(result);
    ui->textBrowser->insertPlainText(q_str);
}




void APIWindow::on_btn_autoFileName_clicked()
{
    api_auto_fileName();
    ui->textBrowser->insertPlainText("已创建文件：");
    ui->textBrowser->insertPlainText("\r\n");
    ui->textBrowser->insertPlainText(QString::fromStdString(api_fileWriter->outPutFileName));
    ui->textBrowser->insertPlainText("\r\n");
    ui->textBrowser->insertPlainText("\r\n");
}


void APIWindow::on_btn_writeFile_clicked()
{
    if(api_write_file())
    {
        ui->textBrowser->insertPlainText("请先创建文件");
        ui->textBrowser->insertPlainText("\r\n");
        ui->textBrowser->insertPlainText("\r\n");
    }
    else
    {
        ui->textBrowser->insertPlainText("已将数据写入文件");
        ui->textBrowser->insertPlainText("\r\n");
        ui->textBrowser->insertPlainText("\r\n");
    }

}


void APIWindow::on_textBrowser_textChanged()
{
    ui->textBrowser->moveCursor(QTextCursor::End);
}

