#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

struct Player {
    QString login;
    QTcpSocket *socket;
};

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);

private:
    QTcpServer *m_server;
    QList<QTcpSocket*> m_clients;
    QList<Player> m_players;

    void startGame();
    int calculateMove(const QString& argument);

signals:

public slots:
    void handleNewConnection();
    void handleData();
    void handleDisconnect();
};

#endif // SERVER_H
