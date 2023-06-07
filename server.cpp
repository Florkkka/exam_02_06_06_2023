#include "server.h"
#include <QDebug>
#include <QStringList>
#include <QFile>
#include <QTextStream>

Server::Server(QObject *parent) : QObject(parent)
{
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &Server::handleNewConnection);
    m_server->listen(QHostAddress::Any, 33333);
    qDebug() << "Server started. Listening on port 33333...";
}

void Server::handleNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket *socket = m_server->nextPendingConnection();
        qDebug() << "New connection from:" << socket->peerAddress().toString();
        connect(socket, &QTcpSocket::readyRead, this, &Server::handleData);
        connect(socket, &QTcpSocket::disconnected, this, &Server::handleDisconnect);
        m_clients.append(socket);
    }
}

void Server::handleData()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    QByteArray requestData = socket->readAll();
    qDebug() << "Received data:" << requestData;

    QString requestString(requestData);
    QStringList requestParts = requestString.split('&');

    if (requestParts.size() >= 2) {
        QString command = requestParts.at(0);
        QString argument = requestParts.at(1);

        if (command == "start_game") {
            qDebug() << "User joined the game:" << argument;
            Player player;
            player.login = argument;
            player.socket = socket;
            m_players.append(player);

            // Проверяем, если уже есть два игрока, запускаем игру
            if (m_players.size() == 2) {
                startGame();
            }
        }
        else if (command == "move") {
            if (m_players.size() == 2) {
                qDebug() << "Move received from:" << argument;
      
                int result = calculateMove(argument);

                // Отправляем результат хода обратно клиенту
                QString response = QString::number(result);
                socket->write(response.toUtf8());
            }
            else {
                qDebug() << "Not enough players to make a move";
            }
        }
        else if (command == "stop_game") {
            qDebug() << "Game stopped by user:" << argument;
            m_players.clear();
        }
        else if (command == "stats") {
            qDebug() << "Stats requested by user:" << argument;
            QString filename = "D:/stats.txt";
            QFile file(filename);
            if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
                QTextStream stream(&file);
                stream << "Stats requested by user: " << argument << "\n";
                file.close();
            } else {
                qDebug() << "Failed to open file:" << filename;
            }
        }
    }
}

void Server::handleDisconnect()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    qDebug() << "Disconnected:" << socket->peerAddress().toString();
    m_clients.removeOne(socket);
    socket->deleteLater();
}

int Server::calculateMove(const QString& argument)
{
    // Возвращаем длину аргумента
    return argument.length();
}

void Server::startGame()
{
    qDebug() << "Starting the game!";
}
