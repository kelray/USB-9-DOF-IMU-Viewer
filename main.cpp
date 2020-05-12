#include "mainwindow.h"
#include <QApplication>
#include <QCheckBox>
#include <QColor>
#include <QComboBox>
#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsWidget>
#include <QLabel>
#include <QLineSeries>
#include <QtMath>
#include <QPushButton>
#include <QString>
#include <QSignalMapper>
#include <QTimer>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QWidget>
#include <QMessageBox>
#include <QProgressBar>
#include <QSlider>
#include <QSpinBox>
#include <QComboBox>
#include <QTextEdit>
#include <QRadioButton>
#include <QMenu>
#include <QDial>
#include <QTabWidget>
#include <QMenuBar>
#include <QCheckBox>
#include <QTime>
#include <QDate>
#include <QDateTime>
#include <QApplication>
#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <iso646.h>
#include "GetErrName.h"
#include "MPU9250.h"
#include "mcp2221_dll_um.h"

//Global variables
void *handle;
QString string;
QTextEdit *TextBox;
QTextEdit *ADCdataBox;
QVector<double> x(101), y(101);
int i = 0, j = 0;
unsigned char RxData[7];
unsigned char TxData[2];

//Chart variables
using namespace QtCharts;
QChartView *chartViewAcc;
QLineSeries *seriesX;
QLineSeries *seriesY;
QLineSeries *seriesZ;
QChart *chartAcc;
QPointF numX, numY, numZ;
QComboBox *AccFscaleDropList;
QComboBox *GyroFscaleDropList;
QValueAxis *axisYAcc;

QChartView *chartViewGyro;
QLineSeries *seriesYaw;
QLineSeries *seriesPitch;
QLineSeries *seriesRoll;
QChart *chartGyro;
QPointF numYaw, numPitch, numRoll;
QValueAxis *axisYGyro;

QChartView *chartViewMagneto;
QLineSeries *seriesMx;
QLineSeries *seriesMy;
QLineSeries *seriesMz;
QChart *chartMagneto;
QPointF numMx, numMy, numMz;
QValueAxis *axisYMagneto;
QPointF numIMU[9];

//checkboxes
QCheckBox *GPIO0chkBx;
QCheckBox *GPIO1chkBx;
QCheckBox *GPIO2chkBx;
QCheckBox *GPIO3chkBx;
QCheckBox *SaveTofilechkbx;

int count = 0;
double xVal = 0, yVal = 0, zVal = 0;
qreal x1RangeMax = 180, x1RangeMin = 0, y1RangeMax = 16, y1RangeMin = -16;
qreal yGyroRangeMin = -20, yGyroRangeMax = 20, xGyroRangeMin = 0, xGyroRangeMax = 180;
qreal yMagnetoRangeMin = -480, yMagnetoRangeMax = 480, xMagnetoRangeMin = 0, xMagnetoRangeMax = 180;

char *token;
int RxBufferCounter = 0;
QString xValStr, yValStr, zValStr;
double IMUval[9];
double AccVal[3];
double GyroVal[3];
int FullScaleRange = 0;

//MCP2221 variables
wchar_t SerNum = 0x0000075428;
wchar_t LibVer[6];
wchar_t MfrDescriptor[30];
wchar_t ProdDescrip[30];
int ver = 0;
int error = 0;
int flag = 0;

unsigned int delay = 0;
unsigned int ReqCurrent;
unsigned int PID = 0xDD;
unsigned int VID = 0x4D8;
unsigned int NumOfDev = 0;
unsigned char PowerAttrib;

//I/O variables
unsigned char pinFunc[4] = {MCP2221_GPFUNC_IO, MCP2221_GPFUNC_IO, MCP2221_GPFUNC_IO, MCP2221_GPFUNC_IO};
unsigned char pinDir[4] = {MCP2221_GPDIR_OUTPUT, MCP2221_GPDIR_OUTPUT, MCP2221_GPDIR_OUTPUT, MCP2221_GPDIR_OUTPUT};
unsigned char OutValues[4] = {MCP2221_GPVAL_LOW, MCP2221_GPVAL_LOW, MCP2221_GPVAL_LOW, MCP2221_GPVAL_LOW};
unsigned int ADCbuffer[3];
unsigned char DacVal = 0;
unsigned int ADCreading = 0;
unsigned int NegativeFlag = 0;

#define I2cAddr7bit 1
#define I2cAddr8bit 0

//File logging variables
FILE * logFile;		
int fileFlag = 0;
QString fileName;
char FileName[80] = "log-";

//Create instance of MPU9250
extern unsigned char MPU9250_I2C_ADDR; 	//default MPU9250 I2C address
MPU9250 IMU(MPU9250_I2C_ADDR);

//Exit function
void ExitFunc()
{
    fclose(logFile);
    Mcp2221_Reset(handle);
}

//Configure MCP2221
void Mcp2221_config()
{
    ver = Mcp2221_GetLibraryVersion(LibVer);		//Get DLL version
    if(ver == 0)
    {
        string = "Library (DLL) version: "+QString::fromWCharArray(LibVer);
        TextBox->append(string);
    }
    else
    {
        error = Mcp2221_GetLastError();
        string = "Cannot get version, error: " + QString::fromStdString(Mcp2221_GetErrorName(error));
        TextBox->append(string);
    }

    //Get number of connected devices with this VID & PID
    Mcp2221_GetConnectedDevices(VID, PID, &NumOfDev);
    if(NumOfDev == 0)
    {
        string = "No MCP2221 devices connected";
        TextBox->append(string);
    }
    else
    {
        string = "Number of devices found: " + QString::number(NumOfDev);
        TextBox->append(string);
    }

    //open device by S/N
    //handle = Mcp2221_OpenBySN(VID, PID, &SerNum);

    //Open device by index
    handle = Mcp2221_OpenByIndex(VID, PID, NumOfDev-1);
    if(error == NULL)
    {
        string = "Connection successful";
        TextBox->append(string);
    }
    else
    {
        error = Mcp2221_GetLastError();
        string = "Error message is "+ QString::number(error) + " " + QString(Mcp2221_GetErrorName(error));
        TextBox->append(string);
        _sleep(10000);
    }

    //Get manufacturer descriptor
    flag = Mcp2221_GetManufacturerDescriptor(handle, MfrDescriptor);
    if(flag == 0)
    {
        string = "Manufacturer descriptor: " + QString::fromWCharArray(MfrDescriptor);
        TextBox->append(string);
    }
    else
    {
        string = "Error getting descriptor: " + QString::number(flag);
        TextBox->append(string);
    }

    //Get product descriptor
    flag = Mcp2221_GetProductDescriptor(handle, ProdDescrip);
    if(flag == 0)
    {
        string = "Product descriptor: " + QString::fromWCharArray(ProdDescrip);
        TextBox->append(string);
    }
    else
    {
        string = "Error getting product descriptor:" + QString::number(flag);
        TextBox->append(string);
    }

    //Get power attributes
    flag = Mcp2221_GetUsbPowerAttributes(handle, &PowerAttrib, &ReqCurrent);
    if(flag == 0)
    {
        string = "Power Attributes " + QString::number(PowerAttrib) + "\nRequested current units = " + QString::number(ReqCurrent) + "\nRequested current(mA) = " + QString::number(ReqCurrent*2);
        TextBox->append(string);
    }
    else
    {
        string = "Error getting power attributes:"+ QString::number(flag);
        TextBox->append(string);
    }

    //Set I2C bus
    flag = Mcp2221_SetSpeed(handle, 500000);    //set I2C speed to 400 KHz
    if(flag == 0)
    {
        string = "I2C is configured";
        TextBox->append(string);
    }
    else
    {
        string = "Error setting I2C bus:"+ QString::number(flag);
        TextBox->append(string);
    }

    //Set I2C advanced parameters
    flag = Mcp2221_SetAdvancedCommParams(handle, 10, 100);  //10ms timeout, try 1000 times
    if(flag == 0)
    {
        string = "I2C advanced settings set";
        TextBox->append(string);
    }
    else
    {
        string = "Error setting I2C advanced settings:"+ QString::number(flag);
        TextBox->append(string);
    }

    //Set GPIO
    flag = Mcp2221_SetGpioSettings(handle, RUNTIME_SETTINGS, pinFunc, pinDir, OutValues);
    if(flag != 0)
    {
        string = "Error setting GPIO, error: "+ QString::number(flag);
        TextBox->append(string);
    }
}

//Digital output control checkbox
void MainWindow::ChkBxCallback(int index)
{
    if(GPIO0chkBx->isChecked())
        OutValues[0] = 1;
    else
        OutValues[0] = 0;

    if(GPIO1chkBx->isChecked())
        OutValues[1] = 1;
    else
        OutValues[1] = 0;

    if(GPIO2chkBx->isChecked())
        OutValues[2] = 1;
    else
        OutValues[2] = 0;

    if(GPIO3chkBx->isChecked())
        OutValues[3] = 1;
    else
        OutValues[3] = 0;

	//Logging MPU9250 data to a CSV file
    if(SaveTofilechkbx->isChecked())
    {
            fileFlag = 1;
            if(!logFile)	//check if a logging file exists and create one if it doesn't
            {
                logFile = fopen(FileName, "a+");
                fprintf(logFile,"Ax,Ay,Az,Gx,Gx,Gz,Mx,My,Mz,Temperature\n");
            }
    }
    else
        fileFlag = 0;

    Mcp2221_SetGpioValues(handle, OutValues);
}

//Logging timer
void MainWindow::LogTimeCb()
{
    //fclose(logFile);
    exit(0);
}

//Chart plotting callback
void MainWindow::updateChart()
{

    IMU.readSensor();
    
    IMUval[0] = IMU.getAccelX_mss();
    IMUval[1] = IMU.getAccelY_mss();
    IMUval[2] = IMU.getAccelZ_mss();
    IMUval[3] = IMU.getGyroX_rads();
    IMUval[4] = IMU.getGyroY_rads();
    IMUval[5] = IMU.getGyroZ_rads();
    IMUval[6] = IMU.getMagX_uT();
    IMUval[7] = IMU.getMagY_uT();
    IMUval[8] = IMU.getMagZ_uT();

    count++;
    if(count == x1RangeMax)
    {
        count = 0;
        seriesX->clear();
        seriesY->clear();
        seriesZ->clear();
        seriesYaw->clear();
        seriesPitch->clear();
        seriesRoll->clear();
        seriesMx->clear();
        seriesMy->clear();
        seriesMz->clear();
    }
    numX.setX(count);
    numX.setY(IMUval[0]);
    numY.setX(count);
    numY.setY(IMUval[1]);
    numZ.setX(count);
    numZ.setY(IMUval[2]);

    numYaw.setX(count);
    numYaw.setY(IMUval[3]);
    numPitch.setX(count);
    numPitch.setY(IMUval[4]);
    numRoll.setX(count);
    numRoll.setY(IMUval[5]);
	
    numMx.setX(count);
    numMx.setY(IMUval[6]);
    numMy.setX(count);
    numMy.setY(IMUval[7]);
    numMz.setX(count);
    numMz.setY(IMUval[8]);

    seriesX->append(numX);
    seriesY->append(numY);
    seriesZ->append(numZ);
    seriesYaw->append(numYaw);
    seriesPitch->append(numPitch);
    seriesRoll->append(numRoll);
    seriesMx->append((numMx));
    seriesMy->append((numMy));
    seriesMz->append((numMz));

    if(fileFlag == 1)
    {
        fprintf(logFile, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
                IMUval[0],IMUval[1],IMUval[2],IMUval[3],IMUval[4],IMUval[5],
                IMUval[6],IMUval[7],IMUval[8],IMU.getTemperature_C());
    }
}

void MainWindow::AccScale(int index)
{
    switch(AccFscaleDropList->currentIndex())
    {
        case 0:
            FullScaleRange = MPU9250::ACCEL_RANGE_2G;
            y1RangeMax = 2.0;
            y1RangeMin = -2.0;
            string = "Setting accelerometer range to: ±2g ";
            break;

        case 1:
            FullScaleRange = MPU9250::ACCEL_RANGE_4G;
            y1RangeMax = 4.0;
            y1RangeMin = -4.0;
            string = "Setting accelerometer range to: ±4g ";
            break;

        case 2:
            FullScaleRange = MPU9250::ACCEL_RANGE_8G;
            y1RangeMax = 8.0;
            y1RangeMin = -8.0;
            string = "Setting accelerometer range to: ±8g ";
            break;

        case 3:
            FullScaleRange = MPU9250::ACCEL_RANGE_16G;
            y1RangeMax = 16.0;
            y1RangeMin = -16.0;
            string = "Setting accelerometer range to: ±16g ";
            break;
    }
    IMU.setAccelRange(MPU9250::AccelRange(FullScaleRange));
    qDebug() << axisYAcc->max() << axisYAcc->min();
    axisYAcc->setRange(y1RangeMin, y1RangeMax);
    TextBox->append(string);
}

void MainWindow::GyroScale(int index)
{
    switch(GyroFscaleDropList->currentIndex())
    {
        case 0:
            FullScaleRange = MPU9250::GYRO_RANGE_250DPS;
            yGyroRangeMax = 250;
            yGyroRangeMin = -250;
            string = "Setting gyroscope range to: ±250°/sec";
            break;

        case 1:
            FullScaleRange = MPU9250::GYRO_RANGE_500DPS;
            yGyroRangeMax = 500;
            yGyroRangeMin = -500;
            string = "Setting gyroscope range to: ±500°/sec";
            break;

        case 2:
            FullScaleRange = MPU9250::GYRO_RANGE_1000DPS;
            yGyroRangeMax = 1000;
            yGyroRangeMin = -1000;
            string = "Setting gyroscope range to: ±1000°/sec";
            break;

        case 3:
            FullScaleRange = MPU9250::GYRO_RANGE_2000DPS;
            yGyroRangeMax = 2000;
            yGyroRangeMin = -2000;
            string = "Setting gyroscope range to: ±2000°/sec";
            break;
    }
    IMU.setGyroRange(MPU9250::GyroRange(FullScaleRange));
    axisYGyro->setRange(yGyroRangeMin, yGyroRangeMax);
    TextBox->append(string);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setFixedSize(1300, 900);
    w.setWindowTitle("MPU9250 9-DOF IMU Viewer");   //Set the window title

    atexit(ExitFunc);	//register exit function

    //Label 1 - MCP2221 info
    QLabel *Label1 = new QLabel(&w);
    Label1->setGeometry(15, 460, 150, 25);
    Label1->setText("Device Configuration info");

    //Textbox for device information
    TextBox = new QTextEdit(&w);
    TextBox->setGeometry(15, 485, 380, 150);
    TextBox->acceptRichText();
    TextBox->setAcceptRichText(true);

    //checkbox for GPIO - output only
    QSignalMapper *CheckBoxSignalMapper = new QSignalMapper(&w);
    GPIO0chkBx = new QCheckBox("GPIO0",&w);
    GPIO0chkBx->setGeometry(410, 485, 100, 20);
    GPIO1chkBx = new QCheckBox("GPIO1",&w);
    GPIO1chkBx->setGeometry(410, 515, 100, 20);
    GPIO2chkBx = new QCheckBox("GPIO2",&w);
    GPIO2chkBx->setGeometry(410, 545, 100, 20);
    GPIO3chkBx = new QCheckBox("GPIO3",&w);
    GPIO3chkBx->setGeometry(410, 575, 100, 20);
    SaveTofilechkbx = new QCheckBox("Save Data to File", &w);
    SaveTofilechkbx->setGeometry(410, 605, 100, 20);

    /********************************************************************/
    //Accelerometer fullscale range
    QLabel *AccRangeLabel = new QLabel(&w);
    AccRangeLabel->setGeometry(15, 650, 275, 35);
    AccRangeLabel->setText("Select Accelerometer Fullscale Range:");

    //Accelerometer Drop-down list
    AccFscaleDropList = new QComboBox(&w);
    AccFscaleDropList->setGeometry(15, 680, 125, 35);
    AccFscaleDropList->addItem("±2g");
    AccFscaleDropList->addItem("±4g");
    AccFscaleDropList->addItem("±8g");
    AccFscaleDropList->addItem("±16g");

    //Gyroscope fullscale range
    QLabel *GyroRangeLabel = new QLabel(&w);
    GyroRangeLabel->setGeometry(15, 725, 250, 35);
    GyroRangeLabel->setText("Select Gyroscope Fullscale Range:");

    //Gyroscope Drop-down list
    GyroFscaleDropList = new QComboBox(&w);
    GyroFscaleDropList->setGeometry(15, 755, 125, 35);
    GyroFscaleDropList->addItem("±250°/sec");
    GyroFscaleDropList->addItem("±500°/sec");
    GyroFscaleDropList->addItem("±1000°/sec");
    GyroFscaleDropList->addItem("±2000°/sec");

    /********************************************************************/
    //Create line series for aX,aY,aZ
    seriesX = new QLineSeries();
    seriesX->setName("X-axis");
    seriesX->setColor(QColor("Red"));
    seriesY = new QLineSeries();
    seriesY->setName("Y-axis");
    seriesY->setColor(QColor("Blue"));
    seriesZ = new QLineSeries();
    seriesZ->setName("Z-axis");
    seriesZ->setColor(QColor("Green"));

    //Accelerometer Chart
    chartAcc = new QChart();
    //chartAcc->setGeometry(15, 15, 620, 450);
    chartAcc->legend()->show();

    //Acelerometer char axes
    QValueAxis *axisXAcc = new QValueAxis;
    axisXAcc->setRange(x1RangeMin, x1RangeMax);
    axisXAcc->setTitleText("Time");
    axisYAcc = new QValueAxis;
    axisYAcc->setRange(y1RangeMin, y1RangeMax);
    axisYAcc->setTitleText("Acceleration (g)");

    chartAcc->addAxis(axisXAcc, Qt::AlignBottom);
    chartAcc->addAxis(axisYAcc, Qt::AlignLeft);
    chartAcc->addSeries(seriesX);
    chartAcc->addSeries(seriesY);
    chartAcc->addSeries(seriesZ);

    seriesX->attachAxis(axisXAcc);
    seriesX->attachAxis(axisYAcc);
    seriesY->attachAxis(axisXAcc);
    seriesY->attachAxis(axisYAcc);
    seriesZ->attachAxis(axisXAcc);
    seriesZ->attachAxis(axisYAcc);

    //chartAcc->createDefaultAxes();         //comment this line for dynamic axes
    chartAcc->setTitle("MPU9250 Accelerometer Reading");
    chartAcc->setPlotAreaBackgroundVisible(true);
    chartAcc->setBackgroundVisible(false);

    //Create Accelerometer Chart View Widget
    chartViewAcc = new QChartView(chartAcc, &w);
    chartViewAcc->setGeometry(15, 15, 620, 450);
	
    /********************************************************************/
    //MPU9250 Gyroscope chart
    //Create line series for gX,gY,gZ
    seriesYaw = new QLineSeries();
    seriesYaw->setName("Yaw");
    seriesYaw->setColor(QColor("Red"));
    seriesPitch = new QLineSeries();
    seriesPitch->setName("Pitch");
    seriesPitch->setColor(QColor("Blue"));
    seriesRoll = new QLineSeries();
    seriesRoll->setName("Roll");
    seriesRoll->setColor(QColor("Green"));

    //Gyroscope Chart things
    chartGyro = new QChart();
    chartGyro->legend()->show();

    //gyroscope char axes
    QValueAxis *axisXGyro = new QValueAxis;
    axisXGyro->setRange(x1RangeMin, x1RangeMax);
    axisXGyro->setTitleText("Time");

    axisYGyro = new QValueAxis;
    axisYGyro->setRange(yGyroRangeMin, yGyroRangeMax);
    axisYGyro->setTitleText("Gyration (°/sec)");

    chartGyro->addAxis(axisXGyro, Qt::AlignBottom);
    chartGyro->addAxis(axisYGyro, Qt::AlignLeft);
    chartGyro->addSeries(seriesYaw);
    chartGyro->addSeries(seriesPitch);
    chartGyro->addSeries(seriesRoll);

    //Attach axis
    seriesYaw->attachAxis(axisXGyro);
    seriesYaw->attachAxis(axisYGyro);
    seriesPitch->attachAxis(axisXGyro);
    seriesPitch->attachAxis(axisYGyro);
    seriesRoll->attachAxis(axisXGyro);
    seriesRoll->attachAxis(axisYGyro);

    chartGyro->setTitle("MPU9250 Gyroscope Reading");
    chartGyro->setPlotAreaBackgroundVisible(true);
    chartGyro->setBackgroundVisible(false);

    //Create gyroscope Chart View Widget
    chartViewGyro = new QChartView(chartGyro, &w);
    chartViewGyro->setGeometry(650, 15, 620, 450);
    /*********************************************************************************/

    //Create line series for aX,aY,aZ
    seriesMx = new QLineSeries();
    seriesMx->setName("Mag. X");
    seriesMx->setColor(QColor("Red"));
    seriesMy = new QLineSeries();
    seriesMy->setName("Mag. Y");
    seriesMy->setColor(QColor("Blue"));
    seriesMz = new QLineSeries();
    seriesMz->setName("Mag. Z");
    seriesMz->setColor(QColor("Green"));

    //Magnetometer Chart
    chartMagneto = new QChart();
    chartMagneto->legend()->show();

    //magnetometer axes
    QValueAxis *axisXMagneto = new QValueAxis;
    axisXMagneto->setRange(xMagnetoRangeMin, xMagnetoRangeMax);
    axisXMagneto->setTitleText("Time");
    axisYMagneto = new QValueAxis;
    axisYMagneto->setRange(yMagnetoRangeMin, yMagnetoRangeMax);
    axisYMagneto->setTitleText("Magnetic Field (μT)");

    chartMagneto->addAxis(axisXMagneto, Qt::AlignBottom);
    chartMagneto->addAxis(axisYMagneto, Qt::AlignLeft);
    chartMagneto->addSeries(seriesMx);
    chartMagneto->addSeries(seriesMy);
    chartMagneto->addSeries(seriesMz);

    seriesMx->attachAxis(axisXMagneto);
    seriesMx->attachAxis(axisYMagneto);
    seriesMy->attachAxis(axisXMagneto);
    seriesMy->attachAxis(axisYMagneto);
    seriesMz->attachAxis(axisXMagneto);
    seriesMz->attachAxis(axisYMagneto);

    chartMagneto->setTitle("MPU9250 Magnetometer Reading");
    chartMagneto->setPlotAreaBackgroundVisible(true);
    chartMagneto->setBackgroundVisible(false);

    //Create Accelerometer Chart View Widget
    chartViewMagneto = new QChartView(chartMagneto, &w);
    chartViewMagneto->setGeometry(660, 435, 620, 450);
    /*********************************************************************************/

    //plotting timer and connect it to real-time slot
    QTimer *timer = new QTimer(&w);
    QObject::connect(timer, SIGNAL(timeout()), &w, SLOT(updateChart()));
    timer->start(0);    //Interval 0 means to refresh as fast as possible
    /*********************************************************************************/

    //Get time stamp for logging file name
    char *LocalTime = 0;
    char temp[24] = {0};
    time_t LogTime;	//Local time at the time of creation
    struct tm * timeinfo;

    time(&LogTime);
    LocalTime = ctime(&LogTime);
    timeinfo = localtime (&LogTime);
    strftime(temp, sizeof(temp), "%d%b%Y-%H-%M-%S", timeinfo);	//day-month-year-hour-minute-second
    strcat(FileName, temp);
    strcat(FileName, ".csv");

    //Configure any connected MCP2221
    Mcp2221_config();

    // start communication with IMU
    int status = IMU.begin();
    qDebug("MPU9250 status: %d\n", status);
    w.show();

    QObject::connect(AccFscaleDropList, SIGNAL(currentIndexChanged(int)), &w, SLOT(AccScale(int)));
    QObject::connect(GyroFscaleDropList, SIGNAL(currentIndexChanged(int)), &w, SLOT(GyroScale(int)));
    QObject::connect(CheckBoxSignalMapper, SIGNAL(mapped(int)), &w, SLOT(ChkBxCallback(int)));
    CheckBoxSignalMapper->setMapping(GPIO0chkBx, 0);
    CheckBoxSignalMapper->setMapping(GPIO1chkBx, 1);
    CheckBoxSignalMapper->setMapping(GPIO2chkBx, 2);
    CheckBoxSignalMapper->setMapping(GPIO3chkBx, 3);
    CheckBoxSignalMapper->setMapping(SaveTofilechkbx, 4);
    QObject::connect(GPIO0chkBx, SIGNAL(stateChanged(int)), CheckBoxSignalMapper, SLOT(map()));
    QObject::connect(GPIO1chkBx, SIGNAL(stateChanged(int)), CheckBoxSignalMapper, SLOT(map()));
    QObject::connect(GPIO2chkBx, SIGNAL(stateChanged(int)), CheckBoxSignalMapper, SLOT(map()));
    QObject::connect(GPIO3chkBx, SIGNAL(stateChanged(int)), CheckBoxSignalMapper, SLOT(map()));
    QObject::connect(SaveTofilechkbx, SIGNAL(stateChanged(int)), CheckBoxSignalMapper, SLOT(map()));

    return a.exec();
}
