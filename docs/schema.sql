-- Primary Key uniqually identifies each row
CREATE TABLE monitors(
    id SERIAL PRIMARY KEY, -- Automatically generate a unique incrementing number for each new row
    name VARCHAR(255) NOT NULL, -- Can never be empty, max length 255
    url TEXT NOT NULL, -- Any length of text allowed, URLs could be long
    check_interval_seconds INTEGER NOT NULL DEFAULT 30, -- 30ms by default 
    is_active BOOLEAN NOT NULL DEFAULT true, 
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW() -- Date and time
);


CREATE TABLE checks (
    id BIGSERIAL PRIMARY KEY, -- The ID of each check that we have completed, could be large which is why we are using BIG SERIAL
    monitor_id INTEGER NOT NULL REFERENCES monitors(id) ON DELETE CASCADE, -- Foreign key, if deleting website, removing everything associated
    checked_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    status_code INTEGER, 
    response_time_ms INTEGER,
    is_up BOOLEAN NOT NULL, 
    error_message TEXT
);

CREATE TABLE alerts (
    id SERIAL PRIMARY KEY, 
    monitor_id INTEGER NOT NULL REFERENCES monitors(id) ON DELETE CASCADE, 
    alery_type VARCHAR(50) NOT NULL, 
    destination TEXT NOT NULL,
    is_active BOOLEAN NOT NULL DEFAULT true
);

CREATE INDEX idx_checks_monitor_time ON checks (monitor_id, checked_at DESC);