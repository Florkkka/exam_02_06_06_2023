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
            // Здесь можно добавить логику для добавления пользователя в игру
            // Например, создать объект игрока и добавить его в список игроков
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
                // Здесь можно добавить логику для выполнения хода
                // Например, вызвать функцию, которая сравнит значение функции двух аргументов
                // и определит победителя
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
            // Здесь можно добавить логику для завершения игры по запросу игрока
            // Например, отправить сообщение об окончании игры и очистить список игроков
            // и/или выполнить другие необходимые действия
            m_players.clear();
        }
        else if (command == "stats") {
            qDebug() << "Stats requested by user:" << argument;
            // Здесь можно добавить логику для просмотра статистики
            // Например, записать статистику в текстовый файл
            QString filename = "D:/stats.txt";
            QFile file(filename);
            if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
                QTextStream stream(&file);
                stream << "Stats requested by user: " << argument << "\n";
                // Запишите дополнительную статистику, если необходимо
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
    // Здесь можно реализовать вашу функцию для вычисления значения хода
    // Верните значение функции, которое будет использоваться для определения победителя
    // В данном случае, просто возвращаем длину аргумента
    return argument.length();
}

void Server::startGame()
{
    qDebug() << "Starting the game!";
    // Здесь можно добавить код для начала игры
    // Например, отправить сообщение игрокам о начале игры
}
