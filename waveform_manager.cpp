#include "waveform_manager.h"

WaveFormManager::WaveFormManager(QObject *parent) : QObject(parent)
{
}

void WaveFormManager::initHighPrioritySound()
{
    qreal duration = 11.520;
    int sequence[20] = {160,-80,160,-80,160,-320,160,-80,160,-720,
                        160,-80,160,-80,160,-320,160,-80,160,-8080};

    highPriority.setFormat(20000,1,32,"audio/pcm",QAudioFormat::LittleEndian,QAudioFormat::Float);
    highPriority.setVolume(1.000);
    highPriority.setWaveForm(800,duration,sequence);
    highPriority.play();
}

void WaveFormManager::initMediumPrioritySound()
{
    qreal duration = 17.200;
    int sequence[6] = {240,-160,240,-160,240,-16160};

    mediumPriority.setFormat(20000,1,32,"audio/pcm",QAudioFormat::LittleEndian,QAudioFormat::Float);
    mediumPriority.setVolume(1.000);
    mediumPriority.setWaveForm(800,duration,sequence);
    mediumPriority.play();
}

void WaveFormManager::initQRSSound()
{
    qrsWave.setFormat(44100,1,32,"audio/pcm",QAudioFormat::LittleEndian,QAudioFormat::SignedInt);
    qrsWave.setVolume(1.000);
    qrsWave.setFile("test.raw");
    timer->start(500);
}
