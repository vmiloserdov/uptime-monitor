# Uptime Monitor

A self-hosted uptime monitoring tool that checks whether your websites and APIs are up, and alerts you when they go down.

## My Idea

I wanted a simple, self-hosted alternative to tools like UptimeRobot — one where you own your data and can run unlimited monitors for free.

## How It Works

A C daemon runs in the background, probing URLs at configurable intervals using threads. Results are written to a PostgreSQL database. A Django web app reads from the same database and displays a dashboard with uptime history and alerts.

## Tech Stack

- **C** — background prober daemon (libcurl, libpq, pthreads)
- **Python / Django** — web dashboard
- **PostgreSQL** — shared database between the prober and the web app

## Note on AI Assistance

The idea and architecture for this project are mine. I used Claude as a coding assistant to help implement and debug the code while learning how to use C and Django together.

## Self-Hosting
```bash
# Clone the repo
git clone https://github.com/yourusername/uptime-monitor

# Copy and fill in your environment variables
cp .env.example .env

# Run the prober
cd prober && ./run.sh

# Run the web app
cd webapp && export $(cat ../.env | xargs) && python manage.py runserver
```

## Status

Work in progress.