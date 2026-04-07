#include "goole_calendar_events.hpp"

#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

static const QString CLIENT_ID     = "YOUR_CLIENT_ID.apps.googleusercontent.com";
static const QString CLIENT_SECRET = "YOUR_CLIENT_SECRET";

GoogleCalendarEvents::GoogleCalendarEvents(QWidget *parent)
    : QWidget(parent)
    , layout(nullptr)
    , statusLabel(nullptr)
    , eventList(nullptr)
    , authButton(nullptr)
    , oauth(new QOAuth2AuthorizationCodeFlow(this))
    , network(new QNetworkAccessManager(this))
{
    setupUi();
    setupOAuth();
}

void GoogleCalendarEvents::setupUi()
{
    layout = new QVBoxLayout(this);

    statusLabel = new QLabel("Not authenticated", this);
    authButton = new QPushButton("Sign in with Google", this);
    eventList = new QListWidget(this);

    layout->addWidget(statusLabel);
    layout->addWidget(authButton);
    layout->addWidget(eventList);

    connect(authButton, &QPushButton::clicked,
            this, &GoogleCalendarEvents::startAuthentication);
}

void GoogleCalendarEvents::setupOAuth()
{
    oauth->setClientIdentifier(CLIENT_ID);
    oauth->setClientIdentifierSharedKey(CLIENT_SECRET);

    oauth->setAuthorizationUrl(QUrl("https://accounts.google.com/o/oauth2/v2/auth"));
    oauth->setAccessTokenUrl(QUrl("https://oauth2.googleapis.com/token"));

    oauth->setScope("https://www.googleapis.com/auth/calendar.readonly");

    auto *replyHandler = new QOAuthHttpServerReplyHandler(8080, this);
    oauth->setReplyHandler(replyHandler);

    connect(oauth, &QOAuth2AuthorizationCodeFlow::granted, this, [this]() {
        statusLabel->setText("Authenticated");
        fetchEvents();
    });
}

void GoogleCalendarEvents::startAuthentication()
{
    statusLabel->setText("Authenticating...");
    oauth->grant();
}

void GoogleCalendarEvents::fetchEvents()
{
    QUrl url("https://www.googleapis.com/calendar/v3/calendars/primary/events");

    QUrlQuery query;
    query.addQueryItem("maxResults", "10");
    query.addQueryItem("orderBy", "startTime");
    query.addQueryItem("singleEvents", "true");
    query.addQueryItem("timeMin", QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setRawHeader("Authorization",
                         "Bearer " + oauth->token().toUtf8());

    QNetworkReply *reply = network->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            statusLabel->setText("Failed to fetch events");
            reply->deleteLater();
            return;
        }

        parseEvents(reply->readAll());
        reply->deleteLater();
    });
}

void GoogleCalendarEvents::parseEvents(const QByteArray &jsonData)
{
    eventList->clear();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    QJsonObject root = doc.object();
    QJsonArray items = root.value("items").toArray();

    for (const QJsonValue &value : items) {
        QJsonObject event = value.toObject();
        QString summary = event.value("summary").toString("(No title)");

        QJsonObject startObj = event.value("start").toObject();
        QString startTime = startObj.value("dateTime").toString();

        if (startTime.isEmpty()) {
            startTime = startObj.value("date").toString();
        }

        eventList->addItem(startTime + "  -  " + summary);
    }

    statusLabel->setText(QString("Loaded %1 events").arg(items.size()));
}
