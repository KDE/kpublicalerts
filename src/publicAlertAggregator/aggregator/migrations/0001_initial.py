# Generated by Django 4.1.1 on 2022-09-11 10:29

import aggregator.models
import django.contrib.gis.db.models.fields
from django.db import migrations, models
import uuid


class Migration(migrations.Migration):

    initial = True

    dependencies = []

    operations = [
        migrations.CreateModel(
            name="Alert",
            fields=[
                (
                    "id",
                    models.UUIDField(
                        default=uuid.uuid4,
                        editable=False,
                        primary_key=True,
                        serialize=False,
                    ),
                ),
                ("sourceId", models.CharField(max_length=255)),
                ("alertId", models.CharField(max_length=255)),
                ("bbox", django.contrib.gis.db.models.fields.PolygonField(srid=4326)),
                (
                    "capData",
                    models.FileField(
                        null=True, upload_to=aggregator.models.alert_upload_path
                    ),
                ),
                ("issueTime", models.DateTimeField()),
                ("expireTime", models.DateTimeField(null=True)),
                ("sourceUrl", models.CharField(max_length=255, null=True)),
            ],
        ),
        migrations.CreateModel(
            name="Subscription",
            fields=[
                (
                    "id",
                    models.UUIDField(
                        default=uuid.uuid4,
                        editable=False,
                        primary_key=True,
                        serialize=False,
                    ),
                ),
                ("bbox", django.contrib.gis.db.models.fields.PolygonField(srid=4326)),
                ("upEndpoint", models.CharField(max_length=255)),
            ],
        ),
        migrations.AddConstraint(
            model_name="alert",
            constraint=models.UniqueConstraint(
                models.F("sourceId"),
                models.F("alertId"),
                name="sourceId_alertId_unique",
            ),
        ),
    ]
