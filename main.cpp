#include <QCoreApplication>
#include "waveform_manager.h"

/* mehdi farhadi */
/* mfarhadi.ece.iut.ac.ir */
/* 2018/01/30 */

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    WaveFormManager d;

    if( a.arguments().contains("high") )
        d.initHighPrioritySound();
    else if ( a.arguments().contains("medium") )
        d.initMediumPrioritySound();
    else
        d.initQRSSound();

    return a.exec();
}
