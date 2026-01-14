# Emergency and weather alert client

Client for https://invent.kde.org/webapps/foss-public-alert-server/.

See https://alerts.kde.org/sources/status for supported alert sources.

## Contributing

Matrix channel: [#FPAS_discussion](https://matrix.to/#/#FPAS_discussion:tchncs.de)

### Testing

Continuous Flatpak builds:

```
flatpak install https://cdn.kde.org/flatpak/kpublicalerts-nightly/org.kde.publicalerts.flatpakref
```

### Building

General introduction: https://community.kde.org/Get_Involved

Building:
- Android: https://develop.kde.org/docs/packaging/android/building_applications/
- Other platforms: https://community.kde.org/Get_Involved/development

Important external components:
- Common Alerting Protocol (CAP) library: https://invent.kde.org/libraries/kweathercore
- UnifiedPush client library: https://invent.kde.org/libraries/kunifiedpush
