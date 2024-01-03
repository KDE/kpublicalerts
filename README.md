# Weather and emergency alert push notification demo

This is currently only meant as a tech demo, do not trust in production!

## Server setup

The server consists of three parts:
* An off-the-shelf [PostGIS](https://postgis.net/) database server.
* The alert aggregation service providing the outside-facing REST API and sending out push notifications.
* The feeder process collecting, augmenting and normalizing alert feeds.

### PostGIS setup

No special configuration is required, but there needs to be an empty database
and, depending on your setup, a corresponding database user.

### Aggregator service setup

The aggregator service uses [Django](https://www.djangoproject.com/) and needs a few other Python modules
as dependencies as well, see `publicAlertAggregator/requirements.txt`. Install those via your distribution or
`pip` (see also the [pip user guide](https://pip.pypa.io/en/stable/user_guide/)).

The aggregator service needs to be configured to find your PostGIS database. This
can be done in `publicAlertsAggregator/settings.py` or via environment variables.
* `POSTGRES_HOST`: IP address or host name of the PostGIS server
* `POSTGRES_DATABASE`: name of the PostGIS database
* `POSTGRES_USER`: name of the user on the PostGIS database
* `POSTGRES_PASSWORD`: password for connecting to the PostGIS database

The aggregator service is started as follows for local development:

```
python3 manage.py collectstatic
python3 manage.py migrate
python3 manage.py runserver 8000
```

The first two steps are only necessary when the static files or the database layout changed
respectively, you'll only need the last command otherwise.

For a production deployment, you'll likely want to put this behind a HTTP server,
see the [Django deployment documentation](https://docs.djangoproject.com/en/5.0/howto/deployment/).

Also note that the aggregator service does not deal with access control itself,
that's for the outside HTTP server to handle. For a production setup, make sure
to not publicly expose the REST APIs for the feeder service and the admin UI.

### Feeder service setup

Like the aggregator service the feeder depends on a few Python modules, specified
in `publicAlertFeeder/requirements.txt`.

By default the feeder expects the aggregation service accessible via HTTP on the
same host, but this can be changed via the `AGGREGATOR_HOST` environment variable.

The feeder service is a simple Python script that just fetches all configured feeds, sends
their alerts to the aggregator and terminates. It's meant to run in an infinite loop with a
sleep by the caller:

```
while true; do
    python3 publicAlertFeeder.py
    sleep 60
done
```
