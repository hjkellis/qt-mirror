#pragma once

#include <QWidget>

class QVBoxLayout;
class QLabel;
class QListWidget;
class QPushButton;
class QNetworkAccessManager;
class QOAuth2AuthorizationCodeFlow;

class GoogleCalendarEvents : public QWidget {
Q_OBJECT

public:
    explicit GoogleCalendarEvents(QWidget *parent = nullptr);

private slots:
    void startAuthentication();
    void fetchEvents();

private:
    void setupUi();
    void setupOAuth();
    void parseEvents(const QByteArray &jsonData);

    QVBoxLayout *layout;
    QLabel *statusLabel;
    QListWidget *eventList;
    QPushButton *authButton;

    QOAuth2AuthorizationCodeFlow *oauth;
    QNetworkAccessManager *network;
};
