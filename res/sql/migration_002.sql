CREATE TABLE IF NOT EXISTS bands (
        id SERIAL PRIMARY KEY,
        name VARCHAR(10) UNIQUE NOT NULL,
        start_freq DECIMAL(12,6),
        end_freq DECIMAL(12,6),
        enabled BOOLEAN
);

CREATE TYPE dxcc_mode AS ENUM (
        'CW',
        'PHONE',
        'DIGITAL'
);

CREATE TABLE IF NOT EXISTS modes (
        id SERIAL PRIMARY KEY,
        name VARCHAR(15) UNIQUE NOT NULL,
        submodes JSON,
        rprt VARCHAR(10),
        dxcc dxcc_mode,
        enabled BOOLEAN
);
