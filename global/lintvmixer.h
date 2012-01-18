 
#ifndef LINTVMIXER_H
#define LINTVMIXER_H

#include <QObject>
#include <QStringList>

class KProcess;
class LinTVProcess;

class LinTVMixer : public QObject
{
    Q_OBJECT

public:
    LinTVMixer(QObject *parent = 0);
    LinTVMixer(QString mixer, int volume = 80, QObject *parent = 0);
    ~LinTVMixer();

    QString audioInput() const { return m_audioInput; }
    void setAudioInput(QString );
    void setVolume(int volume, bool relative = false);
    void mute();
    void unmute();

    void setMuted(bool );

    QStringList simpleMixers();

private slots:
    void receivedStdout(KProcess *, char *, int );

private:
    void loadCommon();


private:
    LinTVProcess *m_amixerProcess;

    QString m_audioInput;
    int m_volume;
    bool m_mute;

    int m_cmdType;

    bool m_commandDone;

    //QStringList m_mixers;
};

#endif