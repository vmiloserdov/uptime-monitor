from django.shortcuts import render, redirect, get_object_or_404
from django.contrib.auth.decorators import login_required
from django.contrib.auth import login, logout, authenticate
from django.contrib.auth.forms import AuthenticationForm, UserCreationForm
from django.contrib import messages
from .models import Monitor, Check, Alert
from .forms import MonitorForm, AlertForm
from django.db.models import Avg, Count
from django.utils import timezone
from datetime import timedelta


def login_view(request):
    if request.method == 'POST':
        form = AuthenticationForm(data=request.POST)
        if form.is_valid():
            user = form.get_user()
            login(request, user)
            return redirect('dashboard')
    else:
        form = AuthenticationForm()
    return render(request, 'monitors/login.html', {'form': form})


def logout_view(request):
    logout(request)
    return redirect('login')


def signup_view(request):
    if request.method == 'POST':
        form = UserCreationForm(request.POST)
        if form.is_valid():
            user = form.save()
            login(request, user)
            return redirect('dashboard')
    else:
        form = UserCreationForm()
    return render(request, 'monitors/signup.html', {'form': form})


@login_required
def dashboard(request):
    monitors = Monitor.objects.filter(is_active=True)
    monitor_data = []
    for monitor in monitors:
        latest_check = Check.objects.filter(monitor=monitor).order_by('-checked_at').first()
        thirty_days_ago = timezone.now() - timedelta(days=30)
        total = Check.objects.filter(monitor=monitor, checked_at__gte=thirty_days_ago).count()
        up = Check.objects.filter(monitor=monitor, checked_at__gte=thirty_days_ago, is_up=True).count()
        uptime = round((up / total * 100), 2) if total > 0 else 0
        monitor_data.append({
            'monitor': monitor,
            'latest_check': latest_check,
            'uptime': uptime,
        })
    return render(request, 'monitors/dashboard.html', {'monitor_data': monitor_data})


@login_required
def add_monitor(request):
    if request.method == 'POST':
        form = MonitorForm(request.POST)
        if form.is_valid():
            form.save()
            messages.success(request, 'Monitor added successfully!')
            return redirect('dashboard')
    else:
        form = MonitorForm()
    return render(request, 'monitors/add_monitor.html', {'form': form})


@login_required
def delete_monitor(request, monitor_id):
    monitor = get_object_or_404(Monitor, id=monitor_id)
    if request.method == 'POST':
        monitor.is_active = False
        monitor.save()
        messages.success(request, 'Monitor deleted.')
    return redirect('dashboard')


@login_required
def monitor_detail(request, monitor_id):
    monitor = get_object_or_404(Monitor, id=monitor_id)
    checks = Check.objects.filter(monitor=monitor).order_by('-checked_at')[:50]
    thirty_days_ago = timezone.now() - timedelta(days=30)
    total = Check.objects.filter(monitor=monitor, checked_at__gte=thirty_days_ago).count()
    up = Check.objects.filter(monitor=monitor, checked_at__gte=thirty_days_ago, is_up=True).count()
    uptime = round((up / total * 100), 2) if total > 0 else 0
    alerts = Alert.objects.filter(monitor=monitor)

    if request.method == 'POST':
        alert_form = AlertForm(request.POST)
        if alert_form.is_valid():
            alert = alert_form.save(commit=False)
            alert.monitor = monitor
            alert.save()
            messages.success(request, 'Alert added!')
            return redirect('monitor_detail', monitor_id=monitor.id)
    else:
        alert_form = AlertForm()

    return render(request, 'monitors/detail.html', {
        'monitor': monitor,
        'checks': checks,
        'uptime': uptime,
        'alerts': alerts,
        'alert_form': alert_form,
    })


@login_required
def delete_alert(request, alert_id):
    alert = get_object_or_404(Alert, id=alert_id)
    if request.method == 'POST':
        alert.delete()
        messages.success(request, 'Alert deleted.')
    return redirect('monitor_detail', monitor_id=alert.monitor.id)