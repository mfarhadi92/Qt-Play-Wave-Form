#ifndef WAVEFORMMANAGER_H
#define WAVEFORMMANAGER_H

#include <QObject>
#include <QByteArray>
#include <QAudioOutput>
#include <QBuffer>
#include <QtMath>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QFile>

class WaveFormManager : public QObject
{
    Q_OBJECT
    struct WaveForm {

    private:
        QBuffer*        input = nullptr;
        QFile*          sourceFile = nullptr;
        QAudioOutput*   audio = nullptr;
        enum IODeviceType{EMPTY=0,FILE,BUFFER};
        int             sampleRate,channelCount,sampleSize;
        qreal           volume;
        QString         codec;
        QByteArray*     waveByteArray = nullptr;

        QAudioFormat::Endian        byteOrder;
        QAudioFormat::SampleType    sampleType;
        IODeviceType    deviceType = EMPTY;

    private:
        void setAudioOutput()
        {
            QAudioFormat    audioFormat;
            audioFormat.setSampleRate(sampleRate);
            audioFormat.setChannelCount(channelCount);
            audioFormat.setSampleSize(sampleSize);
            audioFormat.setCodec(codec);
            audioFormat.setByteOrder(byteOrder);
            audioFormat.setSampleType(sampleType);

            QAudioDeviceInfo deviceInfo(QAudioDeviceInfo::defaultOutputDevice());
            if(!deviceInfo.isFormatSupported(audioFormat))
                qWarning() << "Raw audio format not supported by backend, cannot play audio.";

            audio   = new QAudioOutput(audioFormat,0);
            audio->setVolume(volume);
        }

    public:
        void setFormat(const int sampleRate_ , const int channelCount_ , const int sampleSize_ , const QString codec_ ,
                       const QAudioFormat::Endian byteOrder_ , const QAudioFormat::SampleType sampleType_ )
        {
            sampleRate      = sampleRate_;
            channelCount    = channelCount_;
            sampleSize      = sampleSize_;
            codec           = codec_;
            byteOrder       = byteOrder_;
            sampleType      = sampleType_;
        }

        void setVolume(const qreal volume_ ) { volume   = volume_ ; }

        void clear()
        {
            if( sourceFile && sourceFile->isOpen() )
            {
                sourceFile->flush();
                sourceFile->close();
            }
            delete sourceFile;
            if( input )
                delete input;
            if( audio )
                delete audio;

            sourceFile  = nullptr;
            audio       = nullptr;
            input       = nullptr;

            deviceType  = EMPTY;
        }

        void setFile(QString name)
        {
            clear();
            setAudioOutput();
            sourceFile  = new QFile();
            sourceFile->setFileName(name);
            if( sourceFile->open(QIODevice::ReadOnly) )
                deviceType  = FILE;
            else
                deviceType  = EMPTY;
        }

        void setByteArray(QByteArray *bt)
        {
            clear();
            setAudioOutput();
            input       = new QBuffer(bt);
            input->open(QIODevice::ReadOnly);
            deviceType  = BUFFER;
        }

        template<int N>
        void setWaveForm(const qreal frequency , const qreal duration , const int (&sequencyList)[N] )
        {
            deviceType      = BUFFER;
            const quint32 n = static_cast<quint32>(duration * sampleRate);

            if( waveByteArray )
            {
                delete waveByteArray;
                waveByteArray   = nullptr;
            }

            waveByteArray   = new QByteArray();
            waveByteArray->resize(sizeof(float) * n);

            const int sequenceCount = (sizeof(sequencyList) / sizeof(*sequencyList));
            int currentMS           = 0;
            int lastMS              = -1;
            int currentSequencyIdx  = 0;
            int step                = 0;
            int stepCount           = 0 ;
            bool isHigh             = false;

            for (quint32 i = 0; i < n; i++)
            {
                currentMS = i / (sampleRate/1000);
                if( currentMS != lastMS )
                {
                    if( step == stepCount )
                    {
                        step    = 0;
                        if( sequenceCount <= currentSequencyIdx)
                        {
                            stepCount   = n;
                            isHigh      = false;
                        }
                        else
                        {
                            stepCount   = abs(sequencyList[currentSequencyIdx]);
                            if( sequencyList[currentSequencyIdx] > 0 )
                                isHigh      = true;
                            else
                                isHigh      = false;
                        }
                        currentSequencyIdx++;
                    }
                    step++;
                }

                lastMS  = currentMS;
                qreal sinVal = 0;
                if( isHigh )
                    sinVal  = (qreal)qSin(2.0 * M_PI * frequency * i / sampleRate);

                float sample = (float)sinVal;
                char *ptr = (char*)(&sample);
                char byte00 = *ptr;
                char byte01 = *(ptr + 1);
                char byte02 = *(ptr + 2);
                char byte03 = *(ptr + 3);

                (*waveByteArray)[4 * i] = byte00;
                (*waveByteArray)[4 * i + 1] = byte01;
                (*waveByteArray)[4 * i + 2] = byte02;
                (*waveByteArray)[4 * i + 3] = byte03;
            }
            setByteArray(waveByteArray);
        }

        inline void play()
        {
            switch( deviceType )
            {
            case FILE:
                if( sourceFile && audio )
                {
                    sourceFile->seek(0);
                    audio->start(sourceFile);
                }
                break;
            case BUFFER:
                if( input && audio )
                {
                    input->seek(0);
                    audio->start(input);
                }
                break;
            default:
                break;
            }
        }
    };
    WaveForm    qrsWave,highPriority,mediumPriority;

public:
    explicit WaveFormManager(QObject *parent = 0);

    void initHighPrioritySound();
    void initMediumPrioritySound();
    void initQRSSound();
};

#endif // WAVEFORMMANAGER_H
