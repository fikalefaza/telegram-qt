/*
    Copyright (C) 2014 Alexandr Akulich <akulichalexander@gmail.com>

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
    LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
    OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#ifndef CTELEGRAMCORE_HPP
#define CTELEGRAMCORE_HPP

#include <QObject>
#include <QByteArray>

#include "TLTypes.hpp"
#include "crypto-rsa.hpp"

class CTelegramStream;
class CTelegramTransport;

class CTelegramCore : public QObject
{
    Q_OBJECT
public:
    enum AuthState {
        AuthNone,
        AuthPqRequested,
        AuthDhRequested,
        AuthDhGenerationResultRequested,
        AuthSuccess
    };

    explicit CTelegramCore(QObject *parent = 0);

    void setAppId(quint32 newId);
    bool setAppHash(const QString &newHash);

    CTelegramTransport *transport() const { return m_transport; }
    void setTransport(CTelegramTransport *newTransport);

    static quint64 formatTimeStamp(qint64 timeInMs);
    static inline quint64 formatClientTimeStamp(qint64 timeInMs) { return formatTimeStamp(timeInMs) & ~3UL; }

    static quint64 timeStampToMSecsSinceEpoch(quint64 ts);

    void initAuth();

    AuthState authState() { return m_authState; }

    void requestPqAuthorization();
    bool answerPqAuthorization(const QByteArray &payload);
    void requestDhParameters();
    bool answerDh(const QByteArray &payload);
    void requestDhGenerationResult();
    bool processServersDHAnswer(const QByteArray &payload);

    inline TLNumber128 clientNonce() const { return m_clientNonce; }
    inline TLNumber128 serverNonce() const { return m_serverNonce; }

    inline quint64 pq() const { return m_pq; }
    inline quint64 p() const { return m_p; }
    inline quint64 q() const { return m_q; }

    inline quint64 serverPublicFingersprint() const { return m_serverPublicFingersprint; }

signals:
    void pqReceived();
    void authStateChanged();

private slots:
    void whenReadyRead();

private:
    void initTmpAesKey();

    void sendPackage(const QByteArray &buffer);

    void setAuthState(AuthState newState);

    quint32 m_appId;
    QString m_appHash;

    CTelegramTransport *m_transport;

    TLNumber128 m_clientNonce;
    TLNumber128 m_serverNonce;
    TLNumber256 m_newNonce;

    quint64 m_pq;
    quint32 m_p;
    quint32 m_q;

    quint64 m_serverPublicFingersprint;
    SRsaKey m_rsaKey;

    QByteArray m_tmpAesKey;
    QByteArray m_tmpAesIv;

    quint32 m_g;

    QByteArray m_dhPrime;
    QByteArray m_gA;
    QByteArray m_b;

    AuthState m_authState;
    quint64 m_authId;

    quint64 m_authRetryId;

};

#endif // CTELECORE_HPP
