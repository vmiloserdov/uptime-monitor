from django.urls import path
from . import views

urlpatterns = [
    path('login/', views.login_view, name='login'),
    path('logout/', views.logout_view, name='logout'),
    path('signup/', views.signup_view, name='signup'),
    path('dashboard/', views.dashboard, name='dashboard'),
    path('monitors/add/', views.add_monitor, name='add_monitor'),
    path('monitors/<int:monitor_id>/delete/', views.delete_monitor, name='delete_monitor'),
    path('monitors/<int:monitor_id>/', views.monitor_detail, name='monitor_detail'),
]