from django.db import models

from django.db import models


class Monitor(models.Model):
    name = models.CharField(max_length=255)
    url = models.URLField()
    check_interval_seconds = models.IntegerField(default=30)
    is_active = models.BooleanField(default=True)
    created_at = models.DateTimeField(auto_now_add=True)

    class Meta:
        managed = False
        db_table = 'monitors'

    def __str__(self):
        return self.name


class Check(models.Model):
    monitor = models.ForeignKey(Monitor, on_delete=models.CASCADE)
    checked_at = models.DateTimeField(auto_now_add=True)
    status_code = models.IntegerField(null=True, blank=True)
    response_time_ms = models.IntegerField(null=True, blank=True)
    is_up = models.BooleanField()
    error_message = models.TextField(null=True, blank=True)

    class Meta:
        managed = False
        db_table = 'checks'

    def __str__(self):
        return f"{self.monitor.name} - {self.checked_at}"


class Alert(models.Model):
    monitor = models.ForeignKey(Monitor, on_delete=models.CASCADE)
    alert_type = models.CharField(max_length=50)
    destination = models.TextField()
    is_active = models.BooleanField(default=True)

    class Meta:
        managed = False
        db_table = 'alerts'

    def __str__(self):
        return f"{self.monitor.name} - {self.destination}"
