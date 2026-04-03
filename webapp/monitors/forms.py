from django import forms
from .models import Monitor, Alert


class MonitorForm(forms.ModelForm):
    class Meta:
        model = Monitor
        fields = ['name', 'url', 'check_interval_seconds']
        widgets = {
            'name': forms.TextInput(attrs={'placeholder': 'My Website'}),
            'url': forms.URLInput(attrs={'placeholder': 'https://example.com'}),
            'check_interval_seconds': forms.NumberInput(attrs={'min': 10}),
        }


class AlertForm(forms.ModelForm):
    class Meta:
        model = Alert
        fields = ['alert_type', 'destination']
        widgets = {
            'alert_type': forms.Select(choices=[('email', 'Email')]),
            'destination': forms.TextInput(attrs={'placeholder': 'you@example.com'}),
        }